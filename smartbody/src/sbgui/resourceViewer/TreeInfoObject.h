#pragma once
#include <sb/SBObject.h>
#include <sb/SBSubject.h>

class TreeInfoObject : public SmartBody::SBObject
{
public:
	TreeInfoObject(void);
	~TreeInfoObject(void);

	virtual void notify(SmartBody::SBSubject* subject);
};
