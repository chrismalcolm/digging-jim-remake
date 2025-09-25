#pragma once
#include <cstdint>
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

static constexpr uint32_t QUOTA_MIN = 0u;
static constexpr uint32_t QUOTA_MAX = 999u;
static constexpr uint32_t DIAMOND_VALUE_MIN = 0u;
static constexpr uint32_t DIAMOND_VALUE_MAX = 99u;
static constexpr uint32_t EXTRA_DIAMOND_VALUE_MIN = 0u;
static constexpr uint32_t EXTRA_DIAMOND_VALUE_MAX = 99u;
static constexpr uint32_t AMOEBA_GRWOTH_MAX_MIN = 0u;
static constexpr uint32_t AMOEBA_GRWOTH_MAX_MAX = 9999u;
static constexpr uint32_t TIME_MIN = 0u;
static constexpr uint32_t TIME_MAX = 999u;
static constexpr uint32_t MAGIC_WALL_TIME_MIN = 0u;
static constexpr uint32_t MAGIC_WALL_TIME_MAX = 999u;
static constexpr uint32_t AMOEBA_GROWTH_SPEED_MIN = 1u;
static constexpr uint32_t AMOEBA_GROWTH_SPEED_MAX = 1000u;
static constexpr uint32_t PLASMA_GROWTH_SPEED_MIN = 1u;
static constexpr uint32_t PLASMA_GROWTH_SPEED_MAX = 1000u;
static constexpr uint32_t HUE_MIN = 0u;
static constexpr uint32_t HUE_MAX = 200u;
static constexpr uint32_t SAT_MIN = 0u;
static constexpr uint32_t SAT_MAX = 200u;
static constexpr uint32_t LUM_MIN = 0u;
static constexpr uint32_t LUM_MAX = 200u;

static constexpr ImGuiInputTextFlags INPUT_FLAGS = ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_ParseEmptyRefVal;

namespace Cave {

    /**
     * @struct Properties
     * @brief Represents the properties of a cave.
     *
     * The order of these properties matches how they appear in a .cav file.
     */
    struct Properties {
        uint32_t width;
        uint32_t height;
        uint32_t plasmaGrowthSpeed;
        uint32_t time;
        uint32_t quota;
        uint32_t diamondValue;
        uint32_t extraDiamondValue;
        uint32_t amoebaGrowthSpeed;
        uint32_t amoebaGrowthMax;
        uint32_t magicWallTime;
        uint32_t hue;
        uint32_t sat;
        uint32_t lum;
    };

    /**
     * @brief Opens an ImGui editor window for modifying cave properties.
     *
     * @details
     * This function displays an interactive ImGui window that allows editing
     * of cave properties such as diamond quota, scoring values, amoeba growth
     * parameters, time limits, plasma growth speed, and cave colors.
     *
     * @param window The SFML render window hosting ImGui.
     * @param props Reference to the current cave properties (modified in-place).
     * @param original Backup of the original cave properties, used for reset.
     * @param diamondCount The current number of diamonds in the cave (used by "Count" button).
     * @param trigger Reference to a flag set to true if the user clicks "OK".
     */
    void editCaveProperties(sf::RenderWindow& window, Cave::Properties& props, Cave::Properties original, const int& diamondCount, bool& trigger);
}


