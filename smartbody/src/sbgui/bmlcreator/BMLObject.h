#ifndef _BMLOBJECT_H
#define _BMLOBJECT_H

#include "sb/SBObject.h"
#include <string>

class BMLObject : public SmartBody::SBObject
{
	public:
		BMLObject();
		~BMLObject();

		virtual void notify(SBSubject* subject);

		virtual void constructBML();
		virtual std::string getBML();

	protected:
		std::string _bml;

};

#endif