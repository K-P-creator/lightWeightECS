#pragma once

//full HD
constexpr int WIDTH(1920);
constexpr int HEIGHT(1080);

extern unsigned int entityId;
extern float paddleSpeed;

constexpr float startingPaddleSpeed(190.0f);
constexpr float paddleAcceleration(3.0f);

constexpr float startingBallSpeed(70.0f);
constexpr float ballAcceleration(5.0f);

//score tracking data
extern int leftScore;
extern int rightScore;

constexpr float timestep(1.0f/120.0f);