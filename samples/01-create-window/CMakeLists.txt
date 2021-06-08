cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(01-create-window VERSION 1.0.2 LANGUAGES CXX)

set(EXECUTABLE_NAME 01-create-window)
set(SOURCES "main.cpp")

add_executable(${EXECUTABLE_NAME} ${SOURCES})

target_compile_features(${EXECUTABLE_NAME} PRIVATE cxx_std_17)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE sfml-system sfml-network sfml-graphics sfml-window)
