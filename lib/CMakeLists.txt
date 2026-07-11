
add_library(DiggingJimLib STATIC
    src/Camera/Camera.cpp
    src/Cave/Manager/Data.cpp
    src/Cave/Manager/File.cpp
    src/Cave/Manager/Manager.cpp
    src/Cave/Map/Camera.cpp
    src/Cave/Map/Enemy.cpp
    src/Cave/Map/Entity.cpp
    src/Cave/Map/Falling.cpp
    src/Cave/Map/Jim.cpp
    src/Cave/Map/Map.cpp
    src/Cave/Map/Misc.cpp
    src/Cave/Map/Movement.cpp
    src/Cave/Map/Static.cpp
    src/Cave/Map/Update.cpp
    src/Cave/Properties/Properties.cpp
    src/Cave/Settings/Settings.cpp
    src/HUD/Developer/CameraCenter.cpp
    src/HUD/Developer/FPSCounter.cpp
    src/HUD/Developer/PositionDisplay.cpp
    src/HUD/Level/Panel.cpp
    src/HUD/MainMenu/MainMenu.cpp
    src/Image/Manager.cpp
    src/Input/Input.cpp
    src/Renderer/TextRenderer.cpp
    src/Renderer/TileRenderer.cpp
    src/Shader/Shader.cpp
    src/Sound/Manager.cpp
    src/Utils/Counter.cpp
 )

target_compile_features(DiggingJimLib PUBLIC cxx_std_17)

target_include_directories(DiggingJimLib PUBLIC include ${CMAKE_SOURCE_DIR}/game/include)

target_link_libraries(DiggingJimLib PUBLIC SFML::Graphics SFML::Audio ImGui-SFML::ImGui-SFML wx::wxcore wx::wxbase)
