# lightWeightECS

A lightweight, ECS-style game engine built for fun experiments and solid performance.

## Features:

### Entity-Component-System Architecture
Custom-built ECS for handling many game objects efficiently.

### Collision Test Demo
Simulates tons of randomly moving objects with hitboxes and bouncing behavior for stress testing.

### Two-Player Pong Game
Found in the "pong" folder, using the same ECS backend.

### MultiThreaded Test Demo
I have added threading to the physics. It seems to allow me to render ~2x as many objects while
maintatining a steady 120fps. On my machine I can render ~800 objects with velocities and hitboxes

### QuadTree Test Demo
I have implemented a quadTree for localizing collision calculations. This allows for higher framerates when
rendering large amounts of moving and bouncing entities. Try setting the counts to high numbers in the 
globals.h file and observe the frame rate savings! It's not as dramatic a difference as the multi vs single
threads, but it is definately an improvement.

## Build Instructions (Linux):

Make sure SFML is installed before compiling.

To compile Pong:
`g++ -o pong pongMain.cpp -lsfml-graphics -lsfml-window -lsfml-system`

To compile the Collision Tests (test, multi-threaded and quadTree):
`g++ -o test testMain.cpp -lsfml-graphics -lsfml-window -lsfml-system`

## Why ECS for Pong?

The ECS may be total overkill for my pong demo, but I may build other 2D collision based games off this framework.

## Future Plans:

### Timestep support
For smoother physics with different framerates

### Smarter collision handling
Localize collision detection

### Networking
So you can play online against all 0 users
