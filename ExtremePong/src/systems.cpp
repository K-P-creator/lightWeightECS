#include "../include/systems.h"


void movementSystem::updatePosition (const entity & e, velocityComponent* v, positionComponent* p, 
    componentManager & cm, std::vector<entity> & ents)
    {
    //cout << "Updating position of " << e.entity_id << endl;
    if (!v || !p) return;

    //add accleration to velocity
    auto a = cm.getComponent<accelerationComponent>(e);
    if(a){
        if (v->vx > 0) v->vx += a->ax * timestep;
        else if (v->vx < 0) v->vx -= a->ax * timestep;

        if (v->vy > 0) v->vy += a->ay * timestep;
        else if (v->vy < 0) v->vy -= a->ay * timestep;
    }
    //cout << "vx = " << v->vx << endl << "vy = " << v->vy << endl;
    // Add velocity to position
    //cout << "prev pos = (" << p->px << "," << p->py << ")\n";
    p->px += v->vx * timestep;
    p->py += v->vy * timestep;

    //cout << "current pos = (" << p->px << "," << p->py << ")\n";
    //OOB checking for objects travelling off into oblivion
    if ((p->px > WIDTH*1.2 || p->px < 0-WIDTH*.2) || 
        (p->py > HEIGHT*1.2 || p->py < 0-HEIGHT*.2)) {
        //clear all the components to prepare for deletion
        cm.clearEntityComponents(e);

        //delete the entity from the vector of entities
        std::erase(ents, e);
    }
}


void rectRenderSystem::renderRect(rectangleSizeComponent * rec, positionComponent * p,
    colorComponent * c, outlineComponent * o, sf::RenderWindow & window)
{
    if (!p || !rec) return;

    rectangle.setSize(sf::Vector2f(rec->rx, rec->ry));
    rectangle.setPosition(sf::Vector2f(p->px, p->py));
    rectangle.setFillColor(sf::Color(c->r, c->g, c->b));

    if (o){
        rectangle.setOutlineColor(sf::Color(o->r,o->b,o->g));
        rectangle.setOutlineThickness(5.0f);
    }else{
        rectangle.setOutlineThickness(0);
    }
    window.draw(rectangle);
}


void circRenderSystem::renderCirc(circleSizeComponent * r, positionComponent * p,
    colorComponent * c, outlineComponent * o, sf::RenderWindow & window)
{
    if (r == nullptr) return;
    if (p == nullptr) return;

    circle.setRadius(r->r);
    circle.setPosition(sf::Vector2f(p->px, p->py));
    circle.setFillColor(sf::Color(c->r, c->g, c->b));

    if (o){
        circle.setOutlineColor(sf::Color(o->r,o->b,o->g));
        circle.setOutlineThickness(5.0f);
    }else{
        circle.setOutlineThickness(0);
    }

    window.draw(circle);
}


bool collisionSystem::checkCollision(const entity e, positionComponent * p1, hitboxComponent * h1, 
    velocityComponent * v1, componentManager & cm){
    if (h1->type == 'p' || h1->type == 'w') return false;
    //nullptr checks
    if (!p1 || !h1) return false;
    bool collide = false;

    //log all collisions that occur, then handle them later
    //vector <pair <float, float>> collisionLog; 

    //iterate through all ents with a hitbox
    //cout << "Starting Loop...\n";
    for (auto & c : cm.getMap<hitboxComponent>()){
        
        if (c.first.entity_id != e.entity_id){

            //cout << "Getting p2...\n";
            auto * p2 = cm.getComponent<positionComponent>(c.first);
            hitboxComponent & h2 = c.second;

            //nullptr check
            if (!p2) continue;

            //cout << "Getting Col Face...\n";
            //check object collision
            char face = getCollisionFace(p1,p2,h1,&h2);

            //cout << "Face Got...\n";
            if (!v1) continue;
            if (face != '\0'){
                if (h2.type == 'l'){
                    resetBall(e, cm, true);
                    //cout << "LEFT GOAL\n";
                    leftScore++;
                    return true;
                }
                if (h2.type == 'r'){
                    resetBall(e, cm, false);
                    //cout << "RIGHT GOAL\n";
                    rightScore++;
                    return true;
                }
                //log the face that collided
                //multiple by 1.01 to speed up the ball for every bounce
                switch (face){
                    case 'l': 
                        //collisionLog.emplace_back(-1.0f,0.0f);
                        if (v1->vx < 0) v1->vx *= -1.01;
                        //cout << "CASE L\n";
                    break;

                    case 'r':
                        //collisionLog.emplace_back(1.0f,0.0f);
                        if (v1->vx > 0) v1->vx *= -1.01;
                        //cout << "CASE R\n";
                    break;

                    case 't':
                        //collisionLog.emplace_back(0.0f,-1.0f);
                        if (v1->vy < 0) v1->vy *= -1.01;
                        //cout << "CASE T\n";
                    break;

                    case 'b':
                        //collisionLog.emplace_back(0.0f,1.0f);
                        if (v1->vy > 0) v1->vy *= -1.01;
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
char collisionSystem::getCollisionFace(positionComponent * p1, positionComponent * p2,
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


//reset the ball to middle and aim it at the player who just scored
void collisionSystem::resetBall (const entity & e, componentManager & cm, bool side){
    float speed;
    if (!side){
        speed = startingBallSpeed * -1.0f;
    }else{
        speed = startingBallSpeed;
    }
    cm.addComponent<velocityComponent>(e, velocityComponent(speed, speed));
    cm.addComponent<positionComponent>(e, positionComponent(WIDTH/2, HEIGHT/2));

    paddleSpeed = startingPaddleSpeed;
    return;
}