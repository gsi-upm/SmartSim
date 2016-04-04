#include "SBReachManager.h"

#include <sb/SBScene.h>
#include <sb/SBReach.h>

namespace SmartBody {

SBReachManager::SBReachManager()
{
}

SBReachManager::~SBReachManager()
{
}


SBAPI SBReach* SBReachManager::createReachWithTag( std::string characterName, std::string reachTag )
{
	SBScene* scene = SmartBody::SBScene::getScene();
	// get the character
	SBCharacter* character = scene->getCharacter(characterName);
	if (!character)
	{
		LOG("Character named %s does not exist.", characterName.c_str());
		return NULL;
	}
	std::string charReachTag = characterName + "_" + reachTag;
	std::map<std::string, SBReach*>::iterator iter = _reaches.find(charReachTag);
	if (iter != _reaches.end())
	{
		// remove the old reach data
		SBReach* reach = (*iter).second;
		removeReach(reach);
		_reaches.erase(iter);
	}

	SBReach* reach = new SBReach(character, reachTag);
	//_reaches.insert(pair<std::string, SBReach*>(characterName, reach));
	_reaches[charReachTag] = reach;
	return reach;
}


SBReach* SBReachManager::createReach(std::string characterName)
{
	return createReachWithTag(characterName,"default");
}

void SBReachManager::removeReach(SBReach* reach)
{
	SBCharacter* character = reach->getCharacter();
	 
	// clean up all the reach structures...
	// ...
	// ...
}

int SBReachManager::getNumReaches()
{
	return _reaches.size();
}

SBReach* SBReachManager::getReach(std::string characterName, std::string reachTag)
{
	std::string charReachTag = characterName + "_" + reachTag;
	std::map<std::string, SBReach*>::iterator iter = _reaches.find(charReachTag);
	if (iter != _reaches.end())
	{
		return (*iter).second;
	}
	else
	{
		return NULL;
	}
}



}
