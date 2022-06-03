cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

set(BUILD_WITH_EASY_PROFILER ON)
set(EASY_PROFILER_NO_GUI ON)
set(BUILD_SHARED_LIBS OFF)
set(EASY_OPTION_LOG ON)

include(FetchContent)

FetchContent_Declare(
    easy_profiler
    GIT_REPOSITORY https://github.com/yse/easy_profiler.git
    GIT_TAG "v2.1.0"
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    SOURCE_DIR   "${FETCHCONTENT_BASE_DIR}/easy_profiler/easy_profiler"
	BINARY_DIR   "${FETCHCONTENT_BASE_DIR}/easy_profiler/bin"
	SUBBUILD_DIR "${FETCHCONTENT_BASE_DIR}/easy_profiler/subbuild"
)

FetchContent_MakeAvailable(easy_profiler)

# add_library(optick STATIC "${optick_SOURCE_DIR}/src/*.cpp")
# target_include_directories(optick PUBLIC "${optick_SOURCE_DIR}/..")

# if (OPTICK_ENABLED)
# 	target_compile_definitions(optick PUBLIC "OPTICK_ENABLED")
# endif()
