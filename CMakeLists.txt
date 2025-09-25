cmake_minimum_required(VERSION 3.28)
project(DiggingJim LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(include)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

set(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "" FORCE)

# Fetch Dear ImGui
FetchContent_Declare(
    ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.1
)
FetchContent_MakeAvailable(ImGui)

# Tell CMake before ImGui-SFML config runs
set(IMGUI_DIR ${imgui_SOURCE_DIR} CACHE PATH "imgui top-level directory" FORCE)

# Fetch ImGui-SFML
FetchContent_Declare(
    ImGui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG v3.0
)
FetchContent_MakeAvailable(ImGui-SFML)

add_executable(main
    src/main.cpp
    src/Input/Input.cpp
    src/HUD/Developer/FPSCounter.cpp
    src/HUD/Editor/Panel.cpp
    src/Cave/Map/Map.cpp
    src/Cave/Map/Movement.cpp
    src/Cave/Map/Update.cpp
    src/Cave/Map/Camera.cpp
    src/Cave/Map/Enemy.cpp
    src/Cave/Map/Static.cpp
    src/Cave/Map/Falling.cpp
    src/Cave/Map/Jim.cpp
    src/Cave/Map/Entity.cpp
    src/Cave/Map/Misc.cpp
    src/Renderer/TileRenderer.cpp
    src/Utils/Counter.cpp
    src/Sound/Manager.cpp
    src/HUD/Level/Panel.cpp
    src/Renderer/TextRenderer.cpp
    src/Game/Game.cpp
    src/Cave/Manager/Manager.cpp
    src/Shader/Shader.cpp
    src/Image/Manager.cpp
    src/HUD/Developer/PositionDisplay.cpp
    src/Camera/Camera.cpp
    src/HUD/MainMenu/MainMenu.cpp
    src/HUD/Developer/CameraCenter.cpp
    src/Cave/Properties/Properties.cpp
    src/Cave/Manager/File.cpp
    src/Cave/Manager/Data.cpp
    src/Game/Settings.cpp)

set_target_properties(main PROPERTIES OUTPUT_NAME "DiggingJim")
target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics SFML::Audio ImGui-SFML::ImGui-SFML)

add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)