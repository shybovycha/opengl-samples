# ShootThem!

## Quick overview

This is a very simple shooting game I made many years ago. It is written in C++ with Irrlicht game engine and IrrKlang library for sounds. All level models are hand-made, other assets are taken from third-party games or libraries.

## Compiling on Linux

First of all, install some dependencies, but I really can't remember which ones =)

Then, use this command line to generate the binary file:

    g++ main.cpp \
		-Iirrlicht-1.5.2/include/ \
		-Lirrlicht-1.5.2/lib/Linux/ \
		-IirrKlang-64bit-1.5.0/include/ \
		-LirrKlang-64bit-1.5.0/lib/linux-gcc-64 \
		-m64 \
		-lIrrlicht -lIrrKlang -lGL -lGLU \
		-lXrandr -lXext -lX11 -lXxf86vm \
		-o ShootThem

Or modify it to use the 32-bit version of `IrrKlang` (just remove the `-64bit` piece from `-I` and `-L` options).

## Compiling on Windows

You should have no problems adding both irrlicht and irrklang libraries along with `main.cpp` file (yes, the correct versions of irr* libraries are provided in this repository) to a new Microsoft Visual Studio project and compiling it.
