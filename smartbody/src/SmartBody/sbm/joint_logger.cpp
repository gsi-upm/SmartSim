/*
 *  joint_logger.cpp - part of SmartBody-lib
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

#include "vhcl.h"

#include <iostream>
#include <sstream>
#include <string>

#include <controllers/me_ct_blend.hpp>
#include <controllers/me_controller_tree_root.hpp>
#include "joint_logger.hpp"

using namespace std;


namespace joint_logger {
	//  These probably shouldn't global,
	//  but I don't know where they really belong.
	set<string> joints_to_log;
	double      logging_duration = 0.1;
}

int joint_logger::set_func( srArgBuffer& args, mcuCBHandle *mcu_p ) {
	EvaluationLogger* logger = mcu_p->logger_p;

	string attribute = args.read_token();

	if( attribute == "joints" ) {
		// Adjust the list of joints to print when running

		string arg = args.read_token();
		if( arg == "+" ) {
			// Adding to existing joints
			arg = args.read_token();
			while( arg.length() ) {
				joints_to_log.insert( arg );
				arg = args.read_token();
			}
		} else if( arg == "-" ) {
			// Removing from existing joints
			arg = args.read_token();
			while( arg.length() ) {
				joints_to_log.erase( arg );
				arg = args.read_token();
			}
		} else {
			// Setting the complete set of joints
			joints_to_log.clear();
			while( arg.length() ) {
				joints_to_log.insert( arg );
				arg = args.read_token();
			}
		}

		for (std::map<std::string, SbmCharacter*>::iterator iter = mcu_p->getCharacterMap().begin();
			iter != mcu_p->getCharacterMap().end();
			iter++)
		{
			(*iter).second->ct_tree_p->set_logged_joints( joints_to_log );
		}

		return CMD_SUCCESS;
	} else if( attribute == "duration" ) {
		string arg = args.read_token();
		if( arg.length() ) {
			istringstream in( arg );
			double d;
			in >> d;
			if( in.fail() ) {
				std::stringstream strstr;
				strstr << "ERROR: joint_logger::set_func(..) \" failed to read duration as a double from \"" << arg << "\".";
				LOG(strstr.str().c_str());
				return CMD_FAILURE;
			}
			joint_logger::logging_duration = d;
			return CMD_SUCCESS;
		} else {
			std::stringstream strstr;
			strstr << "ERROR: joint_logger::set_func(..) \" missing argument to set " << attribute << ".";
			LOG(strstr.str().c_str());
			return CMD_FAILURE;
		}
	} else if( attribute == "" ) {
		LOG("ERROR: joint_logger::set_func(..) missing attribute to set. Expected \"joints\" or \"duration\".");
		return CMD_FAILURE;
	} else {
		LOG("ERROR: joint_logger::set_func(..) unknown attribute: %s", attribute.c_str());
		return CMD_FAILURE;
	}
}

int joint_logger::print_func( srArgBuffer& args, mcuCBHandle *mcu_p ) {
	EvaluationLogger* logger = mcu_p->logger_p;

	string attribute = args.read_token();

	if( attribute == "joints" ) {
		ostringstream out;

		set<string>::iterator i = joints_to_log.begin();
		set<string>::iterator end = joints_to_log.end();
		for(; i != end; ++i ) {
			out << " " << *i;
		}

		cout << "joint_logger joints:" << out.str() << endl;
		return CMD_SUCCESS;
	} else if( attribute == "duration" ) {
		LOG("joint_logger duration: %f", joint_logger::logging_duration);
		return CMD_SUCCESS;
	} else if( attribute == "" ) {
		LOG("ERROR: joint_logger::print_func(..) missing attribute to print. Expected \"joints\" or \"duration\".");
		return CMD_FAILURE;
	} else {
		LOG("ERROR: joint_logger::print_func(..) unknown attribute: %s", attribute.c_str());
		return CMD_FAILURE;
	}
}

int joint_logger::start_stop_func( srArgBuffer& args, mcuCBHandle *mcu_p ) {
	EvaluationLogger* logger = mcu_p->logger_p;

	string command = args.read_token();
	if( command=="" || command=="start" ) {
		logger->set_expiration_time( mcu_p->time + joint_logger::logging_duration );
		return CMD_SUCCESS;
	} else if( command=="stop" ) {
		logger->set_expiration_time( 0 );
		return CMD_SUCCESS;
	} else {
		LOG("ERROR: joint_logger::start_stop_func(..) unknown joint_logger command: \"%s\"", command.c_str());
		return CMD_FAILURE;
	}
}



joint_logger::EvaluationLogger::EvaluationLogger()
:	_expiration_time( 0 ),
	_enabled( false )
{}

// Prints context name and type,
// with initial data for the context's selected channels.
void joint_logger::EvaluationLogger::context_pre_evaluate(
	double time,
	MeControllerContext& context,
	MeFrameData& frame
) {
	const std::string& context_type = context.context_type();
	bool is_root = context_type == MeControllerTreeRoot::CONTEXT_TYPE;
	if( is_root ) {
		_enabled = (time < _expiration_time);
		_indent_len = 0;
	}
	if( _enabled ) {
		ostringstream out;
		if( _indent.length() != _indent_len ) {
			_indent.assign( _indent_len, '\t' );
		}
		out << _indent << "CONTEXT: " << context_type
			<< " \"" << context.name()
			<< "\" (time " << time << ")" << endl;

		if( is_root || context_type==MeCtBlend::Context::CONTEXT_TYPE )
			format_channel_data( context, frame, out );

		clog << out.str();

		++_indent_len;
	}
}

// Prints the context name and type, denoting completion.
void joint_logger::EvaluationLogger::context_post_evaluate(
	double time,
	MeControllerContext& context,
	MeFrameData& frame
) {
	if( _enabled ) {
		--_indent_len;

		const std::string& context_type = context.context_type();
		if( context_type == MeControllerTreeRoot::CONTEXT_TYPE 
			|| context_type == MeCtBlend::Context::CONTEXT_TYPE ) {
			if( _indent.length() != _indent_len ) {
				_indent.assign( _indent_len, '\t' );
			}
			ostringstream out;
			//out << _indent << "CONTEXT: " << context_type
			//	<< " \"" << context.name()
			//	<< "\" (time " << time << ")" << endl;
			format_channel_data( context, frame, out );
			clog << out.str();
		}
	}
}

// Prints the controller results for the context's selected channels.
void joint_logger::EvaluationLogger::controller_pre_evaluate(
	double time,
	MeControllerContext& context,
	MeController& ct,
	MeFrameData& frame
) {
	if( _enabled ) {
		ostringstream out;
		if( _indent.length() != _indent_len ) {
			_indent.assign( _indent_len, '\t' );
		}
		out << _indent << "CONTROLLER: " << ct.controller_type();
		const char* ct_name = ct.getName().c_str();
		if( ct_name )
			out << " \"" << ct_name << "\"";
		out << " (time " << time << ")" << endl;

		clog << out.str();
		++_indent_len;
	}
}

// Prints the controller results for the context's selected channels.
void joint_logger::EvaluationLogger::controller_post_evaluate(
	double time,
	MeControllerContext& context,
	MeController& ct,
	MeFrameData& frame
) {
	if( _enabled ) {
		--_indent_len;

		ostringstream out;
		if( _indent.length() != _indent_len ) {
			_indent.assign( _indent_len, '\t' );
		}
		format_channel_data( context, frame, out );
		clog << out.str();
	}
}


// Formats the channel data at current indent into the ostringstream
void joint_logger::EvaluationLogger::format_channel_data(
	MeControllerContext& context,
	MeFrameData& frame,
	ostringstream& out
) {
	const set<int>& indices = context.get_logged_channel_indices();
	if( indices.size() == 0 )
		return;

	set<int>::const_iterator i   = indices.begin();
	set<int>::const_iterator end = indices.end();

	while( i != end ) {
		int index = *i;
		SkChannelArray& channels = context.channels();
		SkChannel::Type type = channels.type(index);
		int buffer_index = context.toBufferIndex( index );
		SrBuffer<float>& buffer = frame.buffer();

		out << _indent << index << "\t" << channels.name(index)
			<< " (" << SkChannel::type_name(type) << "): ";
		switch( SkChannel::size(type) ) {
			case 1:
				out << buffer[ buffer_index ] << endl;
				break;
			case 2:  // Swing
				out << buffer[ buffer_index ] << ", "
					<< buffer[ buffer_index+1 ] << endl;
				break;
			case 4:  // Quat
				out << buffer[ buffer_index ] << ", "
					<< buffer[ buffer_index+1 ] << ", "
					<< buffer[ buffer_index+2 ] << ", "
					<< buffer[ buffer_index+3 ] << endl;
				break;
		}

		++i;
	}
}

