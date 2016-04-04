#ifndef _BMLREACHOBJECT_
#define _BMLREACHOBJECT_

#include "BMLObject.h"

class BMLReachObject : public BMLObject
{
	public:
		SBAPI BMLReachObject();
		SBAPI ~BMLReachObject();

		SBAPI virtual BMLObject* copy();
		SBAPI virtual void notify(SBSubject* subject);

};
#endif