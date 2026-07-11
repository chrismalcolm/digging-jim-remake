#pragma once
#include "Cave/Entity/Base.h"
#include "Utils/Random.h"

namespace Cave::Entity {

    /**
     * @class Boulder
     * @brief They fall down if there's no support under them and falls off unstable surfaces such as diamonds, other boulders and brick walls.
     * They are quite dangerous when falling down, but also useful for hitting enemies.
     * Jim can push boulders around with a little effort.
     */
    class Boulder : public Base {
    public:
        Boulder()
            : Base(Type::Boulder, Utils::randomInteger(84, 87)) {
            addTrait(Trait::Pushable);
            addTrait(Trait::Slippery);
        }
    };
}