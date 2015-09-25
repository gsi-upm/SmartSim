#ifndef _STEERMANAGER_H_
#define _STEERMANAGER_H_

#include <sb/SBTypes.h>
#include <vhcl.h>
#include <sb/SBService.h>

class SteerSuiteEngineDriver;
namespace SteerLib
{
	class BoxObstacle;
}

namespace SmartBody {

class SBSteerAgent;

class SBSteerManager : public SmartBody::SBService
{
	public:
		SBAPI SBSteerManager();
		SBAPI ~SBSteerManager();
		
		SBAPI virtual void setEnable(bool enable);
		SBAPI virtual void start();
		SBAPI virtual void beforeUpdate(double time);
		SBAPI virtual void update(double time);
		SBAPI virtual void afterUpdate(double time);
		SBAPI virtual void stop();

		SBAPI virtual void onCharacterDelete(SBCharacter* character);

		SBAPI SteerSuiteEngineDriver* getEngineDriver();

		SBAPI SBSteerAgent* createSteerAgent(std::string name);
		SBAPI void removeSteerAgent(std::string name);
		SBAPI int getNumSteerAgents();
		SBAPI SBSteerAgent* getSteerAgent(std::string name);
		SBAPI std::vector<std::string> getSteerAgentNames();
		SBAPI std::map<std::string, SBSteerAgent*>& getSteerAgents();

	protected:
		std::map<std::string, SBSteerAgent*> _steerAgents;
		std::vector<SteerLib::BoxObstacle*> _boundaryObstacles;

		SteerSuiteEngineDriver* _driver;
		double _maxUpdateFrequency;


};

}

#endif 