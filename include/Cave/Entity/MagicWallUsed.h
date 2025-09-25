#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class MagicWallUsed
     * @brief It will dissolve boulders and diamonds.
     */
    class MagicWallUsed : public Base {
    public:
        MagicWallUsed()
            : Base(Type::MagicWallUsed, 88) {
            addTrait(Trait::Immutable);
        }
    };
}