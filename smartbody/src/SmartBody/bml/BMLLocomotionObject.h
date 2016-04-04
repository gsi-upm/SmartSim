#ifndef _BMLLOCOMOTIONOBJECT_
#define _BMLLOCOMOTIONOBJECT_

#include "BMLObject.h"

class BMLLocomotionObject : public BMLObject
{
	public:
		SBAPI BMLLocomotionObject();
		SBAPI ~BMLLocomotionObject();

		SBAPI virtual BMLObject* copy();
		SBAPI virtual void notify(SBSubject* subject);

};
#endif