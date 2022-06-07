cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

option(TRACY_ENABLE OFF)

# set(TRACY_MANUAL_LIFETIME ON)
# set(TRACY_DELAYED_INIT ON)
#set(TRACY_ON_DEMAND ON)
set(TRACY_CALLSTACK ON)

include(FetchContent)

FetchContent_Declare(
    tracy
    GIT_REPOSITORY https://github.com/wolfpld/tracy.git
    GIT_TAG "v0.8.1"
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    SOURCE_DIR   "${FETCHCONTENT_BASE_DIR}/tracy/tracy"
    BINARY_DIR   "${FETCHCONTENT_BASE_DIR}/tracy/bin"
    SUBBUILD_DIR "${FETCHCONTENT_BASE_DIR}/tracy/subbuild"
)

FetchContent_MakeAvailable(tracy)

add_library(tracy STATIC "${tracy_SOURCE_DIR}/TracyClient.cpp")
target_include_directories(tracy PUBLIC "${tracy_SOURCE_DIR}/..")

if (TRACY_ENABLE)
    target_compile_definitions(tracy PUBLIC "TRACY_ENABLE" "TRACY_DELAYED_INIT" "TRACY_ON_DEMAND")
endif()
