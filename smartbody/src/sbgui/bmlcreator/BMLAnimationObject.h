#ifndef _BMLANIMATIONOBJECT_
#define _BMLANIMATIONOBJECT_

#include "BMLObject.h"

class BMLAnimationObject : public BMLObject
{
	public:
		BMLAnimationObject();
		~BMLAnimationObject();

		virtual void notify(SmartBody::SBSubject* subject);

};
#endif