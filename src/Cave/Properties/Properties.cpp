#include "Cave/Properties/Properties.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <algorithm>

void Cave::editCaveProperties(sf::RenderWindow& window, Cave::Properties& props, Cave::Properties original, const int& diamondCount, bool& trigger) {
    // Local copies for editing via ImGui
    int quota = props.quota;
    int diamondValue = props.diamondValue;
    int extraDiamondValue = props.extraDiamondValue;
    int amoebaGrowthMax = props.amoebaGrowthMax;
    int time = props.time;
    int magicWallTime = props.magicWallTime;

    // Configure editor window
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Edit Cave Properties");

    // Apply consistent padding and spacing
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(64, 8));

    // =============================
    // Left Column: Diamond settings
    // =============================
    ImGui::BeginGroup();
    ImGui::Text("Diamonds");
    ImGui::Separator();

    ImGui::PushItemWidth(128);
    ImGui::InputInt("Diamond Quota", &quota, 1, 1, INPUT_FLAGS);
    if (ImGui::Button("Count")) {
        quota = diamondCount;
    }
    ImGui::InputInt("Diamond Value", &diamondValue, 1, 1, INPUT_FLAGS);
    ImGui::InputInt("Extra Diamond Value", &extraDiamondValue, 1, 1, INPUT_FLAGS);
    ImGui::PopItemWidth();
    ImGui::EndGroup();

    ImGui::SameLine();

    // ====================================
    // Right Column: Amoeba + Time settings
    // ====================================
    ImGui::BeginGroup();
    ImGui::Text("Amoeba Growth");
    ImGui::Separator();
    ImGui::PushItemWidth(128);
    ImGui::InputInt("Amoeba Growth Max", &amoebaGrowthMax, 1, 1, INPUT_FLAGS);

    ImGui::NewLine();
    ImGui::Text("Time & Magic Wall");
    ImGui::Separator();
    ImGui::InputInt("Time", &time, 1, 1, INPUT_FLAGS);
    ImGui::InputInt("Magic Wall Expiry Time", &magicWallTime, 1, 1, INPUT_FLAGS);
    ImGui::EndGroup();

    ImGui::NewLine();

    // ======================
    // Growth speed sliders
    // ======================
    ImGui::Text("Amoeba Growth Speed");
    ImGui::Separator();
    ImGui::PushItemWidth(480);
    ImGui::SliderInt("Amoeba Growth Speed", reinterpret_cast<int*>(&props.amoebaGrowthSpeed),
        static_cast<int>(AMOEBA_GROWTH_SPEED_MIN), static_cast<int>(AMOEBA_GROWTH_SPEED_MAX));

    ImGui::NewLine();
    ImGui::Text("Plasma Growth Speed");
    ImGui::Separator();
    ImGui::SliderInt("Plasma Growth Speed", reinterpret_cast<int*>(&props.plasmaGrowthSpeed),
        static_cast<int>(PLASMA_GROWTH_SPEED_MIN), static_cast<int>(AMOEBA_GROWTH_SPEED_MAX));

    ImGui::NewLine();

    // ==================
    // Color adjustments
    // ==================
    ImGui::Text("Colors");
    ImGui::Separator();
    ImGui::SliderInt("Hue", reinterpret_cast<int*>(&props.hue), static_cast<int>(HUE_MIN), static_cast<int>(HUE_MAX));
    ImGui::SliderInt("Saturation", reinterpret_cast<int*>(&props.sat), static_cast<int>(SAT_MIN), static_cast<int>(SAT_MAX));
    ImGui::SliderInt("Luminance", reinterpret_cast<int*>(&props.lum), static_cast<int>(LUM_MIN), static_cast<int>(LUM_MAX));

    ImGui::NewLine();

    // ===========================
    // Clamp and sanitize values
    // ===========================
    quota = std::max(0, quota);
    diamondValue = std::max(0, diamondValue);
    extraDiamondValue = std::max(0, extraDiamondValue);
    amoebaGrowthMax = std::max(0, amoebaGrowthMax);
    time = std::max(0, time);
    magicWallTime = std::max(0, magicWallTime);

    props.quota = std::clamp(static_cast<uint32_t>(quota), QUOTA_MIN, QUOTA_MAX);
    props.diamondValue = std::clamp(static_cast<uint32_t>(diamondValue), DIAMOND_VALUE_MIN, DIAMOND_VALUE_MAX);
    props.extraDiamondValue = std::clamp(static_cast<uint32_t>(extraDiamondValue), EXTRA_DIAMOND_VALUE_MIN, EXTRA_DIAMOND_VALUE_MAX);
    props.amoebaGrowthMax = std::clamp(static_cast<uint32_t>(amoebaGrowthMax), AMOEBA_GRWOTH_MAX_MIN, AMOEBA_GRWOTH_MAX_MAX);
    props.time = std::clamp(static_cast<uint32_t>(time), TIME_MIN, TIME_MAX);
    props.magicWallTime = std::clamp(static_cast<uint32_t>(magicWallTime), MAGIC_WALL_TIME_MIN, MAGIC_WALL_TIME_MAX);

    // ===================
    // Action buttons
    // ===================
    ImGui::PopStyleVar(1);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));

    ImGui::BeginGroup();
    if (ImGui::Button("OK")) trigger = true;
    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::BeginGroup();
    if (ImGui::Button("Reset")) props = original;
    ImGui::EndGroup();

    // Restore global style
    ImGui::PopStyleVar(3);
    ImGui::End();
}

