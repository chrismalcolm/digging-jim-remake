#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class Ore
     * @brief Hit it hard with a boulder and a diamond will appear!
     * Falls down like a boulder.
     */
    class Ore : public Base {
    public:
        Ore()
            : Base(Type::Ore, 36) {
            addTrait(Trait::Pushable);
            addTrait(Trait::Slippery);
        }
    };
}
