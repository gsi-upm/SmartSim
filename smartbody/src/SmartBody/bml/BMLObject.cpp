#include "BMLObject.h"
#include <sstream>
#include <sb/SBAttribute.h>
#include <boost/lexical_cast.hpp>

BMLObject::BMLObject()
{
	createStringAttribute("id", "", true, "Basic", 1000, false, false, false, 
		"Id of this BML request");
}

BMLObject::~BMLObject()
{
}

BMLObject* BMLObject::copy()
{
	return new BMLObject();
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

void BMLObject::parse(rapidxml::xml_node<>* node)
{
	std::string behaviorName = node->name();

	for (rapidxml::xml_attribute<>* attr = node->first_attribute();
		 attr; 
		 attr = attr->next_attribute())
	{
		std::string attrName = attr->name();
		std::string attrValue = attr->value();

		// check to see if the attribute exists on this object
		SmartBody::SBAttribute* attribute = this->getAttribute(attrName);
		if (!attribute)
		{
			LOG("No parameter named '%s' in BML '%s', ignoring...", attrName.c_str(), this->getName().c_str());
		}

		SmartBody::StringAttribute* stringAttribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
		if (stringAttribute)
		{
			const std::vector<std::string>& validValues = stringAttribute->getValidValues();
			if (validValues.size() > 0)
			{
				bool match = false;
				for (std::vector<std::string>::const_iterator validIter = validValues.begin();
					 validIter != validValues.end();
					 validIter++)
				{
					if ((*validIter) == attrValue)
					{
						match = true;
						break;
					}
				}
				if (!match)
				{
					LOG("Bad value '%s' for parameter '%s' in BML behavior '%s', setting to '%s' instead.", attrValue.c_str(), attrName.c_str(), behaviorName.c_str(), stringAttribute->getDefaultValue().c_str());
				}
				else
				{
					stringAttribute->setValue(attrValue);
				}
			}
		}
		
		SmartBody::DoubleAttribute* doubleAttribute = dynamic_cast<SmartBody::DoubleAttribute*>(attribute);
		if (doubleAttribute)
		{
			try 
			{
				double val = boost::lexical_cast<double>(attrValue);
				if (doubleAttribute->getMin() > val)
				{
					LOG("Value '%s' for parameter '%s' in BML behavior '%s' is too small. Setting to '%f' instead.", attrValue.c_str(), attrName.c_str(), behaviorName.c_str(), doubleAttribute->getMin());			
					doubleAttribute->setValue(doubleAttribute->getMin());
				}
				else if (doubleAttribute->getMax() < val)
				{
					LOG("Value '%s' for parameter '%s' in BML behavior '%s' is too large. Setting to '%f' instead.", attrValue.c_str(), attrName.c_str(), behaviorName.c_str(), doubleAttribute->getMax());			
					doubleAttribute->setValue(doubleAttribute->getMax());
				}
			}
			catch(...)
			{ 
				LOG("Value '%s' for parameter '%s' in BML behavior '%s' is not a number. Setting to '%f' instead.", attrValue.c_str(), attrName.c_str(), behaviorName.c_str(), doubleAttribute->getDefaultValue());
				doubleAttribute->setValue(doubleAttribute->getDefaultValue());
			}
		}

		SmartBody::IntAttribute* intAttribute = dynamic_cast<SmartBody::IntAttribute*>(attribute);
		if (intAttribute)
		{
			try 
			{
				int val = boost::lexical_cast<int>(attrValue);
				if (intAttribute->getMin() > val)
				{
					LOG("Value '%s' for parameter '%s' in BML behavior '%s' is too small. Setting to '%f' instead.", attrValue.c_str(), attrName.c_str(), behaviorName.c_str(), intAttribute->getMin());			
					intAttribute->setValue(intAttribute->getMin());
				}
				else if (intAttribute->getMax() < val)
				{
					LOG("Value '%s' for parameter '%s' in BML behavior '%s' is too large. Setting to '%f' instead.", attrValue.c_str(), attrName.c_str(), behaviorName.c_str(), intAttribute->getMax());			
					intAttribute->setValue(intAttribute->getMax());
				}
			}
			catch(...)
			{ 
				LOG("Value '%s' for parameter '%s' in BML behavior '%s' is not an integer. Setting to '%d' instead.", attrValue.c_str(), attrName.c_str(), behaviorName.c_str(), intAttribute->getDefaultValue());
				doubleAttribute->setValue(intAttribute->getDefaultValue());
			}
		}

		SmartBody::BoolAttribute* boolAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
		if (boolAttribute)
		{
			if (attrValue == "true" || 
				attrValue == "TRUE")
			{
				boolAttribute->setValue(true);
			}
			else if (attrValue == "false" || 
				     attrValue == "FALSE")
			{
				boolAttribute->setValue(false);
			}
			else
			{
				LOG("Value '%s' for parameter '%s' in BML behavior '%s' is not a true/false value. Setting to '%s' instead.", attrValue.c_str(), attrName.c_str(), behaviorName.c_str(), boolAttribute->getDefaultValue() ? "true" : "false");
			}
		}
	}
}

std::vector<SmartBody::SBSyncPoint*> BMLObject::getSyncPointObjects()
{
	std::vector<SmartBody::SBSyncPoint*> syncPoints;

	std::string id = this->getStringAttribute("id");
	if (id != "")
	{
			SmartBody::SBSyncPoint* spStart = new SmartBody::SBSyncPoint(id + ":start");
			SmartBody::SBSyncPoint* spEnd = new SmartBody::SBSyncPoint(id + ":end");
			syncPoints.push_back(spStart);
			syncPoints.push_back(spEnd);
	}

	return syncPoints;
}

std::vector<SmartBody::SBTrigger*> BMLObject::getTriggers()
{
	std::vector<SmartBody::SBTrigger*> triggers;

	std::string start = this->getStringAttribute("start");
	if (start != "")
	{
		SmartBody::SBTrigger* trigger = new SmartBody::SBTrigger();
		trigger->setStartTag(start);
		triggers.push_back(trigger);
	}

	std::string end = this->getStringAttribute("end");
	if (end != "")
	{
		SmartBody::SBTrigger* trigger = new SmartBody::SBTrigger();
		trigger->setEndTag(end);
		triggers.push_back(trigger);
	}

	return triggers;

}

void BMLObject::start()
{
}

void BMLObject::stop()
{
}
