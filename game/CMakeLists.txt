add_executable(DiggingJim
    src/main.cpp
    src/Game/Game.cpp
    src/Game/Settings.cpp
    $<$<BOOL:${WIN32}>:res/game.rc>
)

target_include_directories(DiggingJim PRIVATE include)

target_link_libraries(DiggingJim PRIVATE DiggingJimLib)

if(WIN32)
    target_link_options(DiggingJim PRIVATE
        $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup>
        $<$<AND:$<CONFIG:Release>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-mwindows>
    )
endif()

add_custom_command(
    TARGET DiggingJim POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:DiggingJim>/assets
    COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:DiggingJim>/caves
)
