add_executable(DiggingJimBuilder
    src/main.cpp
    src/Editor/Editor.cpp
    src/HUD/Panel.cpp
    src/HUD/Toolbar.cpp
    ${CMAKE_SOURCE_DIR}/game/src/Game/Game.cpp
    ${CMAKE_SOURCE_DIR}/game/src/Game/Settings.cpp
    ${CMAKE_SOURCE_DIR}/lib/tinyfiledialogs/tinyfiledialogs.c
    $<$<BOOL:${WIN32}>:res/builder.rc>
)

target_compile_features(DiggingJimBuilder PRIVATE cxx_std_17)

target_include_directories(DiggingJimBuilder PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/game/include
    ${CMAKE_SOURCE_DIR}/lib/include
    ${CMAKE_SOURCE_DIR}/lib/tinyfiledialogs
)

target_link_libraries(DiggingJimBuilder PRIVATE DiggingJimLib)
if(WIN32)
    target_link_libraries(DiggingJimBuilder PRIVATE dwmapi)
    target_link_options(DiggingJimBuilder PRIVATE
        $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup>
        $<$<AND:$<CONFIG:Release>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-mwindows>
    )
endif()

add_custom_command(
    TARGET DiggingJimBuilder POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:DiggingJimBuilder>/assets
    COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:DiggingJimBuilder>/caves
)
