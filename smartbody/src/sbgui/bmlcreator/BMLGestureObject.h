#ifndef _BMLGESTUREOBJECT_
#define _BMLGESTUREOBJECT_

#include "BMLObject.h"

class BMLGestureObject : public BMLObject
{
	public:
		BMLGestureObject();
		~BMLGestureObject();

		virtual void notify(SBSubject* subject);

};
#endif