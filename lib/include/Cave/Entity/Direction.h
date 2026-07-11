#pragma once
#include <array>
#include "Utils/Random.h"

namespace Cave::Entity {

    /**
     * @enum Direction
     * @brief Represents the four cardinal directions, plus a sentinel value.
     */
    enum class Direction {
        /// @brief Left direction
        LEFT = 0,

        /// @brief Right direction
        RIGHT,

        /// @brief Up direction
        UP,

        /// @brief Down direction
        DOWN,

        /// @brief No direction
        NO_DIRECTION,
    };

    /// @brief All four cardinal directions.
    inline constexpr std::array<Direction, 4> ALL_DIRECTIONS = {
        Direction::LEFT,
        Direction::RIGHT,
        Direction::UP,
        Direction::DOWN,
    };

    /// @brief Horizontal directions: left and right.
    inline constexpr std::array<Direction, 2> HORIZONTAL_DIRECTIONS = {
        Direction::LEFT,
        Direction::RIGHT,
    };

    /// @brief Vertical directions: up and down.
    inline constexpr std::array<Direction, 2> VERTICAL_DIRECTIONS = {
        Direction::UP,
        Direction::DOWN,
    };

    /**
     * @brief Get the clockwise arc order starting from a given direction.
     *
     * Produces an array of four directions, ordered clockwise with respect
     * to the provided starting direction.
     *
     * Example: If direction = LEFT, the order is {UP, LEFT, DOWN, RIGHT}.
     *
     * @param direction The starting direction.
     * @return std::array<Direction, 4> Directions in clockwise order.
     */
    inline std::array<Direction, 4> clockwiseArc(Direction direction) {
        switch (direction) {
        case Direction::LEFT:
            return { Direction::UP, Direction::LEFT, Direction::DOWN, Direction::RIGHT };
        case Direction::RIGHT:
            return { Direction::DOWN, Direction::RIGHT, Direction::UP, Direction::LEFT };
        case Direction::UP:
            return { Direction::RIGHT, Direction::UP, Direction::LEFT, Direction::DOWN };
        case Direction::DOWN:
            return { Direction::LEFT, Direction::DOWN, Direction::RIGHT, Direction::UP };
        default:
            return { Direction::NO_DIRECTION, Direction::NO_DIRECTION, Direction::NO_DIRECTION, Direction::NO_DIRECTION };
        }
    }

    /**
     * @brief Get the anticlockwise arc order starting from a given direction.
     *
     * Produces an array of four directions, ordered anticlockwise with respect
     * to the provided starting direction.
     *
     * Example: If direction = LEFT, the order is {DOWN, LEFT, UP, RIGHT}.
     *
     * @param direction The starting direction.
     * @return std::array<Direction, 4> Directions in anticlockwise order.
     */
    inline std::array<Direction, 4> anticlockwiseArc(Direction direction) {
        switch (direction) {
        case Direction::LEFT:
            return { Direction::DOWN, Direction::LEFT, Direction::UP, Direction::RIGHT };
        case Direction::RIGHT:
            return { Direction::UP, Direction::RIGHT, Direction::DOWN, Direction::LEFT };
        case Direction::UP:
            return { Direction::LEFT, Direction::UP, Direction::RIGHT, Direction::DOWN };
        case Direction::DOWN:
            return { Direction::RIGHT, Direction::DOWN, Direction::LEFT, Direction::UP };
        default:
            return { Direction::NO_DIRECTION, Direction::NO_DIRECTION, Direction::NO_DIRECTION, Direction::NO_DIRECTION };
        }
    }

    /**
     * @brief Select a random direction from the four cardinal directions.
     *
     * @return Direction A random direction (LEFT, RIGHT, UP, or DOWN).
     */
    inline Direction getRandomDirection() {
        return ALL_DIRECTIONS[Utils::randomInteger(0, 3)];
    }
}
