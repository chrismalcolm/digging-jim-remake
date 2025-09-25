#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class StartDoorOpen
     * @brief Jim entering a level through the start door.
     */
    class StartDoorOpen : public Base {
    public:
        StartDoorOpen()
            : Base(Type::StartDoorOpen, Animation{ getFrames(), 0 }) {
            addTrait(Trait::Transient);
        }

    private:
        /**
         * @brief Provides the animation frames for the StartDoorOpen amination.
         *
         * The StartDoorOpen cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 160, 161, 162, 163, 164, 164, 165, 165, 166, 166, 167, 167 };
        }
    };
}