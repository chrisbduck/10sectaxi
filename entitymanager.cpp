//------------------------------------------------------------------------------
// EntityManager: Manages all interactive elements in the game that do not have
//                their own subsystems - i.e., the player, enemies, etc.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "entitymanager.h"

#include "useful.h"

#include <sstream>

//------------------------------------------------------------------------------
// EntityManager
//------------------------------------------------------------------------------

EntityManager::EntityManager()
{
}

//------------------------------------------------------------------------------

EntityManager::~EntityManager()
{
	shutDown();
}

//------------------------------------------------------------------------------

EntityManager& EntityManager::instance()
{
	static EntityManager sInstance;
	return sInstance;
}

//------------------------------------------------------------------------------

void EntityManager::registerFactory(const std::string &lrType, Entity::FactoryFn lpFactoryFunc)
{
	mFactories[lrType] = lpFactoryFunc;
	printf("Registered factory for \"%s\"\n", lrType.c_str());
}

//------------------------------------------------------------------------------

void EntityManager::registerEntity(Entity *lpNewEntity)
{
	mEntities.push_back(lpNewEntity);
	if (lpNewEntity->name().empty())
		setNameForEntity(lpNewEntity);
	
	printf("Registered entity \"%s\" at (%.1f, %.1f)\n", lpNewEntity->name().c_str(), lpNewEntity->x(), lpNewEntity->y());
}

//------------------------------------------------------------------------------

void EntityManager::setNameForEntity(Entity *lpEntity)
{
	const char* lpType = lpEntity->type();
	std::ostringstream lOutStr;
	lOutStr << lpType << " " << mEntities.size();
	std::string lName = lOutStr.str();
	lpEntity->setName(lName);
}

//------------------------------------------------------------------------------

void EntityManager::update(float lTimeDeltaSec)
{
	for (Entity* lpEntity: mEntities)
		lpEntity->update(lTimeDeltaSec);
}

//------------------------------------------------------------------------------

void EntityManager::render() const
{
	for (Entity* lpEntity: mEntities)
		lpEntity->render();
}

//------------------------------------------------------------------------------

Entity* EntityManager::create(const std::string& lrParameterString)
{
	std::vector<std::string> lAllParameters = split(lrParameterString);
	std::string lType = getStringParam(lAllParameters, 0);
	lAllParameters.erase(lAllParameters.begin());
	return create(lType, lAllParameters);
}

//------------------------------------------------------------------------------

Entity* EntityManager::create(const std::string& lrType, const std::string& lrParameterString)
{
	std::vector<std::string> lParameters = split(lrParameterString);
	return create(lrType, lParameters);
}

//------------------------------------------------------------------------------

Entity* EntityManager::create(const std::string& lrType, const std::vector<std::string>& lrParameters)
{
	// Find the right factory
	auto liFactory = mFactories.find(lrType);
	if (liFactory == mFactories.end())
	{
		printf("Factory for \"%s\" not found.\n", lrType.c_str());
		return nullptr;
	}
	
	// Create the entity
	Entity::FactoryFn lpFactoryFunc = liFactory->second;
	Entity* lpNewEntity = lpFactoryFunc(lrParameters);
	registerEntity(lpNewEntity);
	return lpNewEntity;
}

//------------------------------------------------------------------------------

void EntityManager::init()
{
}

//------------------------------------------------------------------------------

void EntityManager::shutDown()
{
	for (Entity* lpEntity: mEntities)
		delete lpEntity;
	mEntities.clear();
}

//------------------------------------------------------------------------------
// EntityFactory
//------------------------------------------------------------------------------

EntityFactory::EntityFactory(const std::string& lrType, Entity::FactoryFn pFunc)
{
	EntityManager::instance().registerFactory(lrType, pFunc);
}

//------------------------------------------------------------------------------

Entity* createTestEntity(const std::vector<std::string>& lrParams)
{
	return new Entity(getFloatParam(lrParams, 0), getFloatParam(lrParams, 1));
}

EntityFactory sTestFactory("test", &createTestEntity);


