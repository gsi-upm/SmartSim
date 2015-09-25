#ifndef _BMLFACEOBJECT_
#define _BMLFACEOBJECT_

#include "BMLObject.h"

class BMLFaceObject : public BMLObject
{
	public:
		BMLFaceObject();
		~BMLFaceObject();

		virtual void notify(SBSubject* subject);

};
#endif