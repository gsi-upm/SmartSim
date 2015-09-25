#include "BMLSpeechObject.h"
#include <sstream>
#include <sb/SBAttribute.h>
#include <vhcl.h>

BMLSpeechObject::BMLSpeechObject() : BMLObject()
{
	setName("speech");

	typeAttr = createStringAttribute("type", "text/plain", "", "Basic", 10, false, false, false, "Format of speech content.");
#ifndef __APPLE__
	std::vector<std::string> types;
//	types.push_back("text/plain");
	//types.push_back("application/ssml+xml");
//	typeAttr->setValidValues(types);
#endif

	refAttr = createStringAttribute("ref", "", "", "Basic", 100, false, false, false, "Reference .xml file that contains speech information. Either set this attribute, or enter the content.");
	contentAttr = createStringAttribute("content", "", "", "Basic", 110, false, false, false, "Content of speech, such as 'hello, my name is John'. When using ssml, tags (such as <emphasis>) can be used.");

	policyAttr = createStringAttribute("policy", "", "", "Basic", 120, false, false, false, "Utterance policy that overrides character's default behavior of attribute utterancePolicy: queue (play utterance after others), ignore (don't play this utterance if others are being played), interrupt (interrupt other utterance, play this one)");
	std::vector<std::string> policyTypes;
	policyTypes.push_back("");
	policyTypes.push_back("ignore");
	policyTypes.push_back("queue");
	policyTypes.push_back("interrupt");
	policyAttr->setValidValues(policyTypes);


}

BMLSpeechObject::~BMLSpeechObject()
{
}

std::string BMLSpeechObject::getBML()
{
	std::stringstream strstr;

	strstr << "<" << getName();
	strstr << " type=\"" << typeAttr->getValue() << "\"";
	if (refAttr->getValue() != "")
	{
		strstr << " ref=\"" << refAttr->getValue() << "\"";
	}
	if (policyAttr->getValue() != "")
	{
		strstr << " policy=\"" << policyAttr->getValue() << "\"";
	}
	strstr << ">";
	if (contentAttr->getValue() != "")
	{
		std::string content = contentAttr->getValue();
		std::string content2 = vhcl::Replace(content, "'", "\\'");
		strstr << content2;
	}
	strstr << "</" << getName() << ">";

	return strstr.str();
	
}

void BMLSpeechObject::notify(SBSubject* subject)
{
	BMLObject::notify(subject);
}
