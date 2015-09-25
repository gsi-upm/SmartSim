#ifndef _BMLSTATEOBJECT_
#define _BMLSTATEOBJECT_

#include "BMLObject.h"

class BMLStateObject : public BMLObject
{
	public:
		BMLStateObject();
		~BMLStateObject();

		virtual void notify(SBSubject* subject);

};
#endif