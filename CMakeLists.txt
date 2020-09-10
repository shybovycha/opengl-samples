cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(shoot-them VERSION 1.0.1 LANGUAGES CXX)

set(IRRLICHT_PATH ${CMAKE_CURRENT_LIST_DIR}/Dependencies/irrlicht-1.8.3)
set(IRRKLANG_PATH ${CMAKE_CURRENT_LIST_DIR}/Dependencies/irrKlang-1.5.0)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message("64 bit platform detected")
    set(EX_PLATFORM 64)
    set(EX_PLATFORM_NAME "x64")
else()
    message("32 bit platform detected")
    set(EX_PLATFORM 32)
    set(EX_PLATFORM_NAME "x86")
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
            set(IRRLICHT_PATH_SUFFIX Win32-visualstudio Win64-visualstudio)
        endif()

        if(MINGW)
            set(IRRLICHT_PATH_SUFFIX Win32-gcc)
        endif()
    endif()

    find_library(IRRLICHT_LIBRARY_PATH
            Irrlicht
            PATHS ${IRRLICHT_PATH}/lib/
            PATH_SUFFIXES ${IRRLICHT_PATH_SUFFIX})

    set(IRRLICHT_PATH_SUFFIX ${CMAKE_CURRENT_LIST_DIR}/${IRRLICHT_PATH_SUFFIX})

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

    message(STATUS "Looking for IrrKlang in ${IRRKLANG_PATH}/bin with ${IRRKLANG_PATH_SUFFIX}")

    find_library(IRRKLANG_LIBRARY_PATH
            irrKlang
            PATHS ${IRRKLANG_PATH}/lib/
            PATH_SUFFIXES ${IRRKLANG_PATH_SUFFIX})

    set(IRRKLANG_PATH ${CMAKE_CURRENT_LIST_DIR}/${IRRKLANG_LIBRARY_PATH})

    message("Found IrrKlang: ${IRRKLANG_LIBRARY_PATH}")
endif()

add_subdirectory(Game)
