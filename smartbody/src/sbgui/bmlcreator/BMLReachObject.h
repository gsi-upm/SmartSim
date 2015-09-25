#ifndef _BMLREACHOBJECT_
#define _BMLREACHOBJECT_

#include "BMLObject.h"

class BMLReachObject : public BMLObject
{
	public:
		BMLReachObject();
		~BMLReachObject();

		virtual void notify(SBSubject* subject);

};
#endif