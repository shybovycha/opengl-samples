# ShootThem!

## Quick overview

This is a very simple shooting game I made many years ago. It is written in C++ with Irrlicht game engine and IrrKlang library for sounds. All level models are hand-made, other assets are taken from third-party games or libraries.

## Build

1. Unpack `irrKlang` and `irrlicht` libraries within the `Dependencies` directory
2. Create the `build` directory, open it with the terminal and run `cmake .. && make`

## TODO

[x] remove implicit namespace imports
[] replace explicit colors with named constants
[] replace raw pointers with smart pointers
[x] fix wobbling effect
[x] record a video (https://youtu.be/m65z9kvWdYA)
[] fix timer speed
[] display endgame screen with high scores ¯\_(ツ)_/¯
[] import each scene from a scene file rather than DAT files with questionable undocumented format
[] load game settings from a separate file rather than being hard-coded vector population code O_o
[] add splash screen
[] encapsulate logic in classes
