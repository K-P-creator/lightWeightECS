#include "../include/systems.h"
#include "../include/globals.h"
#include "../include/components.h"
#include "../include/entity.h"


#include <vector>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <string>
#include <chrono>

using namespace std;


//define globals 
    unsigned int entityId = 0;
    //score tracking data
    int leftScore = 0;
    int rightScore = 0;
    //used for paddle acceleration physics
    float paddleSpeed = startingPaddleSpeed;


int main()
{
    //create entity vectors
    vector<entity> entityVec;
    componentManager cm;
    
    //create system manager
    systemManager sm;
    
    //create floor rectangle
    entity floor(entityId++);
    entityVec.push_back(floor);
    cm.addComponent<positionComponent>(floor, {0, HEIGHT - 10});
    cm.addComponent<rectangleSizeComponent>(floor, {WIDTH, 10});
    cm.addComponent<colorComponent>(floor, colorComponent(255,255,255));
    cm.addComponent<hitboxComponent>(floor, {WIDTH, 10,1,'w'});
    
    //create ceiling
    entity ceiling(entityId++);
    entityVec.push_back(ceiling);
    cm.addComponent<positionComponent>(ceiling, {0, 0});
    cm.addComponent<rectangleSizeComponent>(ceiling, {WIDTH, 10});
    cm.addComponent<colorComponent>(ceiling, {255,255,255});
    cm.addComponent<hitboxComponent>(ceiling, {WIDTH, 10,1,'w'});
    
    //create left wall
    entity leftWall(entityId++);
    entityVec.push_back(leftWall);
    cm.addComponent<positionComponent>(leftWall, {0, 0});
    cm.addComponent<rectangleSizeComponent>(leftWall, {10, HEIGHT});
    cm.addComponent<colorComponent>(leftWall, colorComponent(255,255,255));
    cm.addComponent<hitboxComponent>(leftWall, {10, HEIGHT,1, 'l'});
    
    //create right wall
    entity rightWall(entityId++);
    entityVec.push_back(rightWall);
    cm.addComponent<positionComponent>(rightWall, {WIDTH - 10, 0});
    cm.addComponent<rectangleSizeComponent>(rightWall, {10, HEIGHT});
    cm.addComponent<colorComponent>(rightWall, colorComponent(255,255,255));
    cm.addComponent<hitboxComponent>(rightWall, {10, HEIGHT,1, 'r'});
    

    // Create left paddle
    entity leftPaddle(entityId++);
    entityVec.push_back(leftPaddle);
    cm.addComponent<positionComponent>(leftPaddle, {100, HEIGHT / 2 - (HEIGHT / 10) / 2});
    cm.addComponent<rectangleSizeComponent>(leftPaddle, {10, HEIGHT / 10});
    cm.addComponent<colorComponent>(leftPaddle, colorComponent(255, 0, 0));
    cm.addComponent<hitboxComponent>(leftPaddle, {10, HEIGHT / 10, 1, 'p'});
    cm.addComponent<velocityComponent>(leftPaddle, {0,0});
    cm.addComponent<accelerationComponent>(leftPaddle, {paddleAcceleration, paddleAcceleration});
    cm.addComponent<outlineComponent>(leftPaddle, outlineComponent()); //default is black

    // Create right paddle
    entity rightPaddle(entityId++);
    entityVec.push_back(rightPaddle);
    cm.addComponent<positionComponent>(rightPaddle, {WIDTH - 100, HEIGHT / 2 - (HEIGHT / 10) / 2});
    cm.addComponent<rectangleSizeComponent>(rightPaddle, {10, HEIGHT / 10});
    cm.addComponent<colorComponent>(rightPaddle, colorComponent(255, 255, 0));
    cm.addComponent<hitboxComponent>(rightPaddle, {10, HEIGHT / 10, 1, 'p'});
    cm.addComponent<velocityComponent>(rightPaddle, {0,0});
    cm.addComponent<accelerationComponent>(rightPaddle, {paddleAcceleration, paddleAcceleration});
    cm.addComponent<outlineComponent>(rightPaddle, outlineComponent()); //default is black


    // Create ball
    entity ball(entityId++);
    entityVec.push_back(ball);
    cm.addComponent<positionComponent>(ball, {WIDTH / 2, HEIGHT / 2});
    cm.addComponent<circleSizeComponent>(ball, {25});
    cm.addComponent<velocityComponent>(ball, {startingBallSpeed, startingBallSpeed});
    cm.addComponent<colorComponent>(ball, colorComponent(255, 20, 147));
    cm.addComponent<hitboxComponent>(ball, {50, 50, 1, 'b'});  // Approximate as square
    cm.addComponent<accelerationComponent>(ball, {ballAcceleration,ballAcceleration});
    cm.addComponent<textureComponent>(ball, textureComponent("../resources/ball.png"));

    // framerate tracking data
    sf::Clock fpsClock;
    float fpsTimer = 0.0f;
    int frameCount = 0;
    float currentFPS = 0.0f;    

    // Load font (ensure you have a .ttf file available)
    sf::Font font;
    if (!font.openFromFile("../resources/Arial.ttf")) {
        std::cerr << "Failed to load font!\n";
        return 1;
    }

    // FPS Text Setup
    sf::Text fpsText(font);
    fpsText.setCharacterSize(16);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition({10.f, 10.f});

    //init entity count
    int entCount = entityVec.size();

    // Scoreboard Text Setup
    sf::Text scoreText(font);
    scoreText.setCharacterSize(48);  // Large font
    scoreText.setFillColor(sf::Color::White);
    scoreText.setStyle(sf::Text::Bold);

    // --- Create SFML window at 1080p HS resolution
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "2d_game_sfml");
    window.setFramerateLimit(120);

    //timestep setup
    using clock = std::chrono::high_resolution_clock;
    using timepoint = std::chrono::time_point<clock>;
    
    timepoint currentTime = clock::now();
    double accumulator = 0.0;
    double t = 0.0;

    //sprite for background
    sf::Texture fieldTexture("../resources/field.png");
    sf::Sprite field(fieldTexture);
    field.setPosition({0,0});

    while (window.isOpen())
    {
        //flags for logging pressed keys
        bool w,s,U,D;   

        //Poll events
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()){
                window.close();
            }
            else if (const auto * key = event->getIf<sf::Event::KeyPressed>()){
                if (key->scancode == sf::Keyboard::Scancode::W) w = true;
                if (key->scancode == sf::Keyboard::Scancode::S) s = true;
                if (key->scancode == sf::Keyboard::Scancode::Up) U = true;
                if (key->scancode == sf::Keyboard::Scancode::Down) D = true;
            }
            else if (const auto * key = event->getIf<sf::Event::KeyReleased>()){
                if (key->scancode == sf::Keyboard::Scancode::W) w = false;
                if (key->scancode == sf::Keyboard::Scancode::S) s = false;
                if (key->scancode == sf::Keyboard::Scancode::Up) U = false;
                if (key->scancode == sf::Keyboard::Scancode::Down) D = false;
            }
        }

        //Paddle Components
        auto * pl = cm.getComponent<positionComponent>(leftPaddle);
        auto * vl = cm.getComponent<velocityComponent>(leftPaddle);
        auto * pr = cm.getComponent<positionComponent>(rightPaddle);
        auto * vr = cm.getComponent<velocityComponent>(rightPaddle);

        //Input handling for left paddle
        if ((w && s) || (!w && !s)) vl->vy = 0;
        else if (w && !s){
            if (pl->py > 10) vl->vy = -1.0f * paddleSpeed;
            else vl->vy = 0;
        }
        else if (!w && s){
            if (pl->py <= HEIGHT - (HEIGHT/10 + 10)) vl->vy = paddleSpeed;
            else vl->vy = 0;
        }

        //handling for right paddle
        if ((U && D) || (!U && !D)) vr->vy = 0;
        else if (U && !D){
            if (pr->py > 10) vr->vy = -1.0f * paddleSpeed;
            else vr->vy = 0;
        }
        else if (!U && D){
            if (pr->py <= HEIGHT - (HEIGHT/10 + 10)) vr->vy = paddleSpeed;
            else vr->vy = 0;
        }

        //setup for timestep
        // Get current time
        timepoint newTime = clock::now();

        // Calculate frame time in seconds
        chrono::duration<double> deltaTime = newTime - currentTime;
        double frameTime = deltaTime.count();

        // Cap to avoid spiral of death
        if (frameTime > 0.25)
            frameTime = 0.25;

        currentTime = newTime;
        accumulator += frameTime;

        // Fixed timestep loop
        while (accumulator >= timestep) {
            sm.runPhysicsSystems(entityVec, cm); // Physics update
            //manually accelerate paddle speed for now
            paddleSpeed += (paddleAcceleration * timestep);
            t += timestep;
            accumulator -= timestep;
        }

        window.clear();
        window.draw(field);
        sm.render(entityVec, cm, window); // Rendering


        //render framerate
        frameCount++;
        fpsTimer += fpsClock.restart().asSeconds();

        if (fpsTimer >= 0.5f) { // update every half second
            entCount = entityVec.size();
            currentFPS = frameCount / fpsTimer;
        
            ostringstream ss;
            ss.precision(0);
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
        scoreText.setPosition({(WIDTH / 2) - (textBounds.size.x/2), 30});  // Top center of the screen

        window.draw(scoreText);
        window.draw(fpsText);

        window.display();
    }

    return 0;
}