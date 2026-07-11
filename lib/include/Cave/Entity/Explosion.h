#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class Explosion
     * @brief An explosion will destroy all cave entities which are not indestructible.
     */
    class Explosion : public Base {
    public:
        Explosion()
            : Base(Type::Explosion, Animation{ getFrames(), 0 }) {
            addTrait(Trait::Transient);
        }

    private:
        /**
         * @brief Provides the animation frames for the Explosion.
         *
         * The Explosion cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 124, 125, 126, 127 };
        }
    };
}