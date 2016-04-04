#include "SBBmlProcessor.h"

#include <sb/SBSimulationManager.h>
#include <sb/SBCommandManager.h>
#include <sb/SBVHMsgManager.h>
#include <sb/SBScene.h>
#include <bml/bml_processor.hpp>
#include <sbm/rapidxml_utils.hpp>
#include <bml/BMLObject.h>
#include <bml/BMLAnimationObject.h>
#include <bml/BMLBodyObject.h>
#include <bml/BMLConstraintObject.h>
#include <bml/BMLEventObject.h>
#include <bml/BMLFaceObject.h>
#include <bml/BMLGazeObject.h>
#include <bml/BMLGestureObject.h>
#include <bml/BMLHandObject.h>
#include <bml/BMLHeadObject.h>
#include <bml/BMLLocomotionObject.h>
#include <bml/BMLNoiseObject.h>
#include <bml/BMLReachObject.h>
#include <bml/BMLSaccadeObject.h>
#include <bml/BMLSpeechObject.h>
#include <bml/BMLStateObject.h>

namespace SmartBody {

SBTrigger::SBTrigger()
{
	_offset = 0;
}

SBTrigger::~SBTrigger()
{
}

void SBTrigger::setOffset(double val)
{
	_offset = val;
}

double SBTrigger::getOffset()
{
	return _offset;
}

void SBTrigger::setStartTag(std::string tag)
{
	_startTag = tag;
}

std::string SBTrigger::getStartTag()
{
	return _startTag;
}

void SBTrigger::setEndTag(std::string tag)
{
	_endTag = tag;
}

std::string SBTrigger::getEndTag()
{
	return _endTag;
}

SBSyncPoint::SBSyncPoint(const std::string& name)
{
	_name = name;
}

SBSyncPoint::~SBSyncPoint()
{
}


SBBMLSchedule::SBBMLSchedule()
{
}

SBBMLSchedule::~SBBMLSchedule()
{
}

void SBBMLSchedule::setCharacter(std::string name)
{
	_characterName = name;
}

std::string SBBMLSchedule::getCharacter()
{
	return _characterName;
}

void SBBMLSchedule::remove()
{
	// mark any dependent blocks since they are no longer valid
}

SBBmlProcessor::SBBmlProcessor()
{
	_bmlProcessor = new BML::Processor();
	
	BMLObject* obj = NULL;
	obj = new BMLAnimationObject();
	_bmlHandlers[obj->getName()] = obj;

	obj = new BMLBodyObject();
	_bmlHandlers[obj->getName()] = obj;

	obj = new BMLConstraintObject();
	_bmlHandlers[obj->getName()] = obj;

	obj = new BMLEventObject();
	_bmlHandlers[obj->getName()] = obj;

	obj = new BMLFaceObject();
	_bmlHandlers[obj->getName()] = obj;

	obj = new BMLGazeObject();
	_bmlHandlers[obj->getName()] = obj;

	obj = new BMLGestureObject();
	_bmlHandlers[obj->getName()] = obj;

	obj = new BMLHandObject();
	_bmlHandlers[obj->getName()] = obj;

	obj = new BMLHeadObject();
	_bmlHandlers[obj->getName()] = obj;

	obj = new BMLLocomotionObject();
	_bmlHandlers[obj->getName()] = obj;

	obj = new BMLNoiseObject();
	_bmlHandlers[obj->getName()] = obj;

	obj = new BMLReachObject();
	_bmlHandlers[obj->getName()] = obj;

	obj = new BMLSaccadeObject();
	_bmlHandlers[obj->getName()] = obj;

	obj = new BMLSpeechObject();
	_bmlHandlers[obj->getName()] = obj;

	obj = new BMLStateObject();
	_bmlHandlers[obj->getName()] = obj;
	
}

SBBmlProcessor::~SBBmlProcessor()
{
	delete _bmlProcessor;
}

// This command is inside bml_processor.cpp. So unable to rewrite, instead, re-routine to bp.
void SBBmlProcessor::vrSpeak(std::string agent, std::string recip, std::string msgId, std::string msg)
{
	std::stringstream msgStr;
	msgStr << agent << " " << recip << " " << msgId << " " << msg;
	srArgBuffer vrMsg(msgStr.str().c_str());
	BML::Processor* bp = SmartBody::SBScene::getScene()->getBmlProcessor()->getBMLProcessor();
	bp->vrSpeak_func(vrMsg, SmartBody::SBScene::getScene()->getCommandManager());
}

void SBBmlProcessor::vrAgentBML(std::string op, std::string agent, std::string msgId, std::string msg)
{
	if (op == "request" || op == "start" || op == "end")
	{
		std::stringstream msgStr;
		msgStr << agent << " " << msgId << " " << op << " " << msg;
		srArgBuffer vrMsg(msgStr.str().c_str());
		BML::Processor* bp = SmartBody::SBScene::getScene()->getBmlProcessor()->getBMLProcessor();
		bp->vrAgentBML_cmd_func(vrMsg, SmartBody::SBScene::getScene()->getCommandManager());
	}
	else
	{
		LOG("vrAgentBML option %s not recognized!", op.c_str());
		return;	
	}
}

std::string SBBmlProcessor::build_vrX(std::ostringstream& buffer, const std::string& cmd, const std::string& char_id, const std::string& recip_id, const std::string& content, bool for_seq ) 
{
	

	std::stringstream msgId;
	SmartBody::IntAttribute* intAttr = dynamic_cast<SmartBody::IntAttribute*>(SmartBody::SBScene::getScene()->getAttribute("bmlIndex"));

	if (SmartBody::SBScene::getScene()->getProcessId()!= "")
		msgId << "sbm_" << SmartBody::SBScene::getScene()->getProcessId() << "_test_bml_" << (intAttr->getValue());
	else
		msgId << "sbm_test_bml_" << intAttr->getValue();
	intAttr->setValue(intAttr->getValue() + 1);

	buffer.str("");
	if( for_seq )
		buffer << "send " << cmd << " ";
	buffer << char_id << " "<< recip_id << " " << msgId.str() << std::endl << content;
	return msgId.str();
}

std::string SBBmlProcessor::send_vrX( const char* cmd, const std::string& char_id, const std::string& recip_id,
			const std::string& seq_id, bool echo, bool send, const std::string& bml ) 
{
	
	std::ostringstream msg;
	std::string msgId = "";
	bool all_characters = ( char_id=="*" );
	//LOG("send_vrX cmd =  %s, bml = %s", cmd, bml.c_str());
	if( seq_id.length()==0 ) {
		if( echo ) {
			msgId = build_vrX( msg, cmd, char_id, recip_id, bml, false );
			// don't log a vrX message
			////LOG("%s %s", cmd, msg.str().c_str());
		}

		if( send ) {
			// execute directly
			if( all_characters )
			{
				const std::vector<std::string>& characterNames = SmartBody::SBScene::getScene()->getCharacterNames();
				for (std::vector<std::string>::const_iterator iter = characterNames.begin();
					iter != characterNames.end();
					iter++)
				{
					SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(*iter);
					msgId = build_vrX( msg, cmd, character->getName().c_str(), recip_id, bml, false );	
					//LOG("vvmsg cmd =  %s, msg = %s", cmd, msg.str().c_str());
					//SmartBody::SBScene::getScene()->getVHMsgManager()->send2( cmd, msg.str().c_str() );
					SmartBody::SBScene::getScene()->getCommandManager()->execute(cmd, const_cast<char*>(msg.str().c_str()));
				}
			} else {
				msgId = build_vrX( msg, cmd, char_id, recip_id, bml, false );
				//LOG("vvmsg cmd =  %s, msg = %s", cmd, msg.str().c_str());
				//SmartBody::SBScene::getScene()->getVHMsgManager()->send2( cmd, msg.str().c_str() );
				SmartBody::SBScene::getScene()->getCommandManager()->execute(cmd, const_cast<char*>(msg.str().c_str()));
			}
		}		
		return msgId;
	}else {
		// Command sequence to trigger vrSpeak
		srCmdSeq *seq = new srCmdSeq(); // sequence file that holds the bml command(s)
		seq->offset( (float)( SmartBody::SBScene::getScene()->getSimulationManager()->getTime()) );

		if( echo ) {
			msg << "echo // Running sequence \"" << seq_id << "\"...";
			if( seq->insert( 0, msg.str().c_str() )!=CMD_SUCCESS ) {
				std::stringstream strstr;
				strstr << "WARNING: send_vrX(..): Failed to insert echo header command for character \"" << char_id << "\".";
				////LOG(strstr.str().c_str());
			}
			msgId = build_vrX( msg, cmd, char_id, recip_id, bml, false );
			if( seq->insert( 0, msg.str().c_str() )!=CMD_SUCCESS ) {
				std::stringstream strstr;
				strstr << "WARNING: send_vrX(..): Failed to insert echoed command for character \"" << char_id << "\".";
				////LOG(strstr.str().c_str());
			}
		}
		if( all_characters )
		{
			const std::vector<std::string>& characterNames = SmartBody::SBScene::getScene()->getCharacterNames();
			for (std::vector<std::string>::const_iterator iter = characterNames.begin();
				iter != characterNames.end();
				iter++)
			{
				SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(*iter);
				msgId = build_vrX( msg, cmd, character->getName().c_str(), recip_id, bml, true );
				if( seq->insert( 0, msg.str().c_str() )!=CMD_SUCCESS ) {
					std::stringstream strstr;
					strstr << "WARNING: send_vrX(..): Failed to insert vrSpeak command for character \"" << char_id << "\".";
					////LOG(strstr.str().c_str());
				}
			}
		} else {
			msgId = build_vrX( msg, cmd, char_id, recip_id, bml, true );
			if( seq->insert( 0, msg.str().c_str() )!=CMD_SUCCESS ) {
				std::stringstream strstr;
				strstr << "WARNING: send_vrX(..): Failed to insert vrSpeak command for character \"" << char_id << "\".";
				////LOG(strstr.str().c_str());
			}
		}

		if( send ) {
			SmartBody::SBScene::getScene()->getCommandManager()->getActiveSequences()->removeSequence(seq_id, true); // remove old sequence by this name
			if( !SmartBody::SBScene::getScene()->getCommandManager()->getActiveSequences()->addSequence(seq_id, seq ))
			{
				std::stringstream strstr;
				strstr << "ERROR: send_vrX(..): Failed to insert seq into active sequences.";
				////LOG(strstr.str().c_str());
				return msgId;
			}
		} else {
			SmartBody::SBScene::getScene()->getCommandManager()->getPendingSequences()->removeSequence(seq_id, true);  // remove old sequence by this name
			if (SmartBody::SBScene::getScene()->getCommandManager()->getPendingSequences()->addSequence(seq_id, seq))
			{
				std::stringstream strstr;
				strstr << "ERROR: send_vrX(..): Failed to insert seq into pending sequences.";
				////LOG(strstr.str().c_str());
				return msgId;
			}
		}
		return msgId;
	}
}

std::string SBBmlProcessor::execBML(std::string character, std::string bml)
{
	std::ostringstream entireBml;
	entireBml	<< "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	const std::string& procId = SmartBody::SBScene::getScene()->getProcessId();
	if (procId != "")
		entireBml << "<act procid=\"" << procId << "\">\n";
	else
		entireBml << "<act>\n";
	entireBml   << "\t<bml>\n"
				<< "\t\t" << bml
				<< "\t</bml>\n"
				<< "</act>";		

	if (SmartBody::SBScene::getScene()->getBoolAttribute("useNewBMLParsing"))
	{
		std::vector<BMLObject*> behaviors = parseBML(entireBml.str());
		scheduleBML(behaviors);
	}
	//return send_vrX( "vrSpeak", character, "ALL", "", true, true, entireBml.str() );
	return send_vrX( "vrSpeak", character, "ALL", "", false, true, entireBml.str() );	
}

std::string SBBmlProcessor::execBMLFile(std::string character, std::string filename)
{
	//return send_vrX( "vrSpeak", character, "ALL", "", true, true, filename );
	return send_vrX( "vrSpeak", character, "ALL", "", false, true, filename );
}

std::string SBBmlProcessor::execXML(std::string character, std::string xml)
{
	std::ostringstream entireXML;
	entireXML	<< "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
				<< xml;
	//return send_vrX( "vrSpeak", character, "ALL", "", true, true, entireXML.str() );
	return send_vrX( "vrSpeak", character, "ALL", "", false, true, entireXML.str() );
	// do not send out VHMsg command, run it directly in this SmartBody instance.
	//return send_vrX( "vrSpeak", character, "ALL", "", false, false, entireXML.str() );
}

void SBBmlProcessor::execBMLAt(double time, std::string character, std::string bml)
{
	SBScene* scene = SBScene::getScene();

	std::stringstream strstr;
	strstr << "bml char " << character << " " << bml;
	scene->commandAt((float) time, strstr.str());
}

void SBBmlProcessor::execBMLFileAt(double time, std::string character, std::string filename)
{
	SBScene* scene = SBScene::getScene();

	std::stringstream strstr;
	strstr << "bml char " << character << " file " << filename;
	scene->commandAt((float) time, strstr.str());
}

void SBBmlProcessor::execXMLAt(double time, std::string character, std::string xml)
{
	SBScene* scene = SBScene::getScene();

	std::stringstream strstr;
	strstr << "bml char " << character << " " << xml;
	scene->commandAt((float) time, strstr.str());
}

void SBBmlProcessor::interruptCharacter(const std::string& character, double seconds)
{
	SBScene* scene = SBScene::getScene();
	SBCharacter* sbCharacter = scene->getCharacter(character);
	if (!sbCharacter)
	{
		LOG("No character named '%s' found. Interrupt not done.", character.c_str());
		return;
	}

	BML::Processor* bp = SmartBody::SBScene::getScene()->getBmlProcessor()->getBMLProcessor();
	bp->interrupt(sbCharacter, seconds, scene);
	
}

void SBBmlProcessor::interruptBML(const std::string& character, const std::string& id, double seconds)
{
	SBScene* scene = SBScene::getScene();
	SBCharacter* sbCharacter = scene->getCharacter(character);
	if (!sbCharacter)
	{
		LOG("No character named '%s' found. Interrupt not done.", character.c_str());
		return;
	}

	BML::Processor* bp = SmartBody::SBScene::getScene()->getBmlProcessor()->getBMLProcessor();
	bp->interrupt(sbCharacter, id, seconds, scene);
}


BML::Processor* SBBmlProcessor::getBMLProcessor()
{
	return _bmlProcessor;
}

std::vector<BMLObject*> SBBmlProcessor::parseBML(const std::string& bml)
{
	rapidxml::xml_document<> doc;
	doc.parse< 0 >((char*) bml.c_str());
	
	std::string participant = "";
	std::vector<BMLObject*> behaviors;

	// process each act
	rapidxml::xml_node<>* actNode = doc.first_node("act"); 
	while (actNode)
	{
		// process each bml
		rapidxml::xml_node<>* participantNode = actNode->first_node("participant");
		if (participantNode)
		{
			rapidxml::xml_attribute<>* attr = participantNode->first_attribute("id");
			if (attr)
			{
				participant = attr->name();
			}
		}

		rapidxml::xml_node<>* bmlNode = actNode->first_node("bml");
		while (bmlNode)
		{
			rapidxml::xml_node<>* behaviorNode = bmlNode->first_node();
			while (behaviorNode)
			{
				char* behaviorName = behaviorNode->name();
				std::map<std::string, BMLObject*>::iterator iter = _bmlHandlers.find(behaviorName);
				if (iter == _bmlHandlers.end())
				{
					LOG("BML parsing problem: cannot find handler for BML instruction: '%s'", behaviorName);
					continue;
				}
				BMLObject* bmlHandlerObject = (*iter).second;
				// create an instance of this object that will handle the parsing
				BMLObject* bmlInstance = bmlHandlerObject->copy();
				// parse the XML
				bmlInstance->parse(behaviorNode);

				behaviors.push_back(bmlInstance);

				behaviorNode = behaviorNode->next_sibling();
			}
			bmlNode = bmlNode->next_sibling("bml");

		}
		actNode = actNode->next_sibling();
	}

	return behaviors;
}

void SBBmlProcessor::scheduleBML(std::vector<BMLObject*>& behaviors)
{
	for (std::vector<BMLObject*>::iterator iter = behaviors.begin();
		 iter != behaviors.end();
		 iter++)
	{


	}
}

void SBBmlProcessor::processBML(double time)
{
	for (std::map<std::string, SBBMLSchedule*>::iterator iter = _bmlSchedules.begin();
		 iter != _bmlSchedules.end();
		 iter++)
	{
		SBBMLSchedule* schedule = (*iter).second;
		std::string characterName = (*iter).first;
		SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(characterName);
		if (!character)
		{
			// character no longer exists, remove the schedule
			schedule->remove();
		}

		// go through all the 


	}
}

} // namespace

