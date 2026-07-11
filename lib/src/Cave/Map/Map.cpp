#include "Cave/Map/Map.h"
#include <algorithm>
#include <unordered_map>
#include <utility>
#include "Cave/Entity/Direction.h"
#include "Cave/Entity/Entity.h"
#include "Cave/Entity/Transition.h"
#include "Cave/Manager/Data.h"
#include "Renderer/TileRenderer.h"
#include "Sound/Manager.h"
#include "Utils/Counter.h"
#include "Utils/Random.h"

Cave::Map::Map(Game* game)
	: m_game(game), m_tileRenderer(&game->imageManager), m_loadingTileRenderer(&game->imageManager)
{
	initEntityUpdateMaps();
}

void Cave::Map::load() {
	if (!m_tileRenderer.load(Image::Texture::CaveTiles, { 32, 32 })) {
		throw std::runtime_error("Error: Unable to load map texture.\n");
	}
	if (!m_loadingTileRenderer.load(Image::Texture::CaveLoadingTiles, { 32, 32 })) {
		throw std::runtime_error("Error: Unable to load map loading texture.\n");
	}
}

void Cave::Map::generateMap(const Cave::Properties* properties, const std::vector<char>& tileData) {

	// Reset start door location
	m_startDoorIndex = OUT_OF_BOUNDS_INDEX;

	// Set map dimensions
	width = properties->width;
	height = properties->height;

	// Offset helpers based on width and height
	m_horizontalOffsets = { -1, 1 };
	m_verticalOffsets = { -width, width };
	m_adjacentOffsets = { -1, 1, -width, width };
	m_explosionOffsets = { -width - 1, -width, -width + 1, -1, 0, 1, width - 1, width, width + 1 };

	// Create caveEntities grid and loading tile grid
	caveEntities.resize(0);
	m_loaded.resize(0);
	for (int index = 0; index < width * height; ++index) {
		m_loaded.push_back(false);
		caveEntities.push_back(Cave::Data::getTileEntity(tileData[index]));
		if (Cave::Data::isStartDoor(tileData[index])) m_startDoorIndex = index;
	}

	// Update tube textures
	for (int index = 0; index < width * height; ++index) {
		updateTubeTexture(index);
	}

	// Reset Jim variables
	m_jimIndex = m_startDoorIndex;
	m_introDelayOccurred = false;
	m_jimTraversingDirt = false;

	// Reset magic wall variables
	m_magicWallStarted = false;
	m_magicWallTimer = properties->magicWallTime * 8; // x8 to convert from ticks to seconds.
	
	// Reset plasma variables
	m_plasmaGrowthSpeed = properties->plasmaGrowthSpeed;

	// Reset amoeba variables
	m_amoebaGrowthCount = 0;
	m_amoebaChecked = false;
	m_amobeaIsTrapped = false;
	m_amoebaIsCompletelyTrapped = false;
	m_amoebaSurpassedMaxGrowth = false;
	m_amoebaGrowthSpeed = properties->amoebaGrowthSpeed;
	m_amoebaGrowthMax = properties->amoebaGrowthMax;

	// Reset detonator variables
	m_detonatorTriggered = false;

	// Reset loading variables
	m_loadRate = CAVE_LOAD_RATE;

	// Reset camera variables
	m_cameraSpeed = 4;

	// Set quota variables
	m_quotaReached = false;

	// Set cave state to loading
	m_state = Cave::State::Load;

	// Game logic
	m_game->sendSignal(GameSignal::CaveLoad);
	m_game->setTime(properties->time);
}

void Cave::Map::setEditorMode() {
	std::fill(m_loaded.begin(), m_loaded.end(), true);
	m_state = Cave::State::Pause;
}

void Cave::Map::prepareForPlay() {
	for (int i = 0; i < width * height; ++i) {
		switch (getEntityType(i)) {
		case Cave::Entity::Type::MagicWallActive:
			setEntity(i, Cave::Entity::MagicWallInactive());
			break;
		case Cave::Entity::Type::HorizontalWallPlaceholder:
			setEntity(i, Cave::Entity::HorizontalWall());
			break;
		case Cave::Entity::Type::VerticalWallPlaceholder:
			setEntity(i, Cave::Entity::VerticalWall());
			break;
		default:
			break;
		}
	}
}

void Cave::Map::placeEntity(const int& index, Cave::Entity::Base& entity) {
	if (!inBounds(index) || getEntityTransitioning(index)) {
		return;
	}
	caveEntities[index] = entity;

	// Update tube animation
	updateTubeTexture(index);
	for (int offset : m_adjacentOffsets) {
		int neighbourIndex = index + offset;
		if (inBounds(neighbourIndex)) {
			updateTubeTexture(neighbourIndex);
		}
	}
}

int Cave::Map::getDiamondCount() {
	int diamondCount = 0;
	for (int index = 0; index < width * height; index++) {
		if (hasTrait(Cave::Entity::Trait::Collectable, index)) {
			diamondCount++;
		}
	}
	return diamondCount;
}

Cave::Entity::Base Cave::Map::getEntity(const int& index) const {
	if (!inBounds(index)) {
		return Cave::Entity::NoEntity();
	}
	return caveEntities[index];
}

void Cave::Map::setEntity(const int& index, Cave::Entity::Base&& entity) {
	if (!inBounds(index)) {
		return;
	}
	caveEntities[index].transferTransition(entity);
	caveEntities[index] = entity;
}

bool Cave::Map::inBounds(const int& index) const {
	return index >= 0 && index < width * height;
}

bool Cave::Map::onBorder(const int& index) const {
	int x = index % width;
	int y = index / width;
	return x == 0 || y == 0 || x == width - 1 || y == height - 1;
}

int Cave::Map::getIndex(const int& sourceIndex, const Cave::Entity::Direction& direction) const {
	// Cannot get index if no direction is given
	if (direction == Cave::Entity::Direction::NO_DIRECTION) {
		return OUT_OF_BOUNDS_INDEX;
	}

	// Cannot get index if destination is out of bounds
	int destinationIndex = sourceIndex + m_adjacentOffsets[static_cast<size_t>(direction)];
	if (!inBounds(destinationIndex)) {
		return OUT_OF_BOUNDS_INDEX;
	}

	return destinationIndex;
}

bool Cave::Map::isAdjacentTo(const int& index, const Cave::Entity::Trait& trait) const {
	for (int offset : m_adjacentOffsets) {
		int neighbourIndex = index + offset;
		if (inBounds(neighbourIndex) && hasTrait(trait, neighbourIndex)) {
			return true;
		}
	}
	return false;
}

bool Cave::Map::isAdjacentTo(const int& index, const Cave::Entity::Type& type) const {
	for (int offset : m_adjacentOffsets) {
		int neighbourIndex = index + offset;
		if (inBounds(neighbourIndex) && getEntityType(neighbourIndex) == type) {
			return true;
		}
	}
	return false;
}

bool Cave::Map::isHorizontalAdjacentTo(const int& index, const Cave::Entity::Type& type) const {
	for (int offset : m_horizontalOffsets) {
		int neighbourIndex = index + offset;
		if (inBounds(neighbourIndex) && getEntityType(neighbourIndex) == type) {
			return true;
		}
	}
	return false;
}

bool Cave::Map::isVerticalAdjacentTo(const int& index, const Cave::Entity::Type& type) const {
	for (int offset : m_verticalOffsets) {
		int neighbourIndex = index + offset;
		if (inBounds(neighbourIndex) && getEntityType(neighbourIndex) == type) {
			return true;
		}
	}
	return false;
}
