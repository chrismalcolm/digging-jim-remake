#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class Jim
     * @brief In this game, you control a creature called 'Jim' into the subterranean caves of his home planet Akturien.
     */
    class Jim : public Base {
    public:
        Jim()
            : Base(Type::Jim, idleAnimation()) {
            addTrait(Trait::Crushable);
            addTrait(Trait::Reactive);
        }

        /// @brief Jim's idle animation
        /// @return Animation for Jim's idle animation.
        static const Animation& idleAnimation() {
            static Animation animation{ idleFrames(), 0 };
            return animation;
        }

        /// @brief Jim's blink animation
        /// @return Animation for Jim's blink animation.
        static const Animation& blinkAnimation() {
            static Animation animation{ blinkFrames(), 0 };
            return animation;
        }

        /// @brief Jim's move left animation
        /// @return Animation for Jim's move left animation.
        static const Animation& moveLeftAnimation() {
            static Animation animation{ moveLeftFrames(), 0 };
            return animation;
        }

        /// @brief Jim's move right animation
        /// @return Animation for Jim's move right animation.
        static const Animation& moveRightAnimation() {
            static Animation animation{ moveRightFrames(), 0 };
            return animation;
        }

        /// @brief Jim's push left animation
        /// @return Animation for Jim's push left animation.
        static const Animation& pushLeftAnimation() {
            static Animation animation{ pushLeftFrames(), 0 };
            return animation;
        }

        /// @brief Jim's push right animation
        /// @return Animation for Jim's push right animation.
        static const Animation& pushRightAnimation() {
            static Animation animation{ pushRightFrames(), 0 };
            return animation;
        }

    private:
        /**
         * @brief Provides the animation frames for the Jim's idle amination.
         *
         * Jim cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> idleFrames() {
            return { 168, 169, 170, 171, 172, 173 };
        }

        /**
         * @brief Provides the animation frames for the Jim's blink amination.
         *
         * Jim cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> blinkFrames() {
            return { 174, 175 };
        }

        /**
         * @brief Provides the animation frames for the Jim's move left amination.
         *
         * Jim cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> moveLeftFrames() {
            return { 184, 185, 186, 187, 188, 189, 190, 191 };
        }

        /**
         * @brief Provides the animation frames for the Jim's move right amination.
         *
         * Jim cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> moveRightFrames() {
            return { 176, 177, 178, 179, 180, 181, 182, 183 };
        }

        /**
         * @brief Provides the animation frames for the Jim's push left amination.
         *
         * Jim cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> pushLeftFrames() {
            return { 198, 199, 200, 201, 202, 203 };
        }

        /**
         * @brief Provides the animation frames for the Jim's push right amination.
         *
         * Jim cycles through a fixed set of sprite frames.
         *
         * @return A vector of integer frame indices used for animation.
         */
        static std::vector<int> pushRightFrames() {
            return { 192, 193, 194, 195, 196, 197 };
        }
    };
}