#pragma once
#include "Cave/Entity/Base.h"
#include "Utils/Random.h"

namespace Cave::Entity {

    /**
     * @class Diamond
     * @brief Jim's goal is to get as much of these as possible.
     * He has to collect a certain amount to complete a cave.
     * Diamonds behaves much in the same way as boulders, so don't get hit on the head by them.
     */
    class Diamond : public Base {
    public:
        Diamond()
            : Base(Type::Diamond, Animation{ getFrames(), Utils::randomInteger(0, 7) }) {
            addTrait(Trait::Collectable);
            addTrait(Trait::Slippery);
            addTrait(Trait::Traversable);
        }

    private:
        /**
         * @brief Provides the animation frames for the Diamond.
         *
         * The Diamond cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 16, 17, 18, 19, 20, 21, 22, 23 };
        }
    };
}