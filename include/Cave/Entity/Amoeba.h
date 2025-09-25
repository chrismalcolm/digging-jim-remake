#pragma once
#include "Cave/Entity/Base.h"
#include "Utils/Counter.h"

namespace Cave::Entity {

    /**
     * @class Amoeba
     * @brief Expands random in all directions through dirt and free space at a cave specific speed.
     * Amoeba will kill all creatures except Jim and is therefore often useful for killing large quatities of baddies!
     * Amoeba turns into boulders when it reaches a certain cave specific size.
     * If Jim succeed in enclosing all amoeba in a cave and by this makes it impossible to grow futher it will transform into diamonds!
     */
    class Amoeba : public Base {
    public:
        Amoeba()
            : Base(Type::Amoeba, Animation{ getFrames(), Utils::TickCounter(8) }) {
            addTrait(Trait::Reactive);
        }

    private:
        /**
         * @brief Provides the animation frames for the Amoeba.
         *
         * The Amoeba cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 144, 145, 146, 147, 148, 149, 150 ,151 };
        }
    };
}