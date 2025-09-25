#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class MagicWallInactive
     * @brief When hit by a boulder or diamond it will activate and start glowing.
     */
    class MagicWallInactive : public Base {
    public:
        MagicWallInactive()
            : Base(Type::MagicWallInactive, 88) {}
    };
}