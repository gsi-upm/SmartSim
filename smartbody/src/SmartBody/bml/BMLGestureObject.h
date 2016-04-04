#ifndef _BMLGESTUREOBJECT_
#define _BMLGESTUREOBJECT_

#include "BMLObject.h"

class BMLGestureObject : public BMLObject
{
	public:
		SBAPI BMLGestureObject();
		SBAPI ~BMLGestureObject();

		SBAPI virtual BMLObject* copy();
		SBAPI virtual void notify(SBSubject* subject);

};
#endif