cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(game VERSION 1.0.1 LANGUAGES CXX)

set(SOURCES
    "src/main.cpp"
    "src/Settings.h"
    "src/Level.h"
    "src/Score.h"
    "src/PlayerState.h"
    "src/GameState.h"
    "src/ActionDispatcher.h"
    "src/IrrlichtEventReceiver.h"
    "src/Renderer.h"
    "src/ResourceManager.h"
    "src/ModernResourceManager.h"
    "src/IrrlichtRenderer.h"
    "src/Application.h"
    "src/QueueAction.h"
    "src/Application.cpp"
    "src/GameState.cpp"
    "src/ActionDispatcher.cpp"
    "src/IrrlichtEventReceiver.cpp"
    "src/IrrlichtRenderer.cpp"
    "src/Level.cpp"
    "src/ModernResourceManager.cpp"
    "src/PlayerState.cpp"
    "src/Score.cpp"
    "src/QueueAction.cpp"
    "src/IrrlichtHUD.h"
    "src/IrrlichtHUD.cpp"
    "src/CScreenQuadSceneNode.h"
    "src/CScreenQuadSceneNode.cpp"
    "src/CDrunkShaderCallback.h"
    "src/CDrunkShaderCallback.cpp"
    "src/Vector3.cpp"
    "src/Vector3.h")

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

target_precompile_headers(
    ${EXECUTABLE_NAME}
    PRIVATE
    "src/stdafx.h"
)

# Copy resources
add_custom_command(
    TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/resources
)

add_custom_command(
    TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/data $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/data
)
