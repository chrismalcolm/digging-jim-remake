#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class TubeUp
     * @brief There are two basic types of tubes: bi-directional and one-way.
     * Jim can move trough bi-directional tubes from both sides, while one-way tubes only allows him to enter from one side.
     * Only Jim can pass tubes.
     */
    class TubeUp : public Base {
    public:
        TubeUp()
            : Base(Type::TubeUp, wallTube()) {
            addTrait(Trait::Immutable);
            addTrait(Trait::Indestructible);
            addTrait(Trait::WarpableUp);
        }

        static const Animation& solidWallTube() {
            static Animation animation{ { 116 }, 0 };
            return animation;
        }

        static const Animation& wallTube() {
            static Animation animation{ { 117 }, 0 };
            return animation;
        }
    };
}