//Entity struct

//Contains a unique ID that must be assigned when a new entity is created
//ID will be used to look up the components of the entity by the game engine

#pragma once

#include <stdexcept>
#include <cstdint>
#include <iostream>

struct entity{
    std::uint32_t entity_id; 

    entity(): entity_id(0xFFFFFFFF){}

    entity(int id) : entity_id(id){}
    ~entity() = default;

    bool operator==(const entity& other) const {
        return entity_id == other.entity_id;
    }

    bool operator != (const entity& other) const{
        return entity_id != other.entity_id;
    }

    bool isValid() const {
        return entity_id != 0xFFFFFFFF;
    }
};

// Required for unordered_map hashing
namespace std {
    template<>
    struct hash<entity> {
        std::size_t operator()(const entity& e) const noexcept {
            if (e.entity_id == 0xFFFFFFFF) {
                std::cerr << "Warning: Hashing invalid entity!\n";
            }
            return std::hash<uint32_t>{}(e.entity_id);
        }
    };
}
