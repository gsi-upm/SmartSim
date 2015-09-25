#include "BMLGestureObject.h"
#include <sb/SBAttribute.h>

BMLGestureObject::BMLGestureObject() : BMLObject()
{
	setName("gesture");

	// TODO: align the bml specification
/*
	std::vector<std::string> gestures;
	gestures.push_back("POINT");
	gestures.push_back("REACH");
	gestures.push_back("BEAT");
	gestures.push_back("DEPICT");
	gestures.push_back("SIGNAL");
*/
	createStringAttribute("name", "", "", "Basic", 40, false, false, false, "Name of the gesture animation, basically playing one animation.");

	SmartBody::StringAttribute* lexemeAttr = createStringAttribute("lexeme", "", "", "Basic", 50, false, false, false, "Lexeme of gesture");
//	typeAttr->setValidValues(gestures);
	createStringAttribute("type", "", "", "Basic", 60, false, false, false, "Type of lexeme.");

	std::vector<std::string> modes;
	modes.push_back("LEFT_HAND");
	modes.push_back("RIGHT_HAND");
	modes.push_back("BOTH_HANDS");
	SmartBody::StringAttribute* modeAttr = createStringAttribute("mode", "", "", "Basic", 70, false, false, false, "Which hand is involved. Should be one of the following: left, right, both.");
	modeAttr->setValidValues(modes);

	createStringAttribute("sbm:style", "", "", "Basic", 80, false, false, false, "Style of the gestures. Used to differentiate gestures with same type, posture and mode.");
	createBoolAttribute("sbm:additive", false, true, "Basic", 90, false, false, false, "Whether this gesture is additive or not.");

	//createStringAttribute("target", "", "", "Basic", 90, false, false, false, "Target for POINT and REACH gestures.");
	createStringAttribute("start", "", "", "Basic", 100, false, false, false, "Time when gesture starts.");
	createStringAttribute("ready", "", "", "Basic", 110, false, false, false, "Time when gesture is fully blended in.");
	createStringAttribute("prestroke_hold", "", true, "Basic", 115, false, false, false, "Pre stroke hold.");
	createStringAttribute("stroke_start", "", true, "Basic", 120, false, false, false, "Time when the gesture reaches its stroke start phase.");
	createStringAttribute("stroke", "", "", "Basic", 130, false, false, false, "Time of gesture's stroke.");
	createStringAttribute("stroke_end", "", true, "Basic", 140, false, false, false, "Time when the gesture reaches its stroke end phase.");
	createStringAttribute("poststroke_hold", "", true, "Basic", 155, false, false, false, "Post stroke hold.");
	createStringAttribute("relax", "", "", "Basic", 160, false, false, false, "Time when gesture begins to blend out.");
	createStringAttribute("end", "", "", "Basic", 170, false, false, false, "Time when gesture is fully blended out.");

	createStringAttribute("sbm:joint-range", "", true, "Basic", 175, false, false, false, "Perlin noise added to these joints divided by space.");
	createDoubleAttribute("sbm:frequency", 0.02, true, "Basic", 180, false, false, false, "Perlin noise frequency.");
	createDoubleAttribute("sbm:scale", 0.03, true, "Basic", 190, false, false, false, "Perlin noise amplitude.");
}

BMLGestureObject::~BMLGestureObject()
{
}

void BMLGestureObject::notify(SBSubject* subject)
{
	BMLObject::notify(subject);
}

