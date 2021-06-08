cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(shoot-them VERSION 1.0.2 LANGUAGES CXX)

add_subdirectory(samples/01-create-window)
add_subdirectory(samples/02-init-opengl)
add_subdirectory(samples/03-fps-camera)
add_subdirectory(samples/04-render-mesh)
