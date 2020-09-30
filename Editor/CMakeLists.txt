cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Editor VERSION 1.0.1 LANGUAGES CXX)

set(EXECUTABLE_NAME Editor)

set(SOURCES
    "Sources/main.cpp"
    "Sources/ApplicationDelegate.h"
    "Sources/ApplicationDelegate.cpp"
    "Sources/IrrlichtEventReceiver.h"
    "Sources/IrrlichtEventReceiver.cpp"
    "Sources/Application.h"
    "Sources/Application.cpp"
    "Sources/Level.h"
    "Sources/Level.cpp"
    "Sources/GameManagerNodeData.h"
    "Sources/GameManagerNodeData.cpp"
    "Sources/Target.h"
    "Sources/Target.cpp"
    "Sources/GameData.h"
    "Sources/GameData.cpp"
    "Sources/GameManagerTree.h"
    "Sources/GameManagerTree.cpp"
    "Sources/GUIElementId.h"
    "Sources/CameraSceneNodeAnimator.h"
    "Sources/CameraSceneNodeAnimator.cpp"
    "Sources/Light.h"
    "Sources/Light.cpp"
    "Sources/LevelEntity.h"
    "Sources/LevelEntity.cpp"
    "Sources/SaveFileDialog.h"
    "Sources/SaveFileDialog.cpp")

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
    add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${SOURCE_FILES})
else()
    add_executable(${EXECUTABLE_NAME} ${SOURCES})

    # file(COPY Resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    # file(COPY Data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

find_package(irrlicht CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LIBRARIES} tinyxml2::tinyxml2 Irrlicht)

# Copy resources
add_custom_command(
    TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/Resources $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/Resources
)
