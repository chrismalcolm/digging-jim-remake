#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class Detonator
     * @brief When a detonator is activated all TNT boxes in the cave will explode!
     */
    class Detonator : public Base {
    public:
        Detonator()
            : Base(Type::Detonator, 218) {}
    };
}