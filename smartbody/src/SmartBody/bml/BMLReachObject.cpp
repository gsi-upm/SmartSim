#include "BMLReachObject.h"
#include <sb/SBAttribute.h>

BMLReachObject::BMLReachObject() : BMLObject()
{
	setName("reach");

	createStringAttribute("target", "", true, "Basic", 10, false, false, false, "reach target");
	createVec3Attribute("sbm:target-pos", 0,0,0, true, "Basic", 10, false, false, false, "reach target position");
	
	SmartBody::StringAttribute* reachActionAttr = createStringAttribute("sbm:action","",true,"Basic", 20, false, false, false,"the action to be executed when reaching.");
	std::vector<std::string> reachActions;
	reachActions.push_back("pick-up");
	reachActions.push_back("put-down");
	reachActions.push_back("touch");
	reachActions.push_back("point-at");
	reachActionAttr->setValidValues(reachActions);

	SmartBody::StringAttribute* reachTypeAttr = createStringAttribute("sbm:reach-type","",true,"Basic", 20, false, false, false,"use left hand or right hand.");
	std::vector<std::string> reachTypess;
	reachTypess.push_back("left");
	reachTypess.push_back("right");
	reachTypess.push_back("none");	
	reachTypeAttr->setValidValues(reachTypess);

	createStringAttribute("sbm:handle", "", true, "Basic", 30, false, false, false, "Handle of this reach instance, can be reused during later reach commands.");
	createBoolAttribute("sbm:foot-ik",true,true,"Basic", 40, false, false, false,"whether to apply ik and fixed the foot sliding.");
	createBoolAttribute("sbm:use-locomotion",false,true,"Basic", 40, false, false, false,"whether to use locomotion for target far away.");

	createBoolAttribute("sbm:reach-finish",false,true,"Basic", 50, false, false, false,"whether to finish the reaching action and return to the rest pose.");

	createDoubleAttribute("sbm:reach-velocity",-1.0,true,"Basic", 60, false, false, false,"the end effector movement velocity when interpolating two poses.");
	createDoubleAttribute("sbm:reach-duration",-1.0,true,"Basic", 70, false, false, false,"the duration for hand to hold still on the target before automatically return to the rest pose.If it is '-1.0', then the duration is infinite");



	// no support for lef/right arm for now
// 	StringAttribute* reachArmAttr = createStringAttribute("reach-arm", "", "", "Basic", 10, false, false, false, "which arm will be used for reaching 'left' or 'right'");
// 	std::vector<std::string> reachArms;
// 	reachArms.push_back("left");
// 	reachArms.push_back("right");
// 	reachArmAttr->setValidValues(reachArms);

	createStringAttribute("start", "", "", "Basic", 100, false, false, false, "Time when reach starts.");
}

BMLReachObject::~BMLReachObject()
{
}

void BMLReachObject::notify(SBSubject* subject)
{
	BMLObject::notify(subject);
}

BMLObject* BMLReachObject::copy()
{
	return new BMLReachObject();
}
