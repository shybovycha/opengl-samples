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

add_subdirectory(Game)
add_subdirectory(Editor)
