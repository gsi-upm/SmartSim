#ifndef _SBHANDCONFIGURATION_H_
#define _SBHANDCONFIGURATION_H_

#include <sb/SBScene.h>
#include <string>

namespace SmartBody{


class SBHandConfiguration{

public:
	SBAPI SBHandConfiguration();
	SBAPI ~SBHandConfiguration();

	SBAPI void addMotion(std::string motionName);
	SBAPI void removeMotion(std::string motionName);
	SBAPI int getNumMotions();
	SBAPI void printMotionNames();
	SBAPI SBMotion* getMotion(int i);
	SBAPI std::vector<std::string> getMotionNames();

private:
	
	// loaded motions
	std::vector<SBMotion*> _motions;
};

}

#endif