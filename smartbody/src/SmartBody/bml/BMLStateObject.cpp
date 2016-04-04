#include "BMLStateObject.h"
#include <sb/SBAttribute.h>

BMLStateObject::BMLStateObject() : BMLObject()
{
	setName("blend");
	SmartBody::StringAttribute* stateAttr = createStringAttribute("name", "", "", "Basic", 10, false, false, false, "Blend Name.");
	SmartBody::StringAttribute* modeAttr = createStringAttribute("mode", "", "", "Basic", 20, false, false, false, "Mode. Either scheduling new blend or updating current blend, default setting is schedule");
	std::vector<std::string> modes;
	modes.push_back("schedule");
	modes.push_back("update");
	modeAttr->setValidValues(modes);	

	SmartBody::StringAttribute* loopAttr = createStringAttribute("sbm:wrap-mode", "", "", "Basic", 30, false, false, false, "Wrap mode for current blend. Loop or Once.Default is Loop");
	std::vector<std::string> loops;
	loops.push_back("Loop");
	loops.push_back("Once");
	loops.push_back("Clamp");
	loopAttr->setValidValues(loops);

	SmartBody::StringAttribute* startNowAttr = createStringAttribute("sbm:schedule-mode", "", "", "Basic", 40, false, false, false, "Schedule mode for the blend. Now or Queued.Default is Queued");
	std::vector<std::string> startNow;
	startNow.push_back("Now");
	startNow.push_back("Queued");
	startNowAttr->setValidValues(startNow);

	SmartBody::StringAttribute* additiveAttr = createStringAttribute("sbm:blend-mode", "", "", "Basic", 50, false, false, false, "Blend mode for current blend. Overwrite or Additive.Default is Overwrite");
	std::vector<std::string> additive;
	additive.push_back("Overwrite");
	additive.push_back("Additive");
	additiveAttr->setValidValues(additive);

	createStringAttribute("sbm:partial-joint", "", "", "Basic", 60, false, false, false, "Starting joint name inside skeleton hierarchy for additive blending.");

	createStringAttribute("x", "", "", "Basic", 70, false, false, false, "Parameter X, would be used in 1D|2D|3D space.");
	createStringAttribute("y", "", "", "Basic", 80, false, false, false, "Parameter Y, would be used in 1D|2D space.");
	createStringAttribute("z", "", "", "Basic", 90, false, false, false, "Parameter Z, would be used in 3D space.");

	createStringAttribute("start", "", true, "Basic", 100, false, false, false, "When the blend will be scheduled");
	createStringAttribute("ready", "", true, "Basic", 110, false, false, false, "");
	createStringAttribute("stroke", "", true, "Basic", 120, false, false, false, "");
	createStringAttribute("relax", "", true, "Basic", 130, false, false, false, "");
	createStringAttribute("end", "", true, "Basic", 140, false, false, false, "");
}

BMLStateObject::~BMLStateObject()
{
}

void BMLStateObject::notify(SBSubject* subject)
{
	BMLObject::notify(subject);

	notifyObservers();
}

BMLObject* BMLStateObject::copy()
{
	return new BMLStateObject();
}