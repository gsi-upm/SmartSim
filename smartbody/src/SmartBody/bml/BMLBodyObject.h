#ifndef _BMLBODYOBJECT_
#define _BMLBODYOBJECT_

#include "BMLObject.h"

class BMLBodyObject : public BMLObject
{
	public:
		SBAPI BMLBodyObject();
		SBAPI ~BMLBodyObject();

		SBAPI virtual BMLObject* copy();
		SBAPI virtual void notify(SmartBody::SBSubject* subject);

};
#endif