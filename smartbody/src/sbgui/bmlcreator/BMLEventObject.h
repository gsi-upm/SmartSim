#ifndef _BMLEVENTOBJECT_
#define _BMLEVENTOBJECT_

#include "BMLObject.h"

class BMLEventObject : public BMLObject
{
	public:
		BMLEventObject();
		~BMLEventObject();

		virtual void notify(SBSubject* subject);
};
#endif