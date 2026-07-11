#include "Cave/Map/Map.h"

void Cave::Map::updateJim(const int& index) {
	m_jimIndex = index;
	if (m_game->inputSystem.isPressed(Input::Action::SelfDestruct) || m_game->getTime() <= 0) {
		createExplosion(index);
		return;
	}
	updateEntityAnimation(index);

	if (m_game->inputSystem.isPressed(Input::Action::MoveUp)) {
		updateJimMovement(index, Cave::Entity::Facing::NEUTRAL, Cave::Entity::Direction::UP, Cave::Entity::Trait::WarpableUp);
	}
	else if (m_game->inputSystem.isPressed(Input::Action::MoveDown)) {
		updateJimMovement(index, Cave::Entity::Facing::NEUTRAL, Cave::Entity::Direction::DOWN, Cave::Entity::Trait::WarpableDown);
	}
	else if (m_game->inputSystem.isPressed(Input::Action::MoveRight)) {
		updateJimMovement(index, Cave::Entity::Facing::RIGHT, Cave::Entity::Direction::RIGHT, Cave::Entity::Trait::WarpableRight);
	}
	else if (m_game->inputSystem.isPressed(Input::Action::MoveLeft)) {
		updateJimMovement(index, Cave::Entity::Facing::LEFT, Cave::Entity::Direction::LEFT, Cave::Entity::Trait::WarpableLeft);
	}
	else {
		updateJimIdle(index);
	}
}

void Cave::Map::updateJimIdle(const int& index) {
	if (getEntityAnimation(index) == Cave::Entity::Jim::idleAnimation()) {
		if (Utils::randomInteger(0, 63) == 0) setEntityAnimation(index, Cave::Entity::Jim::blinkAnimation());
	}
	else if (getEntityAnimation(index) == Cave::Entity::Jim::blinkAnimation()) {
		if (caveEntities[index].animationLoopCompleted()) setEntityAnimation(index, Cave::Entity::Jim::idleAnimation());
	}
	else {
		setEntityAnimation(index, Cave::Entity::Jim::idleAnimation());
	}

	setEntityFacing(index, Cave::Entity::Facing::NEUTRAL);
}

void Cave::Map::updateJimMovement(const int& index, const Cave::Entity::Facing& facing, const Cave::Entity::Direction& direction, const Cave::Entity::Trait& warpTrait) {
	bool collectMode = m_game->inputSystem.isPressed(Input::Action::Collect);
	int inFront = getIndex(index, direction);

	if (handleJimComplete(index, inFront)) return;

	updateJimFacing(index, inFront, collectMode, facing);

	if (handleJimTubeWarp(index, inFront, collectMode, direction, warpTrait)) return;

	if (handleJimPushDetonator(index, inFront)) return;

	if (handleJimPush(index, inFront, collectMode, direction)) return;

	if (handleJimTraverse(index, inFront, collectMode, facing, direction)) return;
}

void Cave::Map::updateJimFacing(const int& index, const int& inFront, const bool& collectMode, const Cave::Entity::Facing& facing) {
	Cave::Entity::Facing oldFacing = getEntityFacing(index);
	Cave::Entity::Facing newFacing = (facing == Cave::Entity::Facing::NEUTRAL) ? ((oldFacing == Cave::Entity::Facing::NEUTRAL) ? Cave::Entity::Facing::RIGHT : oldFacing) : facing;
	setEntityFacing(index, newFacing);
}


bool Cave::Map::handleJimTraverse(const int& index, const int& inFront, const bool& collectMode, const Cave::Entity::Facing& facing, const Cave::Entity::Direction& direction) {
	if (!hasTrait(Cave::Entity::Trait::Traversable, inFront)) {
		(facing == Cave::Entity::Facing::NEUTRAL) ? setJimMoveAmination(index) : setJimPushAmination(index);
		return false;
	}
	setJimMoveAmination(index);

	bool collectable = hasTrait(Cave::Entity::Trait::Collectable, inFront);
	bool digging = getEntityType(inFront) == Cave::Entity::Type::Dirt;
	if (collectMode) {
		if (!getEntityTransitioning(inFront)) {
			if (collectable) {
				m_game->soundManager.play(Sound::Effect::Collect);
				m_game->sendSignal(GameSignal::CollectDiamond);
			}
			else if (digging) {
				m_jimTraversingDirt = true;
			}
			setEntity(inFront, Cave::Entity::Space());
		}
		return true;
	}
	if (moveEntity(index, direction)) {
		m_jimIndex = inFront;
		if (collectable) {
			m_game->soundManager.play(Sound::Effect::Collect);
			m_game->sendSignal(GameSignal::CollectDiamond);
		}
		else if (digging) {
			m_jimTraversingDirt = true;
		}
		return true;
	}
	return false;
}

bool Cave::Map::handleJimPush(const int& index, const int& inFront, const bool& collectMode, const Cave::Entity::Direction& direction) {
	if (!hasTrait(Cave::Entity::Trait::Pushable, inFront)) {
		return false;
	}
	if (!hasTrait(Cave::Entity::Trait::Empty, inFront, direction)) {
		if (collectMode) {
			setJimMoveAmination(index);
			return true;
		}
		return false;
	}
	m_game->inputSystem.increasePushTimer();
	if (m_game->inputSystem.registerPush() && pushEntity(index, direction)) {
		m_jimIndex = inFront;
		m_game->soundManager.play(Sound::Effect::Drop);
		return true;
	}
	return false;
}

bool Cave::Map::handleJimPushDetonator(const int& index, const int& inFront) {
	switch (getEntityType(inFront)) {
	case Cave::Entity::Type::Detonator:
		setEntity(inFront, Cave::Entity::DetonatorTriggered());
		[[fallthrough]];
	case Cave::Entity::Type::DetonatorTriggered:
		[[fallthrough]];
	case Cave::Entity::Type::DetonatorUsed:
		setJimMoveAmination(index);
		return true;
	default:
		return false;
	}
}

bool Cave::Map::handleJimTubeWarp(const int& index, const int& inFront, const bool& collectMode, const Cave::Entity::Direction& direction, const Cave::Entity::Trait& warpTrait) {
	if (!hasTrait(warpTrait, inFront)) {
		return false;
	}
	if (!hasTrait(Cave::Entity::Trait::Empty, inFront, direction)) {
		return false;
	}

	if (collectMode) {
		setJimMoveAmination(index);
		m_game->soundManager.play(Sound::Effect::Tube);
		return true;
	}
	setJimMoveAmination(index);
	if (warpEntity(index, direction)) {
		m_cameraSpeed = 8;
		m_jimIndex = getIndex(inFront, direction);
		m_game->soundManager.play(Sound::Effect::Tube);
		return true;
	}
	return false;
}

bool Cave::Map::handleJimComplete(const int& index, const int& inFront) {
	switch (getEntityType(inFront)) {
	case Cave::Entity::Type::ExitDoorOpening:
	case Cave::Entity::Type::ExitDoorOpen:
		m_jimIndex = inFront;
		setEntity(index, Cave::Entity::Space());
		setEntity(inFront, Cave::Entity::ExitDoorComplete());
		m_game->soundManager.play(Sound::Effect::Yippee);
		m_game->sendSignal(GameSignal::CavePass);
		m_state = Cave::State::Pass;
		return true;
	default:
		return false;
	}
}


void Cave::Map::setJimMoveAmination(const int& index) {
	(getEntityFacing(index) == Cave::Entity::Facing::LEFT) ?
		setEntityAnimation(index, Cave::Entity::Jim::moveLeftAnimation()) :
		setEntityAnimation(index, Cave::Entity::Jim::moveRightAnimation());
}

void Cave::Map::setJimPushAmination(const int& index) {
	(getEntityFacing(index) == Cave::Entity::Facing::LEFT) ?
		setEntityAnimation(index, Cave::Entity::Jim::pushLeftAnimation()) :
		setEntityAnimation(index, Cave::Entity::Jim::pushRightAnimation());
}

void Cave::Map::updateStartDoor(const int& index) {
	if (!m_introDelayOccurred) {
		m_introDelayOccurred = true;
		return;
	}
	setEntity(index, Cave::Entity::StartDoorOpen());
	m_game->soundManager.play(Sound::Effect::Open);
}

void Cave::Map::updateStartDoorOpen(const int& index) {
	updateEntityAnimation(index);

	if (caveEntities[index].animationLoopCompleted()) {
		setEntity(index, Cave::Entity::Jim());
		if (m_state == Cave::State::Intro) m_state = Cave::State::Play;
		m_game->sendSignal(GameSignal::CaveStart);
	}
}

void Cave::Map::updateExitDoor(const int& index) {
	if (m_game->caveQuotaReached()) {
		setEntity(index, Cave::Entity::ExitDoorOpening());
	}
}