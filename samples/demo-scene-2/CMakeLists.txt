cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(demo-scene-2 VERSION 1.0.2 LANGUAGES CXX)

set(EXECUTABLE_NAME demo-scene-2)
set(SOURCES "main.cpp" "Skybox.hpp" "Skybox.cpp" "AbstractDrawable.hpp" "Mesh.hpp" "Mesh.cpp" "AssimpMeshLoader.hpp" "AssimpMeshLoader.cpp" "stdafx.cpp")
set(PRECOMPILED_HEADER "stdafx.hpp")

add_executable(${EXECUTABLE_NAME} ${SOURCES})

target_compile_features(${EXECUTABLE_NAME} PRIVATE cxx_std_20)

target_precompile_headers(${EXECUTABLE_NAME} PUBLIC ${PRECOMPILED_HEADER})

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE sfml-system sfml-graphics sfml-window)

find_package(globjects CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE globjects::globjects)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${GLM_LIBRARIES})

find_package(OpenGL REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${OPENGL_LIBRARIES})

find_package(assimp CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE assimp::assimp)

# copy media
add_custom_command(TARGET ${EXECUTABLE_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/../media $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/media)
add_custom_command(TARGET ${EXECUTABLE_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/media $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/media)
