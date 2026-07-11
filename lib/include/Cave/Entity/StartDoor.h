#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class StartDoor
     * @brief Jim enters a level through the start door.
     */
    class StartDoor : public Base {
    public:
        StartDoor()
            : Base(Type::StartDoor, 160) {}
    };
}