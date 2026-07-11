#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class Bomb
     * @brief These bombs are extremely easy to set off!
     * They'll go off if other objects hit them or if they fall down and hits other objects.
     */
    class Bomb : public Base {
    public:
        Bomb()
            : Base(Type::Bomb, Animation{ getFrames(), 0 }) {
            addTrait(Trait::Crushable);
            addTrait(Trait::Pushable);
            addTrait(Trait::Slippery);
        }

    private:
        /**
         * @brief Provides the animation frames for the Bomb.
         *
         * The Bomb cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 136, 137, 138, 139, 140, 141, 142, 143 };
        }
    };
}