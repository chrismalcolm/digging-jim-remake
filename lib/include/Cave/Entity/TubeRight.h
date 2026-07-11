#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class TubeRight
     * @brief There are two basic types of tubes: bi-directional and one-way.
     * Jim can move trough bi-directional tubes from both sides, while one-way tubes only allows him to enter from one side.
     * Only Jim can pass tubes.
     */
    class TubeRight : public Base {
    public:
        TubeRight()
            : Base(Type::TubeRight, wallTube()) {
            addTrait(Trait::Immutable);
            addTrait(Trait::Indestructible);
            addTrait(Trait::WarpableRight);
        }

        static const Animation& solidWallTube() {
            static Animation animation{ { 110 }, 0 };
            return animation;
        }

        static const Animation& wallTube() {
            static Animation animation{ { 111 }, 0 };
            return animation;
        }
    };
}