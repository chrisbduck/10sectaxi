//------------------------------------------------------------------------------
// EntityManager: Manages all interactive elements in the game that do not have
//                their own subsystems - i.e., the player, enemies, etc.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include "entity.h"

#include <string>
#include <unordered_map>
#include <vector>

//------------------------------------------------------------------------------

// Construct a new EntityFactory object anywhere in global scope to register a
// type of entity along with a function to create it
struct EntityFactory
{
	EntityFactory(const std::string& lrType, Entity::FactoryFn pFunc);
};

//------------------------------------------------------------------------------

class EntityManager
{
public:
	EntityManager();
	~EntityManager();
	
	static EntityManager& instance();
	
	void init();
	void shutDown();
	
	void registerFactory(const std::string& lrType, Entity::FactoryFn lpFactoryFunc);
	void registerEntity(Entity* lpNewEntity);	// only call this when creating entities outside EntityManager::create()
	
	void setNameForEntity(Entity* lpEntity);	// sets a default name; call after registering
	
	Entity* create(const std::string& lrParameterString);
	Entity* create(const std::string& lrType, const std::string& lrParameterString);
	Entity* create(const std::string& lrType, const std::vector<std::string>& lrParameters);
	
	const std::vector<Entity*>& allEntities() const { return mEntities; }
	
	void update(float lTimeDeltaSec);
	void render() const;
	
private:
	std::vector<Entity*> mEntities;
	std::unordered_map<std::string, Entity::FactoryFn> mFactories;
};

#define gEntityManager EntityManager::instance()

//------------------------------------------------------------------------------

#endif // ENTITYMANAGER_H
