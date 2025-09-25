#include "Cave/Map/Map.h"

std::vector<int> Cave::Map::preUpdateCaveEntities() {
	std::vector<int> indicies;
	for (int index = 0; index < width * height; ++index) {
		if (hasTrait(Cave::Entity::Trait::Immutable, index)) continue;
		updateEntityTransition(index);
		if (hasTrait(Cave::Entity::Trait::Static, index)) continue;
		if (!m_TickCounter.onTick()) continue;
		indicies.emplace_back(index);
		setEntityUpdated(index, false);
	}

	m_jimTraversingDirt = false;

	if (m_amoebaChecked && m_amobeaIsTrapped) m_amoebaIsCompletelyTrapped = true;
	if (m_amoebaGrowthCount >= m_amoebaGrowthMax) m_amoebaSurpassedMaxGrowth = true;
	m_amobeaIsTrapped = true;
	m_amoebaChecked = false;
	return indicies;
}

void Cave::Map::updateCaveEntities(const std::vector<int> indicies) {
	m_cameraSpeed = 4;
	switch (m_state) {
	case Cave::State::Intro:
		updateEntityDuringIntro(indicies);
		break;
	case Cave::State::Play:
		[[fallthrough]];
	case Cave::State::Pass:
		[[fallthrough]];
	case Cave::State::Fail:
		updateActiveEntity(indicies);
		break;
	case Cave::State::Load:
		[[fallthrough]];
	case Cave::State::Pause:
		[[fallthrough]];
	case Cave::State::End:
		updateInactiveEntity(indicies);
		break;
	default:
		break;
	}

	// Handle audio when Jim is traversing through dirt.
	// The sound plays on loop so long as Jim continues to traverse through dirt.
	if (m_jimTraversingDirt && !m_game->soundManager.isPlaying(Sound::Effect::Dig)) {
		m_game->soundManager.loop(Sound::Effect::Dig);
	}
	else if (!m_jimTraversingDirt && m_game->soundManager.isPlaying(Sound::Effect::Dig)) {
		m_game->soundManager.stop(Sound::Effect::Dig);
	}

	// Handle magic wall timer and audio
	// Once activated, magic wall will be active for the duration of the magic wall time, and then stop indefinitely.
	// Whilst active, the magic wall sound will play on loop.
	if (m_magicWallTimer > 0 && m_magicWallStarted) {
		m_magicWallTimer--;
		if (!m_game->soundManager.isPlaying(Sound::Effect::MagicWall)) {
			m_game->soundManager.loop(Sound::Effect::MagicWall);
		}
	}
	else if (m_magicWallTimer <= 0 && m_game->soundManager.isPlaying(Sound::Effect::MagicWall)) {
		m_game->soundManager.stop(Sound::Effect::MagicWall);
	}

	// Handle all amoeba logic
	// Amoeba sound will play on loop so long as it is present in the cave.
	if (m_state == Cave::State::Play) {
		if (m_amoebaChecked && !m_game->soundManager.isPlaying(Sound::Effect::Amoeba)) {
			m_game->soundManager.loop(Sound::Effect::Amoeba);
		}
		else if (!m_amoebaChecked && m_game->soundManager.isPlaying(Sound::Effect::Amoeba)) {
			m_game->soundManager.stop(Sound::Effect::Amoeba);
		}
	}
	else if (
		m_state != Cave::State::Pause &&
		m_state != Cave::State::Fail &&
		m_state != Cave::State::Pass &&
		m_game->soundManager.isPlaying(Sound::Effect::Amoeba)
		) {
		m_game->soundManager.stop(Sound::Effect::Amoeba);
	}
}

void Cave::Map::updateActiveEntity(const std::vector<int> indicies) {
	for (int index : indicies) {
		if (getEntityUpdated(index)) continue;

		setEntityUpdated(index, true);
		Cave::Entity::Type type = getEntityType(index);
		auto it = m_entityUpdateMap.find(type);
		if (it != m_entityUpdateMap.end()) {
			it->second(index);
		}
	}
	if (!m_quotaReached && m_game->caveQuotaReached()) {
		// If Jim was ever in this cave, play the unlock sound when quota is reached
		if (m_jimIndex != OUT_OF_BOUNDS_INDEX) m_game->soundManager.play(Sound::Effect::Unlock);
		m_quotaReached = true;
	}
}

void Cave::Map::updateInactiveEntity(const std::vector<int> indicies) {
	for (int index : indicies) {
		if (getEntityUpdated(index)) continue;

		setEntityUpdated(index, true);
		Cave::Entity::Type type = getEntityType(index);
		switch (type) {
		case Cave::Entity::Type::Jim: updateJimIdle(index); updateEntityAnimation(index); break;
		case Cave::Entity::Type::Explosion: updateTransientEntity(index, Cave::Entity::Space()); break;
		case Cave::Entity::Type::OreTransformation: updateTransientEntity(index, Cave::Entity::Diamond()); break;
		case Cave::Entity::Type::CaveGullExplosion: updateTransientEntity(index, Cave::Entity::Diamond()); break;
		case Cave::Entity::Type::ExitDoorOpening: updateTransientEntity(index, Cave::Entity::ExitDoorOpen()); break;
		case Cave::Entity::Type::ExitDoorComplete: updateTransientEntity(index, Cave::Entity::ExitDoorFinished()); break;
		case Cave::Entity::Type::DetonatorTriggered: updateTransientEntity(index, Cave::Entity::DetonatorUsed()); break;
		case Cave::Entity::Type::BreakingFragileDiamond: updateTransientEntity(index, Cave::Entity::Space()); break;
		default: if (!hasTrait(Cave::Entity::Trait::Immutable, index)) updateEntityAnimation(index); break;
		}
	}
}

void Cave::Map::updateEntityDuringIntro(const std::vector<int> indicies) {
	for (int index : indicies) {
		if (getEntityUpdated(index)) continue;

		setEntityUpdated(index, true);
		Cave::Entity::Type type = getEntityType(index);
		switch (type) {
		case Cave::Entity::Type::StartDoor: updateStartDoor(index); break;
		case Cave::Entity::Type::StartDoorOpen: updateStartDoorOpen(index); break;
		case Cave::Entity::Type::ExitDoor: break;
		default:
			auto it = m_entityUpdateMap.find(type);
			if (it != m_entityUpdateMap.end()) {
				it->second(index);
			}
			break;
		}
	}
}

void Cave::Map::initEntityUpdateMaps() {
	m_entityUpdateMap[Cave::Entity::Type::Jim] = [this](int i) { updateJim(i); };

	m_entityUpdateMap[Cave::Entity::Type::ExitDoor] = [this](int i) { updateExitDoor(i); };
	m_entityUpdateMap[Cave::Entity::Type::Amoeba] = [this](int i) { updateAmoeba(i); };
	m_entityUpdateMap[Cave::Entity::Type::Plasma] = [this](int i) { updatePlasma(i); };
	m_entityUpdateMap[Cave::Entity::Type::HorizontalWall] = [this](int i) { updateHorizontalWall(i); };
	m_entityUpdateMap[Cave::Entity::Type::VerticalWall] = [this](int i) { updateVerticalWall(i); };
	m_entityUpdateMap[Cave::Entity::Type::MagicWallActive] = [this](int i) { updateMagicWallActive(i); };
	m_entityUpdateMap[Cave::Entity::Type::MagicWallInactive] = [this](int i) { updateMagicWallInactive(i); };
	m_entityUpdateMap[Cave::Entity::Type::TNT] = [this](int i) { updateTNT(i); };
	m_entityUpdateMap[Cave::Entity::Type::DetonatorUsed] = [this](int i) { updateDetonatorUsed(i); };

	m_entityUpdateMap[Cave::Entity::Type::Protozo] = [this](int i) { updateProtoza(i); };
	m_entityUpdateMap[Cave::Entity::Type::CaveGull] = [this](int i) { updateCaveGull(i); };
	m_entityUpdateMap[Cave::Entity::Type::Cilia] = [this](int i) { updateCilia(i); };
	m_entityUpdateMap[Cave::Entity::Type::Eater] = [this](int i) { updateEater(i); };
	m_entityUpdateMap[Cave::Entity::Type::Aggressor] = [this](int i) { updateAggressor(i); };

	m_entityUpdateMap[Cave::Entity::Type::Explosion] = [this](int i) { updateTransientEntity(i, Cave::Entity::Space()); };
	m_entityUpdateMap[Cave::Entity::Type::OreTransformation] = [this](int i) { updateTransientEntity(i, Cave::Entity::Diamond()); };
	m_entityUpdateMap[Cave::Entity::Type::CaveGullExplosion] = [this](int i) { updateTransientEntity(i, Cave::Entity::Diamond()); };
	m_entityUpdateMap[Cave::Entity::Type::ExitDoorOpening] = [this](int i) { updateTransientEntity(i, Cave::Entity::ExitDoorOpen()); };
	m_entityUpdateMap[Cave::Entity::Type::ExitDoorComplete] = [this](int i) { updateTransientEntity(i, Cave::Entity::ExitDoorFinished()); };
	m_entityUpdateMap[Cave::Entity::Type::DetonatorTriggered] = [this](int i) { updateTransientEntity(i, Cave::Entity::DetonatorUsed()); };
	m_entityUpdateMap[Cave::Entity::Type::BreakingFragileDiamond] = [this](int i) { updateTransientEntity(i, Cave::Entity::Space()); };

	m_entityUpdateMap[Cave::Entity::Type::Diamond] = [this](int i) { updateFallableEntity(i); };
	m_entityUpdateMap[Cave::Entity::Type::FragileDiamond] = [this](int i) { updateFallableEntity(i); };
	m_entityUpdateMap[Cave::Entity::Type::Ore] = [this](int i) { updateFallableEntity(i); };
	m_entityUpdateMap[Cave::Entity::Type::Boulder] = [this](int i) { updateFallableEntity(i); };
	m_entityUpdateMap[Cave::Entity::Type::Bomb] = [this](int i) { updateFallableEntity(i); };
}

void Cave::Map::update(Camera camera) {
	switch (m_state) {
	case Cave::State::Load:
		// Handle loading tiles disappearing
		for (int index = 0; index < width * height; index++) {
			if (Utils::randomInteger(0, std::max(m_loadRate - CAVE_LOAD_THTRESHOLD, 0)) == 0) m_loaded[index] = true;
		}
		// Enter the Intro state once the loading has finished
		if (m_loadRate > 0) m_loadRate--;
		else m_state = Cave::State::Intro;
		break;
	case Cave::State::Play:
		// Handle game being paused
		if (m_game->inputSystem.wasPressed(Input::Action::Pause)) {
			m_state = Cave::State::Pause;
			if (!m_game->isGamePaused()) m_game->sendSignal(GameSignal::CavePause);
		}
		break;
	case Cave::State::Pause:
		// Handle game being unpaused
		if (m_game->inputSystem.wasPressed(Input::Action::Pause)) {
			m_state = Cave::State::Play;
			if (m_game->isGamePaused()) m_game->sendSignal(GameSignal::CaveUnpause);
		}
		break;
	case Cave::State::Pass:
		// Handle cave end once bonus points have been calculated and the cave timer has reached 0
		if (m_game->getTime() == 0) m_state = Cave::State::End;
		break;
	case Cave::State::Fail:
		// Handle cave end when the player enters a confirm action
		if (m_game->inputSystem.isPressed(Input::Action::Confirm)) m_state = Cave::State::End;
		break;
	case Cave::State::End:
		// Handle loading tiles covering the entity tiles
		if (m_loadRate < CAVE_LOAD_RATE - CAVE_UNLOAD_RATE) m_loadRate = CAVE_LOAD_RATE - CAVE_UNLOAD_RATE;
		for (int index = 0; index < width * height; index++) {
			if (Utils::randomInteger(m_loadRate, CAVE_LOAD_RATE) == CAVE_LOAD_RATE) m_loaded[index] = false;
		}
		// Enter the Load state once the unloading has finished
		if (m_loadRate < CAVE_LOAD_RATE) m_loadRate++;
		else {
			m_reset = true;
			m_state = Cave::State::Load;
			m_game->sendSignal(GameSignal::CaveLoad);
		}
		break;
	default:
		break;
	}

	if (m_game->inputSystem.wasPressed(Input::Action::Quit)) m_game->sendSignal(GameSignal::GotoMainMenu);

	// Cave entities are updated every tick
	std::vector<int> indicies = preUpdateCaveEntities();
	if (m_TickCounter.onTick()) {
		updateCaveEntities(indicies);
	}

	// Visible tiles are refreshed each frame based on the camera position
	updateVisibleTiles(camera);
}

void Cave::Map::updateVisibleTiles(Camera camera) {
	sf::Vector2f viewOrigin = camera.getCenter() - camera.getSize() / 2.f;

	int min_x = std::min(std::max(static_cast<int>(viewOrigin.x / 32) - 1, 0), width);
	int max_x = std::min(min_x + 21 + 1, width);

	int min_y = std::min(std::max(static_cast<int>(viewOrigin.y / 32), 0), height);
	int max_y = std::min(min_y + 13 + 1, height);

	std::vector<Cave::Entity::Base> entities;

	if (m_state == Cave::State::Load) {
		std::vector<bool> loaded;
		for (int y = min_y; y < max_y; ++y) {
			for (int x = min_x; x < max_x; ++x) {
				loaded.push_back(m_loaded[y * width + x]);
			}
		}
		m_loadingTileRenderer.updateLoadingTexture(loaded, { 0, 0 }, sf::IntRect({ min_x, min_y }, { max_x - min_x,max_y - min_y }));
	}
	else if (m_state == Cave::State::End) {
		std::vector<bool> loaded;
		for (int y = min_y; y < max_y; ++y) {
			for (int x = min_x; x < max_x; ++x) {
				loaded.push_back(m_loaded[y * width + x]);
			}
		}
		m_loadingTileRenderer.updateLoadingTexture(loaded, { 0, 0 }, sf::IntRect({ min_x, min_y }, { max_x - min_x,max_y - min_y }));
	}

	for (int y = min_y; y < max_y; ++y) {
		for (int x = min_x; x < max_x; ++x) {
			entities.push_back(caveEntities[y * width + x]);
		}
	}
	m_tileRenderer.updateTexture(entities, { 0, 0 }, sf::IntRect({ min_x, min_y }, { max_x - min_x,max_y - min_y }));
}

void Cave::Map::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	m_tileRenderer.render(target, states);
	sf::RenderStates noShaderStates = states;
	noShaderStates.shader = nullptr;
	if (m_state == Cave::State::Load || m_state == Cave::State::End) {
		m_loadingTileRenderer.render(target, noShaderStates);
	}
}