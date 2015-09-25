#include "BMLBodyObject.h"
#include <sb/SBAttribute.h>

BMLBodyObject::BMLBodyObject() : BMLObject()
{
	setName("body");

	SmartBody::SBAttribute* postureAttr = createStringAttribute("posture", "", true, "Basic", 20, false, false, false, "Motion name of the idle posture");

	SmartBody::SBAttribute* startAttr = createStringAttribute("start", "", true, "Basic", 100, false, false, false, "When the new posture will be activated and begin blending with the existing posture");
	SmartBody::SBAttribute* readyAttr = createStringAttribute("ready", "", true, "Basic", 110, false, false, false, "When the new posture will be fully blended in and the old posture will be gone ");
}

BMLBodyObject::~BMLBodyObject()
{
}

void BMLBodyObject::notify(SmartBody::SBSubject* subject)
{
	BMLObject::notify(subject);

	notifyObservers();
}
