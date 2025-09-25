#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class DetonatorUsed
     * @brief A detonator that has been used.
     */
    class DetonatorUsed : public Base {
    public:
        DetonatorUsed()
            : Base(Type::DetonatorUsed, 222) {}
    };
}