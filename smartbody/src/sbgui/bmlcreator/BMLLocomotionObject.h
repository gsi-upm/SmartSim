#ifndef _BMLLOCOMOTIONOBJECT_
#define _BMLLOCOMOTIONOBJECT_

#include "BMLObject.h"

class BMLLocomotionObject : public BMLObject
{
	public:
		BMLLocomotionObject();
		~BMLLocomotionObject();

		virtual void notify(SBSubject* subject);

};
#endif