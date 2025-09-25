#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class TubeLeft
     * @brief There are two basic types of tubes: bi-directional and one-way.
     * Jim can move trough bi-directional tubes from both sides, while one-way tubes only allows him to enter from one side.
     * Only Jim can pass tubes.
     */
    class TubeLeft : public Base {
    public:
        TubeLeft()
            : Base(Type::TubeLeft, wallTube()) {
            addTrait(Trait::Immutable);
            addTrait(Trait::Indestructible);
            addTrait(Trait::WarpableLeft);
        }

        static const Animation& solidWallTube() {
            static Animation animation{ { 112 }, 0 };
            return animation;
        }

        static const Animation& wallTube() {
            static Animation animation{ { 113 }, 0 };
            return animation;
        }
    };
}