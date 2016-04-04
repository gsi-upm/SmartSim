#include "BMLEventObject.h"
#include <sb/SBAttribute.h>

BMLEventObject::BMLEventObject() : BMLObject()
{
	setName("sbm:event");
	createStringAttribute("message", "", true, "Basic", 50, false, false, false, "Command to be sent when event is triggered.");

	createStringAttribute("start", "", true, "Basic", 100, false, false, false, "When this event will be triggered. Message will be interpreted as a Python command.");
	createStringAttribute("stroke", "", true, "Basic", 100, false, false, false, "When this event will be triggered. Message will be interpreted as an old-style sbm command.");
}

BMLEventObject::~BMLEventObject()
{
}

void BMLEventObject::notify(SBSubject* subject)
{
	BMLObject::notify(subject);
}

BMLObject* BMLEventObject::copy()
{
	return new BMLEventObject();
}