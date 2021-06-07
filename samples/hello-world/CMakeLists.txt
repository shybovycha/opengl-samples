cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(hello-world VERSION 1.0.2 LANGUAGES CXX)

set(EXECUTABLE_NAME hello-world)
set(SOURCES "main.cpp")

add_executable(${EXECUTABLE_NAME} ${SOURCES})

target_compile_features(${EXECUTABLE_NAME} PRIVATE cxx_std_17)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE sfml-system sfml-network sfml-graphics sfml-window)

find_package(tinyxml2 CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE tinyxml2::tinyxml2)

find_package(globjects CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE globjects::globjects)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE glm)

# copy media files to the target directory
# add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory  ${CMAKE_CURRENT_LIST_DIR}/../../assets $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/assets)
