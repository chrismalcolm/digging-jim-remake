#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class CaveGullExplosion
     * @brief The explosion of a cave gull, resulting in a diamond.
     */
    class CaveGullExplosion : public Base {
    public:
        CaveGullExplosion()
            : Base(Type::CaveGullExplosion, Animation{ getFrames(), 0 }) {
            addTrait(Trait::Transient);
        }

    private:
        /**
         * @brief Provides the animation frames for the CaveGullExplosion.
         *
         * The CaveGullExplosion cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 80, 81, 82, 83 };
        }
    };
}