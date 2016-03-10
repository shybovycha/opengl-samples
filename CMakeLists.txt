cmake_minimum_required(VERSION 3.1)
project(shoot-them)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(IRRLICHT_PATH Dependencies/irrlicht-1.5.2)
set(IRRKLANG_PATH Dependencies/irrKlang-1.5.0)

find_package(X11)
find_package(OpenGL)
find_package(ZLIB)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message("64 bit platform detected")
    SET(EX_PLATFORM 64)
    SET(EX_PLATFORM_NAME "x64")
else()
    message("32 bit platform detected")
    SET(EX_PLATFORM 32)
    SET(EX_PLATFORM_NAME "x86")
endif()

if (NOT IRRLICHT_LIBRARY_PATH)
    if (UNIX)
        set(IRRLICHT_PATH_SUFFIX Linux)
    endif()

    if (APPLE)
        set(IRRLICHT_PATH_SUFFIX MacOSX)
    endif()

    if (WIN32)
        if (MSVC)
            set(IRRLICHT_PATH_SUFFIX Win32-visualstudio Win64-visualstudio)
        endif()

        if (MINGW)
            set(IRRLICHT_PATH_SUFFIX Win32-gcc)
        endif()
    endif()

    find_library(IRRLICHT_LIBRARY_PATH
            NAMES Irrlicht
            PATHS ${IRRLICHT_PATH}/lib/
            PATH_SUFFIXES ${IRRLICHT_PATH_SUFFIX})

    message(STATUS "Found Irrlicht: ${IRRLICHT_LIBRARY_PATH}")
endif()

if (NOT IRRKLANG_LIBRARY_PATH)
    if (UNIX)
        set(IRRKLANG_PATH_SUFFIX linux-gcc)
    endif()

    if (APPLE)
        set(IRRKLANG_PATH_SUFFIX macosx-gcc)
    endif()

    if (WIN32)
        if (MSVC)
            set(IRRKLANG_PATH_SUFFIX winx64 dotnet-4)
        endif()
    endif()

    if (EX_PLATFORM EQUAL 64)
        if (NOT APPLE)
            set(IRRKLANG_PATH_SUFFIX ${IRRKLANG_PATH_SUFFIX}-64)
        endif()
    endif()

    message(STATUS "Looking for IrrKlang in ${IRRKLANG_PATH}/bin with ${IRRKLANG_PATH_SUFFIX}")

    find_library(IRRKLANG_LIBRARY_PATH
            NAMES irrklang
            PATHS ${IRRKLANG_PATH}/bin/
            PATH_SUFFIXES ${IRRKLANG_PATH_SUFFIX})

    message(STATUS "Found IrrKlang: ${IRRKLANG_LIBRARY_PATH}")
endif()

set(LIBRARIES
        ${IRRLICHT_LIBRARY_PATH}
        ${IRRKLANG_LIBRARY_PATH}
        ${X11_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${ZLIB_LIBRARIES})

if (NOT APPLE)
    set(LIBRARIES ${LIBRARIES} ${X11_Xxf86vm_LIB})
endif()

include_directories(${IRRLICHT_PATH}/include)
include_directories(${IRRKLANG_PATH}/include)

set(SOURCE_FILES Game/Sources/main.cpp)
set(EXECUTABLE_NAME shoot-them)

if (APPLE)
    find_library(IOKIT NAMES IOKit)
    find_library(OpenGL NAMES OpenGL)
    find_library(COREVIDEO NAMES CoreVideo)
    find_library(COCOA NAMES Cocoa)
    SET(LIBRARIES ${LIBRARIES} ${IOKIT} ${COREVIDEO} ${COCOA})
endif()

if (APPLE)
    add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${SOURCE_FILES})
else()
    add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})
endif()

target_link_libraries(${EXECUTABLE_NAME} ${LIBRARIES})

file(COPY Game/Data DESTINATION ${PROJECT_BINARY_DIR})
file(COPY Game/Resources DESTINATION ${PROJECT_BINARY_DIR})