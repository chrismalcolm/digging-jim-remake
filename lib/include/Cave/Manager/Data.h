#pragma once

#include "Cave/Properties/Properties.h"
#include "Cave/Entity/Entity.h"
#include <vector>

namespace Cave {

    /**
     * @brief Represents a single cave's data.
     *
     * This struct stores the cave properties and the associated tile data.
     */
    struct Data {
        /**
         * @brief Properties of the cave.
         */
        Cave::Properties properties;

        /**
         * @brief Raw tile data for the cave.
         *
         * Each element corresponds to a tile in the cave grid.
         */
        std::vector<char> tileData;

        /**
         * @brief Get the corresponding entity from the tile data char.
         */
        static Cave::Entity::Base getTileEntity(const char& tile);

        /**
         * @brief Whether the tile is the start door.
         */
        static bool isStartDoor(const char& tile);
    };
}
