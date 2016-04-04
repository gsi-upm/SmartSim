#ifndef _BMLSPEECHOBJECT_
#define _BMLSPEECHOBJECT_

#include "BMLObject.h"

class BMLSpeechObject : public BMLObject
{
	public:
		SBAPI BMLSpeechObject();
		SBAPI ~BMLSpeechObject();

		SBAPI virtual std::string getBML();

		SBAPI virtual BMLObject* copy();
		SBAPI virtual void notify(SBSubject* subject);

	protected:
		SmartBody::StringAttribute* typeAttr;
		SmartBody::StringAttribute* refAttr;
		SmartBody::StringAttribute* contentAttr;
		SmartBody::StringAttribute* policyAttr;
};
#endif