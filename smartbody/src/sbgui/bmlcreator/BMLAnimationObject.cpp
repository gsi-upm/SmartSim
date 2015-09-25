#include "BMLAnimationObject.h"
#include <sb/SBAttribute.h>

BMLAnimationObject::BMLAnimationObject() : BMLObject()
{
	setName("animation");

	createStringAttribute("name", "", true, "Basic", 50, false, false, false, "Name of the motion to be played");
	SmartBody::DoubleAttribute* speedAttr = createDoubleAttribute("speed", 1.0, true, "Basic", 50, false, false, false, "Speedup of the motion ('2' will play the motion twice as fast)");
	speedAttr->setMin(0);


	createStringAttribute("start", "", true, "Basic", 100, false, false, false, "When the motion will start playing");
	createStringAttribute("ready", "", true, "Basic", 110, false, false, false, "When the motion will be fully blended");
	createStringAttribute("stroke_start", "", true, "Basic", 120, false, false, false, "When the motion reaches its stroke start phase.");
	createStringAttribute("stroke", "", true, "Basic", 130, false, false, false, "When the motion reaches its stroke phase.");
	createStringAttribute("stroke_end", "", true, "Basic", 140, false, false, false, "When the motion reaches its stroke start phase.");
	createStringAttribute("relax", "", true, "Basic", 150, false, false, false, "When the motion reaches its relax phase.");
	createStringAttribute("end", "", true, "Basic", 160, false, false, false, "When the motion finishes.");
}

BMLAnimationObject::~BMLAnimationObject()
{
}

void BMLAnimationObject::notify(SmartBody::SBSubject* subject)
{
	BMLObject::notify(subject);
}
