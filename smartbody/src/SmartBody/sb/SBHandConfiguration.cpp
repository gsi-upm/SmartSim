#include "SBHandConfiguration.h"

#include <sb/SBMotion.h>
#include <sb/SBAssetManager.h>


namespace SmartBody {

SBHandConfiguration::SBHandConfiguration()
{
}

SBHandConfiguration::~SBHandConfiguration()
{
	// don't delete any motions here
	_motions.clear();
}

void SBHandConfiguration::addMotion(std::string motionName)
{
	// extract motion from scene
	SBMotion* motion = SmartBody::SBScene::getScene()->getAssetManager()->getMotion(motionName);

	if (!motion)
	{
		LOG("Unable to find motion named %s",motionName.c_str());
		return;
	}

	// add motion
	_motions.push_back(motion);
}

void SBHandConfiguration::removeMotion(std::string motionName)
{
	SBMotion* motion = SmartBody::SBScene::getScene()->getAssetManager()->getMotion(motionName);

	if (!motion)
	{
		LOG("Unable to find motion named %s",motionName.c_str());
		return;
	}

	std::vector<SBMotion*>::iterator iter = std::find(_motions.begin(), _motions.end(), motion);

	// if found remove
	if (iter != _motions.end())
	{
		_motions.erase(iter);
	}
}

int SBHandConfiguration::getNumMotions()
{
	return _motions.size();
}

void SBHandConfiguration::printMotionNames()
{
	std::vector<SBMotion*>::iterator iter = _motions.begin();
	for (; iter != _motions.end() ; iter++)
	{
		SBMotion* motion = *iter;
		LOG("Motion name is %s", motion->getName().c_str());
	}
}

SBMotion* SBHandConfiguration::getMotion(int i)
{
	if (i >= _motions.size())
	{
		LOG("i is greater than motion vector size");
		return NULL;
	}

	return _motions[i];
}

std::vector<std::string> SBHandConfiguration::getMotionNames()
{
	std::vector<std::string> motionNames;
	std::vector<SBMotion*>::iterator iter = _motions.begin();
	for (; iter != _motions.end() ; iter++)
	{
		SBMotion* motion = *iter;
		motionNames.push_back(motion->getName());
	}

	return motionNames;
}



}
