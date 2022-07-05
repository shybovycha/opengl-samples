cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(21-deferred-rendering VERSION 1.0.2 LANGUAGES CXX)

set(EXECUTABLE_NAME 21-deferred-rendering)
set(SOURCES "src/main.cpp" "src/common/AbstractMesh.cpp" "src/common/AbstractSkyboxBuilder.cpp" "src/common/AbstractMeshBuilder.cpp" "src/common/AssimpModel.cpp" "src/common/CubemapSkyboxBuilder.cpp" "src/common/MultimeshModel.cpp" "src/common/SimpleSkyboxBuilder.cpp" "src/common/SingleMeshModel.cpp" "src/common/Skybox.cpp")

add_executable(${EXECUTABLE_NAME} ${SOURCES})

target_include_directories(${EXECUTABLE_NAME} PRIVATE "src")

target_precompile_headers(${EXECUTABLE_NAME} PRIVATE "src/common/stdafx.hpp")

target_compile_features(${EXECUTABLE_NAME} PRIVATE cxx_std_20)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE sfml-system sfml-network sfml-graphics sfml-window)

find_package(globjects CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE globjects::globjects)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${GLM_LIBRARIES})

find_package(OpenGL REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${OPENGL_LIBRARIES})

find_package(assimp CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE assimp::assimp)

option(HIGH_DPI ON)

if(HIGH_DPI)
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE HIGH_DPI)
endif()

# copy media
add_custom_command(TARGET ${EXECUTABLE_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/../media $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/media)
add_custom_command(TARGET ${EXECUTABLE_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/media $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/media)