// Definition for the QuadTree data stracture
// used to localize the collisions in the scene

#include "components.h"
#include "entity.h"
#include "globals.h"

#include <memory>
#include <array>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

using namespace std;

#ifdef DEBUG
#define DBG(x) std::cout << x << std::endl
#else
#define DBG(x)
#endif

class quadTree
{
private:
	unsigned int level;
	vector<entity> objects;

	//origin of the area
	int oX, oY;

	//bounds of the area
	int bX, bY;

	//nodes of the quadtree
	array<unique_ptr<quadTree>, 4> nodes;

	//reference to the component manager
	componentManager& cm;

public:
	quadTree(int level, int x, int y, int width, int height, componentManager& c)
		: level(level), oX(x), oY(y), bX(width), bY(height), cm(c)
	{
		nodes[0] = nullptr;
		nodes[1] = nullptr;
		nodes[2] = nullptr;
		nodes[3] = nullptr;
	}

	quadTree(componentManager& c) : level(0), oX(0), oY(0), bX(WIDTH), bY(HEIGHT), cm(c)
	{
		nodes[0] = nullptr;
		nodes[1] = nullptr;
		nodes[2] = nullptr;
		nodes[3] = nullptr;
	}

	~quadTree() = default;


	//builds the quadtree with the given entities
	void buildTree(vector<entity>& ents) {
		insertNodes(ents);
	}


	//inserts a vector of entities into the quadtree
	void insertNodes(vector<entity>& ent) {
		for (auto& i : ent) {
			insertNode(i);
		}
	}


	//clears the quadtree and resets all nodes
	void clearTree() {
		objects.clear();
		for (auto& i : nodes) {
			if (i != nullptr) {
				i->clearTree();
				i.reset();
			}
		}
		nodes[0] = nullptr;
		nodes[1] = nullptr;
		nodes[2] = nullptr;
		nodes[3] = nullptr;
		level = 0;
	}


	//inserts an entity into the quadtree
	void insertNode(entity& ent) {
		DBG("Inserting ent " << ent.entity_id);

		//checks if the entity is in bounds of the quadtree node
		//inserts if the node has no children and the object count is less than the max objects
		if (nodes[0] == nullptr && objects.size() < MAX_OBJECTS) {
			if (inBounds(ent)) {
				DBG("Ent inserted into node " << level);
				objects.push_back(ent);
			}
			return;
		}


		//splits the node if the objecct count gets too high
		if (nodes[0] == nullptr && objects.size() >= MAX_OBJECTS && level < MAX_LEVEL) {
			DBG("Splitting node " << level);
			split();

			return;
		}


		//if the node has child nodes, insert the entity into the child nodes
		if (nodes[0] != nullptr) {
			for (auto& i : nodes) {
				DBG("Inserting to child node(s)");
				if (i == nullptr) continue;
				i->insertNode(ent);
			}
			return;
		}

		if (level == MAX_LEVEL){
			objects.push_back(ent);

			return;
		}
	}


	//splits the node into 4 child nodes and distributes the objects to the child nodes
	//this will handle the allocation of entities to the child nodes
	void split() {
		int subWidth = bX / 2;
		int subHeight = bY / 2;
		int x = oX;
		int y = oY;

		nodes[0] = make_unique<quadTree>(level + 1, x + subWidth, y, subWidth, subHeight, cm);
		nodes[1] = make_unique<quadTree>(level + 1, x, y, subWidth, subHeight, cm);
		nodes[2] = make_unique<quadTree>(level + 1, x, y + subHeight, subWidth, subHeight, cm);
		nodes[3] = make_unique<quadTree>(level + 1, x + subWidth, y + subHeight, subWidth, subHeight, cm);

		for (auto& i : objects) {
			for (auto& j : nodes) {
				if (j == nullptr) continue;
				j->insertNode(i);
			}
		}

		objects.clear();
		return;
	}


	//checks if the entity is in bounds of the quadtree node
	bool inBounds(entity& ent) {
		auto* p1 = cm.getComponent<positionComponent>(ent);
		auto* h1 = cm.getComponent<hitboxComponent>(ent);
		if (!p1 || !h1) return false;

		if (p1->px < oX + bX + EPSILON_ME &&
			p1->px + h1->x > oX - EPSILON_ME &&
			p1->py < oY + bY + EPSILON_ME &&
			p1->py + h1->y > oY - EPSILON_ME) {
			DBG("Ent " << ent.entity_id << " is in bounds");
			return true;
		}

		return false;
	}


	//returns a vector of entities that are near the given entity and may collide
	vector<entity> getCollisions(entity& ent) {
		//stores the entities to check
		vector<entity> collidingEntities{};

		//check if the entity is in bounds
		if (!inBounds(ent)) return collidingEntities;

		//check if the entity is in the current node
		if (!objects.empty() && nodes[0] == nullptr) {
			DBG("No child nodes in getCollisions");
			for (auto& i : objects) {
				if (i.entity_id == ent.entity_id) continue;
				collidingEntities.push_back(i);
			}
		}

		//check if the entity is in the child nodes
		for (auto& i : nodes) {
			DBG("Checking child nodes in getCollisions");
			if (i == nullptr) continue;
			auto temp = i->getCollisions(ent);
			collidingEntities.insert(collidingEntities.end(), temp.begin(), temp.end());
		}

		return collidingEntities;
	}
};
