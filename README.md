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

#### [01-create-window](/samples/01-create-window)

![](/Screenshots/sample-01-sfml-window.png)

create an SFML window

#### [02-init-opengl](/samples/02-init-opengl)

![](/Screenshots/sample-02-opengl.png)

initialize OpenGL context with globjects

#### [03-fps-camera](/samples/03-fps-camera)

![](/Screenshots/sample-03-fps-camera.png)

implement a simple user-controlled first-person camera with GLM

#### [04-render-mesh](/samples/04-render-mesh)

![](/Screenshots/sample-04-rendering-indexed-geometry.png)

render 3D shape with OpenGL

#### [05-texture](/samples/05-texture)

![](/Screenshots/sample-05-texture.png)

load texture from file and render a _textured_ 3D model

#### [06-lighting](/samples/06-lighting)

![](/Screenshots/sample-06-lighting.png)

simple lighting techniques (Blinn-Phong), basically a simple shader test

#### [07-model-loading](/samples/07-model-loading)

![](/Screenshots/sample-07-model-loading.png)

load a 3D model from file and render it with OpenGL

#### [08-frame-buffer](/samples/08-frame-buffer)

![](/Screenshots/sample-08-frame-buffer.png)

render to framebuffer and then to the screen

### Rendering techniques

#### [09-shadow-mapping](/samples/09-shadow-mapping)

![](/Screenshots/sample-09-shadow-mapping-0.png)

simple shadow mapping

#### [10-particles](/samples/10-particles)

![](/Screenshots/sample-10-particles-3.png)

rendering particles; simple particle engine (TODO: should use [SPARK](https://github.com/Synxis/SPARK), like in [skylicht engine](https://github.com/skylicht-lab/skylicht-engine) ?)

#### [13-terrain](/samples/13-terrain)

![](/Screenshots/sample-13-terrain-2.png)

rendering a terrain; very simple, not paginated (TODO)

#### [14-point-light](/samples/14-point-light)

![](/Screenshots/sample-14-point-light-with-light-maps.png)

point light source (using cubemaps)

#### [15-bloom](/samples/15-bloom)

![](/Screenshots/sample-15-bloom-2.png)

bloom effect

#### [16-anti-aliasing](/samples/16-anti-aliasing)

![](/Screenshots/sample-16-anti-aliasing-2.png)
![](/Screenshots/sample-16-anti-aliasing-3.png)

simple anti-aliasing using out-of-the-box OpenGL capabilities

#### [17-skybox](/samples/17-skybox)

![](/Screenshots/sample-17-skybox.png)

rendering skybox

#### [18-reflection](/samples/18-reflection)

![](/Screenshots/sample-18-reflection-2.png)
![](/Screenshots/sample-18-reflection-3.png)
![](/Screenshots/sample-18-reflection-4.png)

rendering reflective objects (using cubemaps)

#### [19-gui](/samples/19-gui)

![](/Screenshots/sample-19-gui.png)

simple GUI with [Dear ImGUI](https://github.com/ocornut/imgui)

#### ~~[20-scripting](/samples/20-scripting)~~ (TODO)

#### [21-deferred-rendering](/samples/21-deferred-rendering)

![](/Screenshots/sample-21-deferred-rendering-1.png)

deferred rendering, aka render different attributes of each pixel to the framebuffers first
and then combine them all into a final frame in one go (potentially applying post-processing effects) and display on the screen in one go

#### [24-screen-space-ambient-occlusion](/samples/24-screen-space-ambient-occlusion)

![](/Screenshots/sample-24-ssao-1.png)
![](/Screenshots/sample-24-ssao-6.png)

simple SSAO implementation

#### [26-raymarching](/samples/26-raymarching)

![](/Screenshots/sample-26-raymarching-2.png)

volumetric light using raymarching

#### ~~[27-animated-model](/samples/27-animated-model)~~ (TODO)

### Optimization techniques

#### [11-instance-rendering](/samples/11-instance-rendering)

![](/Screenshots/sample-11-instanced-rendering.png)

rendering multiple instances of an object using OpenGL capabilities to render many things in one draw call

#### [12-cascade-shadow-mapping](/samples/12-cascade-shadow-mapping)

![](/Screenshots/pssm_issue_2_0_lookAt_setup.png)
![](/Screenshots/sample-12-cascade-shadow-mapping-1.png)
![](/Screenshots/sample-12-cascade-shadow-mapping-2.png)
![](/Screenshots/sample-12-cascade-shadow-mapping-3.png)

optimizing shadow mapping for large (think outdoor, landscape) scenes

#### [22-fast-approximation-anti-aliasing](/samples/22-fast-approximation-anti-aliasing)

![](/Screenshots/sample-22-fxaa-0.png)
![](/Screenshots/sample-22-fxaa-4.png)

faster anti-aliasing algorithm

#### [23-multi-sample-anti-aliasing](/samples/23-multi-sample-anti-aliasing)

![](/Screenshots/sample-23-msaa.png)

MSAA algorithm

#### [25-horizon-based-ambient-occlusion](/samples/25-horizon-based-ambient-occlusion)

![](/Screenshots/sample-25-hbao-1.png)
![](/Screenshots/sample-25-hbao-3.png)

HBAO, nVidia algorithm, optimization for performance and quality over SSAO

#### [28-multi-draw-indirect](/samples/28-multi-draw-indirect)

![](/Screenshots/sample-28-draw-multi-indirect.png)

optimized "batched" rendering using modern OpenGL capabilities (`glMultiDrawIndirect`), reducing the number of draw calls
