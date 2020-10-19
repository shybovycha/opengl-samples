cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(shoot-them VERSION 1.0.1 LANGUAGES CXX)

include(FetchContent)

if (APPLE)
    set(MAKE_COMMAND make)
    # find_program(MAKE_COMMAND NAMES gmake nmake make)
endif()

if (UNIX)
    set(MAKE_COMMAND make)
    # find_program(MAKE_COMMAND NAMES gmake nmake make)
endif()

if(WIN32)
    set(MAKE_COMMAND "")
endif()

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

    FetchContent_Declare(irrlicht
        URL ${CMAKE_CURRENT_LIST_DIR}/dependencies/irrlicht-1.8.4.zip
    )

    FetchContent_GetProperties(irrlicht)

    if(NOT irrlicht_POPULATED)
        FetchContent_Populate(irrlicht)
    endif()

    set(IRRLICHT_PATH ${irrlicht_SOURCE_DIR})

    add_library(irrlicht SHARED IMPORTED GLOBAL)

    set_target_properties(
        irrlicht PROPERTIES
        IMPORTED_LOCATION ${IRRLICHT_PATH}/lib/${IRRLICHT_PATH_SUFFIX}/Irrlicht${CMAKE_LINK_LIBRARY_SUFFIX}
        IMPORTED_IMPLIB ${IRRLICHT_PATH}/lib/${IRRLICHT_PATH_SUFFIX}/Irrlicht${CMAKE_IMPORT_LIBRARY_SUFFIX}
        INTERFACE_INCLUDE_DIRECTORIES ${IRRLICHT_PATH}/include
    )
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
            set(IRRKLANG_PATH_SUFFIX Winx64-visualStudio)
        endif()
    endif()

    FetchContent_Declare(irrklang
        URL ${CMAKE_CURRENT_LIST_DIR}/dependencies/irrKlang-${EX_PLATFORM}bit-1.5.0.zip
    )

    FetchContent_GetProperties(irrklang)

    if(NOT irrklang_POPULATED)
        FetchContent_Populate(irrklang)
    endif()

    set(IRRKLANG_PATH ${irrklang_SOURCE_DIR})

    add_library(irrklang SHARED IMPORTED GLOBAL)

    set_target_properties(
        irrklang PROPERTIES
        IMPORTED_LOCATION ${IRRKLANG_PATH}/lib/${IRRKLANG_PATH_SUFFIX}/irrKlang${CMAKE_LINK_LIBRARY_SUFFIX}
        IMPORTED_IMPLIB ${IRRKLANG_PATH}/lib/${IRRKLANG_PATH_SUFFIX}/irrKlang${CMAKE_IMPORT_LIBRARY_SUFFIX}
        INTERFACE_INCLUDE_DIRECTORIES ${IRRKLANG_PATH}/include
    )
endif()

FetchContent_Declare(tinyxml2
    URL ${CMAKE_CURRENT_LIST_DIR}/dependencies/tinyxml2-master.zip
)

FetchContent_MakeAvailable(tinyxml2)

add_subdirectory(game)
add_subdirectory(editor)
