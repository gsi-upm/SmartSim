#include "TreeInfoObject.h"
#include <sb/SBAttribute.h>

TreeInfoObject::TreeInfoObject(void)
{
}

TreeInfoObject::~TreeInfoObject(void)
{
}

void TreeInfoObject::notify( SBSubject* subject )
{
	SmartBody::SBAttribute* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
	if (attribute)
	{
		//constructBML();
		notifyObservers();
	}
}