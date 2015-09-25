#include "BMLNoiseObject.h"
#include <sb/SBAttribute.h>

BMLNoiseObject::BMLNoiseObject() : BMLObject()
{
	setName("sbm:noise");
	
	createStringAttribute("sbm:joint-range", "", true, "Basic", 0, false, false, false, "joint range need to be added noise");
	createDoubleAttribute("sbm:scale", 0.02f, true, "Basic", 10, false, false, false, "scale of the perlin noise");
	createDoubleAttribute("sbm:frequency", 0.03f, true, "Basic", 20, false, false, false, "frequency of the perlin noise");

	SmartBody::StringAttribute* enableAttr = createStringAttribute("enable", "", true, "Basic", 30, false, false, false, "Wrap mode for current blend. Loop or Once.Default is Loop");
	std::vector<std::string> flags;
	flags.push_back("true");
	flags.push_back("false");
	enableAttr->setValidValues(flags);

	/*
	createStringAttribute("start", "", true, "Basic", 100, false, false, false, "When the blend will be scheduled");
	createStringAttribute("ready", "", true, "Basic", 110, false, false, false, "");
	createStringAttribute("stroke", "", true, "Basic", 120, false, false, false, "");
	createStringAttribute("relax", "", true, "Basic", 130, false, false, false, "");
	createStringAttribute("end", "", true, "Basic", 140, false, false, false, "");
	*/
}

BMLNoiseObject::~BMLNoiseObject()
{
}

void BMLNoiseObject::notify(SBSubject* subject)
{
	BMLObject::notify(subject);

	notifyObservers();
}
