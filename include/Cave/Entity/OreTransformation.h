#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {
    
    /**
     * @class OreTransformation
     * @brief An ore that is transforming into a diamond.
     * Falls down like a boulder.
     */
    class OreTransformation : public Base {
    public:
        OreTransformation()
            : Base(Type::OreTransformation, Animation{ getFrames(), 0 }) {
            addTrait(Trait::Slippery);
            addTrait(Trait::Transient);
        }

    private:
        /**
         * @brief Provides the animation frames for the OreTransformation amination.
         *
         * The OreTransformation cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 37, 38, 39, 40 };
        }
    };
}
