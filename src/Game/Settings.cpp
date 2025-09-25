#include "Game/Game.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>

static inline std::string trim(std::string s) {
    auto l = s.find_first_not_of(" \t\r\n");
    if (l == std::string::npos) return "";
    auto r = s.find_last_not_of(" \t\r\n");
    return s.substr(l, r - l + 1);
}

static inline std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return s;
}

static inline bool parseBoolToken(const std::string& token) {
    // Accept a variety of boolean representations
    std::string t = toLower(trim(token));
    if (t.empty()) return false;
    if (t == "1" || t == "true" || t == "yes" || t == "on" || t == "y" || t == "t") return true;
    return false;
}

static inline bool parseFloat(const std::string& token, float& out) {
    std::istringstream ss(trim(token));
    ss.imbue(std::locale::classic());
    ss >> out;
    return !ss.fail();
}

GameSettings Game::loadGameOptionsFromFile(const std::string& path) {
    GameSettings opts; // start with defaults

    std::ifstream in(path);
    if (!in.is_open()) {
        // Fail silently and return defaults (matching requested behaviour)
        return opts;
    }

    std::string line;
    while (std::getline(in, line)) {
        std::string s = trim(line);
        if (s.empty()) continue;
        if (s.front() == '#' || s.front() == ';') continue;

        auto eq = s.find('=');
        if (eq == std::string::npos) continue; // ignore malformed lines

        std::string key = toLower(trim(s.substr(0, eq)));
        std::string val = trim(s.substr(eq + 1));

        if (key == "audio") {
            opts.audio = parseBoolToken(val);
        }
        else if (key == "audiovolume" || key == "audio_volume") {
            float v = 0.f;
            if (parseFloat(val, v)) {
                // clamp to expected range 0.0 - 100.0
                if (v < 0.f) v = 0.f;
                if (v > 100.f) v = 100.f;
                opts.audioVolume = v;
            }
            // else keep default
        }
        else if (key == "joystickcontrol" || key == "joystick_control") {
            opts.joystickControl = parseBoolToken(val);
        }
        else if (key == "fixedcolours" || key == "fixed_colours") {
            opts.fixedColours = parseBoolToken(val);
        }
        else if (key == "setrefreshrateonstart" || key == "set_refresh_rate_on_start") {
            opts.setRefreshRateOnStart = parseBoolToken(val);
        }
        // unknown keys are ignored
    }

    return opts;
}

bool Game::saveGameOptionsToFile(const GameSettings& opts, const std::string& path) {
    try {
        std::ofstream out(path, std::ios::out | std::ios::trunc);
        if (!out.is_open()) {
            return false; // fail silently
        }

        // Write a simple human-readable file
        out << "# Game options (key = value)\n";
        out << "audio = " << (opts.audio ? "true" : "false") << "\n";
        out << "audioVolume = " << opts.audioVolume << "\n";
        out << "joystickControl = " << (opts.joystickControl ? "true" : "false") << "\n";
        out << "fixedColours = " << (opts.fixedColours ? "true" : "false") << "\n";
        out << "setRefreshRateOnStart = " << (opts.setRefreshRateOnStart ? "true" : "false") << "\n";

        return true;
    }
    catch (...) {
        // swallow all exceptions and fail silently
        return false;
    }
}
