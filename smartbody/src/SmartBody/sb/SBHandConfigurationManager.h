#pragma once

#ifndef _SBHANDCONFIGURATIONMANAGER_H_
#define _SBHANDCONFIGURATIONMANAGER_H_

#include <sb/SBTypes.h>
#include <string>
#include <map>
#include <vector>

namespace SmartBody {

class SBHandConfiguration;

class SBHandConfigurationManager{

public:

	SBAPI SBHandConfigurationManager();
	SBAPI ~SBHandConfigurationManager();

	// create a hand configuration
	SBAPI SBHandConfiguration* createHandConfiguration(std::string configName);
	SBAPI SBHandConfiguration* getHandConfiguration(std::string configName);
	SBAPI void removeHandConfiguration(std::string configName);
	SBAPI int getNumHandConfigurations();
	SBAPI void printHandConfiguration(std::string configName);

private:

	// map of configurations
	std::map<std::string, SBHandConfiguration*> _configMap;
};



}
#endif