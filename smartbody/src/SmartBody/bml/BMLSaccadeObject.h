#ifndef _BMLSACCADEOBJECT_
#define _BMLSACCADEOBJECT_

#include "BMLObject.h"

class BMLSaccadeObject : public BMLObject
{
	public:
		SBAPI BMLSaccadeObject();
		SBAPI ~BMLSaccadeObject();

		SBAPI virtual BMLObject* copy();
		SBAPI virtual void notify(SBSubject* subject);
};

#endif