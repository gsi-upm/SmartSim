#include "BMLObject.h"
#include <sstream>
#include <sb/SBAttribute.h>

BMLObject::BMLObject()
{
	createStringAttribute("id", "", true, "Basic", 1000, false, false, false, 
		"Id of this BML request");
}

BMLObject::~BMLObject()
{
}

void BMLObject::constructBML()
{
	std::stringstream strstr;

	strstr << "<" << getName();

	int numUsedElements = 0;
	std::map<std::string, SmartBody::SBAttribute*>& attributes = getAttributeList();
	for (std::map<std::string, SmartBody::SBAttribute*>::iterator iter = attributes.begin();
		 iter != attributes.end();
		 iter++)
	{
		SmartBody::SBAttribute* attribute = (*iter).second;

		SmartBody::BoolAttribute* boolAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
		if (boolAttribute)
		{
			if (boolAttribute->getValue() != boolAttribute->getDefaultValue())
			{
				strstr << " " << boolAttribute->getName() << "=\"" << (boolAttribute->getValue()? "true" : "false") << "\"";
				numUsedElements++;
			}
		}
		
		SmartBody::IntAttribute* intAttribute = dynamic_cast<SmartBody::IntAttribute*>(attribute);
		if (intAttribute)
		{
			if (intAttribute->getValue() != intAttribute->getDefaultValue())
			{
				strstr << " " << intAttribute->getName() << "=\"" << intAttribute->getValue() << "\"";
				numUsedElements++;
			}
		}

		SmartBody::DoubleAttribute* doubleAttribute = dynamic_cast<SmartBody::DoubleAttribute*>(attribute);
		if (doubleAttribute)
		{
			if (doubleAttribute->getValue() != doubleAttribute->getDefaultValue())
			{
				strstr << " " << doubleAttribute->getName() << "=\"" << doubleAttribute->getValue()<< "\"";
				numUsedElements++;
			}
		}

		SmartBody::Vec3Attribute* vecAttribute = dynamic_cast<SmartBody::Vec3Attribute*>(attribute);
		if (vecAttribute)
		{
			if (vecAttribute->getValue() != vecAttribute->getDefaultValue())
			{
				strstr << " " << vecAttribute->getName() << "=\"" << vecAttribute->getValue()[0] << " " << vecAttribute->getValue()[1] << " " << vecAttribute->getValue()[2] << "\"";
				numUsedElements++;
			}
		}

		SmartBody::StringAttribute* stringAttribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
		if (stringAttribute)
		{
			if (stringAttribute->getValue() != stringAttribute->getDefaultValue())
			{
				strstr << " " << stringAttribute->getName() << "=\"" << stringAttribute->getValue()<< "\"";
				numUsedElements++;
			}
		}
	}

	strstr << "/>";
	if (numUsedElements > 0)
		_bml = strstr.str();
	else
		_bml = "";
}

std::string BMLObject::getBML()
{
	return _bml;
}

void BMLObject::notify(SBSubject* subject)
{
	SmartBody::SBAttribute* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
	if (attribute)
	{
		constructBML();
		notifyObservers();
	}
}
