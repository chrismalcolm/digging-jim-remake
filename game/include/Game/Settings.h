#pragma once

/// @brief The game settings.
struct GameSettings {
    bool audio = true;
    float audioVolume = 100.f;
    bool joystickControl = false;
    bool fixedColours = false;
    bool setRefreshRateOnStart = false;
};