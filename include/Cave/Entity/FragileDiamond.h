#pragma once
#include "Cave/Entity/Base.h"
#include "Utils/Random.h"

namespace Cave::Entity {

    /**
     * @class FragileDiamond
     * @brief These are as valuable as normal diamonds, but shatter if they fall down or is hit by falling objects.
     */
    class FragileDiamond : public Base {
    public:
        FragileDiamond()
            : Base(Type::FragileDiamond, Animation{ getFrames(), Utils::randomInteger(0, 7) }) {
            addTrait(Trait::Collectable);
            addTrait(Trait::Slippery);
            addTrait(Trait::Traversable);
        }

    private:
        /**
         * @brief Provides the animation frames for the FragileDiamond.
         *
         * The FragileDiamond cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 24, 25, 26, 27, 28, 29, 30, 31 };
        }
    };
}