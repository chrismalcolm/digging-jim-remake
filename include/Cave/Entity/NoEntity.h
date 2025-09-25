#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class NoEntity
     * @brief Represents no entity.
     */
    class NoEntity : public Base {
    public:
        NoEntity()
            : Base() {}
    };
}