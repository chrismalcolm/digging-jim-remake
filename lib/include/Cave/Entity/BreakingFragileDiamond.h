#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class BreakingFragileDiamond
     * @brief A fragile diamond that has been shattered.
     */
    class BreakingFragileDiamond : public Base {
    public:
        BreakingFragileDiamond()
            : Base(Type::BreakingFragileDiamond, Animation{ getFrames(), 0 }) {
            addTrait(Trait::Transient);
        }

    private:
        /**
         * @brief Provides the animation frames for the BreakingFragileDiamond.
         *
         * The BreakingFragileDiamond cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 32, 33, 34, 35 };
        }
    };
}