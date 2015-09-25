#ifndef _BMLNOISEOBJECT_
#define _BMLNOISEOBJECT_

#include "BMLObject.h"

class BMLNoiseObject : public BMLObject
{
public:
	BMLNoiseObject();
	~BMLNoiseObject();

	virtual void notify(SBSubject* subject);

};
#endif