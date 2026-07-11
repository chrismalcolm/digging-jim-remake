#pragma once
#include <algorithm>
#include <optional>
#include <vector>
#include "Cave/Entity/Animation.h"
#include "Cave/Entity/Facing.h"
#include "Cave/Entity/Trait.h"
#include "Cave/Entity/Transition.h"
#include "Cave/Entity/Type.h"
#include "Cave/Entity/Direction.h"

namespace Cave::Entity {

    /**
     * @brief Base class for all cave entities.
     *
     * All game entities inherit from this class.
     */
    class Base {
    public:

        /// @brief Construct an empty entity with no type.
        Base() : m_type(Type::NoType) {}

        /**
         * @brief Construct an entity with a fixed texture.
         *
         * @param type Entity type.
         * @param textureIndex The texture index for the entity.
         */
        Base(Type type, int textureIndex)
            : m_type(type), m_animation(Animation({ textureIndex }, 0)) {
        }

        /**
         * @brief Construct an entity with a predefined animation sequence.
         *
         * @param type Entity type.
         * @param animation Animation sequence for this entity.
         */
        Base(Type type, Animation animation)
            : m_type(type), m_animation(std::move(animation)) {
        }

        /// @brief Virtual destructor for safe polymorphic use.
        virtual ~Base() = default;

        /// @brief Get the type of this entity.
        Type getType() const { return m_type; }

        // ---------------------------------------------------------------------
        // Traits
        // ---------------------------------------------------------------------

        /// @brief Add a trait to the entity (e.g., pushable, crushable).
        void addTrait(Trait trait) { m_traits |= trait; }

        /**
         * @brief Check if the entity has a specific trait.
         * @param trait The trait to check.
         * @return True if the entity has the trait, false otherwise.
         */
        bool hasTrait(const Trait& trait) const { return checkTrait(m_traits, trait); }

        // ---------------------------------------------------------------------
        // Animation
        // ---------------------------------------------------------------------

        /// @brief Get the current animation for the entity.
        Animation getAnimation() const { return m_animation; }

        /**
         * @brief Replace the entity's animation.
         * @param animation The new animation sequence.
         */
        void setAnimation(const Animation& animation) {
            if (m_animation != animation) {
                m_animation = animation;
            }
        }

        /// @brief Advance the animation and transition states by one step.
        void updateAnimation() {
            m_animation.update();
            if (m_transition) {
                m_transition->updateAnimation();
            }
        }

        /// @brief Check if the animation has looped at least once.
        bool animationLoopCompleted() const { return m_animation.loopCompleted; }

        /// @brief Get the texture index of the current animation frame.
        int getCurrentTextureIndex() const { return m_animation.getTextureIndex(); }

        /// @brief Get the texture index of the previous animation frame during a transition.
        int getPreviousTextureIndex() const {
            return m_transition ? m_transition->getTextureIndex() : NO_TEXTURE_INDEX;
        }

        // ---------------------------------------------------------------------
        // Transitions
        // ---------------------------------------------------------------------

        /**
         * @brief Apply an "away" transition in the given direction.
         * @param direction Direction of movement.
         * @param animation Animation to apply during transition.
         */
        void applyAwayTransition(const Cave::Entity::Direction& direction, Animation& animation) {
            if (direction == Cave::Entity::Direction::NO_DIRECTION) {
                return;
            }
            int i = static_cast<int>(direction);
            m_transition = Transition(awayTransitions[i], animation);
        }

        /**
         * @brief Apply a "push" transition in the given direction.
         * @param direction Direction of movement.
         * @param animation Animation to apply during transition.
         */
        void applyPushTransition(const Cave::Entity::Direction& direction, Animation& animation) {
            if (direction == Cave::Entity::Direction::NO_DIRECTION) {
                return;
            }
            int i = static_cast<int>(direction);
            m_transition = Transition(pushTransitions[i], animation);
        }

        /**
         * @brief Apply an "into" transition in the given direction.
         * @param direction Direction of movement.
         * @param animation Animation to apply during transition.
         */
        void applyIntoTransition(const Cave::Entity::Direction& direction, Animation& animation) {
            if (direction == Cave::Entity::Direction::NO_DIRECTION) {
                return;
            }
            int i = static_cast<int>(direction);
            m_transition = Transition(intoTransitions[i], animation);
        }

        /**
         * @brief Reapply transition type with a new animation.
         * @param animation The animation to use for the transition.
         */
        void applyTransitionAmination(Animation& animation) {
            if (!m_transition) {
                return;
            }
            m_transition = Transition(m_transition->getType(), animation);
        }

        /**
         * @brief Transfer this entity�s transition state to another entity.
         * @param entity The target entity to receive the transition.
         */
        void transferTransition(Base& entity) {
            if (!m_transition) {
                return;
            }
            entity.m_transition = std::move(m_transition);
        }

        /// @brief Terminate the current transition unless it is an "away" transition.
        void terminateCurrentTransition() {
            if (!m_transition || m_transition->isAway()) {
                return;
            }
            if (m_transition->isPush()) {
                m_transition->changeToAway();
                return;
            }
            m_transition = std::nullopt;
        }

        /// @brief Terminate the previous transition unless it is an "into" transition.
        void terminatePreviousTransition() {
            if (!m_transition || m_transition->isInto()) {
                return;
            }
            if (m_transition->isPush()) {
                m_transition->changeToInto();

                return;
            }
            m_transition = std::nullopt;
        }

        /// @brief Get the direction of the previous transition.
        Cave::Entity::Direction getPreviousDirection() {
            if (!m_transition) {
                return Cave::Entity::Direction::NO_DIRECTION;
            }
            return m_transition->getPreviousDirection();
        }

        /// @brief Update displacement and handle transition completion.
        void updateTransition() {
            if (m_transition) {
                if (m_transition->isFinished()) {
                    m_transition = std::nullopt;
                }
                else {
                    m_transition->updateDisplacement();
                }
            }
        }

        /// @brief Check if the entity is currently in transition.
        bool isTransitioning() const {
            return m_transition.has_value();
        }

        // ---------------------------------------------------------------------
        // Rendering helpers
        // ---------------------------------------------------------------------

        /**
         * @brief Get the current animation screen-space position for rendering.
         * @return Rectangle in screen coordinates.
         */
        sf::IntRect getCurrentPosition() const {
            if (!m_transition) {
                return { { 0, 0 }, { 32, 32 } };
            }
            if (m_transition->isCurrent()) {
                return m_transition->getMovingPosition();
            }
            return m_transition->getStaticPosition();
        }

        /**
        * @brief Get the previous transition screen-space position for rendering.
        * @return Rectangle in screen coordinates.
        */
        sf::IntRect getPreviousPosition() const {
            if (!m_transition) {
                return { { 0, 0 }, { 32, 32 } };
            }
            if (m_transition->isCurrent()) {
                return m_transition->getStaticPosition();
            }
            return m_transition->getMovingPosition();
        }

        /**
         * @brief Get texture coordinates of the current animation frame.
         * @return Rectangle of texture coordinates.
         */
        sf::IntRect getCurrentTextureCoords() const {
            if (!m_transition) {
                return { { 0, 0 }, { 32, 32 } };
            }
            if (m_transition->isCurrent()) {
                return m_transition->getMovingTextureCoords();
            }
            return m_transition->getStaticTextureCoords();
        }

        /**
         * @brief Get texture coordinates of the previous transition animation frame.
         * @return Rectangle of texture coordinates.
         */
        sf::IntRect getPreviousTextureCoords() const {
            if (!m_transition) {
                return { { 0, 0 }, { 32, 32 } };
            }
            if (m_transition->isCurrent()) {
                return m_transition->getStaticTextureCoords();
            }
            return m_transition->getMovingTextureCoords();
        }

        // ---------------------------------------------------------------------
        // Public state flags
        // ---------------------------------------------------------------------

        /// @brief Flag to be used if the entity is falling
        bool falling = false;

        /// @brief Flag to be used if the entity is currently moving.
        bool moving = false;

        /// @brief Internal flag to prevent reprocessing.
        bool processed = true;

        /// @brief Current facing direction.
        Facing facing = Facing::NEUTRAL;

        /// @brief Combined traits of the entity.
        Trait m_traits = Trait::None;

        /// @brief Last movement direction.
        Cave::Entity::Direction direction = Cave::Entity::Direction::NO_DIRECTION;

    protected:

        /// @brief Entity type identifier.
        Type m_type;

        /// @brief Current animation.
        Animation m_animation;

        /// @brief Optional current transition.
        std::optional<Transition> m_transition;
    };

} // namespace Cave::Entity
