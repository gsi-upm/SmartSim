#ifndef _BMLSTATEOBJECT_
#define _BMLSTATEOBJECT_

#include "BMLObject.h"

class BMLStateObject : public BMLObject
{
	public:
		SBAPI BMLStateObject();
		SBAPI ~BMLStateObject();

		SBAPI virtual BMLObject* copy();
		SBAPI virtual void notify(SBSubject* subject);

};
#endif