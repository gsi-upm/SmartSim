#ifndef _BMLFACEOBJECT_
#define _BMLFACEOBJECT_

#include "BMLObject.h"

class BMLFaceObject : public BMLObject
{
	public:
		SBAPI BMLFaceObject();
		SBAPI ~BMLFaceObject();

		SBAPI virtual BMLObject* copy();
		SBAPI virtual void notify(SBSubject* subject);

};
#endif