# Advanced modern OpenGL rendering technique & optimization samples

This repo contains a number of samples of some advanced rendering techniques with modern OpenGL.
Modern as in 4.4 .. 4.6. There are also few samples showing the rendering optimization techniques.

The samples use few 3rd party libraries for simplifying the code and reducing the boilerplate:

* [SFML](https://github.com/SFML/SFML) - create window, handle user input events (mouse & keyboard), load images from files (for textures)
* [assimp](https://github.com/assimp/assimp) - loading 3D models from files in various formats
* [GLM](https://github.com/g-truc/glm) - math (matrices, vectors, projections)
* [globjects](https://github.com/cginternals/globjects) - OpenGL abstraction interfaces (so no more direct calls to `gl*`)
* [Dear ImGUI](https://github.com/ocornut/imgui) - GUI library (windows, buttons, text input controls, sliders, etc.)

The code is built with [CMake](https://cmake.org/) and [vcpkg](https://github.com/Microsoft/vcpkg) (for dependency management).
I tried sticking to modern C++ guidelines and using C++20 features whenever feasible.

Samples have been built and tested under Windows and OSX.

## Building

```bash
$ cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake
$ cmake --build build
```

## Samples

### Basics

* [01-create-window](/tree/master/samples/01-create-window) - create an SFML window
* [02-init-opengl](/tree/master/samples/02-init-opengl) - initialize OpenGL context with globjects
* [03-fps-camera](/tree/master/samples/03-fps-camera) - implement a simple user-controlled first-person camera with GLM
* [04-render-mesh](/tree/master/samples/04-render-mesh) - render 3D shape with OpenGL
* [05-texture](/tree/master/samples/05-texture) - load texture from file and render a _textured_ 3D model
* [06-lighting](/tree/master/samples/06-lighting) - simple lighting techniques (Blinn-Phong), basically a simple shader test
* [07-model-loading](/tree/master/samples/07-model-loading) - load a 3D model from file and render it with OpenGL
* [08-frame-buffer](/tree/master/samples/08-frame-buffer) - render to framebuffer and then to the screen

### Rendering techniques

* [09-shadow-mapping](/tree/master/samples/09-shadow-mapping) - simple shadow mapping
* [10-particles](/tree/master/samples/10-particles) - rendering particles; simple particle engine (TODO: should use [SPARK](https://github.com/Synxis/SPARK), like in [skylicht engine](https://github.com/skylicht-lab/skylicht-engine) ?)
* [13-terrain](/tree/master/samples/13-terrain) - rendering a terrain; very simple, not paginated (TODO)
* [14-point-light](/tree/master/samples/14-point-light) - point light source (using cubemaps)
* [15-bloom](/tree/master/samples/15-bloom) - bloom effect
* [16-anti-aliasing](/tree/master/samples/16-anti-aliasing) - simple anti-aliasing using out-of-the-box OpenGL capabilities
* [17-skybox](/tree/master/samples/17-skybox) - rendering skybox
* [18-reflection](/tree/master/samples/18-reflection) - rendering reflective objects (using cubemaps)
* [19-gui](/tree/master/samples/19-gui) - simple GUI with [Dear ImGUI](https://github.com/ocornut/imgui)
* ~~[20-scripting](/tree/master/samples/20-scripting)~~ (TODO)
* [21-deferred-rendering](/tree/master/samples/21-deferred-rendering) - deferred rendering
* [24-screen-space-ambient-occlusion](/tree/master/samples/24-screen-space-ambient-occlusion) - simple SSAO implementation
* [26-raymarching](/tree/master/samples/26-raymarching) - volumetric light using raymarching
* ~~[27-animated-model](/tree/master/samples/27-animated-model)~~ (TODO)

### Optimization techniques

* [11-instance-rendering](/tree/master/samples/11-instance-rendering) - rendering multiple instances of an object using OpenGL capabilities
* [12-cascade-shadow-mapping](/tree/master/samples/12-cascade-shadow-mapping) - optimizing shadow mapping for large (think outdoor, landscape) scenes
* [22-fast-approximation-anti-aliasing](/tree/master/samples/22-fast-approximation-anti-aliasing) - faster anti-aliasing algorithm
* [23-multi-sample-anti-aliasing](/tree/master/samples/23-multi-sample-anti-aliasing) - MSAA algorithm
* [25-horizon-based-ambient-occlusion](/tree/master/samples/25-horizon-based-ambient-occlusion) - HBAO, nVidia algorithm, optimization for performance and quality over SSAO
* [28-multi-draw-indirect](/tree/master/samples/28-multi-draw-indirect) - optimized "batched" rendering using modern OpenGL capabilities (`glMultiDrawIndirect`), reducing the number of draw calls
