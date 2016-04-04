#ifndef _BMLHEADOBJECT_
#define _BMLHEADOBJECT_

#include "BMLObject.h"

class BMLHeadObject : public BMLObject
{
	public:
		SBAPI BMLHeadObject();
		SBAPI ~BMLHeadObject();

		SBAPI void start();
		SBAPI void stop();

		SBAPI virtual BMLObject* copy();
		SBAPI virtual void notify(SBSubject* subject);

};
#endif