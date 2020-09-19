cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(shoot-them VERSION 1.0.1 LANGUAGES CXX)

include(ExternalProject)

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

    ExternalProject_Add(irrlicht-dep
        URL ${CMAKE_CURRENT_LIST_DIR}/Dependencies/irrlicht-1.8.4.zip
        PREFIX Dependencies/irrlicht
        SOURCE_SUBDIR source/Irrlicht
        CONFIGURE_COMMAND ""
        BUILD_COMMAND "${MAKE_COMMAND}"
        INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property(irrlicht-dep SOURCE_DIR)
    set(IRRLICHT_PATH ${SOURCE_DIR})

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

    ExternalProject_Add(irrklang-dep
        URL ${CMAKE_CURRENT_LIST_DIR}/Dependencies/irrKlang-64bit-1.5.0.zip
        PREFIX Dependencies/irrklang
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property(irrklang-dep SOURCE_DIR)
    set(IRRKLANG_PATH ${SOURCE_DIR})

    add_library(irrklang SHARED IMPORTED GLOBAL)

    set_target_properties(
        irrklang PROPERTIES
        IMPORTED_LOCATION ${IRRKLANG_PATH}/lib/${IRRKLANG_PATH_SUFFIX}/irrKlang${CMAKE_LINK_LIBRARY_SUFFIX}
        IMPORTED_IMPLIB ${IRRKLANG_PATH}/lib/${IRRKLANG_PATH_SUFFIX}/irrKlang${CMAKE_IMPORT_LIBRARY_SUFFIX}
        INTERFACE_INCLUDE_DIRECTORIES ${IRRKLANG_PATH}/include
    )
endif()

ExternalProject_Add(tinyxml2-dep
    URL ${CMAKE_CURRENT_LIST_DIR}/Dependencies/tinyxml2-master.zip
    PREFIX Dependencies/tinyxml2
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(tinyxml2-dep SOURCE_DIR)
set(TINYXML2_PATH ${SOURCE_DIR})

add_subdirectory(${TINYXML2_PATH})

add_subdirectory(Game)
add_subdirectory(Editor)
