#include "Cave/Map/Map.h"

void Cave::Map::updateProtoza(const int& index) {
	if (handleEnemyBasicUpdate(index)) return;

	auto arc = anticlockwiseArc(getEntityDirection(index));
	for (int i = 0; i < 3; ++i) {
		if (tryMoveEnemy(index, Cave::Entity::Trait::Empty, arc[i])) return;
	}
	if (!getEntityMoving(index) && tryMoveEnemy(index, Cave::Entity::Trait::Empty, arc[3])) return;
	setEntityMoving(index, false);
}

void Cave::Map::updateCaveGull(const int& index) {
	if (handleEnemyBasicUpdate(index)) return;

	auto arc = clockwiseArc(getEntityDirection(index));
	for (int i = 0; i < 3; ++i) {
		if (tryMoveEnemy(index, Cave::Entity::Trait::Empty, arc[i])) return;
	}
	if (!getEntityMoving(index) && tryMoveEnemy(index, Cave::Entity::Trait::Empty, arc[3])) return;
	setEntityMoving(index, false);
}

void Cave::Map::updateCilia(const int& index) {
	if (handleEnemyBasicUpdate(index)) return;

	Cave::Entity::Direction dir = getEntityDirection(index);
	if (!tryMoveEnemy(index, Cave::Entity::Trait::Empty, dir)) {
		setEntityDirection(index, Cave::Entity::getRandomDirection());
	}
}

void Cave::Map::updateEater(const int& index) {
	if (handleEnemyBasicUpdate(index)) return;

	auto arc = clockwiseArc(getEntityDirection(index));
	for (int i = 0; i < 3; ++i) {
		if (i == 1 && tryMoveEnemy(index, Cave::Entity::Trait::Collectable, arc[i])) {
			m_game->soundManager.play(Sound::Effect::Collect);
			return;
		}
		if (tryMoveEnemy(index, Cave::Entity::Trait::Empty, arc[i])) return;
	}
	if (!getEntityMoving(index) && tryMoveEnemy(index, Cave::Entity::Trait::Empty, arc[3])) return;
	setEntityMoving(index, false);
}

void Cave::Map::updateAggressor(const int& index) {
	if (handleEnemyBasicUpdate(index)) return;

	int ex = index % width, ey = index / width;
	int jx = m_jimIndex % width, jy = m_jimIndex / width;

	if (ex > jx && tryMoveEnemy(index, Cave::Entity::Trait::Empty, Cave::Entity::Direction::LEFT)) return;
	if (ex < jx && tryMoveEnemy(index, Cave::Entity::Trait::Empty, Cave::Entity::Direction::RIGHT)) return;
	if (ey > jy && tryMoveEnemy(index, Cave::Entity::Trait::Empty, Cave::Entity::Direction::UP)) return;
	if (ey < jy && tryMoveEnemy(index, Cave::Entity::Trait::Empty, Cave::Entity::Direction::DOWN)) return;
}

bool Cave::Map::handleEnemyBasicUpdate(const int& index) {
	updateEntityAnimation(index);

	if (handleEnemyDeath(index)) return true;

	if (getEntityTransitioning(index)) return true;

	return false;
}

bool Cave::Map::handleEnemyDeath(const int& index) {
	if (isAdjacentTo(index, Cave::Entity::Trait::Reactive)) {
		createExplosion(index);
		return true;
	}
	return false;
}

bool Cave::Map::tryMoveEnemy(const int& index, const Cave::Entity::Trait& trait, const Cave::Entity::Direction& direction) {
	setEntityDirection(index, direction);
	if (hasTrait(trait, index, direction) && moveEntity(index, direction)) {
		setEntityMoving(getIndex(index, direction), true);
		return true;
	}
	return false;
}