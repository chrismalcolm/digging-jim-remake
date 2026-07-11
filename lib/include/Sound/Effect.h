#pragma once

namespace Sound {

    /**
     * @enum Effect
     * @brief Identifiers for all sound effects used in the game.
     *
     * Each value in this enum corresponds to a cave interaction that plays a sound effect.
     * The enum is used by the Sound::Manager to trigger and play sounds.
     */
    enum class Effect {
        /// @brief Amoeba activity sound.
        Amoeba,

        /// @brief Breaking of a fragile diamond.
        Break,

        /// @brief CaveGull explosion.
        CaveGullExplosion,

        /// @brief A diamond is collected.
        Collect,

        /// @brief Diamond is dropped.
        DiamondDrop,

        /// @brief Diamond lands.
        DiamondLand,

        /// @brief Digging traversing through dirt.
        Dig,

        /// @brief Fallable object (except diamond) is dropped.
        /// Also used when a pushable cave entity is pushed.
        Drop,

        /// @brief General explosion.
        Explosion,

        /// @brief Fallable object (except diamond) lands.
        Land,

        /// @brief Magic wall active sound.
        MagicWall,

        /// @brief Start door opening sound.
        Open,

        /// @brief Plasma spawn sound.
        Plasma,

        /// @brief Jim moving through a tube.
        Tube,

        /// @brief Unlocking the exit door.
        Unlock,

        /// @brief Jim is awarded an extra life.
        Yahoo,

        /// @brief Jim completes a cave.
        Yippee
    };

}
