#pragma once
#include "Cave/Entity/Base.h"
#include "Utils/Counter.h"

namespace Cave::Entity {

    /**
     * @class Plasma
     * @brief Expands random in all directions when free space is availible.
     * The speed is level specific, but often quite fast so be careful when releasing it if you don't want to get trapped.
     */
    class Plasma : public Base {
    public:
        Plasma()
            : Base(Type::Plasma, Animation{ getFrames(), Utils::TickCounter(14) }) {
        }

    private:
        /**
         * @brief Provides the animation frames for the Plasma amination.
         *
         * The Plasma cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 152, 153, 154, 155, 156, 157, 158, 159, 158, 157, 156, 155, 154, 153 };
        }
    };
}