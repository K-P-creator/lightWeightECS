# Extreme Pong

## About

In this version of pong I added texture support, timestep and switched some versions around. 
I switched from to C++20, and switched to SFML 3.0.1. 
I also implemented timestep for smoother physics at different framerates. The game can now run at ~4k FPS
and the physics will stay consistent. It is capped at 120 by default. 

## Compilation

Compiler: GCC (MinGW on Windows)

### Prerequisites

- CMake (version 3.16 or higher recommended)
- GCC (via MinGW) on Windows


### Instructions

Set your compiler to GCC

```ps
cmake .. -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
```

Then build

``` ps
mkdir build
cd build

cmake ..
cmake --build .
```
