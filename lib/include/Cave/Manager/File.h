#pragma once

#include "Cave/Manager/Data.h"
#include <string>
#include <vector>

namespace Cave {

    /**
     * @brief Represents a cave file containing multiple caves.
     *
     * This structure stores the original filename of the cave file
     * and a list of caves (each with its own properties and tile data).
     */
    struct File {
        /**
         * @brief Name of the .cav file.
         */
        std::string filename;

        /**
         * @brief List of caves contained in this file.
         *
         * Each element is a Cave::Data struct, which holds the
         * properties and tile data for a single cave.
         */
        std::vector<Data> caves;

        /**
         * @brief Load a cave file from the cave file.
         */
        static Cave::File loadFromFile(const std::string& directory, const std::string& filename);

        /**
         * @brief Save a cave file to the given full path.
         */
        static void saveToFile(const Cave::File& caveFile, const std::string& fullPath);
    };

}
