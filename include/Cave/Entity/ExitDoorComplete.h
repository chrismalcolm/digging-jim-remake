#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class ExitDoorComplete
     * @brief Jim is exiting through the exit door, completing the cave.
     */
    class ExitDoorComplete : public Base {
    public:
        ExitDoorComplete()
            : Base(Type::ExitDoorComplete, Animation{ getFrames(), 0 }) {
            addTrait(Trait::Transient);
        }

    private:
        /**
         * @brief Provides the animation frames for the ExitDoorComplete.
         *
         * The ExitDoorComplete cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 214, 214, 215, 215, 216, 216, 217, 217 };
        }
    };
}