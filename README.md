# ShootThem!

## Quick overview

This is a very simple shooting game I made many years ago. It is written in C++ with Irrlicht game engine and IrrKlang library for sounds. All level models are hand-made, other assets are taken from third-party games or libraries.

## Build

1. Unpack the dependencies from the `Dependencies/` directory: `irrKlang`, `irrlicht` and `tinyxml2` to the same `Dependencies` directory
2. Run CMake: `cmake -Bbuild -H. && cmake --build build`

## TODO

- [x] remove implicit namespace imports
- [ ] replace explicit colors with named constants
- [x] replace raw pointers with smart pointers
- [x] fix wobbling effect
- [x] record a video (https://youtu.be/m65z9kvWdYA)
- [x] fix timer speed
- [ ] display endgame screen with high scores ¯\_(ツ)_/¯
- [x] import each scene from a scene file rather than DAT files with questionable undocumented format
- [x] load game settings from a separate file rather than being hard-coded vector population code O_o
- [ ] add splash screen
- [x] encapsulate logic in classes
- [ ] `ResourceManager` is redundant
- [x] clean up the design - `InputHandler` (aka dispatcher) is probably redundant, `GameState` (aka state) should be read-only, maybe extract event / action queue to a separate class
- [ ] rework the abuse of `std::shared_ptr` and raw pointers (according to https://www.internalpointers.com/post/move-smart-pointers-and-out-functions-modern-c and https://www.modernescpp.com/index.php/c-core-guidelines-passing-smart-pointer )
- [ ] two targets on 3rd level are indestructable
- [x] when the time runs out the next level is not loaded
- [ ] add end level menu
- [ ] change 3D models to the artwork
- [ ] change the default GUI theme
