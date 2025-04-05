#pragma once

#include "entity.h"
#include <unordered_map>
#include <any>
#include <typeindex>
#include <tuple>


//Velocity component for direction of travel
struct velocityComponent
{
    float vx;
    float vy;
    velocityComponent(): vx(0), vy(0) {}
    velocityComponent(float X, float Y): vx(X), vy(Y) {}
    velocityComponent(const velocityComponent & other) = default;
    ~velocityComponent() = default;
};
//Position component
struct positionComponent
{
    float px;
    float py;
    positionComponent(): px(0), py(0) {}
    positionComponent(float X, float Y): px(X), py(Y) {}
    positionComponent(const positionComponent & other) = default;
    ~positionComponent() = default;
};
//Contains colors for all simple objects without textures
struct colorComponent
{
    int r,g,b;
    colorComponent(): r(0), g(0), b(0) {}
    colorComponent(int R, int G, int B): r(R), g(G), b(B) {}
    colorComponent(const colorComponent& other) = default;
    ~colorComponent() = default;
};
//Size component for rectangular objects
struct rectangleSizeComponent
{
    int rx,ry;

    rectangleSizeComponent() : rx(0), ry(0) {}
    rectangleSizeComponent(int x, int y): rx(x), ry(y) {}
    rectangleSizeComponent(const rectangleSizeComponent & other) = default;
    ~rectangleSizeComponent() = default;
};
//Size component for circular objects
struct circleSizeComponent
{
    int r;

    circleSizeComponent() : r(0) {}
    circleSizeComponent(int rad): r(rad) {}
    circleSizeComponent(const circleSizeComponent & other) = default;
    ~circleSizeComponent() = default;
};
//Texture component for textured ents
struct textureComponent
{
    std::string fileName;

    textureComponent() : fileName("\0") {}
    textureComponent(std::string n) : fileName(n) {}
    textureComponent(const textureComponent & other) = default;
    ~textureComponent() = default;
};
//Hitbox for collission elements - this will always be rectangular
struct hitboxComponent
{
    int x,y;

    bool bounce;

    char type; // 'l' for leftgoal, 'r'for right goal, 'b' for ball, 'p' for paddle, 'w' for wall,

    hitboxComponent() : x(0), y(0), bounce(0), type('\0'){}
    hitboxComponent(const int X, const int Y, const bool b, const char t) : x(X), y(Y), bounce(b), type(t){}
    hitboxComponent(const hitboxComponent & other) = default;
    ~hitboxComponent() = default;
};



// Component list for compile-time iteration
using ComponentList = std::tuple<
    velocityComponent,
    positionComponent,
    colorComponent,
    rectangleSizeComponent,
    circleSizeComponent,
    textureComponent,
    hitboxComponent
>;


//class to handle all the components of the scene
//methods to generate component map and handle components
class componentManager {
    public:
        // Add or overwrite a component of type T for entity e
        template <typename T>
        void addComponent(const entity& e, const T& component) const
        {
            auto& map = getMap<T>();
            map[e] = component;
        }
    
        // Get a pointer to the component of type T for entity e
        template <typename T>
        T* getComponent(const entity& e) const
        {
            if (!e.isValid()) return nullptr;
            auto& map = getMap<T>();
            auto it = map.find(e);
    
            // If found, return a pointer to the component
            if (it != map.end()) {
                return &it->second;
            } else {
                return nullptr;
            }
        }
    
        // Check if an entity has a component of type T
        template <typename T>
        bool hasComponent(const entity& e) const
        {
            if (!e.isValid()) return false;
            auto& map = getMap<T>();
            return map.find(e) != map.end();
        }
    
        // Remove a component of type T from entity e
        template <typename T>
        void removeComponent(const entity& e) const
        {
            if (!e.isValid()) return;
            auto& map = getMap<T>();
            map.erase(e);
        }

        //  Clear all components of a type T
        template <typename T>
        void clearComponents() const
        {
            auto & map = getMap<T>();
            map.clear();

            return;
        }

        //  Clear all components for an entity
        void clearEntityComponents(const entity & e) const
        {
            if (!e.isValid()) return;
            //apply the lamda to all types in the component list which
            //will then call all of the remove component functions for each type
            std::apply([&](auto... type) {
                (..., removeComponent<decltype(type)>(e));
            }, ComponentList{});

            return;
        }

        // Storage for components of type T
        // Static map: shared across all instances and calls per component type
        template <typename T>
        std::unordered_map<entity, T>& getMap() const
        {
            static std::unordered_map<entity, T> map;
            return map;
        }
    };


