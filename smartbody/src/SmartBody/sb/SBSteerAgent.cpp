#include "SBSteerAgent.h"

#include <sb/SBCharacter.h>
#include <sbm/PPRAISteeringAgent.h>

namespace SmartBody {

SBSteerAgent::SBSteerAgent() 
{
}

SBSteerAgent::SBSteerAgent(SBCharacter* sbCharacter) 
{
	_character = sbCharacter;
}

SBSteerAgent::~SBSteerAgent()
{
}

void SBSteerAgent::evaluate(double dtime)
{
}

SBCharacter* SBSteerAgent::getCharacter()
{
	return _character;
}

void SBSteerAgent::setSteerStateNamePrefix(std::string prefix)
{
	_stateNamePrefix = prefix;
	PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(this);
	SbmCharacter* character = ppraiAgent->getCharacter();
	if (character)
		character->statePrefix = _stateNamePrefix;
}

const std::string& SBSteerAgent::getSteerStateNamePrefix()
{
	return _stateNamePrefix;
}

void SBSteerAgent::setSteerType(std::string type)
{
	_steerType = type;
	
	SbmCharacter* character = this->getCharacter();
	if (!character)
		return;

	

	if (_steerType == "example")
	{
		if (character->checkExamples())
			character->locomotion_type = character->Example;
		else
			character->locomotion_type = character->Basic;
	}
	if (type == "procedural")
	{
		character->locomotion_type = character->Procedural;
		PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(this);
		if (ppraiAgent)
			ppraiAgent->desiredSpeed = 1.6f;
	}
	if (type == "basic")
	{
		character->locomotion_type = character->Basic;
	}
	if (type == "new")
	{
		character->locomotion_type = character->New;
	}
}

const std::string& SBSteerAgent::getSteerType()
{
	return _steerType;
}

}