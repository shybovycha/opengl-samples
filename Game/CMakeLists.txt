cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Game VERSION 1.0.1 LANGUAGES CXX)

set(SOURCE_FILES Sources/main.cpp)
set(EXECUTABLE_NAME Game)

if(NOT IRRLICHT_LIBRARY_PATH)
    message("[ERROR]" "Found Irrlicht: ${IRRLICHT_LIBRARY_PATH}")
endif()

if(NOT IRRKLANG_LIBRARY_PATH)
    message("[ERROR]" "Found IrrKlang: ${IRRKLANG_LIBRARY_PATH}")
endif()

set(LIBRARIES
        ${IRRLICHT_LIBRARY_PATH}
        ${IRRKLANG_LIBRARY_PATH}
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
    add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

    file(COPY Resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY Data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

message("Found Irrlicht at: ${IRRLICHT_LIBRARY_PATH}")
message("Found IrrKlang at: ${IRRKLANG_LIBRARY_PATH}")
message("Linking libraries: ${LIBRARIES}")

target_include_directories(${EXECUTABLE_NAME} PRIVATE ${IRRLICHT_PATH}/include)
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${IRRKLANG_PATH}/include)

target_link_libraries(${EXECUTABLE_NAME} ${LIBRARIES})
