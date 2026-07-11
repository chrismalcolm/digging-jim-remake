#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class ExitDoorOpening
     * @brief The diamond quota has been met, and the exit door is opening.
     */
    class ExitDoorOpening : public Base {
    public:
        ExitDoorOpening()
            : Base(Type::ExitDoorOpening, Animation{ getFrames(), 0 }) {
            addTrait(Trait::Transient);
        }

    private:
        /**
         * @brief Provides the animation frames for the ExitDoorOpening.
         *
         * The ExitDoorOpening cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 204, 205, 206, 207, 208, 209 ,210, 211, 212, 213 };
        }
    };
}