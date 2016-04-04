#include "SBHandConfigurationManager.h"
#include <sb/SBHandConfiguration.h>
#include <sb/SBScene.h>

namespace SmartBody{

SBAPI SBHandConfigurationManager::SBHandConfigurationManager()
{
}

SBAPI SBHandConfigurationManager::~SBHandConfigurationManager()
{
	std::map<std::string, SBHandConfiguration*>::iterator iter = _configMap.begin();
	for (; iter != _configMap.end(); iter++)
	{
		delete iter->second;
	}
	_configMap.clear();
}

SBAPI SBHandConfiguration* SBHandConfigurationManager::createHandConfiguration(std::string configName)
{
	SBScene* scene = SmartBody::SBScene::getScene();

	std::map<std::string, SBHandConfiguration*>::iterator iter = _configMap.find(configName);
	if (iter != _configMap.end())
	{
		delete iter->second;
		_configMap.erase(iter);
	}

	SBHandConfiguration* config = new SBHandConfiguration();
	_configMap.insert(std::pair<std::string, SBHandConfiguration*>(configName, config));
	return config;
}

SBAPI void SBHandConfigurationManager::removeHandConfiguration(std::string configName)
{
	std::map<std::string, SBHandConfiguration*>::iterator iter = _configMap.find(configName);
	if (iter != _configMap.end())
	{
		delete iter->second;
		_configMap.erase(iter);
	}
}

SBAPI int SBHandConfigurationManager::getNumHandConfigurations()
{
	return _configMap.size();
}

SBAPI void SBHandConfigurationManager::printHandConfiguration(std::string configName)
{
	std::map<std::string, SBHandConfiguration*>::iterator iter = _configMap.find(configName);
	if (iter != _configMap.end())
	{
		std::string name = iter->first;
		LOG("Name of the hand configuration is %s",name.c_str());
		SBHandConfiguration* config = iter->second;
		config->printMotionNames();
	}
}

SBAPI SBHandConfiguration* SBHandConfigurationManager::getHandConfiguration(std::string configName)
{
	std::map<std::string, SBHandConfiguration*>::iterator iter = _configMap.find(configName);
	if (iter != _configMap.end())
	{
		return iter->second;
	}else{
		LOG("Unable to find hand configuration named %s",configName.c_str());
		return NULL;
	}
}

}


