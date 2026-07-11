#pragma once

namespace Image {

    /**
     * @enum Texture
     * @brief Identifiers for all textures used in the game.
     * 
     * These are used with Image::Manager to load and retrieve the corresponding SFML textures.
     */
    enum class Texture {

        /// @brief HUD panel texture for caves (score, time, etc.).
        CaveHUD,

        /// @brief Loading tile overlay texture used when revealing cave tiles.
        CaveLoadingTiles,

        /// @brief Texture for displaying numbers in caves.
        CaveNumbers,

        /// @brief Ccave tileset texture for rendering cave entities.
        CaveTiles,

        /// @brief Sprite displayed when the game has been successfully completed.
        GameCompleted,

        /// @brief Font texture used throught the game.
        GameFont,

        /// @brief Sprite displayed when the player fails the game.
        GameOver,

        /// @brief Bottom background portion of the main menu.
        MainMenuBelow,

        /// @brief Dial texture in the main menu.
        MainMenuDial,

        /// @brief Load Caves menu section in the main menu.
        MainMenuLoadCaves,

        /// @brief Texture for displaying cave number in the main menu.
        MainMenuNumbers,

        /// @brief On/Off toggle buttons in the main menu.
        MainMenuOnOff,

        /// @brief Options menu graphics in the main menu.
        MainMenuOptions,

        /// @brief Selection arrow used in main menu navigation.
        MainMenuSelectArrow,

        /// @brief Top background portion of the main menu.
        MainMenuTop,

        /// @brief Editor fill button tiles.
        EditorFill,

        /// @brief Editor line-fill button tiles.
        EditorLineFill,

        /// @brief Editor ellipse-fill button tiles.
        EditorEllipseFill,

        /// @brief Editor co-ordinate tiles.
        EditorCoords,

        /// @brief Editor test button.
        EditorTestButton,

        /// @brief Editor mini tiles for map.
        EditorMiniTiles
    };

}
