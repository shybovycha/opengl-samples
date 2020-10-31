cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(game VERSION 1.0.1 LANGUAGES CXX)

set(SOURCES
    "src/main.cpp"
    "src/core/Settings.h"
    "src/game/Level.h"
    "src/game/Score.h"
    "src/game/PlayerState.h"
    "src/game/GameState.h"
    "src/core/ActionDispatcher.h"
    "src/renderer/IrrlichtEventReceiver.h"
    "src/core/Renderer.h"
    "src/core/ResourceManager.h"
    "src/core/ModernResourceManager.h"
    "src/renderer/IrrlichtRenderer.h"
    "src/core/Application.h"
    "src/core/QueueAction.h"
    "src/core/Application.cpp"
    "src/game/GameState.cpp"
    "src/core/ActionDispatcher.cpp"
    "src/renderer/IrrlichtEventReceiver.cpp"
    "src/renderer/IrrlichtRenderer.cpp"
    "src/game/Level.cpp"
    "src/core/ModernResourceManager.cpp"
    "src/game/PlayerState.cpp"
    "src/game/Score.cpp"
    "src/core/QueueAction.cpp"
    "src/renderer/IrrlichtHUD.h"
    "src/renderer/IrrlichtHUD.cpp"
    "src/renderer/CScreenQuadSceneNode.h"
    "src/renderer/CScreenQuadSceneNode.cpp"
    "src/renderer/CDrunkShaderCallback.h"
    "src/renderer/CDrunkShaderCallback.cpp"
    "src/core/Vector3.h"
    "src/core/Vector3.cpp"
    "src/core/SceneNode.h"
    "src/core/SceneNode.cpp"
    "src/renderer/IrrlichtSceneNode.h"
    "src/renderer/IrrlichtSceneNode.cpp")

set(EXECUTABLE_NAME game)

set(LIBRARIES
        ${X11_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${ZLIB_LIBRARIES})

if(NOT APPLE)
    set(LIBRARIES ${LIBRARIES} ${X11_Xxf86vm_LIB})
endif()

if(APPLE)
    find_library(IOKIT NAMES IOKit)
    find_library(OpenGL NAMES OpenGL)
    find_library(COREVIDEO NAMES CoreVideo)
    find_library(COCOA NAMES Cocoa)
    set(LIBRARIES ${LIBRARIES} ${IOKIT} ${COREVIDEO} ${COCOA})
endif()

if(APPLE)
    file(GLOB RESOURCE_FILES resources)
    set_source_files_properties(${RESOURCE_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "MacOS")

    add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${SOURCES} ${RESOURCE_FILES})
else()
    add_executable(${EXECUTABLE_NAME} ${SOURCES})

    # file(COPY resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    # file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

add_dependencies(${EXECUTABLE_NAME} tinyxml2 irrlicht irrklang)

target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${LIBRARIES} tinyxml2 irrlicht irrklang)

# Copy libraries' DLLs
add_custom_command(
    TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:tinyxml2> $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
)

if (WIN32)
    if (EX_PLATFORM EQUAL "64")
        set(IRRKLANG_BIN_PATH "${irrklang_SOURCE_DIR}/bin/winx64-visualStudio")
    else()
        set(IRRKLANG_BIN_PATH "${irrklang_SOURCE_DIR}/bin/win32-visualStudio")
    endif()

    message("IrrKlang DLLs: ${IRRKLANG_BIN_PATH}/irrKlang.dll ${IRRKLANG_BIN_PATH}/ikpFlac.dll ${IRRKLANG_BIN_PATH}/ikpMP3.dll")

    add_custom_command(
        TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${IRRKLANG_BIN_PATH}/irrKlang.dll $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
    )

    add_custom_command(
        TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${IRRKLANG_BIN_PATH}/ikpFlac.dll $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
    )

    add_custom_command(
        TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${IRRKLANG_BIN_PATH}/ikpMP3.dll $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
    )
endif()

# Copy resources
add_custom_command(
    TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/resources
)

add_custom_command(
    TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/data $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/data
)
