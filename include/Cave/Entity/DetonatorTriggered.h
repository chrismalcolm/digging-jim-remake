#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class DetonatorTriggered
     * @brief A detonator that has been triggered.
     */
    class DetonatorTriggered : public Base {
    public:
        DetonatorTriggered()
            : Base(Type::DetonatorTriggered, Animation{ getFrames(), 0 }) {
            addTrait(Trait::Transient);
        }

    private:
        /**
         * @brief Provides the animation frames for the DetonatorTriggered.
         *
         * The DetonatorTriggered cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 219, 220, 221 };
        }
    };
}