#pragma once
#include "Cave/Entity/Base.h"
#include "Utils/Random.h"

namespace Cave::Entity {

    /**
     * @class SolidWall
     * @brief Reinforced wall.
     * Cannot be destroyed in any way.
     */
    class SolidWall : public Base {
    public:
        SolidWall()
            : Base(Type::SolidWall, Utils::randomInteger(102, 109)) {
            addTrait(Trait::Immutable);
            addTrait(Trait::Indestructible);
        }
    };
}