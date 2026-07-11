#include "Cave/Manager/File.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

/**
 * @brief Reads a 16-bit unsigned integer from a 2-byte array in reversed (little-endian) order.
 * @param bytes Pointer to a 2-byte array.
 * @return The parsed 16-bit unsigned integer.
 */
static uint16_t readReversedUint16(const char* bytes) {
    return static_cast<uint16_t>(
        (static_cast<unsigned char>(bytes[1]) << 8) |
        (static_cast<unsigned char>(bytes[0]))
        );
}
/**
 * @brief Reads a 32-bit unsigned integer from a 4-byte array in reversed (little-endian) order.
 * @param bytes Pointer to a 4-byte array.
 * @return The parsed 32-bit unsigned integer.
 */
static uint32_t readReversedUint32(const char* bytes) {
    return static_cast<uint32_t>(
        (static_cast<unsigned char>(bytes[3]) << 24) |
        (static_cast<unsigned char>(bytes[2]) << 16) |
        (static_cast<unsigned char>(bytes[1]) << 8) |
        (static_cast<unsigned char>(bytes[0]))
        );
}


Cave::File Cave::File::loadFromFile(const std::string& directory, const std::string& filename) {

    std::ifstream file(directory + filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Unable to load cave file: " + filename);
    }

    char header[88];
    if (!file.read(header, 88)) {
        throw std::runtime_error("Unable to read header for cave file: " + filename);
    }

    if (!(header[0] == 'C' && header[1] == 'A' && header[2] == 'V' && header[3] == 'E')) {
        throw std::runtime_error("Invalid cave file (no magic): " + filename);
    }

    unsigned char numCaves = static_cast<unsigned char>(header[4]);
    if (numCaves == 0) {
        throw std::runtime_error("Invalid number of caves in cave file: " + filename);
    }

    Cave::File caveFile;
    caveFile.filename = filename;
    caveFile.caves.reserve(numCaves);

    for (int i = 0; i < numCaves; i++) {
        Cave::Properties properties{};

        char sizeBytes[4];
        if (!file.read(sizeBytes, 4)) {
            throw std::runtime_error("Could not read size for cave #" + std::to_string(i) + " in cave file: "  + filename);
        }

        uint16_t width = readReversedUint16(&sizeBytes[0]);
        uint16_t height = readReversedUint16(&sizeBytes[2]);

        if (width == 0)  width = 50;
        if (height == 0) height = 30;

        properties.width = width;
        properties.height = height;

        char propsBytes[44];
        if (!file.read(propsBytes, 44)) {
            throw std::runtime_error("Could not read properties for cave #" + std::to_string(i) + " in cave file: " + filename);
        }

        uint32_t* propsArray = reinterpret_cast<uint32_t*>(&properties.plasmaGrowthSpeed);
        for (int j = 0; j < 11; j++) {
            propsArray[j] = readReversedUint32(&propsBytes[j * 4]);
        }

        size_t tileDataCount = static_cast<size_t>(width) * static_cast<size_t>(height);
        std::vector<char> tileData(tileDataCount);

        if (!file.read(tileData.data(), tileDataCount)) {
            throw std::runtime_error("Could not read tiles for cave #" + std::to_string(i) + " in cave file: " + filename);
        }

        caveFile.caves.push_back(Cave::Data{ properties, tileData });
    }

    return caveFile;
}

void Cave::File::saveToFile(const Cave::File& caveFile, const std::string& fullPath)
{
    std::ofstream file(fullPath, std::ios::binary);
    if (!file)
        throw std::runtime_error("Unable to save cave file: " + fullPath);

    // Header: 88 bytes — magic + cave count + padding
    char header[88] = {};
    header[0] = 'C'; header[1] = 'A'; header[2] = 'V'; header[3] = 'E';
    header[4] = static_cast<char>(static_cast<unsigned char>(caveFile.caves.size()));
    file.write(header, 88);

    for (const auto& cave : caveFile.caves)
    {
        // Width + height as two uint16 LE values (4 bytes total)
        char sizeBytes[4] = {};
        auto w = static_cast<uint16_t>(cave.properties.width);
        auto h = static_cast<uint16_t>(cave.properties.height);
        sizeBytes[0] = static_cast<char>(w & 0xFF);
        sizeBytes[1] = static_cast<char>((w >> 8) & 0xFF);
        sizeBytes[2] = static_cast<char>(h & 0xFF);
        sizeBytes[3] = static_cast<char>((h >> 8) & 0xFF);
        file.write(sizeBytes, 4);

        // 11 properties as uint32 LE (44 bytes), starting from plasmaGrowthSpeed
        const uint32_t* propsArray = reinterpret_cast<const uint32_t*>(&cave.properties.plasmaGrowthSpeed);
        char propsBytes[44] = {};
        for (int j = 0; j < 11; ++j)
        {
            uint32_t v = propsArray[j];
            propsBytes[j*4+0] = static_cast<char>(v & 0xFF);
            propsBytes[j*4+1] = static_cast<char>((v >> 8)  & 0xFF);
            propsBytes[j*4+2] = static_cast<char>((v >> 16) & 0xFF);
            propsBytes[j*4+3] = static_cast<char>((v >> 24) & 0xFF);
        }
        file.write(propsBytes, 44);

        // Tile data
        file.write(cave.tileData.data(), static_cast<std::streamsize>(cave.tileData.size()));
    }
}