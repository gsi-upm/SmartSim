#ifndef _BMLSPEECHOBJECT_
#define _BMLSPEECHOBJECT_

#include "BMLObject.h"

class BMLSpeechObject : public BMLObject
{
	public:
		BMLSpeechObject();
		~BMLSpeechObject();

		virtual std::string getBML();

		virtual void notify(SBSubject* subject);

	protected:
		SmartBody::StringAttribute* typeAttr;
		SmartBody::StringAttribute* refAttr;
		SmartBody::StringAttribute* contentAttr;
		SmartBody::StringAttribute* policyAttr;
};
#endif