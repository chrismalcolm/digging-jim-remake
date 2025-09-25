#pragma once
#include "Cave/Entity/Base.h"
#include "Cave/Entity/Direction.h"
#include "Utils/Random.h"

namespace Cave::Entity {

    /**
     * @class CaveGull
     * @brief Hit these creatures with falling objects or bring them into contact with amoeba and you won't regret it!
     * Their explosion will result in 9 diamonds, but don't forget that they still would like to include Jim in that explosion and are as deadly as protozos.
     * They turn right if possible.
     */
    class CaveGull : public Base {
    public:
        CaveGull()
            : Base(Type::CaveGull, Animation{ getFrames(), Utils::randomInteger(0, 7) }) {
            addTrait(Trait::Crushable);
            direction = Cave::Entity::getRandomDirection();
        }

    private:
        /**
         * @brief Provides the animation frames for the CaveGull.
         *
         * The CaveGull cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 72, 73, 74, 75, 76, 77, 78, 79 };
        }
    };
}