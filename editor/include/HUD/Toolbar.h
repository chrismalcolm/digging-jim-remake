#pragma once
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include "HUD/Panel.h"
#include <string>

class Editor;

namespace HUD::Editor {

    /**
     * @class Toolbar
     * @brief The main menu bar rendered at the top of the editor window.
     *
     * Toolbar draws an ImGui main menu bar containing the File, Edit, View, Tools,
     * and Help menus. Each menu item either calls an action on the owning Editor
     * or mutates local display state (e.g. the About box flag). The toolbar also
     * owns the About dialog and loads its text from builder.txt at construction.
     *
     * draw() must be called once per frame inside an active ImGui frame.
     */
    class Toolbar {
    public:

        // ----------------------------------------------------------------------------------
        // Construction
        // ----------------------------------------------------------------------------------

        /**
         * @brief Construct the Toolbar and load the About text from builder.txt.
         * @param editor Pointer to the owning Editor instance.
         */
        Toolbar(::Editor* editor);

        // ----------------------------------------------------------------------------------
        // Drawing
        // ----------------------------------------------------------------------------------

        /**
         * @brief Draw the full menu bar and any open popup dialogs.
         *
         * Must be called once per frame within an active ImGui frame.
         * Forwards user interactions to the Editor via action*() calls.
         *
         * @param editorPanel Pointer to the editor panel, passed to the Tools menu
         *                    for animation and fill-mode state.
         */
        void draw(HUD::Editor::Panel* editorPanel, bool developerMode = false);

        /**
         * @brief Render the tools list as a floating popup at the current position.
         *
         * Call ImGui::SetNextWindowPos and ImGui::OpenPopup("##tools_ctx") before
         * this, then call this every frame — it internally calls BeginPopup/EndPopup.
         *
         * @param editorPanel Used to read and set the selected tile type.
         */
        void drawToolsContextPopup(HUD::Editor::Panel* editorPanel);

    private:

        // ----------------------------------------------------------------------------------
        // Menu drawing helpers
        // ----------------------------------------------------------------------------------

        /**
         * @brief Draw a highlight border around a menu bar item.
         * @param r0      Top-left corner of the item rect.
         * @param r1      Bottom-right corner of the item rect.
         * @param pressed Whether the menu is currently open (affects border style).
         */
        void drawMenuBorder(ImVec2 r0, ImVec2 r1, bool pressed);

        /// @brief Draw the File menu and handle its items.
        void drawFileMenu();

        /// @brief Draw the Edit menu and handle its items.
        void drawEditMenu();

        /// @brief Draw the View menu and handle its items.
        void drawViewMenu();

        /**
         * @brief Draw the Tools menu and handle its items.
         * @param editorPanel Used to read and set animation and fill-mode state.
         */
        void drawToolsMenu(HUD::Editor::Panel* editorPanel);

        /// @brief Render the shared tool item list into the current ImGui window/menu.
        void drawToolItems(HUD::Editor::Panel* editorPanel);

        /// @brief Draw the Help menu and handle its items.
        void drawHelpMenu();

        /// @brief Show the About message box if m_showAbout was set this frame.
        void drawAboutPopup();

        // ----------------------------------------------------------------------------------
        // Members
        // ----------------------------------------------------------------------------------

        /// @brief Pointer to the owning Editor instance.
        ::Editor* m_editor;

        /// @brief Background colour used for hovered menu bar items.
        ImVec4 m_clrBg   = {212/255.f, 208/255.f, 200/255.f, 1.f};

        /// @brief Highlight colour used for hovered items inside open menus.
        ImVec4 m_clrNavy = {150/255.f, 190/255.f, 235/255.f, 1.f};

        /// @brief Current DPI/zoom scale factor (reserved for future use).
        float m_menuScale  = 1.0f;

        /// @brief Indent applied to menu items to align them inside the menu popup.
        float m_menuIndent = 0.f;

        /// @brief Set to true when the user clicks Help → About; consumed by drawAboutPopup().
        bool m_showAbout = false;

        /// @brief Text loaded from builder.txt, displayed in the About message box.
        std::string m_aboutText;
    };
}
