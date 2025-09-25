#pragma once
#include "Cave/Entity/Base.h"
#include "Cave/Entity/Direction.h"
#include "Utils/Random.h"

namespace Cave::Entity {

    /**
     * @class Eater
     * @brief Not only will these nasty two headed buggers kill Jim if they run into him, they also gourge themselves in diamonds.
     * Make sure to get rid of them if possible. They turn right if free space is availible.
     */
    class Eater : public Base {
    public:
        Eater()
            : Base(Type::Eater, Animation{ getFrames(), Utils::randomInteger(0, 6) }) {
            addTrait(Trait::Crushable);
            direction = Cave::Entity::getRandomDirection();
        }

    private:
        /**
         * @brief Provides the animation frames for the Eater.
         *
         * The Eater cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 41, 42, 43, 44, 45, 46, 47 };
        }
    };
}