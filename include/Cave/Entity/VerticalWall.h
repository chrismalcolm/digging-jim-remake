#pragma once
#include "Cave/Entity/Base.h"
#include "Utils/Random.h"

namespace Cave::Entity {

    /**
     * @class VerticalWall
     * @brief There are two types of expanding wall: a horizontal and a vertical.
     * They look like normal brick wall, but will expand in horizontal or vertical directions if free space is availible.
     * Quite rare, but sometimes used for traps so watch out!
     */
    class VerticalWall : public Base {
    public:
        VerticalWall()
            : Base(Type::VerticalWall, Utils::randomInteger(88, 95)) {}
    };
}