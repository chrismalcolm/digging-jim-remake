#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {
    /**
     * @class ExitDoorFinished
     * @brief The exit door when the cave has been completed.
     */
    class ExitDoorFinished : public Base {
    public:
        ExitDoorFinished()
            : Base(Type::ExitDoorFinished, 217) {
        }
    };
}