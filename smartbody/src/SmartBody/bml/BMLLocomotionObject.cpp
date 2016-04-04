#include "BMLLocomotionObject.h"
#include <sb/SBAttribute.h>

BMLLocomotionObject::BMLLocomotionObject() : BMLObject()
{
	setName("locomotion");

	SmartBody::StringAttribute* typeAttr = createStringAttribute("type", "", "", "Basic", 20, false, false, false, "What type of locomotion are you using. Either example or procedural, default setting is basic");
	createStringAttribute("target", "", "", "Basic", 10, false, false, false, "Where you want the character to move. Format is (X Z)/pawn name/direction. Direction is only under sbm:step manner.");
	std::vector<std::string> types;
	types.push_back("basic");
	types.push_back("example");
	types.push_back("procedural");
	typeAttr->setValidValues(types);
	SmartBody::StringAttribute* mannerAttr = createStringAttribute("manner", "", "", "Basic", 30, false, false, false, "The manner character takes when moving. e.g. walk, jog, run, step. This attribute does not apply to procedural locomotion. Default setting is walk.");
	std::vector<std::string> manners;
	manners.push_back("walk");
	manners.push_back("jog");
	manners.push_back("run");
	manners.push_back("sbm:step");
	manners.push_back("sbm:jump");
	mannerAttr->setValidValues(manners);
	createStringAttribute("facing", "", "", "Basic", 40, false, false, false, "The global facing angle of the agent. To cancel a facing, enter a value that is larger than 180 or smaller than -180");
	createDoubleAttribute("speed",-1.0,true,"Basic", 45, false, false, false, "Forward locomotion speed in m/s. Speeds less than zero will be ignored.");
	createDoubleAttribute("sbm:braking",1.2,true,"Basic", 45, false, false, false, "The braking factor agent. If it's larger, the agent would tend to slow down eariler when it's reaching the target. Default is 1.2");
	createStringAttribute("sbm:follow", "", "", "Basic", 50, false, false, false, "The other agent that this agent would follow. To cancel the following, enter an empty agent or an invalid agent");
	createStringAttribute("proximity", "", "", "Basic", 60, false, false, false, "How close would this agent approach the target position, default setting is 1.5");
	createStringAttribute("sbm:accel", "", "", "Basic", 70, false, false, false, "The moving speed acceleration, default setting is 2");
	createStringAttribute("sbm:scootaccel", "", "", "Basic", 80, false, false, false, "The scooting acceleration, default setting is 200");
	createStringAttribute("sbm:angleaccel", "", "", "Basic", 90, false, false, false, "The angle speed acceleration, default setting is 450");
	createStringAttribute("sbm:numsteps", "", "", "Basic", 100, false, false, false, "Number of steps, only available if manner is sbm:step, default setting is 1");
	createStringAttribute("start", "", "", "Basic", 200, false, false, false, "Time when locomotion starts.");
	}

BMLLocomotionObject::~BMLLocomotionObject()
{
}

void BMLLocomotionObject::notify(SBSubject* subject)
{
	BMLObject::notify(subject);
}

BMLObject* BMLLocomotionObject::copy()
{
	return new BMLLocomotionObject();
}
