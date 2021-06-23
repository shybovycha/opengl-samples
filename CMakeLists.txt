cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(shoot-them VERSION 1.0.2 LANGUAGES CXX)

add_subdirectory(samples/01-create-window)
add_subdirectory(samples/02-init-opengl)
add_subdirectory(samples/03-fps-camera)
add_subdirectory(samples/04-render-mesh)
add_subdirectory(samples/05-texture)
add_subdirectory(samples/06-lighting)
add_subdirectory(samples/07-model-loading)
add_subdirectory(samples/08-frame-buffer)
add_subdirectory(samples/09-shadow-mapping)
