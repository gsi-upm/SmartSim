#ifndef _BMLCONSTRAINTOBJECT_
#define _BMLCONSTRAINTOBJECT_

#include "BMLObject.h"

class BMLConstraintObject : public BMLObject
{
	public:
		BMLConstraintObject();
		~BMLConstraintObject();

		virtual void notify(SBSubject* subject);
};
#endif