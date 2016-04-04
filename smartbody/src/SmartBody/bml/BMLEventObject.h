#ifndef _BMLEVENTOBJECT_
#define _BMLEVENTOBJECT_

#include "BMLObject.h"

class BMLEventObject : public BMLObject
{
	public:
		SBAPI BMLEventObject();
		SBAPI ~BMLEventObject();

		SBAPI virtual BMLObject* copy();
		SBAPI virtual void notify(SBSubject* subject);
};
#endif