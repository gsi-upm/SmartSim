#ifndef _BMLCONSTRAINTOBJECT_
#define _BMLCONSTRAINTOBJECT_

#include "BMLObject.h"

class BMLConstraintObject : public BMLObject
{
	public:
		SBAPI BMLConstraintObject();
		SBAPI ~BMLConstraintObject();

		SBAPI virtual BMLObject* copy();
		SBAPI virtual void notify(SBSubject* subject);
};
#endif