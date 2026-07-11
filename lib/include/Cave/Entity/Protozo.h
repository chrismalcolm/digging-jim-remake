#pragma once
#include "Cave/Entity/Base.h"
#include "Cave/Entity/Direction.h"
#include "Utils/Random.h"

namespace Cave::Entity {

    /**
     * @class Protozo
     * @brief Quite common critter in the caves.
     * They move around through free space and will turn left if possible.&nbsp;
     * If one of them comes in contact with Jim it will explode and kill him at the spot. 
     * When hit by a falling object they'll also explode and this is often useful for removing brick walls and other obstacles.
     * Amoeba is highly dangerous for Protozos too.
     */
    class Protozo : public Base {
    public:
        Protozo()
            : Base(Type::Protozo, Animation{ getFrames(), Utils::randomInteger(0, 7) }) {
            addTrait(Trait::Crushable);
            direction = Cave::Entity::getRandomDirection();
        }

    private:
        /**
         * @brief Provides the animation frames for the Protozo amination.
         *
         * The Protozo cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 48, 49, 50, 51, 52, 53, 54, 55 };
        }
    };
}