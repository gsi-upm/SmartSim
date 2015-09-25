#ifndef _BMLBODYOBJECT_
#define _BMLBODYOBJECT_

#include "BMLObject.h"

class BMLBodyObject : public BMLObject
{
	public:
		BMLBodyObject();
		~BMLBodyObject();

		virtual void notify(SmartBody::SBSubject* subject);

};
#endif