#pragma once
#include "Entity.h"
#include <string>
#include <list>
#include <vector>

class EntityManager {
private:
	std::list<Entity* const> entities;

	// Use as a buffer when user wants to find 
	// many entities.
	std::vector<Entity* const> findResults;

	// Keep destroyed entities here. Remove entities 
	// next frame and release memory.
	std::list<Entity* const> destroyedEntities;
	
	void freeEntities();
public:
	EntityManager();

	bool addEntity(Entity* const entity);
	bool removeEntity(Entity* const entity);
	bool containsEntity(Entity* const entity) const;

	std::vector<Entity* const>& findEntitiesWithTag(const std::string& tag);
	
	Entity* const findEntityWithTag(const std::string& tag) const;
	Entity* const findEntityWithID(int id) const;

	void update();
	void draw();

	~EntityManager();
};

