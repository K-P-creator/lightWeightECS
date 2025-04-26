#pragma once

#include "../include/entity.h"
#include "../include/components.h"
#include "../include/globals.h"
#include "../include/systems.h"

#include <cmath>
#include <vector>
#include <utility>

#include "../build/_deps/sfml-src/include/SFML/Graphics.hpp"
#include "../build/_deps/sfml-src/include/SFML/System.hpp"
#include "../build/_deps/sfml-src/include/SFML/Window.hpp"


using namespace std;

//handles linear movement for entities with positions and velocities
class movementSystem
{
    public:
    void updatePosition (const entity & e, velocityComponent* v, positionComponent* p, 
                        componentManager & cm, std::vector<entity> & ents);
};

//renders rectangles on screen
class rectRenderSystem
{
    private:
        sf::RectangleShape rectangle;

    public:
        void renderRect(rectangleSizeComponent * rec, positionComponent * p,
                        colorComponent * c, outlineComponent * o, sf::RenderWindow & window);
};

//renders circles
class circRenderSystem
{
    private:
        sf::CircleShape circle;

    public:
        void renderCirc(circleSizeComponent * r, positionComponent * p,
                        colorComponent * c, outlineComponent * o, sf::RenderWindow & window);
};

//checks collisions within the hitbox map
class collisionSystem
{
    public:
    bool checkCollision(const entity e, positionComponent * p1, hitboxComponent * h1, 
                        velocityComponent * v1, componentManager & cm);


    //returns the face of the colission (ie. t, b, l, r) returns \0 for no collision
    char getCollisionFace(positionComponent * p1, positionComponent * p2,
                          hitboxComponent * h1, hitboxComponent * h2);


    void resetBall (const entity & e, componentManager & cm, bool side);

};


//handles all systems in a scene
//will auto create all systems
class systemManager
{
    private:
        rectRenderSystem rec;
        circRenderSystem cir;
        movementSystem mov;
        collisionSystem col;
    public:

        //runs all dynamic systems
        void runPhysicsSystems(std::vector <entity> & ent, componentManager & cm){
            for (const auto & e : ent){
                auto *v = cm.getComponent<velocityComponent>(e);
                auto *h = cm.getComponent<hitboxComponent>(e);
                auto *p = cm.getComponent<positionComponent>(e);
            
                //check entity collisions
                //cout << "Checking collisions...\n";    
                col.checkCollision(e, p, h, v, cm);

                //update positionns
                mov.updatePosition(e, v, p, cm, ent);

                //update paddle speed
                //paddleSpeed += paddleAcceleration * timestep;
            }
        }

        void render(std::vector <entity> & ents, componentManager & cm, sf::RenderWindow & w){
            for (auto & i : ents){  
                auto *s = cm.getComponent<rectangleSizeComponent>(i);
                auto *p = cm.getComponent<positionComponent>(i);
                auto *c = cm.getComponent<colorComponent>(i);
                auto *t = cm.getComponent<textureComponent>(i);
                auto *o = cm.getComponent<outlineComponent>(i);

                if (!p) continue;

                if (t){
                    sf::Sprite sp(t->texture);
                    sp.setPosition({p->px,p->py});
                    w.draw(sp);
                    continue;
                }

                //draw circles and squares
                if (!s){
                    auto *s = cm.getComponent<circleSizeComponent>(i);
                    cir.renderCirc(s,p,c,o,w);
                }else{
                    rec.renderRect(s,p,c,o,w);
                }
            }
            return;
        }
};
