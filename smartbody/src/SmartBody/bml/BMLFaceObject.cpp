#include "BMLFaceObject.h"
#include <sb/SBAttribute.h>

BMLFaceObject::BMLFaceObject() : BMLObject()
{
	setName("face");

	SmartBody::StringAttribute* typeAttr = createStringAttribute("type", "", "", "Basic", 10, false, false, false);
	std::vector<std::string> types;
	types.push_back("facs");
	typeAttr->setValidValues(types);

	createStringAttribute("au", "", true, "Basic", 50, false, false, false, "Action unit number");
	SmartBody::StringAttribute* sideAttr = createStringAttribute("side", "", true, "Basic", 50, false, false, false, "Action unit number");
	std::vector<std::string> sides;
	sides.push_back("");
	sides.push_back("BOTH");
	sides.push_back("LEFT");
	sides.push_back("RIGHT");
	sideAttr->setValidValues(sides);
	
	createStringAttribute("au", "", true, "Basic", 60, false, false, false, "Action unit number");
	SmartBody::DoubleAttribute* amountAttr = createDoubleAttribute("amount", .5, true, "Basic", 70, false, false, false, "Action unit number");
	amountAttr->setMin(0);

//	not supporting duration now
//	DoubleAttribute* durationAttr = createDoubleAttribute("duration", 1.0, true, "Basic", 80, false, false, false, "Action unit number");
//	durationAttr->setMin(0);

	createStringAttribute("start", "", true, "Basic", 100, false, false, false, "When the face motion will start playing");
	createStringAttribute("ready", "", true, "Basic", 110, false, false, false, "When the face motion will be fully blended");
	createStringAttribute("stroke", "", true, "Basic", 120, false, false, false, "When the face motion reaches its stroke phase.");
	createStringAttribute("relax", "", true, "Basic", 130, false, false, false, "When the face motion reaches its relax phase.");
	createStringAttribute("end", "", true, "Basic", 140, false, false, false, "When the face motion finishes.");

	createStringAttribute("sbm:rampup", "", true, "Basic", 150, false, false, false, "How long does it take for face motion to be fully blended. By default is 0.25");
	createStringAttribute("sbm:rampdown", "", true, "Basic", 160, false, false, false, "How long does it take for face motion to be fully finished. By default is 0.25");
}

BMLFaceObject::~BMLFaceObject()
{
}

void BMLFaceObject::notify(SmartBody::SBSubject* subject)
{
	BMLObject::notify(subject);
}

BMLObject* BMLFaceObject::copy()
{
	return new BMLFaceObject();
}