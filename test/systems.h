#include "entity.h"
#include "components.h"
#include "globals.h"

#include <cmath>
#include <vector>
#include <utility>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

using namespace std;

//handles linear movement for entities with positions and velocities
class movementSystem
{
    public:
    //returns false if object goes OOB and is deleted
    bool updatePosition (const entity & e, velocityComponent* v, positionComponent* p, 
                        componentManager & cm, std::vector<entity> & ents)
    {
        //add velocity to position for new position
        if (v == nullptr) return true;
        if (p == nullptr) return true;

        p->px += v->vx;
        p->py += v->vy;

        //OOB checking for objects travelling off into oblivion
        if (p->px > WIDTH*1.2 || p->px < 0-WIDTH*.2) {
            cm.clearEntityComponents(e);

            //delete the entity from the vector of entities
            auto it = std::find(ents.begin(), ents.end(), e);
            if (it != ents.end()) {
                ents.erase(it);
            }
            return false;
        }
        if (p->py > HEIGHT*1.2 || p->py < 0-HEIGHT*.2){
            cm.clearEntityComponents(e);

            //delete the entity from the vector of entities
            auto it = std::find(ents.begin(), ents.end(), e);
            if (it != ents.end()) {
                ents.erase(it);
            }
            return false;
        }
        return true;
    }
        
};

//renders rectangles on screen
class rectRenderSystem
{
    private:
        sf::RectangleShape rectangle;

    public:
        void renderRect(rectangleSizeComponent * rec, positionComponent * p,
                        colorComponent * c, sf::RenderWindow & window)
        {
            if (rec == nullptr) return;
            if (p == nullptr) return;

            rectangle.setSize(sf::Vector2f(rec->rx, rec->ry));
            rectangle.setPosition(sf::Vector2f(p->px, p->py));
            rectangle.setFillColor(sf::Color(c->r, c->g, c->b));

            window.draw(rectangle);
        }
};

//renders circles
class circRenderSystem
{
    private:
        sf::CircleShape circle;

    public:
        void renderCirc(circleSizeComponent * r, positionComponent * p,
                        colorComponent * c, sf::RenderWindow & window)
        {
            if (r == nullptr) return;
            if (p == nullptr) return;

            circle.setRadius(r->r);
            circle.setPosition(sf::Vector2f(p->px, p->py));
            circle.setFillColor(sf::Color(c->r, c->g, c->b));

            window.draw(circle);
        }
};

//checks collisions within the hitbox map
class collisionSystem
{
    public:
    bool checkCollision(const entity e, positionComponent * p1, hitboxComponent * h1, 
                        velocityComponent * v1, componentManager & cm){
        //nullptr checks
        if (!p1 || !h1) return false;
        bool collide = false;

        //log all collisions that occur, then handle them later
        vector <pair <float, float>> collisionLog; 
        
        //iterate through all ents with a hitbox
        for (auto & c : cm.getMap<hitboxComponent>()){
            if (c.first.entity_id != e.entity_id){

                auto * p2 = cm.getComponent<positionComponent>(c.first);
                hitboxComponent & h2 = c.second;

                //nullptr checks
                if (!p1 || !p2) continue;

                //check object collision
                char face = getCollisionFace(p1,p2,h1,&h2);

                if (face != '\0'){

                    //log the face that collided
                    switch (face){
                        case 'l': 
                        collisionLog.emplace_back(-1.0f,0.0f);
                        if (v1->vx < 0) v1->vx *= -1;
                        //cout << "CASE L\n";
                        break;

                        case 'r':
                        collisionLog.emplace_back(1.0f,0.0f);
                        if (v1->vx > 0) v1->vx *= -1;
                        //cout << "CASE R\n";
                        break;

                        case 't':
                        collisionLog.emplace_back(0.0f,-1.0f);
                        if (v1->vy < 0) v1->vy *= -1;
                        //cout << "CASE T\n";
                        break;

                        case 'b':
                        collisionLog.emplace_back(0.0f,1.0f);
                        if (v1->vy > 0) v1->vy *= -1;
                        //cout << "CASE B\n";
                        break;
                    }
                    collide = true;
                
                    }
            }
        }


        return collide;
    }



    //returns the face of the colission (ie. t, b, l, r) returns \0 for no collision
    char getCollisionFace(positionComponent * p1, positionComponent * p2,
                          hitboxComponent * h1, hitboxComponent * h2){
        //ensure there is a colision before proceeding
        float me = 0.1f;

        if (!(p1->px - me <= p2->px + h2->x&&
              p2->px - me <= p1->px + h1->x&&
              p1->py - me <= p2->py + h2->y&&
              p2->py - me <= p1->py + h1->y)){
                return '\0';
            }

        //calculate the collision face
        float dx = (p1->px + h1->x / 2.0f) - (p2->px + h2->x / 2.0f);
        float dy = (p1->py + h1->y / 2.0f) - (p2->py + h2->y / 2.0f);

        float overlapX = (h1->x + h2->x) / 2.0f - std::abs(dx);
        float overlapY = (h1->y + h2->y) / 2.0f - std::abs(dy);

        if (overlapX < overlapY) {
            // Horizontal collision
            if (dx > 0)
                return 'l'; //collision on left side of obj1
            else
                return 'r';
        } else {
            // Vertical collision
            if (dy > 0)
                return 't'; //col on top side of obj1
            else
                return 'b';
        }

    }

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
        //runs all static systems
        void runStaticSystems(std::vector<entity>& ent, componentManager & cm, sf::RenderWindow & w){
            for (auto & e : ent){
                auto * s = cm.getComponent<rectangleSizeComponent>(e);
                auto * p = cm.getComponent<positionComponent>(e);
                auto * c = cm.getComponent<colorComponent>(e);
                if (!s) {
                    auto s = cm.getComponent<circleSizeComponent>(e);
                    cir.renderCirc(s,p,c,w);
                }else{
                    rec.renderRect(s,p,c,w);
                }
            }
            return;
        }

        //runs all dynamic systems
        void runDynamicSystems(std::vector <entity> & ent, componentManager & cm, sf::RenderWindow & w){
            for (auto it = ent.begin(); it != ent.end();){
                auto *v = cm.getComponent<velocityComponent>(*it);
                auto *h = cm.getComponent<hitboxComponent>(*it);
                auto *p = cm.getComponent<positionComponent>(*it);
                auto *s = cm.getComponent<rectangleSizeComponent>(*it);
                auto *c = cm.getComponent<colorComponent>(*it);
            
                //check entity collisions    
                col.checkCollision(*it,p,h,v,cm);

                //if object is deleted, dont draw
                if (!mov.updatePosition(*it,v,p,cm,ent)) continue;                
                
                //draw circles and squares
                if (!s){
                    auto *s = cm.getComponent<circleSizeComponent>(*it);
                    cir.renderCirc(s,p,c,w);
                }else{
                    rec.renderRect(s,p,c,w);
                }
                it++;
            }
        }
};
