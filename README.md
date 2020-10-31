# ShootThem!

## Quick overview

This is a very simple shooting game I made many years ago.
It is written in C++ with Irrlicht game engine and IrrKlang library for sounds.
All level models are hand-made, other assets are taken from the Internet.

The version on the `master` branch has been significantly changed in 2020 for the sake of curiosity.

**2006:**

_back in 2006 game level editor was an FPS camera and one had to press magic keys on a keyboard to place targets on a level, it was very limited in functionality and it does not make any sense whatsoever to even put screenshots here, since it would simply look like a mesh viewer_

![Screenshot from 2006](https://github.com/shybovycha/shoot-them/raw/master/Screenshots/ShootThem!%2010_09_2020%208_15_14%20PM.png)
![Yet another screenshot from 2006](https://github.com/shybovycha/shoot-them/raw/master/Screenshots/ShootThem!%2013_09_2020%2010_18_52%20PM.png)

**2020:**

The game now has main menu, end-game screen (although very crude, just a text for the moment being), a proper HUD with images and a more mind-bending post-processing for the "drunken" state of the main character:

![Game screenshot from 2020](https://github.com/shybovycha/shoot-them/raw/master/Screenshots/Shoot%20Them!%2019_10_2020%203_54_42%20PM.png)

Game editor now has a proper UI and is generally way more useful, allowing one to move both the objects on the levels, update and remove objects from the level, add lights to the level, save and load different levels from/to files, etc.

![Editor screenshot from 2020](https://github.com/shybovycha/shoot-them/raw/master/Screenshots/Shoot%20Them!%20Editor%2026_09_2020%201_08_56%20PM.png)
![Yet another editor screenshot from 2020](https://github.com/shybovycha/shoot-them/raw/master/Screenshots/Shoot%20Them!%20Editor%2026_09_2020%201_12_56%20PM.png)

## Build

Run CMake: `cmake -Bbuild -H. && cmake --build build`

## TODO

### Game

- [x] remove implicit namespace imports
- [ ] replace explicit colors with named constants
- [x] replace raw pointers with smart pointers
- [x] fix wobbling effect
- [x] record a video (https://youtu.be/m65z9kvWdYA)
- [x] fix timer speed
- [x] display endgame screen with high scores `¯\_(ツ)_/¯`
- [x] import each scene from a scene file rather than DAT files with questionable undocumented format
- [x] load game settings from a separate file rather than being hard-coded vector population code O_o
- [ ] add splash screen
- [x] encapsulate logic in classes
- [x] ~~`ResourceManager` is redundant~~
- [x] clean up the design - `InputHandler` (aka dispatcher) is probably redundant, `GameState` (aka state) should be read-only, maybe extract event / action queue to a separate class
- [ ] rework the abuse of `std::shared_ptr` and raw pointers (according to https://www.internalpointers.com/post/move-smart-pointers-and-out-functions-modern-c and https://www.modernescpp.com/index.php/c-core-guidelines-passing-smart-pointer)
- [x] ~~two targets on 3rd level are indestructable~~
- [x] when the time runs out the next level is not loaded
- [ ] add end level menu
- [ ] change 3D models to the artwork
- [x] ~~change the default GUI theme~~
- [x] do not use exceptions (https://google.github.io/styleguide/cppguide.html#Exceptions)
- [ ] new game in the middle of the game should reset everything
- [x] properly copy resources to the directory with the binary
- [x] ~~copy the DLLs to the directory with the binary~~
- [x] replace status text with some sane HUD
- [ ] add settings menu
- [ ] write settings to file
- [x] read settings from file
- [x] bring the sound back
- [x] replace level meshes with OBJ
- [x] replace level meshes with Collada
- [x] update font in menu
- [x] ~~load UI from file~~
- [x] use shader for the post-processing ("drunk" effect)
- [ ] clamp the rendered picture so there are no artifacts (mb zoom the quad a bit)
- [x] fix collision detection - potentially the ray overlaps the rendering quad first and hence no collisions with targets are detected (either hide the quad or start ray a bit further from the camera)
- [ ] add hints for controls to the first level

### Editor

- [x] add UI
- [x] manage targets with UI elements, not keyboard and mouse click
- [x] display the structure of a level in UI
- [x] add multiple levels in one run
- [x] allow for level removal
- [x] remove targets
- [x] change target position
- [x] place target in front of camera instead of in place of camera
- [x] replace FPS camera with mouse-controlled one
- [x] add helpful help in UI
- [x] update the default low-res font with something more readable
- [x] make button icons use transparency
- [x] prevent game manager window from closing
- [x] ~~save file dialog does not pick up non-existent files~~
- [x] use mesh filename when adding game manager tree node
- [x] store level mesh in level object
- [x] load level mesh when switching levels in manager
- [x] ~~camera can not be controlled with keyboard~~
- [ ] when loading levels file, load models from the same directory
- [x] create custom camera animator to control camera with both keyboard and mouse in a reasonable way
- [x] create custom file dialog to allow saving files
- [x] manage lights
- [x] refactor Light and Target on level to Entity
- [ ] add & manage properties for level entities (?)
- [x] ~~render AABB around selected entity~~
- [x] draw axes to allow for translation of entities
- [x] ~~set ambient light so that levels are not pitch black~~
- [x] do not hide scene nodes - drop them; but first figure out why that causes runtime exception
- [x] replace mouse wheel for moving camera back/forth with Shift + Mouse Right Button + Mouse Move
- [x] camera translation does not work in **current** camera plane, but rather in initial
- [x] moving selected entity relies on the **absolute** offset, whilst it should rely on **relative** offset instead
- [x] ~~load UI from file~~

### Miscellanious

- [ ] extract level reader & writer into separate library
- [x] update Irrlicht
- [ ] replace Irrlicht with Ogre (?)
- [ ] add github releases
- [ ] document progress
- [ ] add screenshots to github page

## Other notes

### CMake

I honestly dislike CMake for it is complex to cook properly. But it is almost a de-facto standard nowadays, so I have to bear with it for now.

The reasons for my hate are:

1. extremely counter-intuitive syntax
2. complex projects are hard to configure (and instead become basically Makefiles)
3. cross-platform is almost non-existent

```
64 bit platform detected
Found Irrlicht: IRRLICHT_LIBRARY_PATH-NOTFOUND
Found IrrKlang: IRRKLANG_LIBRARY_PATH-NOTFOUND
CMake Deprecation Warning at Dependencies/tinyxml2/CMakeLists.txt:11 (cmake_policy):
  The OLD behavior for policy CMP0063 will be removed from a future version
  of CMake.

  The cmake-policies(7) manual explains that the OLD behaviors of all
  policies are deprecated and that a policy should be set to OLD only under
  specific short-term circumstances.  Projects should be ported to the NEW
  behavior and not rely on setting a policy to OLD.


CMake Warning (dev) at Dependencies/tinyxml2/CMakeLists.txt:14 (project):
  Policy CMP0048 is not set: project() command manages VERSION variables.
  Run "cmake --help-policy CMP0048" for policy details.  Use the cmake_policy
  command to set the policy and suppress this warning.

  The following variable(s) would be set to empty:

    PROJECT_VERSION
    PROJECT_VERSION_MAJOR
    PROJECT_VERSION_MINOR
    PROJECT_VERSION_PATCH
This warning is for project developers.  Use -Wno-dev to suppress it.

[ERROR]Found Irrlicht: IRRLICHT_LIBRARY_PATH-NOTFOUND
[ERROR]Found IrrKlang: IRRKLANG_LIBRARY_PATH-NOTFOUND
Found Irrlicht headers at: /Users/ashubovych/projects/forks/shoot-them/Dependencies/irrlicht-1.8.3/include
Found IrrKlang headers at: /Users/ashubovych/projects/forks/shoot-them/Dependencies/irrKlang-1.5.0/include
Linking libraries: IRRLICHT_LIBRARY_PATH-NOTFOUND;IRRKLANG_LIBRARY_PATH-NOTFOUND;/Library/Developer/CommandLineTools/SDKs/MacOSX10.15.sdk/System/Library/Frameworks/IOKit.framework;/Library/Developer/CommandLineTools/SDKs/MacOSX10.15.sdk/System/Library/Frameworks/CoreVideo.framework;/Library/Developer/CommandLineTools/SDKs/MacOSX10.15.sdk/System/Library/Frameworks/Cocoa.framework;tinyxml2
-- Configuring done
CMake Error: The following variables are used in this project, but they are set to NOTFOUND.
Please set them or make sure they are set and tested correctly in the CMake files:
IRRKLANG_LIBRARY_PATH
    linked by target "Game" in directory /Users/ashubovych/projects/forks/shoot-them/Game
IRRLICHT_LIBRARY_PATH
    linked by target "Game" in directory /Users/ashubovych/projects/forks/shoot-them/Game

CMake Warning (dev):
  Policy CMP0042 is not set: MACOSX_RPATH is enabled by default.  Run "cmake
  --help-policy CMP0042" for policy details.  Use the cmake_policy command to
  set the policy and suppress this warning.

  MACOSX_RPATH is not specified for the following targets:

   tinyxml2

This warning is for project developers.  Use -Wno-dev to suppress it.

-- Generating done
CMake Generate step failed.  Build files cannot be regenerated correctly.
```

Also, as [noted by Brad King](https://gitlab.kitware.com/cmake/cmake/-/issues/19348#note_581803):

> > If I understand ExternalProject correctly...it is up to the user to manually specify build artifacts, include directories, etc.
>
> That is true if you want to use ExternalProject_Add for a dependency and add_executable for your own project in a single CMake project.
> However, typically ExternalProject is used in a pure "superbuild" project that does not compile anything itself and instead builds a bunch of
> external projects using their independent native build systems while respecting inter-project dependencies expressed in the ExternalProject_Add calls.
> Each package is built after its dependencies and is simply told by the superbuild where to look for them as complete packages.
> No low-level header or artifact locations need to be spelled out manually.  No deep integration of distinct build systems is needed.

Few other resources on CMake "best practices":

* http://mesos.apache.org/documentation/latest/cmake-examples/
* https://www.youtube.com/watch?v=y9kSr5enrSk

### C++

Upon compilation, there's gonna be a lot of compiler warnings re. `Vector3::${ANYTHING}` being undefined
(similar to `'Vector3<double>::METHOD_NAME': no suitable definition provided for explicit template instantiation request`).

This is caused by the specifics of C++ itself, where unless one either defines the implicit implementation for each 
template type param used in the same header where template is used or define an alias in that header.

The idea is that headers are compiled separately in C++, so just defining a template class does not mean the thing is going
to work.

Note how everything will still continue to work no matter how many of those warnings have been thrown by a compiler.
