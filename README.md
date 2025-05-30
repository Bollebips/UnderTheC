# VoxC

VoxC is a voxel renderer written in C, using OpenGL.
It renders using a compute shader which raymarches through a voxel mesh.

![Demo](Screenshot.png?raw=true)

## What's next

For now this project is a small demo as a learning experience for me regarding compute shaders and particularly raymarching and voxel rendering.

#### Future additions
- designing a performant voxel data format (using sparce octrees)
- lighting
- voxel material data
- import voxel models
- procedural LOD
- ...

#### Distant future additions
The dream is to expand this project into a reusable voxel game engine to explore various topics related to voxels and general game engine design.
- Vulkan support
- Multithreaded Entity Component System
- realtime voxel simulations (water, smoke, ...)
- ...

-------------------------------------------------------------------------------

## How to build

VoxC uses CMake to build.

``` sh
git clone https://github.com/Bollebips/VoxC.git
cd VoxC/Demo

mkdir build
cd build
cmake ..
cmake --build . -j
```

Tested on Windows 11 using Visual Studio 2022 and on Arch Linux using Make.
More testing to follow.

### Requirements
- [Cmake](https://cmake.org/download/) (version 3.14 or higher)
- [Python 3](https://www.python.org/downloads/) (necessary for Glad)

### Dependencies
The necessary dependencies are downloaded and linked automatically through CMake.
- [GLFW 3.4](https://github.com/glfw/glfw)
- [glad 2.0.8](https://github.com/Dav1dde/glad)
- [cglm 0.9.6](https://github.com/recp/cglm)


## Controls

- <kbd>WASD</kbd> to move
- <kbd>Q</kbd> and <kbd>E</kbd> to move up and down
- <kbd>Arrows</kbd> to rotate
- <kbd>Escape</kbd> to quit
