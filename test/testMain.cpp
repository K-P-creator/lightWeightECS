#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "entity.h"
#include "components.h"
#include "systems.h"
#include "globals.h"

#include <vector>
#include <unordered_map>
#include <sstream>
#include <cstdlib>    // For rand()
#include <ctime>      // For seeding rand()

using namespace std;

int entityId(0);

float randInRange(float min, float max) {
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));  // Seed the random number generator

    // --- Create entity vectors
    vector<entity> staticEntityVec;
    vector<entity> dynamEntityVec;

    componentManager cm;

    // --- Create system manager
    systemManager sm;

    // --- Create floor rectangle
    entity floor(entityId++);
    staticEntityVec.push_back(floor);

    cm.addComponent<positionComponent>(floor, positionComponent(0, HEIGHT - 10));
    cm.addComponent<rectangleSizeComponent>(floor, rectangleSizeComponent(WIDTH, 10));
    cm.addComponent<colorComponent>(floor, colorComponent(200, 10, 10));
    cm.addComponent<hitboxComponent>(floor, hitboxComponent(WIDTH, 10,1));

    // --- Create ceiling
    entity ceiling(entityId++);
    staticEntityVec.push_back(ceiling);

    cm.addComponent<positionComponent>(ceiling, positionComponent(0, 0));
    cm.addComponent<rectangleSizeComponent>(ceiling, rectangleSizeComponent(WIDTH, 10));
    cm.addComponent<colorComponent>(ceiling, colorComponent(10, 10, 200));
    cm.addComponent<hitboxComponent>(ceiling, hitboxComponent(WIDTH, 10,1));

    // --- Create left wall
    entity leftWall(entityId++);
    staticEntityVec.push_back(leftWall);

    cm.addComponent<positionComponent>(leftWall, positionComponent(0, 0));
    cm.addComponent<rectangleSizeComponent>(leftWall, rectangleSizeComponent(10, HEIGHT));
    cm.addComponent<colorComponent>(leftWall, colorComponent(10, 200, 10));
    cm.addComponent<hitboxComponent>(leftWall, hitboxComponent(10, HEIGHT,1));

    // --- Create right wall
    entity rightWall(entityId++);
    staticEntityVec.push_back(rightWall);

    cm.addComponent<positionComponent>(rightWall, positionComponent(WIDTH - 10, 0));
    cm.addComponent<rectangleSizeComponent>(rightWall, rectangleSizeComponent(10, HEIGHT));
    cm.addComponent<colorComponent>(rightWall, colorComponent(10, 200, 10));
    cm.addComponent<hitboxComponent>(rightWall, hitboxComponent(10, HEIGHT,1));

    // --- Create rectangles
    for (int i = 0; i < 200; ++i) {
        entity e{entityId++};

        int width  = randInRange(5, 20);
        int height = randInRange(5, 20);
        float x, y;
        bool valid;

        // Retry until no collision
        do {
            valid = true;
            x = randInRange(0, WIDTH - width);
            y = randInRange(0, HEIGHT - height);

            // Check against all dynamic entities
            for (const auto& other : dynamEntityVec) {
                auto* pos2 = cm.getComponent<positionComponent>(other);
                auto* size2 = cm.getComponent<rectangleSizeComponent>(other);
                if (!pos2 || !size2) continue;

                if (x < pos2->px + size2->rx && x + width > pos2->px &&
                    y < pos2->py + size2->ry && y + height > pos2->py) {
                    valid = false;
                    break;
                }
            }

            // Check against all static entities
            for (const auto& other : staticEntityVec) {
                auto* pos2 = cm.getComponent<positionComponent>(other);
                auto* size2 = cm.getComponent<rectangleSizeComponent>(other);
                if (!pos2 || !size2) continue;

                if (x < pos2->px + size2->rx && x + width > pos2->px &&
                    y < pos2->py + size2->ry && y + height > pos2->py) {
                    valid = false;
                    break;
                }
            }

        } while (!valid);

        float vx = randInRange(-2, 2);
        float vy = randInRange(-2, 2);
        int r = randInRange(0, 255);
        int g = randInRange(0, 255);
        int b = randInRange(0, 255);

        cm.addComponent(e, positionComponent(x, y));
        cm.addComponent(e, rectangleSizeComponent(width, height));
        cm.addComponent(e, velocityComponent(vx, vy));
        cm.addComponent(e, colorComponent(r, g, b));
        cm.addComponent(e, hitboxComponent(width, height, 1));  // Hitbox = rect size

        dynamEntityVec.push_back(e);
    }

    // --- Create circles
    for (int i = 0; i < 200; ++i) {
        entity e{entityId++};

        int radius = randInRange(5, 20);
        int x      = randInRange(radius, WIDTH - radius);
        int y      = randInRange(radius, HEIGHT - radius);
        float vx   = randInRange(-2, 2);
        float vy   = randInRange(-2, 2);
        int r      = randInRange(0, 255);
        int g      = randInRange(0, 255);
        int b      = randInRange(0, 255);

        cm.addComponent(e, positionComponent(x, y));
        cm.addComponent(e, circleSizeComponent(radius));
        cm.addComponent(e, velocityComponent(vx, vy));
        cm.addComponent(e, colorComponent(r, g, b));
        cm.addComponent(e, hitboxComponent(radius * 2, radius * 2, 1));  // Approximate as square

        dynamEntityVec.push_back(e);
    }

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

    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(16);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10.f, 10.f);

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

        window.draw(fpsText);

        window.display();
    }

    return 0;
}
