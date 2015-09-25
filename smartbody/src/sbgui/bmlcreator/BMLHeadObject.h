#ifndef _BMLHEADOBJECT_
#define _BMLHEADOBJECT_

#include "BMLObject.h"

class BMLHeadObject : public BMLObject
{
	public:
		BMLHeadObject();
		~BMLHeadObject();

		virtual void notify(SBSubject* subject);

};
#endif