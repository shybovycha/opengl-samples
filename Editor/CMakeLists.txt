cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Editor VERSION 1.0.1 LANGUAGES CXX)

set(EXECUTABLE_NAME Editor)

set(SOURCES
    "Sources/main.cpp" "Sources/ApplicationDelegate.h" "Sources/ApplicationDelegate.cpp" "Sources/IrrlichtEventReceiver.h" "Sources/IrrlichtEventReceiver.cpp" "Sources/Application.h" "Sources/Application.cpp")

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

add_dependencies(${EXECUTABLE_NAME} tinyxml2 irrlicht)

target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${LIBRARIES} tinyxml2 irrlicht)

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

# Copy resources
add_custom_command(
    TARGET ${EXECUTABLE_NAME} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/Resources $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/Resources
)
