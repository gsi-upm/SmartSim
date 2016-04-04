#ifndef _BMLNOISEOBJECT_
#define _BMLNOISEOBJECT_

#include "BMLObject.h"

class BMLNoiseObject : public BMLObject
{
public:
	SBAPI BMLNoiseObject();
	SBAPI ~BMLNoiseObject();

	SBAPI virtual BMLObject* copy();
	SBAPI virtual void notify(SBSubject* subject);

};
#endif