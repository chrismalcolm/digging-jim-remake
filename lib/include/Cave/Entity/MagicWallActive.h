#pragma once
#include "Cave/Entity/Base.h"
#include "Utils/Counter.h"

namespace Cave::Entity {

    /**
     * @class MagicWallActive
     * @brief Looks like a normal brick wall in it's inactive state, except that it seems more perfect.
     * When hit by a boulder or diamond it will activate and start glowing.
     * If there is free space availible under it then boulders that hit will be changed to diamonds and visa versa.
     * The wall is active for a level specific timelapse and will eventually go back to its inactive state where it will dissolve boulders and diamonds.
     */
    class MagicWallActive : public Base {
    public:
        MagicWallActive()
            : Base(Type::MagicWallActive, Animation{ getFrames(), Utils::TickCounter(6) }) {
        }

    private:
        /**
         * @brief Provides the animation frames for the MagicWallActive amination.
         *
         * The MagicWallActive cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> getFrames() {
            return { 96, 97, 98, 99, 100, 101 };
        }
    };
}