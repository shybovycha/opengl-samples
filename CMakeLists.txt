cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(shoot-them VERSION 1.0.1 LANGUAGES CXX)

include(ExternalProject)

find_program(MAKE_EXE NAMES gmake nmake make)

ExternalProject_Add(irrlicht-dep
    URL ${CMAKE_CURRENT_LIST_DIR}/Dependencies/irrlicht-1.8.3.zip
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/Dependencies
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${MAKE_EXE}
    SOURCE_SUBDIR source/Irrlicht
)

ExternalProject_Add(irrklang-dep
    URL ${CMAKE_CURRENT_LIST_DIR}/Dependencies/irrKlang-64bit-1.5.0.zip
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/Dependencies
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)

ExternalProject_Add(tinyxml2-dep
    URL ${CMAKE_CURRENT_LIST_DIR}/Dependencies/tinyxml2-master.zip
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/Dependencies
)

set(IRRLICHT_PATH ${CMAKE_CURRENT_LIST_DIR}/Dependencies/irrlicht-1.8.3)
set(IRRKLANG_PATH ${CMAKE_CURRENT_LIST_DIR}/Dependencies/irrKlang-1.5.0)

if(CMAKE_GENERATOR_PLATFORM EQUAL Win32)
    message("32 bit platform detected")
    set(EX_PLATFORM 32)
    set(EX_PLATFORM_NAME "x86")
else()
    message("64 bit platform detected")
    set(EX_PLATFORM 64)
    set(EX_PLATFORM_NAME "x64")
endif()

if(NOT IRRLICHT_LIBRARY_PATH)
    if(UNIX)
        set(IRRLICHT_PATH_SUFFIX Linux)
    endif()

    if(APPLE)
        set(IRRLICHT_PATH_SUFFIX MacOSX)
    endif()

    if(WIN32)
        if(MSVC)
            if(EX_PLATFORM EQUAL "32")
                set(IRRLICHT_PATH_SUFFIX Win32-visualstudio)
            else()
                set(IRRLICHT_PATH_SUFFIX Win64-visualStudio)
            endif()
        endif()

        if(MINGW)
            set(IRRLICHT_PATH_SUFFIX Win32-gcc)
        endif()
    endif()

    find_library(IRRLICHT_LIBRARY_PATH
            Irrlicht
            PATHS ${IRRLICHT_PATH}/lib/
            PATH_SUFFIXES ${IRRLICHT_PATH_SUFFIX})

    message("Found Irrlicht: ${IRRLICHT_LIBRARY_PATH}")
endif()

if(NOT IRRKLANG_LIBRARY_PATH)
    if(UNIX)
        set(IRRKLANG_PATH_SUFFIX linux-gcc)
    endif()

    if(APPLE)
        set(IRRKLANG_PATH_SUFFIX macosx-gcc)
    endif()

    if(WIN32)
        if(MSVC)
            set(IRRKLANG_PATH_SUFFIX Winx64-visualStudio dotnet-4)
        endif()
    endif()

    find_library(IRRKLANG_LIBRARY_PATH
            irrKlang
            PATHS ${IRRKLANG_PATH}/lib/
            PATH_SUFFIXES ${IRRKLANG_PATH_SUFFIX})

    message("Found IrrKlang: ${IRRKLANG_LIBRARY_PATH}")
endif()

add_subdirectory(Dependencies/tinyxml2)
add_subdirectory(Game)
