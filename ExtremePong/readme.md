# Extreme Pong

## About

In this version of pong I added texture support, timestep and switched some versions around. 
I switched from c++17 to c++20, and also switched to SFML 3.0.1. 
I also implemented timestep for smoother physics at different framerates. The game can now run at like 4k FPS
and the physics will stay consistent. It is capped at 120 though by default. 

## Compilation

Compiler: GCC (MinGW on Windows)

### Prerequisites

- CMake (version 3.16 or higher recommended)
- GCC (via MinGW) on Windows


### Instructions

``` ps
mkdir build
cd build

cmake ..
cmake --build .
```
