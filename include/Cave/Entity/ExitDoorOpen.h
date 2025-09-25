#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class ExitDoorOpen
     * @brief The diamond quota has been met, and the exit door is open.
     */
    class ExitDoorOpen : public Base {
    public:
        ExitDoorOpen()
            : Base(Type::ExitDoorOpen, 213) {
        }
    };
}