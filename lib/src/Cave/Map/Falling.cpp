#include "Cave/Map/Map.h"

void Cave::Map::updateFallableEntity(const int& index) {
	updateEntityAnimation(index);

	updateFallableEntityDirection(index);

	int below = getIndex(index, Cave::Entity::Direction::DOWN);
	if (handleEntityFalling(index, below)) return;
	if (handleEntityLanding(index, below)) return;
	if (handleEntitySlip(index, below)) return;
}

void Cave::Map::updateFallableEntityDirection(const int& index) {
	switch (getEntityDirection(index)) {
	case Cave::Entity::Direction::LEFT:
		[[fallthrough]];
	case Cave::Entity::Direction::RIGHT:
		setEntityDirection(index, Cave::Entity::Direction::NO_DIRECTION);
		break;
	default:
		setEntityDirection(index, Cave::Entity::Direction::DOWN);
		break;
	}
}

bool Cave::Map::handleEntityFalling(const int& index, const int& below) {
	// Make the entity fall if the below is empty
	if (hasTrait(Cave::Entity::Trait::Empty, below)) {
		if (moveEntity(index, Cave::Entity::Direction::DOWN)) {
			if (!getEntityFalling(below)) {
				(hasTrait(Cave::Entity::Trait::Collectable, below)) ?
					m_game->soundManager.play(Sound::Effect::DiamondDrop) :
					m_game->soundManager.play(Sound::Effect::Drop);
			}
			setEntityFalling(below, true);
		}
		return true;
	}

	// Entity is current falling (or slipping)
	if (getEntityTransitioning(index)) {
		return true;
	}

	return false;
}

bool Cave::Map::handleEntityLanding(const int& index, const int& below) {
	// Entity has already landed
	if (!getEntityFalling(index)) {
		return false;
	}
	setEntityFalling(index, false);

	Cave::Entity::Type type = getEntityType(index);

	// If landing entity is a bomb, create an explosion
	if (type == Cave::Entity::Type::Bomb) {
		createExplosion(index);
		return true;
	}

	// If the entity below is crushable, create an explosion
	if (hasTrait(Cave::Entity::Trait::Crushable, below)) {
		createExplosion(below);
		return true;
	}

	Cave::Entity::Type belowType = getEntityType(below);

	// If the landing entity is a boulder and entity below is an ore, transform the ore
	if (type == Cave::Entity::Type::Boulder && belowType == Cave::Entity::Type::Ore) {
		setEntity(below, Cave::Entity::OreTransformation());
		m_game->soundManager.play(Sound::Effect::DiamondLand);
		return false;
	}

	// If the entity below is a fragile diamond, break it
	bool landedOnFragileDiamond = false;
	if (belowType == Cave::Entity::Type::FragileDiamond) {
		setEntity(below, Cave::Entity::BreakingFragileDiamond());
		m_game->soundManager.play(Sound::Effect::Break);
		landedOnFragileDiamond = true;
	}

	// If the entity itself is a fragile diamon, break itself
	if (type == Cave::Entity::Type::FragileDiamond) {
		setEntity(index, Cave::Entity::BreakingFragileDiamond());
		m_game->soundManager.play(Sound::Effect::Break);
		return true;
	}

	if (handleEntityLandingOnMagicWall(index, type, below, belowType)) {
		return true;
	}

	if (!landedOnFragileDiamond) {
		hasTrait(Cave::Entity::Trait::Collectable, index) ?
			m_game->soundManager.play(Sound::Effect::DiamondLand) :
			m_game->soundManager.play(Sound::Effect::Land);
	}

	return false;
}

bool Cave::Map::handleEntityLandingOnMagicWall(const int& index, const Cave::Entity::Type& type, const int& below, const Cave::Entity::Type& belowType) {
	// No interaction with magic wall if entity is not a boulder or diamond
	if (type != Cave::Entity::Type::Boulder && type != Cave::Entity::Type::Diamond) {
		return false;
	}

	// Used magic wall below will consume boulders and diamonds
	if (belowType == Cave::Entity::Type::MagicWallUsed) {
		setEntity(index, Cave::Entity::Space());
		return true;
	}

	// Need magic wall (inactive or active) to exist below
	if (belowType == Cave::Entity::Type::MagicWallInactive) {
		m_magicWallStarted = true;
	}
	else if (belowType != Cave::Entity::Type::MagicWallActive) {
		return false;
	}

	// Replace landed entity with space
	setEntity(index, Cave::Entity::Space());

	// Needs to be empty under the magic wall
	int under = getIndex(below, Cave::Entity::Direction::DOWN);
	if (!hasTrait(Cave::Entity::Trait::Empty, under)) {
		return true;
	}

	// Create opposite entity under magic wall
	(type == Cave::Entity::Type::Boulder) ? setEntity(under, Cave::Entity::Diamond()) : setEntity(under, Cave::Entity::Boulder());
	setEntityFalling(under, true);

	return true;
}

bool Cave::Map::handleEntitySlip(const int& index, const int& below) {
	// Nothing left to handle if the entity below is not slippery
	if (!hasTrait(Cave::Entity::Trait::Slippery, below)) {
		return false;
	}

	// Falling entity is only able to slip if its direction is down
	if (getEntityDirection(index) != Cave::Entity::Direction::DOWN) {
		return false;
	}

	// Slip entity to the right if empty
	if (hasTrait(Cave::Entity::Trait::Empty, index, Cave::Entity::Direction::RIGHT) && hasTrait(Cave::Entity::Trait::Empty, below, Cave::Entity::Direction::RIGHT)) {
		moveEntity(index, Cave::Entity::Direction::RIGHT);
		setEntityFalling(getIndex(index, Cave::Entity::Direction::RIGHT), true);
		setEntityDirection(getIndex(index, Cave::Entity::Direction::LEFT), Cave::Entity::Direction::RIGHT);
		return true;
	}

	// Else attempt to slip to the left
	if (hasTrait(Cave::Entity::Trait::Empty, index, Cave::Entity::Direction::LEFT) && hasTrait(Cave::Entity::Trait::Empty, below, Cave::Entity::Direction::LEFT)) {
		moveEntity(index, Cave::Entity::Direction::LEFT);
		setEntityFalling(getIndex(index, Cave::Entity::Direction::LEFT), true);
		setEntityDirection(getIndex(index, Cave::Entity::Direction::LEFT), Cave::Entity::Direction::LEFT);
		return true;
	}

	return false;
}

void Cave::Map::updateTNT(const int& index) {
	if (m_detonatorTriggered) {
		createExplosion(index);
		return;
	}
	updateFallableEntity(index);
}