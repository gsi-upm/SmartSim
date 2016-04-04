#include "BMLSaccadeObject.h"
#include <sb/SBAttribute.h>

BMLSaccadeObject::BMLSaccadeObject() : BMLObject()
{
	setName("saccade");

	SmartBody::StringAttribute* finishAttr = createStringAttribute("finish", "", true, "Basic", 10, false, false, false, "Turn on or off saccade");
	std::vector<std::string> toggles;
	toggles.push_back("true");
	toggles.push_back("false");
	finishAttr->setValidValues(toggles);

	SmartBody::StringAttribute* modeAttr = createStringAttribute("mode", "", true, "Basic", 20, false, false, false, "Which mode is saccade in. Default is lisen mode");
	std::vector<std::string> modes;
	modes.push_back("talk");
	modes.push_back("listen");
	modes.push_back("think");
	modeAttr->setValidValues(modes);

	/*
	// deprecated by attributes
	createStringAttribute("angle-limit", "", true, "Basic", 30, false, false, false, "angle limit, default is 12 deg for talking mode, 10 deg for listening mode");
	createStringAttribute("sbm:bin0", "", true, "Basic", 40, false, false, false, "percentage for 0 deg, would only take effect if all 8 bins are assigned value");
	createStringAttribute("sbm:bin45", "", true, "Basic", 50, false, false, false, "percentage for 45 deg, would only take effect if all 8 bins are assigned value");
	createStringAttribute("sbm:bin90", "", true, "Basic", 60, false, false, false, "percentage for 90 deg, would only take effect if all 8 bins are assigned value");
	createStringAttribute("sbm:bin135", "", true, "Basic", 70, false, false, false, "percentage for 135 deg, would only take effect if all 8 bins are assigned value");
	createStringAttribute("sbm:bin180", "", true, "Basic", 80, false, false, false, "percentage for 180 deg, would only take effect if all 8 bins are assigned value");
	createStringAttribute("sbm:bin225", "", true, "Basic", 90, false, false, false, "percentage for 225 deg, would only take effect if all 8 bins are assigned value");
	createStringAttribute("sbm:bin270", "", true, "Basic", 100, false, false, false, "percentage for 270 deg, would only take effect if all 8 bins are assigned value");
	createStringAttribute("sbm:bin315", "", true, "Basic", 110, false, false, false, "percentage for 315 deg, would only take effect if all 8 bins are assigned value");
	createStringAttribute("sbm:mean", "", true, "Basic", 120, false, false, false, "gaussian distribution mean, recommended value is 100. Would only take effect if both mean and variant are set");
	createStringAttribute("sbm:variant", "", true, "Basic", 130, false, false, false, "gaussian distribution variant, recommended value is 50. Would only take effect if both mean and variant are set");
	*/

	createStringAttribute("sbm:duration", "", true, "Basic", 200, false, false, false, "duration, only used for explicitly define a saccade");
	createStringAttribute("direction", "", true, "Basic", 200, false, false, false, "direction on a 2D plane, valid value is from -180deg to 180deg, only used for explicitly define a saccade");
	createStringAttribute("magnitude", "", true, "Basic", 200, false, false, false, "magnitude of saccade in degree, only used for explicitly define a saccade");
}

BMLSaccadeObject::~BMLSaccadeObject()
{
}

void BMLSaccadeObject::notify(SBSubject* subject)
{
	BMLObject::notify(subject);
}

BMLObject* BMLSaccadeObject::copy()
{
	return new BMLSaccadeObject();
}

