#include "Cave/Manager/Data.h"

Cave::Entity::Base Cave::Data::getTileEntity(const char& tile) {
	switch (tile) {
	case 0:  return Cave::Entity::Space();
	case 1:  return Cave::Entity::Dirt();
	case 2:  return Cave::Entity::Boulder();
	case 3:  return Cave::Entity::Diamond();
	case 4:  return Cave::Entity::Wall();
	case 5:  return Cave::Entity::SolidWall();
	case 7:  return Cave::Entity::Protozo();
	case 8:  return Cave::Entity::CaveGull();
	case 9:  return Cave::Entity::Amoeba();
	case 12: return Cave::Entity::MagicWallInactive();
	case 13: return Cave::Entity::StartDoor();
	case 14: return Cave::Entity::ExitDoor();
	case 15: return Cave::Entity::HorizontalWall();
	case 16: return Cave::Entity::Detonator();
	case 17: return Cave::Entity::TNT();
	case 18: return Cave::Entity::Bomb();
	case 19: return Cave::Entity::TubeHorizontal();
	case 20: return Cave::Entity::TubeVertical();
	case 21: return Cave::Entity::TubeCross();
	case 22: return Cave::Entity::TubeRight();
	case 23: return Cave::Entity::TubeLeft();
	case 24: return Cave::Entity::TubeUp();
	case 25: return Cave::Entity::TubeDown();
	case 26: return Cave::Entity::FragileDiamond();
	case 27: return Cave::Entity::Eater();
	case 28: return Cave::Entity::Aggressor();
	case 30: return Cave::Entity::VerticalWall();
	case 31: return Cave::Entity::Plasma();
	case 32: return Cave::Entity::Cilia();
	case 33: return Cave::Entity::Ore();
	default:
		return Cave::Entity::Space();
	}
}

bool Cave::Data::isStartDoor(const char& tile) {
	return tile == 13;
}