#ifndef _BMLANIMATIONOBJECT_
#define _BMLANIMATIONOBJECT_

#include "BMLObject.h"

class BMLAnimationObject : public BMLObject
{
	public:
		SBAPI BMLAnimationObject();
		SBAPI ~BMLAnimationObject();

		SBAPI virtual BMLObject* copy();
		SBAPI virtual void notify(SmartBody::SBSubject* subject);

};
#endif