#include "SBRealtimeManager.h"
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sb/SBJoint.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBAttribute.h>
#include <vhcl.h>
#include <iostream>
#include <sstream>

namespace SmartBody {

SBRealtimeManager::SBRealtimeManager() : SBService()
{
	setEnable(false);
	setName("Realtime");
	createBoolAttribute("debug", false, true, "Basic", 60, false, false, false, "Show debugging information.");
	createActionAttribute("dumpData", true, "Basic", 60, false, false, false, "Dump the faceshift data.");
}

SBRealtimeManager::~SBRealtimeManager()
{
	
}

void SBRealtimeManager::setEnable( bool val )
{	
	SBService::setEnable(val);	
	if (val)
		start();
	else
		stop();
}

bool SBRealtimeManager::isEnable()
{
	return SBService::isEnable();
}

void SBRealtimeManager::start()
{
	channelTable.clear();

	initConnection();
}

void SBRealtimeManager::stop()
{
	stopConnection();
}


void SBRealtimeManager::update( double time )
{
	if (!this->isEnable())
		false;
}

void SBRealtimeManager::initConnection()
{
}

void SBRealtimeManager::stopConnection()
{
}

void SBRealtimeManager::setChannelNames(const std::string& channels)
{
	channelTable.clear();
	channelNames.clear();

	std::vector<std::string> tokens;
	vhcl::Tokenize(channels, tokens, " ");
	
	for (size_t c = 0; c < tokens.size(); c++)
	{
		channelNames.push_back(tokens[c]);
		channelTable.insert(std::pair<std::string, std::string>(tokens[c], ""));
	}
}

std::vector<std::string>& SBRealtimeManager::getChannelNames()
{
	return channelNames;
}

void SBRealtimeManager::setData(const std::string& channel, const std::string& data)
{
	if (channelTable.find(channel) != channelTable.end())
	{
		channelTable[channel] = data;
	}
	else
	{
		if (this->getBoolAttribute("debug"))
			LOG("Cannot find channel named: %s", channel.c_str());
	}
}

std::string SBRealtimeManager::getData(const std::string& channel)
{
	std::string data = "";
	if (channelTable.find(channel) != channelTable.end())
	{
		data = channelTable[channel];
	}
	else
	{
		if (this->getBoolAttribute("debug"))
			LOG("Cannot find channel named: %s", channel.c_str());
	}
	
	return data;
}

SrQuat SBRealtimeManager::getDataQuat(const std::string& channel)
{
	SrQuat quat;
	std::string data = getData(channel);
	if (data == "")
		return quat;

	std::vector<std::string> tokens;
	vhcl::Tokenize(data, tokens, " ");
	if (tokens.size() < 4)
		return quat;

	double w = atof(tokens[0].c_str());
	double x = atof(tokens[1].c_str());
	double y = atof(tokens[2].c_str());
	double z = atof(tokens[3].c_str());

	quat.set((float) w, (float) x, (float) y, (float) z);

	return quat;
}

SrMat SBRealtimeManager::getDataMat(const std::string& channel)
{
	SrMat mat;
	std::string data = getData(channel);
	if (data == "")
		return mat;

	std::vector<std::string> tokens;
	vhcl::Tokenize(data, tokens, " ");
	if (tokens.size() < 16)
		return mat;

	for (int m = 0; m < 16; m++)
		mat.set(m, (float) atof(tokens[m].c_str()));
	
	return mat;
}

SrVec SBRealtimeManager::getDataVec(const std::string& channel)
{
	SrVec vec;
	std::string data = getData(channel);
	if (data == "")
		return vec;

	std::vector<std::string> tokens;
	vhcl::Tokenize(data, tokens, " ");
	if (tokens.size() < 3)
		return vec;

	double x = atof(tokens[0].c_str());
	double y = atof(tokens[1].c_str());
	double z = atof(tokens[2].c_str());

	vec.set((float) x, (float) y, (float) z);

	return vec;
}

double SBRealtimeManager::getDataDouble(const std::string& channel)
{
	double val = 0.0;
	std::string data = getData(channel);
	if (data == "")
		return val;

	val = atof(data.c_str());

	return val;
}

int SBRealtimeManager::getDataInt(const std::string& channel)
{
	int val = 0;
	std::string data = getData(channel);
	if (data == "")
		return val;

	val = atoi(data.c_str());

	return val;
}

void SBRealtimeManager::notify(SBSubject* subject)
{
	SBService::notify(subject);

	SBAttribute* attribute = dynamic_cast<SBAttribute*>(subject);
	if (attribute)
	{
		if (attribute->getName() == "dumpData")
		{
			for (std::map<std::string, std::string>::iterator iter =  channelTable.begin();
				 iter != channelTable.end();
				 iter++)
			{
				LOG("[%s] = %s", (*iter).first.c_str(), (*iter).second.c_str());
			}
		}
	}

}

}

