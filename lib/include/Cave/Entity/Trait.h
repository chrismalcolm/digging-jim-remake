#pragma once
#include <cstddef>
#include <type_traits>

namespace Cave::Entity {

    /**
     * @brief Trait flags for cave entities.
     *
     * Traits define properties and behaviors of entities in the cave.
     * They are stored as bit flags so multiple traits can be combined
     * using bitwise operators.
     */
    enum class Trait : uint16_t {

        /// @brief This trait represents no trait.
        None = 0,

        /// @brief This trait represents empty space.
        Empty = 1 << 0,

        /// @brief Entities with this trait can be dug through.
        Free = 1 << 1,

        /// @brief Entities with this trait can be traversed by Jim.
        Traversable = 1 << 2,
        
        /// @brief Entities with this trait can be collected.
        Collectable = 1 << 3,
        
        /// @brief Entities with this trait does not move.
        Static = 1 << 4,

        /// @brief Entities with this trait animation frames never changes or undergo transition.
        Immutable = 1 << 5,

        /// @brief Entities with this trait cannot be destroyed.
        Indestructible = 1 << 6,

        /// @brief Entities with this trait will cause any falling entites from above to slip.
        Slippery = 1 << 7,

        /// @brief Entities with this trait can be pushed.
        Pushable = 1 << 8,
        
        /// @brief Entities with this trait.
        Crushable = 1 << 9,

        /// @brief Entities with this trait will cause an explosion when colliding with enemy entities.
        Reactive = 1 << 10,

        /// @brief Entities with this trait exist only exist to complete their animation cycle once.
        Transient = 1 << 11,
        
        /// @brief Entities with this trait have the ability to warp Jim left.
        WarpableLeft = 1 << 12,

        /// @brief Entities with this trait have the ability to warp Jim right.
        WarpableRight = 1 << 13,

        /// @brief Entities with this trait have the ability to warp Jim up.
        WarpableUp = 1 << 14,

        /// @brief Entities with this trait have the ability to warp Jim down.
        WarpableDown = 1 << 15
        
    };

    /**
     * @brief Perform bitwise OR on two Trait flags.
     * @param lhs Left-hand side trait
     * @param rhs Right-hand side trait
     * @return Trait Result of lhs OR rhs
     */
    inline Trait operator|(Trait lhs, Trait rhs) {
        return static_cast<Trait>(
            static_cast<std::underlying_type_t<Trait>>(lhs) |
            static_cast<std::underlying_type_t<Trait>>(rhs)
            );
    }

    /**
     * @brief Perform bitwise AND on two Trait flags.
     * @param lhs Left-hand side trait
     * @param rhs Right-hand side trait
     * @return Trait Result of lhs AND rhs
     */
    inline Trait operator&(Trait lhs, Trait rhs) {
        return static_cast<Trait>(
            static_cast<std::underlying_type_t<Trait>>(lhs) &
            static_cast<std::underlying_type_t<Trait>>(rhs)
            );
    }

    /**
     * @brief Apply lhs with rhs using in-place OR assignment.
     * @param lhs Left-hand side trait (modified)
     * @param rhs Right-hand side trait
     * @return Trait& Reference to lhs after appying OR
     */
    inline Trait& operator|=(Trait& lhs, Trait rhs) {
        lhs = lhs | rhs;
        return lhs;
    }

    /**
     * @brief Apply lhs with rhs using in-place AND assignment.
     * @param lhs Left-hand side trait (modified)
     * @param rhs Right-hand side trait
     * @return Trait& Reference to lhs after appying AND
     */
    inline Trait& operator&=(Trait& lhs, Trait rhs) {
        lhs = lhs & rhs;
        return lhs;
    }

    /**
     * @brief Check if a trait set contains a specific trait.
     * @param traits Combined traits to check
     * @param check Trait to look for
     * @return true if check is set in traits
     * @return false otherwise
     */
    inline bool checkTrait(Trait traits, Trait check) {
        return static_cast<bool>((traits & check) != Trait::None);
    }

}
