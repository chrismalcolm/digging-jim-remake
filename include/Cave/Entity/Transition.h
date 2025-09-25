#pragma once
#include <SFML/Graphics.hpp>
#include "Cave/Entity/Animation.h"
#include "Cave/Entity/Direction.h"

namespace Cave::Entity {

	/**
	 * @brief Defines the type of transition an entity can perform.
	 *
	 * Transitions describe how an entity moves relative to another entity (referred to as the previous)
	 * such as moving away, moving into, or pushing in a specific direction.
	 * 
	 * @details How does this game engine handle transitions?
	 * 
	 * A transition occurs whenever a cave entity moves to a different tile location.
	 * What is important to understand about these transitions is that the entities don't actually move at all!
	 * Each tile will continur to draw in the same 32x32 location, but will draw two entities at the same time
	 * give the illusion of movement.
	 * This movement can be in any four of the cardinal directions, LEFT, RIGHT, UP or DOWN.
	 * E.g. when Jim traverses throguh dirt, when Jim pushes a boulder.
	 * 
	 * There are two main types of transitition that can occur.
	 * 
	 * The first type is more standard, when the entity at location A is being moved to the location B.
	 * 
	 * +-------+-------+
	 * |       |       |
	 * |   A  -->  B   |
	 * |       |       |
	 * +-------+-------+
	 * 
	 * At the start of this transition, the actual entity is std::move'd to B.
	 * The tile at A becomes a Space entity, but with a AWAY transition.
	 * This away transition as the same animation as whatever entity was at A.
	 * This animation will start off as the full 32x32 tile, and will slowly crop and move to the right until it disappears.
	 * In this AWAY transition, the Sapce is referred to as the current, and the other is reffered to as the previous.
	 * The tile at B now becomes what was at A, by the std::move.
	 * This tile now undergoes a INTO transition, which has the same animation of whatever was at B previously.
	 * This animation will start off as the full 32x32 tile, and will slowly be cropped to the right as the incoming entity is revealed.
	 * In this INTO transition, the entity moved from A is called the current and the other is reffered to as the previous
	 * 
	 * The second type can be labelled as the push transition; when an entity is push by C from D to E.
	 * +-------+-------+-------+
	 * |       |       |       |
	 * |   C  -->  D  -->  E   |
	 * |       |       |       |
	 * +-------+-------+-------+
	 * 
	 * At the start of this transition, the actual entity at D is std::move'd to E and likewise C is std::move'd to D.
	 * The tile at C undergoes the AWAY transition and the tile at E undergoes the INTO transition.
	 * The only change is that the tile at D undergoes a PUSH transition.
	 * For example, if Jim is at C and pushing a boulder at D into empty space at E,
	 * C is the AWAY transition, with the current as Space and the previous as Jim,
	 * E is the INTO transition, with the current as the boulder and the previous as the space
	 * D is the PUSH transition, with the current as Jim and the previous as the boulder.
	 * The push transition will start with the previous in the 32x32 tile, but will move and crop the previous until is disappears.
	 * In its place, the current will also move and become uncropped, until it is fully visible.
	 */
	enum class TransitionType {
		MOVE_LEFT_AWAY,
		MOVE_LEFT_INTO,
		MOVE_LEFT_PUSH,
		MOVE_RIGHT_AWAY,
		MOVE_RIGHT_INTO,
		MOVE_RIGHT_PUSH,
		MOVE_UP_AWAY,
		MOVE_UP_INTO,
		MOVE_UP_PUSH,
		MOVE_DOWN_AWAY,
		MOVE_DOWN_INTO,
		MOVE_DOWN_PUSH,
	};

	/**
	  * @brief Represents a transition state for an entity.
      *
      * A transition controls the displacement and animation
	  * when an entity moves, pushes, or shifts between tiles.
	  */
	struct Transition {

		Transition(TransitionType type, Animation animation) : m_type(type), m_animation(animation) {};

		/// @brief Get the transition type.
		/// @return TransitionType the transition type.
		TransitionType getType() {
			return m_type;
		}

		/// @brief Is the transition from a current entity.
		/// @return bool Whether the transition is from the current entity.
		bool isCurrent() const {
			return !isAway();
		}

		/// @brief If the transition is away.
		/// @return bool Whether the transition is an away transition.
		bool isAway() const {
			return (
				m_type == TransitionType::MOVE_LEFT_AWAY ||
				m_type == TransitionType::MOVE_RIGHT_AWAY ||
				m_type == TransitionType::MOVE_UP_AWAY ||
				m_type == TransitionType::MOVE_DOWN_AWAY
				);
		}

		/// @brief If the transition is push.
		/// @return bool Whether the transition is a push transition.
		bool isPush() const {
			return (
				m_type == TransitionType::MOVE_LEFT_PUSH ||
				m_type == TransitionType::MOVE_RIGHT_PUSH ||
				m_type == TransitionType::MOVE_UP_PUSH ||
				m_type == TransitionType::MOVE_DOWN_PUSH
				);
		}

		/// @brief If the transition is into.
		/// @return bool Whether the transition is an into transition.
		bool isInto() const {
			return (
				m_type == TransitionType::MOVE_LEFT_INTO ||
				m_type == TransitionType::MOVE_RIGHT_INTO ||
				m_type == TransitionType::MOVE_UP_INTO ||
				m_type == TransitionType::MOVE_DOWN_INTO
				);
		}

		/// @brief Run the update for this transition animation.
		void updateAnimation() {
			m_animation.update();
		}

		/// @brief Update the displacement for this transition.
		void updateDisplacement() {
			if (m_displacement < 32) {
				m_displacement += m_inc;
			}
		}

		/// @brief Whether this transition is finished.
		bool isFinished() {
			return (m_displacement >= 32);
		}

		/// @brief Retrieves the current animation texture index.
		int getTextureIndex() const {
			return m_animation.getTextureIndex();
		}

		/// @brief Get the position for the moving entity.
		sf::IntRect getMovingPosition() const {
			switch (m_type) {
			case TransitionType::MOVE_LEFT_AWAY:     return { { 0, 0 }, { 32 - m_displacement, 32 } };
			case TransitionType::MOVE_LEFT_INTO:     return { { 32 - m_displacement, 0 }, { m_displacement, 32 } };
			case TransitionType::MOVE_LEFT_PUSH:     return { { 32 - m_displacement, 0 }, { m_displacement, 32 } };
			case TransitionType::MOVE_RIGHT_AWAY:    return { { m_displacement, 0 }, { 32 - m_displacement, 32 } };
			case TransitionType::MOVE_RIGHT_INTO:    return { { 0, 0 }, { m_displacement, 32 } };
			case TransitionType::MOVE_RIGHT_PUSH:    return { { 0, 0 }, { m_displacement, 32 } };

			case TransitionType::MOVE_UP_AWAY:       return { { 0, 0 }, { 32, 32 - m_displacement } };
			case TransitionType::MOVE_UP_INTO:       return { { 0, 32 - m_displacement }, { 32, m_displacement } };
			case TransitionType::MOVE_UP_PUSH:       return { { 0, 32 - m_displacement }, { 32, m_displacement } };
			case TransitionType::MOVE_DOWN_AWAY:     return { { 0, m_displacement }, { 32, 32 - m_displacement } };
			case TransitionType::MOVE_DOWN_INTO:     return { { 0, 0 }, { 32, m_displacement } };
			case TransitionType::MOVE_DOWN_PUSH:     return { { 0, 0 }, { 32, m_displacement } };
			}
			return { { 0, 0 }, { 0, 0 } };
		}

		/// @brief Get the position for the static entity.
		sf::IntRect getStaticPosition() const {
			switch (m_type) {
			case TransitionType::MOVE_LEFT_AWAY:     return { { 32 - m_displacement, 0 }, { m_displacement, 32 } };
			case TransitionType::MOVE_LEFT_INTO:     return { { 0, 0 }, { 32 - m_displacement, 32 } };
			case TransitionType::MOVE_LEFT_PUSH:     return { { 0, 0 }, { 32 - m_displacement, 32 } };
			case TransitionType::MOVE_RIGHT_AWAY:    return { { 0, 0 }, { m_displacement, 32 } };
			case TransitionType::MOVE_RIGHT_INTO:    return { { m_displacement, 0 }, { 32 - m_displacement, 32 } };
			case TransitionType::MOVE_RIGHT_PUSH:    return { { m_displacement, 0 }, { 32 - m_displacement, 32 } };

			case TransitionType::MOVE_UP_AWAY:       return { { 0, 32 - m_displacement }, { 32, m_displacement } };
			case TransitionType::MOVE_UP_INTO:       return { { 0, 0 }, { 32, 32 - m_displacement } };
			case TransitionType::MOVE_UP_PUSH:       return { { 0, 0 }, { 32, 32 - m_displacement } };
			case TransitionType::MOVE_DOWN_AWAY:     return { { 0, 0 }, { 32, m_displacement } };
			case TransitionType::MOVE_DOWN_INTO:     return { { 0, m_displacement }, { 32, 32 - m_displacement } };
			case TransitionType::MOVE_DOWN_PUSH:     return { { 0, m_displacement }, { 32, 32 - m_displacement } };
			}
			return { { 0, 0 }, { 0, 0 } };
		}

		/// @brief Get the texture co-ordinates for the moving entity.
		sf::IntRect getMovingTextureCoords() const {
			switch (m_type) {
			case TransitionType::MOVE_LEFT_AWAY:     return { { m_displacement, 0 }, { 32 - m_displacement, 32 } };
			case TransitionType::MOVE_LEFT_INTO:     return { { 0, 0 }, { m_displacement, 32 } };
			case TransitionType::MOVE_LEFT_PUSH:     return { { 0, 0 }, { m_displacement, 32 } };
			case TransitionType::MOVE_RIGHT_AWAY:    return { { 0, 0 }, { 32 - m_displacement, 32 } };
			case TransitionType::MOVE_RIGHT_INTO:    return { { 32 - m_displacement, 0 }, { m_displacement, 32 } };
			case TransitionType::MOVE_RIGHT_PUSH:    return { { 32 - m_displacement, 0 }, { m_displacement, 32 } };

			case TransitionType::MOVE_UP_AWAY:       return { { 0, m_displacement }, { 32, 32 - m_displacement } };
			case TransitionType::MOVE_UP_INTO:       return { { 0, 0 }, { 32, m_displacement } };
			case TransitionType::MOVE_UP_PUSH:       return { { 0, 0 }, { 32, m_displacement } };
			case TransitionType::MOVE_DOWN_AWAY:     return { { 0, 0 }, { 32, 32 - m_displacement } };
			case TransitionType::MOVE_DOWN_INTO:     return { { 0, 32 - m_displacement }, { 32, m_displacement } };
			case TransitionType::MOVE_DOWN_PUSH:     return { { 0, 32 - m_displacement }, { 32, m_displacement } };
			}
			return { { 0, 0 }, { 0, 0 } };
		}

		/// @brief Get the texture co-ordinates for the static entity.
		sf::IntRect getStaticTextureCoords() const {
			switch (m_type) {
			case TransitionType::MOVE_LEFT_AWAY:     return { { 32 - m_displacement, 0 }, { m_displacement, 32 } };
			case TransitionType::MOVE_LEFT_INTO:     return { { 0, 0 }, { 32 - m_displacement, 32 } };
			case TransitionType::MOVE_LEFT_PUSH:     return { { m_displacement, 0 }, { 32 - m_displacement, 32 } };
			case TransitionType::MOVE_RIGHT_AWAY:    return { { 0, 0 }, { m_displacement, 32 } };
			case TransitionType::MOVE_RIGHT_INTO:    return { { m_displacement, 0 }, { 32 - m_displacement, 32 } };
			case TransitionType::MOVE_RIGHT_PUSH:    return { { 0, 0 }, { 32 - m_displacement, 32 } };

			case TransitionType::MOVE_UP_AWAY:       return { { 0, 32 - m_displacement }, { 32, m_displacement } };
			case TransitionType::MOVE_UP_INTO:       return { { 0, 0 }, { 32, 32 - m_displacement } };
			case TransitionType::MOVE_UP_PUSH:       return { { 0, m_displacement }, { 32, 32 - m_displacement } };
			case TransitionType::MOVE_DOWN_AWAY:     return { { 0, 0 }, { 32, m_displacement } };
			case TransitionType::MOVE_DOWN_INTO:     return { { 0, m_displacement }, { 32, 32 - m_displacement } };
			case TransitionType::MOVE_DOWN_PUSH:     return { { 0, 0 }, { 32, 32 - m_displacement } };
			}
			return { { 0, 0 }, { 0, 0 } };
		}

		/// @briefGet the direction relative to the previous entity.
		Cave::Entity::Direction getPreviousDirection() const {
			switch (m_type) {
			case TransitionType::MOVE_LEFT_INTO: return Cave::Entity::Direction::RIGHT;
			case TransitionType::MOVE_LEFT_PUSH: return Cave::Entity::Direction::RIGHT;
			case TransitionType::MOVE_RIGHT_INTO: return Cave::Entity::Direction::LEFT;
			case TransitionType::MOVE_RIGHT_PUSH: return Cave::Entity::Direction::LEFT;
			case TransitionType::MOVE_UP_INTO: return Cave::Entity::Direction::DOWN;
			case TransitionType::MOVE_UP_PUSH: return Cave::Entity::Direction::DOWN;
			case TransitionType::MOVE_DOWN_INTO: return Cave::Entity::Direction::UP;
			case TransitionType::MOVE_DOWN_PUSH: return Cave::Entity::Direction::UP;
			default: break;
			}
			return Cave::Entity::Direction::NO_DIRECTION;
		}

		/// @brief Change the transition type from push to away.
		void changeToAway() {
			switch (m_type) {
			case TransitionType::MOVE_LEFT_PUSH:
				m_type = TransitionType::MOVE_LEFT_AWAY;
				return;
			case TransitionType::MOVE_RIGHT_PUSH:
				m_type = TransitionType::MOVE_RIGHT_AWAY;
				return;
			case TransitionType::MOVE_UP_PUSH:
				m_type = TransitionType::MOVE_UP_AWAY;
				return;
			case TransitionType::MOVE_DOWN_PUSH:
				m_type = TransitionType::MOVE_DOWN_AWAY;
				return;
			default: break;
			}
		}

		/// @brief Change the transition type from push to into.
		void changeToInto() {
			switch (m_type) {
			case TransitionType::MOVE_LEFT_PUSH:
				m_type = TransitionType::MOVE_LEFT_INTO;
				return;
			case TransitionType::MOVE_RIGHT_PUSH:
				m_type = TransitionType::MOVE_RIGHT_INTO;
				return;
			case TransitionType::MOVE_UP_PUSH:
				m_type = TransitionType::MOVE_UP_INTO;
				return;
			case TransitionType::MOVE_DOWN_PUSH:
				m_type = TransitionType::MOVE_DOWN_INTO;
				return;
			default: break;
			}
		}

	private:
		/// @brief The current animation of this transition.
		Animation m_animation;

		/// @brief The transition type.
		TransitionType m_type;

		/// @brief The step increment in displacement during the transition.
		int m_inc = 4;

		/// @brief The current displacement
		int m_displacement = m_inc;
	};

	/// @brief Array of away transition.
	constexpr std::array<TransitionType, 4> awayTransitions = {
		TransitionType::MOVE_LEFT_AWAY,
		TransitionType::MOVE_RIGHT_AWAY,
		TransitionType::MOVE_UP_AWAY,
		TransitionType::MOVE_DOWN_AWAY
	};

	/// @brief Array of push transition.
	constexpr std::array<TransitionType, 4> pushTransitions = {
		TransitionType::MOVE_LEFT_PUSH,
		TransitionType::MOVE_RIGHT_PUSH,
		TransitionType::MOVE_UP_PUSH,
		TransitionType::MOVE_DOWN_PUSH
	};

	/// @brief Array of into transition.
	constexpr std::array<TransitionType, 4> intoTransitions = {
		TransitionType::MOVE_LEFT_INTO,
		TransitionType::MOVE_RIGHT_INTO,
		TransitionType::MOVE_UP_INTO,
		TransitionType::MOVE_DOWN_INTO
	};
}