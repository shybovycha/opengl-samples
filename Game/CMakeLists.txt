cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Game VERSION 1.0.1 LANGUAGES CXX)

set(SOURCES
    "Sources/main.cpp" 
    "Sources/Settings.h" 
    "Sources/Level.h"
    "Sources/Score.h"
    "Sources/PlayerState.h"
    "Sources/GameState.h"
    "Sources/ActionDispatcher.h"
    "Sources/IrrlichtEventReceiver.h"
    "Sources/Renderer.h"
    "Sources/ResourceManager.h"
    "Sources/ModernResourceManager.h"
    "Sources/IrrlichtRenderer.h"
    "Sources/Application.h"
    "Sources/QueueAction.h"
    "Sources/Application.cpp"
    "Sources/GameState.cpp"
    "Sources/ActionDispatcher.cpp"
    "Sources/IrrlichtEventReceiver.cpp"
    "Sources/IrrlichtRenderer.cpp"
    "Sources/Level.cpp"
    "Sources/ModernResourceManager.cpp"
    "Sources/PlayerState.cpp"
    "Sources/Score.cpp"
    "Sources/QueueAction.cpp"
    "Sources/IrrlichtHUD.h"
    "Sources/IrrlichtHUD.cpp")

set(EXECUTABLE_NAME Game)

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
    file(GLOB RESOURCE_FILES Resources)
    set_source_files_properties(${RESOURCE_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "MacOS")

    add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${SOURCE_FILES} ${RESOURCE_FILES})
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

add_dependencies(${EXECUTABLE_NAME} tinyxml2 irrlicht irrklang)

target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${LIBRARIES} tinyxml2 irrlicht irrklang)

# Copy libraries' DLLs
add_custom_command(
    TARGET ${EXECUTABLE_NAME} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:tinyxml2> $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
)

# TODO: copy DLLs, not LIBs
# add_custom_command(
#     TARGET ${EXECUTABLE_NAME} POST_BUILD 
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:irrlicht> $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
# )

# add_custom_command(
#     TARGET ${EXECUTABLE_NAME} POST_BUILD 
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:irrklang> $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
# )

# Copy resources
add_custom_command(
    TARGET ${EXECUTABLE_NAME} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/Resources $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/Resources
)

add_custom_command(
    TARGET ${EXECUTABLE_NAME} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/Data $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/Data
)
