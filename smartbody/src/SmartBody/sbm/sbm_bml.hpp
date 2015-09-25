/*
 *  sbm_bml.hpp - part of SmartBody-lib
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
 *  sbm-bml.hpp
 *
 *  Implements the necessary BML interfaces
 *  for SBM's use of the BML libraries.
 */
#ifndef SBM_BML_HPP
#define SBM_BML_HPP


#include <boost/shared_ptr.hpp>
#include "xercesc_utils.hpp"

#include <bml/parser.hpp>

#include "sbm_constants.h"
#include "sr_arg_buff.h"




// Forward Declaration
class mcuCBHandle;


namespace SmartBody {
	class BmlProcessor {
		///////////////////////////////////////////////////////////////////////
		//  Private Data Structures
//		struct BodyPlannerException {
//			const char* message;
//
//			BodyPlannerException( const char* message )
//				: message(message)
//			{}
//		};


//		struct BodyPlannerMsg {
//			const char			*agentId;
//			const char			*recipientId;
//			const char			*msgId;
//			const SbmCharacter	*agent;
//			const DOMDocument	*xml;
//
//			BodyPlannerMsg( const char *agentId, const char *recipientId, const char *msgId, const SbmCharacter *agent, DOMDocument *xml )
//			:	agentId(agentId),
//				recipientId(recipientId),
//				msgId(msgId),
//				agent(agent),
//				xml(xml)
//			{}
//		};

		//////////////////////////////////////////////////////////////////////////
		// Private Constants

		// Private Data
//		srHashMap<BML::BmlRequest>    requests;     // indexed by msgId
//		srHashMap<BML::SpeechRequest> speeches; // indexed by RequestId

		bool _auto_print_schedule;
		bool _log_synchpoints;
		bool _warn_unknown_agents;

		BML_X::Parser* _parser;

		///////////////////////////////////////////////////////////////////////
		//  Static Variables
		static boost::shared_ptr<XercesDOMParser> xmlParser;


	public:
		//////////////////////////////////////////////////////////////////////////
		// Public Methods
		BmlProcessor();
		virtual ~BmlProcessor();

		bool auto_print_schedule()
		{	return _auto_print_schedule; }
		void auto_print_schedule( bool value )
		{	_auto_print_schedule = value; }

		bool log_synchpoints()
		{	return _log_synchpoints; }
		void log_synchpoints( bool value )
		{	_log_synchpoints = value; }

		bool warn_unknown_agents()
		{	return _warn_unknown_agents; }
		void warn_unknown_agents( bool value )
		{	_warn_unknown_agents = value; }



//		void reset();
//
//
//		/**
//		*  Handles vrSpeak messages from agent
//		*/
//		void vrSpeak( BodyPlannerMsg& bpMsg, mcuCBHandle *mcu );
//
//		/**
//		*  Parses <BML> elements
//		*/
//		void parseBML( DOMElement *el, BML::TriggerEvent *trigger, mcuCBHandle *mcu );
//
//		/**
//		*/
//		void speechReply( const char* requestId, const char* error, mcuCBHandle *mcu );
//
//		/**
//		*  Completes final timing calculations and triggers schedule
//		*/
//		void realizeRequest( BML::BmlRequest* request, BodyPlannerMsg& bpMsg, mcuCBHandle *mcu );
//
//		/**
//		*  Handles vrSpoke messages from agent, cleans up old BmlRequest obj.
//		*/
//		int vrSpoke( BodyPlannerMsg& bpMsg, mcuCBHandle *mcu );


	public:
		///////////////////////////////////////////////////////////////////////
		//  Static Command and Message Hooks

		/**
		 *  Notify BodyPlanner of vrAgentBML commands/messages.
		 */
		static int vrAgentBML_cmd_func( srArgBuffer& args, mcuCBHandle *mcu );

		/**
		 *  Notify BodyPlanner of vrSpeak command/message.
		 */
		static int vrSpeak_func( srArgBuffer& args, mcuCBHandle *mcu );

		/**
		 *  Notify BodyPlanner of vrSpoke messages.
		 */
		static int vrSpoke_func( srArgBuffer& args, mcuCBHandle *mcu );

		/**
		 *  Notify BodyPlanner of completed speech request.
		 */
		static int bpSpeechReady_func( srArgBuffer& args, mcuCBHandle *mcu );

		/**
 		 *  Notify BodyPlanner of request timings.
		 */
		static int bp_cmd_func( srArgBuffer& args, mcuCBHandle *mcu );

		/**
 		 *  Handles the command "set bp"
		 */
		static int set_func( srArgBuffer& args, mcuCBHandle *mcu );

		/**
 		 *  Handles the command "print bp"
		 */
		static int print_func( srArgBuffer& args, mcuCBHandle *mcu );
	}; // class BmlProcessor
};  // end namespace SmartBody


#endif // SBM_BML_HPP
