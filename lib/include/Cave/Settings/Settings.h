#pragma once
#include <cstdint>
#include <SFML/Graphics.hpp>
#include "Cave/Properties/Properties.h"

static constexpr uint32_t RANDOM_DIST_BLOCKS_MIN = 0u;
static constexpr uint32_t RANDOM_DIST_BLOCKS_MAX = 9999u;

namespace Cave {

    enum class FillMode {
        Rectangle,
        Line,
        Ellipse,
    };

    struct Settings {
        uint32_t         randomDistBlocks = 50;
        bool             animation        = true;
        Cave::ResizeAnchor resizeAnchor   = Cave::ResizeAnchor::NW;
        FillMode         fillMode         = FillMode::Rectangle;
    };

    /**
     * @brief Opens a Win32 modal dialog for editing cave editor settings.
     *
     * @param window    The SFML render window (used as modal parent).
     * @param settings  Reference to the current settings (modified in-place on OK).
     * @param trigger   Set to true when the dialog closes (OK or Cancel).
     */
    void editCaveSettings(sf::RenderWindow& window, Cave::Settings& settings, bool& trigger, bool developerMode = false);
}
