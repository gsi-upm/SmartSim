#ifndef _STEERAGENT_H_
#define _STEERAGENT_H_

#include <sb/SBTypes.h>
#include <string>

namespace SmartBody {

class SBCharacter;

class SBSteerAgent
{
	public:
		SBAPI SBSteerAgent();
		SBAPI SBSteerAgent(SBCharacter* sbCharacter);
		SBAPI ~SBSteerAgent();

		SBAPI SBCharacter* getCharacter();

		SBAPI virtual void evaluate(double dt);

		SBAPI void setSteerStateNamePrefix(std::string prefix);
		SBAPI const std::string& getSteerStateNamePrefix();
		SBAPI void setSteerType(std::string type);
		SBAPI const std::string& getSteerType();


	private:
		std::string _steerType;
		std::string _stateNamePrefix;
		SBCharacter* _character;
};
}

#endif