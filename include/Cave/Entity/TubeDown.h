#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class TubeDown
     * @brief There are two basic types of tubes: bi-directional and one-way.
     * Jim can move trough bi-directional tubes from both sides, while one-way tubes only allows him to enter from one side.
     * Only Jim can pass tubes.
     */
    class TubeDown : public Base {
    public:
        TubeDown()
            : Base(Type::TubeDown, wallTube()) {
            addTrait(Trait::Immutable);
            addTrait(Trait::Indestructible);
            addTrait(Trait::WarpableDown);
        }

        static const Animation& solidWallTube() {
            static Animation animation{ { 114 }, 0 };
            return animation;
        }

        static const Animation& wallTube() {
            static Animation animation{ { 115 }, 0 };
            return animation;
        }
    };
}