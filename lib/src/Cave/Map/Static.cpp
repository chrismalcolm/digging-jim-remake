#include "Cave/Map/Map.h"

void Cave::Map::updateAmoeba(const int& index) {
	updateEntityAnimation(index);

	// Amoeba will turn into boulders if amoeba grwoth surpasses max growth limit
	if (m_amoebaSurpassedMaxGrowth) {
		setEntity(index, Cave::Entity::Boulder());
		return;
	}

	// Amoeba will turn into diamonds if each amoeba is trapped (not free in any direction)
	if (m_amoebaIsCompletelyTrapped) {
		setEntity(index, Cave::Entity::Diamond());
		return;
	}

	m_amobeaIsTrapped &= handleTrappedAmoeba(index);

	// Random chance to create a new amoeba in a given random direction
	int newAmoebaIndex = getIndex(index, Cave::Entity::getRandomDirection());
	if (hasTrait(Cave::Entity::Trait::Free, newAmoebaIndex) && Utils::randomInteger(0, 1000) <= m_amoebaGrowthSpeed) {
		setEntity(newAmoebaIndex, Cave::Entity::Amoeba());
		m_amobeaIsTrapped &= handleTrappedAmoeba(newAmoebaIndex);
		m_amoebaGrowthCount++;
	}
}

bool Cave::Map::handleTrappedAmoeba(const int& index) {
	m_amoebaChecked = true;
	return !isAdjacentTo(index, Cave::Entity::Trait::Free);
}

void Cave::Map::updatePlasma(const int& index) {
	updateEntityAnimation(index);

	// Random chance to create a new plasma in each direction
	for (auto& direction : Cave::Entity::ALL_DIRECTIONS) {
		int newPlasma = getIndex(index, direction);
		if (hasTrait(Cave::Entity::Trait::Empty, newPlasma) && Utils::randomInteger(0, 1000) <= m_plasmaGrowthSpeed) {
			setEntity(newPlasma, Cave::Entity::Plasma());
			m_game->soundManager.play(Sound::Effect::Plasma);
		}
	}
}

void Cave::Map::updateHorizontalWall(const int& index) {
	for (auto& direction : Cave::Entity::HORIZONTAL_DIRECTIONS) {
		int newWall = getIndex(index, direction);
		if (hasTrait(Cave::Entity::Trait::Empty, newWall)) {
			setEntity(newWall, Cave::Entity::HorizontalWall());
			m_game->soundManager.play(Sound::Effect::Drop);
		}
	}
}

void Cave::Map::updateVerticalWall(const int& index) {
	for (auto& direction : Cave::Entity::VERTICAL_DIRECTIONS) {
		int newWall = getIndex(index, direction);
		if (hasTrait(Cave::Entity::Trait::Empty, newWall)) {
			setEntity(newWall, Cave::Entity::VerticalWall());
			m_game->soundManager.play(Sound::Effect::Drop);
		}
	}
}

void Cave::Map::updateMagicWallActive(const int& index) {
	updateEntityAnimation(index);
	if (m_magicWallTimer <= 0) {
		setEntity(index, Cave::Entity::MagicWallUsed());
	}
}

void Cave::Map::updateMagicWallInactive(const int& index) {
	if (m_magicWallStarted) {
		setEntity(index, Cave::Entity::MagicWallActive());
	}
}

void Cave::Map::updateDetonatorUsed(const int& index) {
	m_detonatorTriggered = true;
}

void Cave::Map::updateTransientEntity(const int& index, Cave::Entity::Base becomes) {
	updateEntityAnimation(index);

	if (caveEntities[index].animationLoopCompleted()) {
		setEntity(index, std::move(becomes));
	}
}