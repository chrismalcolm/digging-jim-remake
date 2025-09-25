#include "Cave/Map/Map.h"

sf::Vector2f Cave::Map::getCameraStartLocation() const {
	if (m_startDoorIndex == OUT_OF_BOUNDS_INDEX) {
		return { 0, 0 };
	}
	if (m_startDoorIndex < width * height / 2) {
		return { static_cast<float>(width * 32), static_cast<float>(height * 32) };
	}
	return { 0.0, 0.0 };
}

sf::Vector2f Cave::Map::getCameraBounds() const {
	// Use + 64 to make room for level HUD at the bottom of the screen.
	return { static_cast<float>(width * 32), static_cast<float>(height * 32 + 64) };
}

sf::Vector2f Cave::Map::updateCameraLocation(Camera& camera, const sf::Vector2f& offset) {

	sf::Vector2f location = camera.getCenter();

	int targetX = (m_jimIndex % width) * 32 + offset.x;
	int targetY = (m_jimIndex / width) * 32 + offset.y + 32 / 2;

	if (location.x < targetX) location.x += m_cameraSpeed;
	if (location.x > targetX) location.x -= m_cameraSpeed;
	if (location.y < targetY) location.y += m_cameraSpeed;
	if (location.y > targetY) location.y -= m_cameraSpeed;

	camera.setCentre(location);

	return { static_cast<float>(targetX), static_cast<float>(targetY) };
}

bool Cave::Map::resetCameraPosition() {
	if (!m_resetCameraPosition) {
		m_resetCameraPosition = true;
		return false;
	}
	return true;
}

bool Cave::Map::requiresReset() {
	if (m_reset) {
		m_reset = false;
		return true;
	}
	return false;
}

void Cave::Map::markForReset() {
	m_reset = true;
	m_resetCameraPosition = true;
}