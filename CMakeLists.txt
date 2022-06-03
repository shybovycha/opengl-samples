cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(opengl-samples VERSION 1.1.0 LANGUAGES CXX)

include(cmake/get_tracy.cmake)
include(cmake/get_easy_profile.cmake)

add_subdirectory(samples)
