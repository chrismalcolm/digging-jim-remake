#pragma once
#include "Cave/Entity/Base.h"
#include "Utils/Random.h"

namespace Cave::Entity {

    /**
     * @class TubeCross
     * @brief There are two basic types of tubes: bi-directional and one-way.
     * Jim can move trough bi-directional tubes from both sides, while one-way tubes only allows him to enter from one side.
     * Only Jim can pass tubes.
     */
    class TubeCross : public Base {
    public:
        TubeCross()
            : Base(Type::TubeCross, Utils::randomInteger(122, 123)) {
            addTrait(Trait::Immutable);
            addTrait(Trait::Indestructible);
            addTrait(Trait::WarpableLeft);
            addTrait(Trait::WarpableRight);
            addTrait(Trait::WarpableUp);
            addTrait(Trait::WarpableDown);
        }
    };
}