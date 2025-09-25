#include "Cave/Manager/Manager.h"
#include "Cave/Manager/File.h"
#include "Cave/Properties/Properties.h"
#include <algorithm>
#include <fstream>
#include <memory>
#include <array>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

Cave::Manager::Manager(Game* game) : m_game(game) {}

void Cave::Manager::load() {
    std::vector<std::string> caveFilenames;
    std::string originalsFile;

    if (!std::filesystem::exists(CAVE_DIRECTORY) || !std::filesystem::is_directory(CAVE_DIRECTORY)) {
        throw std::runtime_error("Invalid directory: " + CAVE_DIRECTORY);
    }

    for (const auto& entry : std::filesystem::directory_iterator(CAVE_DIRECTORY)) {
        if (entry.is_regular_file() && entry.path().extension() == ".cav") {
            std::string filename = entry.path().filename().string();
            if (filename == ORIGINAL_CAVE_FILE) {
                originalsFile = filename;
            }
            else {
                caveFilenames.push_back(filename);
            }
        }
    }

    // If originals.cav was found, insert it at the front
    if (!originalsFile.empty()) {
        caveFilenames.insert(caveFilenames.begin(), originalsFile);
    }

    if (caveFilenames.empty()) {
        throw std::runtime_error("No .cav files found in directory: " + CAVE_DIRECTORY);
    }

    for (auto& caveFilename : caveFilenames) {
        try {
            Cave::File caveFile = Cave::File::loadFromFile(CAVE_DIRECTORY, caveFilename);
            m_cavesData.push_back(caveFile);
        }
        catch (const std::exception& e) {
            std::cerr << "Warning: Failed to load cave file '" << caveFilename << "': " << e.what() << "\n";
        }
    }

    if (m_cavesData.empty()) {
        throw std::runtime_error("No valid .cav files found in directory: " + CAVE_DIRECTORY);
    }
}

std::vector<std::string> Cave::Manager::getCaveFiles() {
    std::vector<std::string> caveFiles;
    for (auto& caveData : m_cavesData) caveFiles.push_back(caveData.filename);
    return caveFiles;
}

bool Cave::Manager::startCave(const int& fileIndex, const int& caveNumber, Cave::Properties* properties, Cave::Map* map) {
    if (m_cavesData.empty() || map == nullptr || properties == nullptr) return false;

    // Clamp cave index to get cave file
    int index = std::clamp(fileIndex, 0, static_cast<int>(m_cavesData.size()  - 1));
    Cave::File file = m_cavesData[index];

    // Clamp cave number to get cave data
    // We subtract 1 from the game cave number, as that is indexed from 1, not 0 like file.caves
    int caveIndex = std::clamp(caveNumber - 1, 0, static_cast<int>(file.caves.size()) - 1);
    Cave::Data data = file.caves[caveIndex];

    // Set the cave properties
    *properties = data.properties;
    m_game->setCaveProperties(*properties);

    // Construct the map
    map->generateMap(properties, data.tileData);

    return true;
}

size_t Cave::Manager::numCaves(const int& fileIndex) {
    if (m_cavesData.empty()) return 0;

    int index = std::clamp(fileIndex, 0, static_cast<int>(m_cavesData.size() - 1));
    Cave::File file = m_cavesData[index];

    return file.caves.size();
}