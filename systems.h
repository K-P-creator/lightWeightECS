//#pragma GCC diagnostic ignored "-Wthread-safety"


#include "entity.h"
#include "components.h"
#include "globals.h"

#include <cmath>
#include <vector>
#include <utility>
#include <thread>
#include <algorithm>
#include <optional>

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
            return false;
        }
        if (p->py > HEIGHT*1.2 || p->py < 0-HEIGHT*.2){
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
    optional<vector<pair<float,float>>> checkCollision(const entity e, positionComponent * p1, hitboxComponent * h1, 
                        velocityComponent * v1, componentManager & cm){
        //nullptr checks
        if (!p1 || !h1) return nullopt;

        //log all collisions that occur, then handle them later
        vector <pair <float, float>> collisionLog; 
        
        //iterate through all ents with a hitbox
        for (const auto & c : cm.getMap<hitboxComponent>()){
            if (c.first.entity_id != e.entity_id){

                auto * p2 = cm.getComponent<positionComponent>(c.first);
                hitboxComponent h2 = c.second;

                //nullptr check
                if (!p2) continue;

                //check object collision
                char face = getCollisionFace(p1,p2,h1,&h2);

                if (face != '\0'){

                    //log the face that collided
                    switch (face){
                        case 'l': 
                        collisionLog.emplace_back(-1.0f,0.0f);
                        //cout << "CASE L\n";
                        break;

                        case 'r':
                        collisionLog.emplace_back(1.0f,0.0f);
                        //cout << "CASE R\n";
                        break;

                        case 't':
                        collisionLog.emplace_back(0.0f,-1.0f);
                        //cout << "CASE T\n";
                        break;

                        case 'b':
                        collisionLog.emplace_back(0.0f,1.0f);
                        //cout << "CASE B\n";
                        break;
                    }                
                }
            }
        }


        if (collisionLog.size() > 0) return collisionLog;
        else return nullopt;
    }



    //returns the face of the collision (ie. t, b, l, r) returns \0 for no collision
    char getCollisionFace(positionComponent * p1, positionComponent * p2,
                          hitboxComponent * h1, hitboxComponent * h2){
        //ensure there is a collision before proceeding
        constexpr float me = 0.1f;

        if (!(p1->px < p2->px + h2->x + me &&
            p1->px + h1->x > p2->px - me &&
            p1->py < p2->py + h2->y + me &&
            p1->py + h1->y > p2->py - me)) {
          return '\0'; // no collision
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

		size_t threadCount = std::max(static_cast<size_t>(thread::hardware_concurrency()), size_t{1});

    public:
        //runs all static systems
        void runStaticSystems(vector<entity>& ent, componentManager & cm, sf::RenderWindow & w){

            //draw static entities
            for (auto& e : ent) {
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
            
            //create threads
            size_t perThread = ent.size() / threadCount;
            if (perThread == 0) perThread = 1;
            vector <thread> threads;


            //lambda for threaded collisions and position updates
            auto runSectionCol = [&](size_t beginIdx, size_t endIdx) {

                for (size_t idx = beginIdx; idx < endIdx; idx++) {

                    auto* v = cm.getComponent<velocityComponent>(ent[idx]);
                    auto* h = cm.getComponent<hitboxComponent>(ent[idx]);
                    auto* p = cm.getComponent<positionComponent>(ent[idx]);


                    //check entity collisions    
                    auto c = col.checkCollision(ent[idx], p, h, v, cm);

                    //if there is a collision, update the velocity
                    if (c) {
                        bool flipX = false;
                        bool flipY = false;
                    
                        for (const auto& normal : c.value()) {
                            if (normal.first  != 0.0f) flipX = true;
                            if (normal.second != 0.0f) flipY = true;
                        }
                    
                        if (flipX) v->vx *= -1.0f;
                        if (flipY) v->vy *= -1.0f;
                    }
                          
                }
            };


            //list of entities to delete after the positions are updated
            vector <entity> delList {};

            //lambda for update positions
            auto runSectionPos = [&](size_t startIdx, size_t endIdx){
                for (size_t idx = startIdx; idx < endIdx; idx ++){
                    auto* v = cm.getComponent<velocityComponent>(ent[idx]);
                    auto* p = cm.getComponent<positionComponent>(ent[idx]);

                    if (!mov.updatePosition(ent[idx],v,p,cm,ent)) delList.push_back(ent[idx]);
                }
            };


            //dispatch threads for collision checks
            for (int i = 0; i < ent.size(); i += perThread) {
                size_t endIdx = min(i + perThread, ent.size());
                threads.emplace_back(runSectionCol, i, endIdx);
            }


            //wait for threads
            for (auto & t : threads) {
                t.join();
            }

            //reset threads
            threads.clear();

            //dispatch threads for position updates
            for (int i = 0; i < ent.size(); i += perThread) {
                size_t endIdx = min(i + perThread, ent.size());
                threads.emplace_back(runSectionPos, i, endIdx);
            }

            //wait for threads
            for (auto & t : threads) {
                t.join();
            }



            //delete any ents logged for deletion
            for (const auto & del : delList){
                cm.clearEntityComponents(del);

                //delete the entity from the vector of entities
                auto it = std::find(ent.begin(), ent.end(), del);
                if (it != ent.end()) {
                    ent.erase(it);
                }
            }

            //delete anything logged for deletion
            //for (auto i : toRemove){
            //    ent.erase(ent.begin() + i);
            //}

            //draw all the dynamic objects
            for (auto it = ent.begin(); it != ent.end();){
                auto *p = cm.getComponent<positionComponent>(*it);
                auto *s = cm.getComponent<rectangleSizeComponent>(*it);
                auto *c = cm.getComponent<colorComponent>(*it);
                
                //draw circles and squares
                if (!s){
                    auto *s = cm.getComponent<circleSizeComponent>(*it);
                    cir.renderCirc(s,p,c,w);
                }else{
                    rec.renderRect(s,p,c,w);
                }
                it++;
            }
            

            return;
        }
};
