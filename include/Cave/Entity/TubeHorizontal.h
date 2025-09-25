#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class TubeHorizontal
     * @brief There are two basic types of tubes: bi-directional and one-way.
     * Jim can move trough bi-directional tubes from both sides, while one-way tubes only allows him to enter from one side.
     * Only Jim can pass tubes.
     */
    class TubeHorizontal : public Base {
    public:
        TubeHorizontal()
            : Base(Type::TubeHorizontal, wallTube()) {
            addTrait(Trait::Immutable);
            addTrait(Trait::Indestructible);
            addTrait(Trait::WarpableLeft);
            addTrait(Trait::WarpableRight);
        }

        static const Animation& solidWallTube() {
            static Animation animation{ { 118 }, 0 };
            return animation;
        }

        static const Animation& wallTube() {
            static Animation animation{ { 119 }, 0 };
            return animation;
        }
    };
}
