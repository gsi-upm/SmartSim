#ifndef _BMLHANDOBJECT_
#define _BMLHANDOBJECT_

#include "BMLObject.h"

class BMLHandObject : public BMLObject
{
	public:
		SBAPI BMLHandObject();
		SBAPI ~BMLHandObject();

		SBAPI virtual BMLObject* copy();
		SBAPI virtual void notify(SBSubject* subject);

};
#endif