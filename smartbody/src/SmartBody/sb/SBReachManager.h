#ifndef _SBREACHMANAGER_H_
#define _SBREACHMANAGER_H_

#include <sb/SBTypes.h>
#include <string>
#include <map>

namespace SmartBody {

class SBReach;

class SBReachManager
{
	public:
		SBAPI SBReachManager();
		SBAPI ~SBReachManager();

		SBAPI SBReach* createReach(std::string characterName);
		SBAPI SBReach* createReachWithTag(std::string characterName, std::string reachTag);

		SBAPI void removeReach(SBReach* reach);
		SBAPI int getNumReaches();
		SBAPI SBReach* getReach(std::string characterName, std::string reachTag);

	protected:
		std::map<std::string, SBReach*> _reaches;
};


}

#endif