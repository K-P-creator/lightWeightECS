# lightWeightECS

A lightweight, ECS-style game engine built for fun experiments and solid performance.

## Features:

### Entity-Component-System Architecture
Custom-built ECS for handling many game objects efficiently.

### Collision Test Demo
Simulates tons of randomly moving objects with hitboxes and bouncing behavior for stress testing.

### Two-Player Pong Game
Found in the "pong" folder, using the same ECS backend.

## Build Instructions (Linux):

Make sure SFML is installed before compiling.

To compile Pong:
`g++ -o pong pongMain.cpp -lsfml-graphics -lsfml-window -lsfml-system`

To compile the Collision Test:
`g++ -o test testMain.cpp -lsfml-graphics -lsfml-window -lsfml-system`

## Why ECS for Pong?

The ECS may be total overkill for my pong demo, but I may build other 2D collision based games off this framework.

## Future Plans:

### Timestep support
For smoother physics with different framerates

### Smarter collision handling
Localize collision detection

### Multithreading
To partition physics workload more efficiently

### Networking
So you can play online against all 0 users
