#ifndef _BMLHANDOBJECT_
#define _BMLHANDOBJECT_

#include "BMLObject.h"

class BMLHandObject : public BMLObject
{
	public:
		BMLHandObject();
		~BMLHandObject();

		virtual void notify(SBSubject* subject);

};
#endif