#pragma once

#include <string>
#include <utility>
#include <vector>
#include "Cave/Properties/Properties.h"
#include "Cave/Manager/File.h"
#include "Cave/Map/Map.h"
#include "Game/Game.h"

/// @brief The directory where this game will search for valid .cav files
const std::string CAVE_DIRECTORY = ".\\assets\\data\\";

/// @brief The name of the original 100 caves file.
const std::string ORIGINAL_CAVE_FILE = "originals.cav";

namespace Cave {

    /**
     * @brief Manages loading cave data and starting caves within the game.
     *
     * It provides functionality to mount a cave's data to a given Cave::Map and Cave::Properties.
     */
    class Manager {
    public:
        /**
         * @brief Construct a new cave manager.
         * 
         * @param game Pointer to the parent Game instance.
         */
        explicit Manager(Game* game);

        /**
         * @brief Loads all available cave files from the cave directory.
         *
         * @details
         * This method scans the configured cave directory (`CAVE_DIRECTORY`) for all
         * `.cav` files. If the special file defined by `ORIGINAL_CAVE_FILE` is found,
         * it is inserted at the beginning of the list to ensure priority loading.
         *
         * @throws std::runtime_error If the cave directory is invalid, no `.cav`
         *         files exist, or no valid files could be loaded.
         */
        void load();

        /**
         * @brief Initializes and starts a cave from the loaded cave files.
         *
         * @details
         * This method selects a cave from the loaded cave files, applies its properties,
         * and generates the corresponding map. The selection process is based on the
         * provided file index and cave number, both of which are clamped to valid ranges:
         *
         * @param fileIndex Index of the cave file within the loaded cave list.
         * @param caveNumber 1-based index of the cave within the selected file.
         * @param properties Pointer to a Cave::Properties object that will be populated
         *        with the selected cave’s properties.
         * @param map Pointer to a Cave::Map object that will be generated with the cave’s
         *        entity and tile data.
         *
         * @return true if the cave was successfully initialized and started, false if
         *         no caves are loaded or if invalid pointers are provided.
         */
        bool startCave(const int& fileIndex, const int& caveNumber, Cave::Properties* properties, Cave::Map* map);

        /**
         * @brief Get the number of caves for a cave file.
         *
         * @param fileINdex Index of the cave file.
         *
         * @return The count of the number of caves for the cave file of the given fileIndex.
         */
        size_t numCaves(const int& fileIndex);

        /**
         * @brief Get the filenames files loaded.
         *
         * @return Filenames of each of the valid .cav files.
         */
        std::vector<std::string> getCaveFiles();

    private:

        /// @brief Game instance.
        Game* m_game;

        /// @brief List of cave files loaded.
        std::vector<Cave::File> m_cavesData;
    };

}
