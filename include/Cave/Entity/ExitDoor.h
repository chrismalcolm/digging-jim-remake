#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class ExitDoor
     * @brief You can exit a level through this door and advance to the next cave when it's not locked.
     * It will unlock when Jim has enough diamonds.
     * Be careful not to include the exit door in explosions!
     */
    class ExitDoor : public Base {
    public:
        ExitDoor()
            : Base(Type::ExitDoor, 204) {}
    };
}