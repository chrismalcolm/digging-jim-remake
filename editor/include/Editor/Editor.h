#pragma once
#include "Game/Game.h"
#include "Cave/Settings/Settings.h"
#include "Cave/Map/Map.h"
#include "Cave/Manager/File.h"
#include "Image/Manager.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

namespace HUD::Editor { class Panel; }

/// @brief The name displayed in the editor window title bar.
const std::string EDITOR_NAME = "Digging Jim Builder";

/**
 * @class Editor
 * @brief Central controller for the Digging Jim Builder cave editor.
 *
 * Editor owns the game instance, image manager, and cave settings used
 * throughout the editor session. All user interactions (menu items, keyboard
 * shortcuts, toolbar buttons) are routed through public action*() methods that
 * set private boolean flags. Each flag is consumed once per frame inside run(),
 * which holds the main loop and all local rendering state. This deferred
 * pattern keeps the HUD and toolbar code free of direct state mutation.
 */
class Editor {
public:

    // ----------------------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------------------

    /// @brief Construct a new Editor instance with default values.
    Editor();

    /**
     * @brief Launch the editor and enter the main loop.
     *
     * Creates the window, loads assets, and runs until the user closes
     * the editor. Returns true on clean exit.
     */
    bool run();

    // ----------------------------------------------------------------------------------
    // File actions
    // ----------------------------------------------------------------------------------

    /// @brief Request a new blank cave file, discarding current contents.
    void actionNewFile();

    /// @brief Open the file-open dialog to load a cave file.
    void actionOpenFile();

    /// @brief Save the current cave file, prompting for a path if unsaved.
    void actionSaveFile();

    /// @brief Always prompt for a save path, even if the file was previously saved.
    void actionSaveFileAs();

    /// @brief Close the editor window.
    void actionExit();

    // ----------------------------------------------------------------------------------
    // Level navigation
    // ----------------------------------------------------------------------------------

    /// @brief Navigate to the next cave in the file.
    void actionNextLevel();

    /// @brief Navigate to the previous cave in the file.
    void actionPrevLevel();

    // ----------------------------------------------------------------------------------
    // Level editing
    // ----------------------------------------------------------------------------------

    /// @brief Insert a new blank cave after the current one.
    void actionInsertLevel();

    /// @brief Delete the current cave from the file.
    void actionDeleteLevel();

    /// @brief Reset the current cave to a blank state (solid wall border, dirt fill).
    void actionClearLevel();

    /// @brief Copy the current cave's tile data to the clipboard.
    void actionCopyLevel();

    /// @brief Paste clipboard tile data into the current cave.
    void actionPasteLevel();

    /// @brief Randomly distribute the selected entity across the cave interior.
    void actionRandomDist();

    // ----------------------------------------------------------------------------------
    // Dialogs
    // ----------------------------------------------------------------------------------

    /// @brief Open the editor settings dialog.
    void actionShowSettings();

    /// @brief Open the cave properties dialog for the current cave.
    void actionShowCaveProperties();

    // ----------------------------------------------------------------------------------
    // View
    // ----------------------------------------------------------------------------------

    /**
     * @brief Toggle between normal (32px) and small (16px) tile display.
     * @param small Pass true to enable small blocks, false for normal size.
     */
    void actionSetSmallBlocks(bool small);

    /// @brief Returns true if small block mode is currently active.
    bool isSmallBlocks() const { return m_smallBlocks; }

    // ----------------------------------------------------------------------------------
    // Cursor
    // ----------------------------------------------------------------------------------

    /// @brief Change the mouse cursor to the given SFML cursor type.
    /// @param type The SFML cursor shape to apply.
    void actionChangeCursor(sf::Cursor::Type type);

    // ----------------------------------------------------------------------------------
    // Test
    // ----------------------------------------------------------------------------------

    /// @brief Save the current file and launch the game to test from the current cave.
    void actionTest();

    // ----------------------------------------------------------------------------------
    // Undo
    // ----------------------------------------------------------------------------------

    /// @brief Request an undo of the last edit.
    void actionUndo();

    /**
     * @brief Snapshot the current map tile state before an edit is applied.
     *
     * Must be called immediately before any destructive edit so that
     * actionUndo() can restore the map to this state. Also sets m_isDirty.
     *
     * @param map The current cave map.
     */
    void saveUndoSnapshot(const Cave::Map& map);

    // ----------------------------------------------------------------------------------
    // Implementation helpers
    // ----------------------------------------------------------------------------------

    /// @brief Fill the current cave with a blank layout (border walls, dirt interior).
    /// @param map The cave map to clear.
    void clearLevel(Cave::Map& map);

    /// @brief Copy the current map's tile data into the internal clipboard.
    /// @param map The cave map to copy from.
    void copyLevel(const Cave::Map& map);

    /// @brief Paste the clipboard tile data into the current map.
    /// @param map The cave map to paste into.
    void pasteLevel(Cave::Map& map);

    /**
     * @brief Write the live map state back into the loadedFile cave entry.
     *
     * Called before navigating away from a cave so that tile edits are not lost.
     *
     * @param map              The live cave map.
     * @param loadedFile       The file whose cave entry will be updated.
     * @param currentCaveIndex Index of the cave to sync.
     */
    void syncCurrentCave(Cave::Map& map, Cave::File& loadedFile, int currentCaveIndex);

    /**
     * @brief Insert a new blank cave after the current index.
     * @param map              The live cave map (used to sync before inserting).
     * @param loadedFile       The file to insert into.
     * @param currentCaveIndex Updated in-place to point to the new cave.
     */
    void insertLevel(Cave::Map& map, Cave::File& loadedFile, int& currentCaveIndex);

    /**
     * @brief Delete the cave at the current index.
     * @param map              The live cave map.
     * @param loadedFile       The file to delete from.
     * @param currentCaveIndex Updated in-place after deletion.
     */
    void deleteLevel(Cave::Map& map, Cave::File& loadedFile, int& currentCaveIndex);

    /**
     * @brief Save the file to the existing saved path, or call saveFileAs if unsaved.
     * @param map              The live cave map (synced before saving).
     * @param loadedFile       The file to save.
     * @param currentCaveIndex Index of the currently active cave.
     * @param window           Owner window for any error dialogs.
     */
    void saveFile(Cave::Map& map, Cave::File& loadedFile, int currentCaveIndex);

    /**
     * @brief Prompt the user for a save path and write the file.
     * @param map              The live cave map (synced before saving).
     * @param loadedFile       The file to save.
     * @param currentCaveIndex Index of the currently active cave.
     */
    void saveFileAs(Cave::Map& map, Cave::File& loadedFile, int currentCaveIndex);

    // ----------------------------------------------------------------------------------
    // Public members
    // ----------------------------------------------------------------------------------

    /// @brief Embedded game instance used for cave rendering and property management.
    Game             game;

    /// @brief Editor-specific cave settings (animation, random dist, etc.).
    Cave::Settings   settings;

    /// @brief Shared image manager for all textures used in the editor.
    Image::Manager   imageManager;

private:

    // ----------------------------------------------------------------------------------
    // Deferred action flags
    // ----------------------------------------------------------------------------------

    /// @brief Set by actionNewFile(); handled each frame in run().
    bool m_doNewFile       = false;

    /// @brief Set by actionOpenFile(); handled each frame in run().
    bool m_doOpenFile      = false;

    /// @brief Set by actionSaveFile(); handled each frame in run().
    bool m_doSaveFile      = false;

    /// @brief Set by actionSaveFileAs(); handled each frame in run().
    bool m_doSaveFileAs    = false;

    /// @brief Set by actionExit(); handled each frame in run().
    bool m_doExit          = false;

    /// @brief Set by actionNextLevel(); handled each frame in run().
    bool m_doNextLevel     = false;

    /// @brief Set by actionPrevLevel(); handled each frame in run().
    bool m_doPrevLevel     = false;

    /// @brief Set by actionInsertLevel(); handled each frame in run().
    bool m_doInsertLevel   = false;

    /// @brief Set by actionDeleteLevel(); handled each frame in run().
    bool m_doDeleteLevel   = false;

    /// @brief Set by actionClearLevel(); handled each frame in run().
    bool m_doClearLevel    = false;

    /// @brief Set by actionCopyLevel(); handled each frame in run().
    bool m_doCopyLevel     = false;

    /// @brief Set by actionPasteLevel(); handled each frame in run().
    bool m_doPasteLevel    = false;

    /// @brief Set by actionRandomDist(); handled each frame in run().
    bool m_doRandomDist    = false;

    /// @brief Set by actionShowSettings(); handled each frame in run().
    bool m_doShowSettings  = false;

    /// @brief Set by actionShowCaveProperties(); handled each frame in run().
    bool m_doShowCaveProps = false;

    /// @brief Whether small (16px) block mode is active.
    bool m_smallBlocks     = false;

    /// @brief Set by actionUndo(); handled each frame in run().
    bool m_doUndo          = false;

    /// @brief Set by actionTest(); handled each frame in run().
    bool m_doTest          = false;

    // ----------------------------------------------------------------------------------
    // Undo state
    // ----------------------------------------------------------------------------------

    /// @brief Tile data snapshot taken before the last edit, used to restore on undo.
    std::vector<char> m_undoTileData;

    /// @brief True when an undo snapshot is available to apply; cleared after use.
    bool m_undoAvailable = false;

    // ----------------------------------------------------------------------------------
    // File state
    // ----------------------------------------------------------------------------------

    /// @brief Full path to the last successfully saved file, empty if never saved.
    std::string m_savedFilePath;

    /// @brief True if there are unsaved edits since the last save or file load.
    bool m_isDirty = false;

    /// @brief True when developer mode is active (toggled with Ctrl+D).
    bool m_developerMode = false;

    /// @brief Tile data copied by copyLevel(), pasted by pasteLevel().
    std::vector<char> m_clipboardTileData;

    // ----------------------------------------------------------------------------------
    // Internal helpers
    // ----------------------------------------------------------------------------------

    /// @brief Pointer to the editor's render window, set at the start of run().
    sf::RenderWindow* m_window = nullptr;

    /// @brief Handle keyboard shortcut events.
    /// @param e The key-pressed event to process.
    void handleShortcuts(const sf::Event::KeyPressed& e, HUD::Editor::Panel* panel);

    /**
     * @brief Check all caves for missing start/exit doors and show a warning per issue.
     *
     * Called after a successful save so the user is informed of any problems
     * without blocking the save itself.
     *
     * @param loadedFile The file whose caves will be checked.
     * @param window     Owner window for the warning dialogs.
     */
    void checkSaveWarnings(const Cave::File& loadedFile);

    /**
     * @brief Prompt the user to save if there are unsaved changes.
     *
     * Shows a Yes/No/Cancel dialog. Yes triggers a save; No skips it.
     * Cancel causes the caller to abort its operation.
     *
     * @param map              The live cave map (needed if save is triggered).
     * @param loadedFile       The current file.
     * @param currentCaveIndex Index of the active cave.
     * @param window           Owner window for the dialogs.
     * @return true if the caller should proceed, false if the user cancelled.
     */
    bool checkUnsavedChanges(Cave::Map& map, Cave::File& loadedFile, int currentCaveIndex);
};
