#pragma once
#include "Cave/Entity/Base.h"
#include "Utils/Random.h"

namespace Cave::Entity {

    /**
     * @class Wall
     * @briefNormal brick wall.
     * Can be destroyed by explosions.
     */
    class Wall : public Base {
    public:
        Wall()
            : Base(Type::Wall, Utils::randomInteger(88, 95)) {
            addTrait(Trait::Immutable);
            addTrait(Trait::Slippery);
        }
    };
}