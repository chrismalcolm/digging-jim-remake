#pragma once
#include "Cave/Entity/Base.h"
#include "Utils/Random.h"

namespace Cave::Entity {

    /**
     * @class Aggressor
     * @brief The horror of every miner! They are very rare, but their accute senses will help them to hunt Jim.
     * Luckily they are not very clever and you should be able to outsmart them by hiding behind complex obstacles.
     */
    class Aggressor : public Base {
    public:
        Aggressor()
            : Base(Type::Aggressor, Animation{ getFrames(), Utils::randomInteger(0, 7) }) {
            addTrait(Trait::Crushable);
        }

    private:
        /**
         * @brief Provides the animation frames for the Aggressor.
         *
         * The Aggressor cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 64, 65, 66, 67, 68, 69, 70, 71 };
        }
    };

}
