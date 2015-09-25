/*
 *  sbm_bml.cpp - part of SmartBody-lib
 *  Copyright (C) 2008  University of Southern California
 *
 *  SmartBody-lib is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public License
 *  as published by the Free Software Foundation, version 3 of the
 *  license.
 *
 *  SmartBody-lib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with SmartBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Andrew n marshall, USC
 */


/*
 *  sbm-bml.cpp
 *
 *  Implements the necessary BML interfaces
 *  for SBM's use of the BML libraries.
 */

#include <iostream>

#include "sbm_bml.hpp"
#include "mcontrol_util.h"



using namespace std;




///////////////////////////////////////////////////////////////////////////////
//  Helper Functions

namespace SmartBody {
	void vrSpeakFailed(
		const char* agent_id,
		const char* recipient,
		const char* message_id,
		const char* error_msg )
	{
		//  Let's not error on our error messages.  Be thorough.
		if( agent_id==NULL || agent_id[0]=='\0' )
			agent_id = "?";
		if( recipient==NULL || recipient[0]=='\0' )
			recipient = "?";
		if( message_id==NULL || message_id[0]=='\0' )
			message_id = "?";
		if( error_msg==NULL || error_msg[0]=='\0' )
			error_msg = "INVALID_ERROR_MESSAGE";

		ostringstream buff;
		cerr << "ERROR: vrSpeak: " << error_msg << "   (agent \"" << agent_id <<
			"\", recipient \"" << recipient << "\", message id \"" << message_id << "\")" << endl;
		buff << agent_id << " " << recipient << " " << message_id << " " << error_msg;
		mcuCBHandle::singleton().vhmsg_send( "vrSpeakFailed", buff.str().c_str() );
	}
};

///////////////////////////////////////////////////////////////////////////////
//  SmartBody::BmlProcessor

boost::shared_ptr<XercesDOMParser> SmartBody::BmlProcessor::xmlParser;


// Constructor
SmartBody::BmlProcessor::BmlProcessor()
:	_auto_print_schedule( false ),
	_log_synchpoints( false ),
	_warn_unknown_agents( true ),
	_parser( BML_X::Parser::new_instance() )
{
	if( xmlParser.get()==NULL ) {
		try {
			xmlParser = boost::shared_ptr<XercesDOMParser>( new XercesDOMParser() );

			xmlParser->setErrorHandler( new HandlerBase() );
			//ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
			//xmlParser->setErrorHandler(errHandler);
			//xmlParser->setErrorHandler( new HandlerBase() );
		} catch( const XMLException& e ) {
			cerr << "ERROR: BML Processor:  XMLException during constructor: "<< e.getMessage() << endl;
		} catch( const std::exception& e ) {
			cerr << "ERROR: BML Processor:  std::exception during constructor: "<< e.what() << endl;
		} catch(...) {
			cerr << "ERROR: BML Processor:  UNKNOWN EXCEPTION DURING CONSTRUCTOR.     <<==================" << endl;
		}
	}
}

SmartBody::BmlProcessor::~BmlProcessor() {
	if( _parser ) {
		delete _parser;
		_parser = NULL;
	}
}



///////////////////////////////////////////////////////////////////////////////
//  Static Command and Message Hooks


//  Notify BodyPlanner of vrSpeak command/message.
int SmartBody::BmlProcessor::vrSpeak_func( srArgBuffer& args, mcuCBHandle *mcu ) {
#if USE_NEW_BML_PROCESSOR
	BmlProcessor& bp = mcu->bml_processor;

	const char *agent_id     = args.read_token();
	const char *recipient_id = args.read_token();
	const char *message_id   = args.read_token();
	char       *xml          = args.read_remainder_raw();
	//cout << "DEBUG: vrSpeak " << agentId << " " << recipientId << " " << messageId << endl;

	try {
		if( xml[0]=='\0' ) {
			vrSpeakFailed( agent_id, recipient_id, message_id, "vrSpeak message incomplete (empty XML argument)." );
			return CMD_FAILURE;
		}
		if( xml[0] == '"' ) {
			++xml;
			int len = strlen( xml );
			xml[--len] = '\0'; // shorten by one, assumed to be another double quote
		}
		if( strstr( xml, "cache-overwrite" )==xml ) {  // if xml begins with "cache-overwrite"
			xml+=15; // skip it
			int whitespace = TRUE;
			while( whitespace ) {
				switch( xml[0] ) {
					case ' ':
					case '\t':
						++xml;
					default:
						whitespace = FALSE;
				}
			}
		}

		SbmCharacter *agent = mcu->character_map.lookup( agent_id );
		if( agent==NULL ) {
			//  Agent is not managed by this SBM process
			if( bp.warn_unknown_agents() )
				cerr << "WARNING: BmlProcessor: Unknown agent \"" << agent_id << "\"." << endl;
			// Ignore
			return CMD_SUCCESS;
		}

		if( agent->scheduler_p==NULL ) {
			vrSpeakFailed( agent_id, recipient_id, message_id, "Uninitialized agent." );
			return CMD_FAILURE;
		}

        DOMDocument *xmlDoc = xml_utils::parseMessageXml( xmlParser.get(), xml );
		if( xmlDoc == NULL ) {
			vrSpeakFailed( agent_id, recipient_id, message_id, "XML parser returned NULL document." );
			return CMD_FAILURE;
		}


		//  TODO!!!
		vrSpeakFailed( agent_id, recipient_id, message_id, "vrSpeak Unimplemented." );
		return CMD_FAILURE;

		//BodyPlannerMsg bpMsg( agent_id, recipient_id, message_id, agent, xmlDoc );
		//bp.vrSpeak( bpMsg, mcu );

		//return( CMD_SUCCESS );
	//} catch( BodyPlannerException& e ) {
	//	ostringstream msg;
	//	msg << "BodyPlannerException: "<<e.message;
	//	vrSpeakFailed( agent_id, recipient_id, message_id, msg.str().c_str() );
	//	return CMD_FAILURE;
	} catch( const std::exception& e ) {
		ostringstream msg;
		msg << "std::exception: "<<e.what();
		vrSpeakFailed( agent_id, recipient_id, message_id, msg.str().c_str() );
		return CMD_FAILURE;
	//} catch( ... ) {
	//	ostringstream msg;
	//	msg << "Unknown exception."<<e.message;
	//	vrSpeakFailed( agent_id, recipient_id, message_id, msg.str().c_str() );
	//	return CMD_FAILURE;
	}
#else
	cout << "ERROR: SmartBody::BmlProcessor::vrSpeak_func(..) called when compiled without USE_NEW_BML_PROCESSOR." << endl;
	return CMD_FAILURE;
#endif  // USE_NEW_BML_PROCESSOR
}

// Notify BodyPlanner of vrSpoke messages.
int SmartBody::BmlProcessor::vrSpoke_func( srArgBuffer& args, mcuCBHandle *mcu ) {
#if USE_NEW_BML_PROCESSOR
	cerr << "ERROR: SmartBody::BmlProcessor::vrSpoke_func(..): Unimplemented." << endl;
	return CMD_FAILURE;
#else
	cout << "ERROR: SmartBody::BmlProcessor::vrSpoke_func(..) called when compiled without USE_NEW_BML_PROCESSOR." << endl;
	return CMD_FAILURE;
#endif  // USE_NEW_BML_PROCESSOR
}

// Notify BodyPlanner of completed speech request.
int SmartBody::BmlProcessor::bpSpeechReady_func( srArgBuffer& args, mcuCBHandle *mcu ) {
#if USE_NEW_BML_PROCESSOR
	cerr << "ERROR: SmartBody::BmlProcessor::bpSpeechReady_func(..): Unimplemented." << endl;
	return CMD_FAILURE;
#else
	cout << "ERROR: SmartBody::BmlProcessor::bpSpeechReady_func(..) called when compiled without USE_NEW_BML_PROCESSOR." << endl;
	return CMD_FAILURE;
#endif  // USE_NEW_BML_PROCESSOR
}

// Notify BodyPlanner of request timings.
int SmartBody::BmlProcessor::bp_cmd_func( srArgBuffer& args, mcuCBHandle *mcu ) {
#if USE_NEW_BML_PROCESSOR
	cerr << "ERROR: SmartBody::BmlProcessor::bp_cmd_func(..): Unimplemented." << endl;
	return CMD_FAILURE;
#else
	cout << "ERROR: SmartBody::BmlProcessor::bp_cmd_func(..) called when compiled without USE_NEW_BML_PROCESSOR." << endl;
	return CMD_FAILURE;
#endif  // USE_NEW_BML_PROCESSOR
}


void set_bp_help() {
	cout << "Available BmlProcessor attributes for \"set bp ...\"" << endl
		    << "\tauto_print_schedule  [on|off]" << endl
		    << "\tlog_synchpoints      [on|off]" << endl
		    << "\twarn_unknown_agents  [on|off]" << endl;
}

// Handles the command "set bodyplanner" or "set bp"
int SmartBody::BmlProcessor::set_func( srArgBuffer& args, mcuCBHandle *mcu ) {
#if USE_NEW_BML_PROCESSOR
	BmlProcessor& bp = mcu->bml_processor;

	string attribute = args.read_token();
	if( attribute=="help" || attribute=="" ) {
		set_bp_help();
		return CMD_SUCCESS;
	} else if( attribute == "auto_print_schedule" ||
	           attribute == "auto-print-schedule" ) {
		string value = args.read_token();
		if( value == "on" ) {
			bp.auto_print_schedule( true );
			return CMD_SUCCESS;
		} else if( value == "off" ) {
			bp.auto_print_schedule( false );
			return CMD_SUCCESS;
		} else {
			cerr << "ERROR: SmartBody::BmlProcessor::set_func(): expected \"on\" or \"off\" for " << attribute <<".  Found \""<<value<<"\"."<< endl;
			return CMD_FAILURE;
		}
	} else if( attribute == "log_synchpoints" ||
	           attribute == "log-synchpoints" ) {
		string value = args.read_token();
		if( value == "on" ) {
			bp.log_synchpoints( true );
			return CMD_SUCCESS;
		} else if( value == "off" ) {
			bp.log_synchpoints( false );
			return CMD_SUCCESS;
		} else {
			cerr << "ERROR: SmartBody::BmlProcessor::set_func(): expected \"on\" or \"off\" for " << attribute <<".  Found \""<<value<<"\"."<< endl;
			return CMD_FAILURE;
		}
	} else if( attribute == "warn_unknown_agents" ||
	           attribute == "warn-unknown-agents" ) {
		string value = args.read_token();
		if( value == "on" ) {
			bp.warn_unknown_agents( true );
			return CMD_SUCCESS;
		} else if( value == "off" ) {
			bp.warn_unknown_agents( false );
			return CMD_SUCCESS;
		} else {
			cerr << "ERROR: SmartBody::BmlProcessor::set_func(): expected \"on\" or \"off\" for " << attribute <<".  Found \""<<value<<"\"."<< endl;
			return CMD_FAILURE;
		}
	} else {
		cerr << "ERROR: SmartBody::BmlProcessor::set_func(): Unknown attribute \"" << attribute <<"\"."<< endl;
		set_bp_help();
        return CMD_NOT_FOUND;
	}
#else
	cout << "ERROR: SmartBody::BmlProcessor::set_func(..) called when compiled without USE_NEW_BML_PROCESSOR." << endl;
	return CMD_FAILURE;
#endif  // USE_NEW_BML_PROCESSOR
}

void print_bp_help() {
	cout << "Available BmlProcessor attributes for \"print bp ...\"" << endl
	     << "\tauto_print_schedule" << endl
	     << "\tlog_synchpoints" << endl
	     << "\twarn_unknown_agents" << endl;
}

// Handles the command "print bp"
int SmartBody::BmlProcessor::print_func( srArgBuffer& args, mcuCBHandle *mcu ) {
#if USE_NEW_BML_PROCESSOR
	BmlProcessor& bp = mcu->bml_processor;

	string attribute = args.read_token();
	if( attribute=="help" || attribute=="" ) {
		print_bp_help();
		return CMD_SUCCESS;
	} else if( attribute == "auto_print_schedule" ||
	           attribute == "auto-print-schedule" ) {
		cout << "BML Processor auto_print_schedule: "<<
			(bp.auto_print_schedule()? "on" : "off") << endl;
		return CMD_SUCCESS;
	} else if( attribute == "log_synchpoints" ||
	           attribute == "log-synchpoints" ) {
		cout << "BML Processor log_synchpoints: "<<
			(bp.log_synchpoints()? "on" : "off") << endl;
		return CMD_SUCCESS;
	} else if( attribute == "warn_unknown_agents" ||
	           attribute == "warn-unknown-agents" ) {
		cout << "BML Processor warn_unknown_agents: "<<
			(bp.warn_unknown_agents()? "on" : "off") << endl;
		return CMD_SUCCESS;
	} else {
		cerr << "ERROR: SmartBody::BmlProcessor::print_func(): Unknown attribute \"" << attribute <<"\"."<< endl;
		print_bp_help();
        return CMD_NOT_FOUND;
	}
#else
	cout << "ERROR: SmartBody::BmlProcessor::print_func(..) called when compiled without USE_NEW_BML_PROCESSOR." << endl;
	return CMD_FAILURE;
#endif  // USE_NEW_BML_PROCESSOR
}
