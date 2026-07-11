namespace Cave::Entity {

    /**
     * @brief Represents the horizontal facing direction of an entity.
     *
     * Used to determine how an entity is oriented for rendering, movement,
     * or animations.
     */
    enum class Facing {

        /// @brief Entity is facing left.
        LEFT,

        /// @brief Entity is facing straight / no particular direction.
        NEUTRAL,

        /// @brief Entity is facing right.
        RIGHT
    };

}
