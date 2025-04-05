#include "systems.h"
#include "globals.h"
#include "components.h"
#include "entity.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <vector>
#include <unordered_map>
#include <sstream>
#include <cstdlib>    // For rand()
#include <ctime>      // For seeding rand()
#include <iostream>
#include <string>

using namespace std;

float randInRange(float min, float max) {
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));  // Seed the random number generator

    //create entity vectors
    vector<entity> staticEntityVec;
    vector<entity> dynamEntityVec;
    componentManager cm;
    
    //create system manager
    systemManager sm;
    
    //create floor rectangle
    entity floor(entityId++);
    staticEntityVec.push_back(floor);
    cm.addComponent<positionComponent>(floor, positionComponent(0, HEIGHT - 10));
    cm.addComponent<rectangleSizeComponent>(floor, rectangleSizeComponent(WIDTH, 10));
    cm.addComponent<colorComponent>(floor, colorComponent(200, 10, 10));
    cm.addComponent<hitboxComponent>(floor, hitboxComponent(WIDTH, 10,1,'w'));
    
    //create ceiling
    entity ceiling(entityId++);
    staticEntityVec.push_back(ceiling);
    cm.addComponent<positionComponent>(ceiling, positionComponent(0, 0));
    cm.addComponent<rectangleSizeComponent>(ceiling, rectangleSizeComponent(WIDTH, 10));
    cm.addComponent<colorComponent>(ceiling, colorComponent(10, 10, 200));
    cm.addComponent<hitboxComponent>(ceiling, hitboxComponent(WIDTH, 10,1,'w'));
    
    //create left wall
    entity leftWall(entityId++);
    staticEntityVec.push_back(leftWall);
    cm.addComponent<positionComponent>(leftWall, positionComponent(0, 0));
    cm.addComponent<rectangleSizeComponent>(leftWall, rectangleSizeComponent(10, HEIGHT));
    cm.addComponent<colorComponent>(leftWall, colorComponent(10, 200, 10));
    cm.addComponent<hitboxComponent>(leftWall, hitboxComponent(10, HEIGHT,1, 'l'));
    
    //create right wall
    entity rightWall(entityId++);
    staticEntityVec.push_back(rightWall);
    cm.addComponent<positionComponent>(rightWall, positionComponent(WIDTH - 10, 0));
    cm.addComponent<rectangleSizeComponent>(rightWall, rectangleSizeComponent(10, HEIGHT));
    cm.addComponent<colorComponent>(rightWall, colorComponent(10, 200, 10));
    cm.addComponent<hitboxComponent>(rightWall, hitboxComponent(10, HEIGHT,1, 'r'));
    
    // Create left paddle
    entity leftPaddle(entityId++);
    staticEntityVec.push_back(leftPaddle);
    cm.addComponent<positionComponent>(leftPaddle, positionComponent(20, HEIGHT / 2 - (HEIGHT / 10) / 2));
    cm.addComponent<rectangleSizeComponent>(leftPaddle, rectangleSizeComponent(10, HEIGHT / 10));
    cm.addComponent<colorComponent>(leftPaddle, colorComponent(255, 255, 255));
    cm.addComponent<hitboxComponent>(leftPaddle, hitboxComponent(10, HEIGHT / 10, 1, 'p'));

    // Create right paddle
    entity rightPaddle(entityId++);
    staticEntityVec.push_back(rightPaddle);
    cm.addComponent<positionComponent>(rightPaddle, positionComponent(WIDTH - 30, HEIGHT / 2 - (HEIGHT / 10) / 2));
    cm.addComponent<rectangleSizeComponent>(rightPaddle, rectangleSizeComponent(10, HEIGHT / 10));
    cm.addComponent<colorComponent>(rightPaddle, colorComponent(255, 255, 255));
    cm.addComponent<hitboxComponent>(rightPaddle, hitboxComponent(10, HEIGHT / 10, 1, 'p'));

    // Create ball
    entity ball(entityId++);
    dynamEntityVec.push_back(ball);
    cm.addComponent<positionComponent>(ball, positionComponent(WIDTH / 2, HEIGHT / 2));
    cm.addComponent<circleSizeComponent>(ball, circleSizeComponent(10));
    cm.addComponent<velocityComponent>(ball, velocityComponent(startingBallSpeed, startingBallSpeed));
    cm.addComponent<colorComponent>(ball, colorComponent(255, 20, 147));
    cm.addComponent<hitboxComponent>(ball, hitboxComponent(20, 20, 1, 'b'));  // Approximate as square

    // framerate tracking data
    sf::Clock fpsClock;
    float fpsTimer = 0.0f;
    int frameCount = 0;
    float currentFPS = 0.0f;    

    // Load font (ensure you have a .ttf file available)
    sf::Font font;
    if (!font.loadFromFile("Arial.ttf")) {
        std::cerr << "Failed to load font!\n";
        return 1;
    }

    // FPS Text Setup
    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(16);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10.f, 10.f);

    // Scoreboard Text Setup
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(48);  // Large font
    scoreText.setFillColor(sf::Color::White);
    scoreText.setStyle(sf::Text::Bold);

    //init entity count
    int entCount = staticEntityVec.size() + dynamEntityVec.size();

    // --- Create SFML window at 1080p HS resolution
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "2d_game_sfml");
    window.setFramerateLimit(120);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

                // Move left paddle (WASD)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            auto * pos = cm.getComponent<positionComponent>(leftPaddle);
            if (pos->py > 0) {
                auto * pos = cm.getComponent<positionComponent>(leftPaddle);
                pos->py -= paddleSpeed;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            auto * pos = cm.getComponent<positionComponent>(leftPaddle);
            if (pos->py < HEIGHT - (HEIGHT / 10)) {
                auto * pos = cm.getComponent<positionComponent>(leftPaddle);
                pos->py += paddleSpeed;
            }            
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            auto * pos = cm.getComponent<positionComponent>(leftPaddle);
            if (pos->px > 0) {
                auto * pos = cm.getComponent<positionComponent>(leftPaddle);
                pos->px -= paddleSpeed;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            auto * pos = cm.getComponent<positionComponent>(leftPaddle);
            if (pos->px < (WIDTH - 10)/2) {
                auto * pos = cm.getComponent<positionComponent>(leftPaddle);
                pos->px += paddleSpeed;
            }
        }
        // Move right paddle (Up/Down/left/right arrows)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            auto * pos = cm.getComponent<positionComponent>(rightPaddle);
            if (pos->py > 0) {
                auto * pos = cm.getComponent<positionComponent>(rightPaddle);
                pos->py -= paddleSpeed;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            auto * pos = cm.getComponent<positionComponent>(rightPaddle);
            if (pos->py < HEIGHT - (HEIGHT / 10)) {
                auto * pos = cm.getComponent<positionComponent>(rightPaddle);
                pos->py += paddleSpeed;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            auto * pos = cm.getComponent<positionComponent>(rightPaddle);
            if (pos->px > (WIDTH - 10)/2) {
                auto * pos = cm.getComponent<positionComponent>(rightPaddle);
                pos->px -= paddleSpeed;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            auto * pos = cm.getComponent<positionComponent>(rightPaddle);
            if (pos->px < WIDTH - 10) {
                auto * pos = cm.getComponent<positionComponent>(rightPaddle);
                pos->px += paddleSpeed;
            }
        }

        window.clear();

        // --- Render static entities
        sm.runStaticSystems(staticEntityVec, cm, window);

        // --- Render dynamic entities
        sm.runDynamicSystems(dynamEntityVec, cm, window);

        //render framerate
        frameCount++;
        fpsTimer += fpsClock.restart().asSeconds();

        if (fpsTimer >= 0.5f) { // update every half second
            entCount = dynamEntityVec.size() + staticEntityVec.size();
            currentFPS = frameCount / fpsTimer;
        
            std::ostringstream ss;
            ss.precision(1);
            ss << std::fixed << "FPS: " << currentFPS << endl;
            ss << std::fixed << "Entity Count: " << entCount;
            fpsText.setString(ss.str());
        
            frameCount = 0;
            fpsTimer = 0.0f;
        }        

        // Update and render the scoreboard
        // Update scoreboard display
        std::ostringstream scoreStream;
        scoreStream << leftScore << "   |   " << rightScore;
        scoreText.setString(scoreStream.str());

        // Center the scoreboard horizontally
        sf::FloatRect textBounds = scoreText.getLocalBounds();
        scoreText.setOrigin(textBounds.width / 2, textBounds.height / 2);
        scoreText.setPosition(WIDTH / 2, 30);  // Top center of the screen

        window.draw(scoreText);

        window.draw(fpsText);

        window.display();
    }

    return 0;
}