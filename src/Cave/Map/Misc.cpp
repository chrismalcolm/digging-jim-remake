#include "Cave/Map/Map.h"

void Cave::Map::handleBoulderRoll(const int& index, const Cave::Entity::Direction& direction) {
	if ((direction == Cave::Entity::Direction::LEFT || direction == Cave::Entity::Direction::RIGHT) && getEntityType(index) == Cave::Entity::Type::Boulder) {
		setEntity(index, Cave::Entity::Boulder());
	}
}

void Cave::Map::createExplosion(const int& index) {
	bool caveGullExplosion = getEntityType(index) == Cave::Entity::Type::CaveGull;
	createExplosion(index, caveGullExplosion);
}

void Cave::Map::createExplosion(const int& index, bool caveGullExplosion) {
	// Track any bombs hit by an explosion
	std::vector<size_t> bombIndices;

	// Create an explosion for each offset
	for (int offset : m_explosionOffsets) {
		int explosionIndex = index + offset;

		// Cannot create an explosion where the entity is indestructible
		if (hasTrait(Cave::Entity::Trait::Indestructible, explosionIndex)) {
			continue;
		}

		// Handle the transitions for entities affected by this explosion creation
		int previousIndex = getIndex(explosionIndex, caveEntities[explosionIndex].getPreviousDirection());
		if (previousIndex != OUT_OF_BOUNDS_INDEX) {
			//caveEntities[previousIndex].applyTransitionAmination(Space().getAnimation());
			caveEntities[previousIndex].terminatePreviousTransition();
		}
		caveEntities[explosionIndex].terminateCurrentTransition();

		// Add index of explosion if conditions are met
		Cave::Entity::Type type = getEntityType(explosionIndex);
		if (!caveGullExplosion && offset != 0 && type == Cave::Entity::Type::Bomb) {
			bombIndices.push_back(explosionIndex);
		}

		// Actually create explosion entity
		caveGullExplosion ? setEntity(explosionIndex, Cave::Entity::CaveGullExplosion()) : setEntity(explosionIndex, Cave::Entity::Explosion());

		// Jim has been killed
		if (type == Cave::Entity::Type::Jim) {
			m_game->sendSignal(GameSignal::CaveFail);
			m_state = Cave::State::Fail;
			// Do not reset camera position if failed
			m_resetCameraPosition = false;
		}
	}

	caveGullExplosion ? m_game->soundManager.play(Sound::Effect::CaveGullExplosion) : m_game->soundManager.play(Sound::Effect::Explosion);

	// Iterate through creating explosions for each bomb triggered
	for (auto& index : bombIndices) {
		createExplosion(index, false);
	}
}

void Cave::Map::updateTubeTexture(const int& index) {
	switch (getEntityType(index)) {
	case Cave::Entity::Type::TubeLeft:
		if (isVerticalAdjacentTo(index, Cave::Entity::Type::SolidWall))
			setEntityAnimation(index, Cave::Entity::TubeLeft::solidWallTube());
		else
			setEntityAnimation(index, Cave::Entity::TubeLeft::wallTube());
		break;
	case Cave::Entity::Type::TubeRight:
		if (isVerticalAdjacentTo(index, Cave::Entity::Type::SolidWall))
			setEntityAnimation(index, Cave::Entity::TubeRight::solidWallTube());
		else
			setEntityAnimation(index, Cave::Entity::TubeRight::wallTube());
		break;
	case Cave::Entity::Type::TubeHorizontal:
		if (isVerticalAdjacentTo(index, Cave::Entity::Type::SolidWall))
			setEntityAnimation(index, Cave::Entity::TubeHorizontal::solidWallTube());
		else
			setEntityAnimation(index, Cave::Entity::TubeHorizontal::wallTube());
		break;
	case Cave::Entity::Type::TubeUp:
		if (isHorizontalAdjacentTo(index, Cave::Entity::Type::SolidWall))
			setEntityAnimation(index, Cave::Entity::TubeUp::solidWallTube());
		else
			setEntityAnimation(index, Cave::Entity::TubeUp::wallTube());
		break;
	case Cave::Entity::Type::TubeDown:
		if (isHorizontalAdjacentTo(index, Cave::Entity::Type::SolidWall))
			setEntityAnimation(index, Cave::Entity::TubeDown::solidWallTube());
		else
			setEntityAnimation(index, Cave::Entity::TubeDown::wallTube());
		break;
	case Cave::Entity::Type::TubeVertical:
		if (isHorizontalAdjacentTo(index, Cave::Entity::Type::SolidWall))
			setEntityAnimation(index, Cave::Entity::TubeVertical::solidWallTube());
		else
			setEntityAnimation(index, Cave::Entity::TubeVertical::wallTube());
		break;

	default:
		break;
	}
}