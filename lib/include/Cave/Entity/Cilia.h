#pragma once
#include "Cave/Entity/Base.h"
#include "Cave/Entity/Direction.h"
#include "Utils/Random.h"

namespace Cave::Entity {

    /**
     * @class Cilia
     * @brief This annoying critter will move in straight lines and pick random directions when obstacles are encountered.
     */
    class Cilia : public Base {
    public:
        Cilia()
            : Base(Type::Cilia, Animation{ getFrames(), Utils::randomInteger(0, 7) }) {
            addTrait(Trait::Crushable);
            direction = Cave::Entity::getRandomDirection();
        }

    private:
        /**
         * @brief Provides the animation frames for the Cilia.
         *
         * The Cilia cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 56, 57, 58, 59, 60, 61, 62, 63 };
        }
    };
}
