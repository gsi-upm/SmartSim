#ifndef _BMLGAZEOBJECT_
#define _BMLGAZEOBJECT_

#include "BMLObject.h"

class BMLGazeObject : public BMLObject
{
	public:
		SBAPI BMLGazeObject();
		SBAPI ~BMLGazeObject();

		SBAPI virtual BMLObject* copy();
		SBAPI virtual void notify(SBSubject* subject);

};
#endif