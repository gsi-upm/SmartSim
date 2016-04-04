/*
 *  me_ct_lifecycle_test.cpp - part of SmartBody-lib's Motion Engine
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

#include "controllers/me_ct_lifecycle_test.hpp"


#include <iostream>
#include <string>


using namespace std;



#define EVALUATION_COUNT_MAX (5)

//////////////////////////////////////////////////////////////////
//  MeCtLifecycleTest

std::string MeCtLifecycleTest::CONTROLLER_TYPE = "MeCtLifecycleTest";


MeCtLifecycleTest::MeCtLifecycleTest()
:	MeCtUnary( new MeCtUnary::Context( this ) ),
	out( NULL ),
	evaluate_count( 0 )
{}

const std::string& MeCtLifecycleTest::controller_type() const {
	print_method_entry( "controller_type()" );

	return CONTROLLER_TYPE;
}

void MeCtLifecycleTest::init( MeController* child, ostream* out ) {
	this->out = out;
	print_method_entry( "init()" );

	MeCtUnary::init( child );

	update_timing_from_child();
}

size_t MeCtLifecycleTest::count_children() {
	print_method_entry( "count_children()" );

	return MeCtUnary::count_children();
}

void MeCtLifecycleTest::controller_init() {
	print_method_entry( "controller_init()" );
}

void MeCtLifecycleTest::context_updated() {
	print_method_entry( "context_updated()" );
	// << "Context is "<< context_str << endl;

	MeCtUnary::controller_init();
	update_timing_from_child();
}

void MeCtLifecycleTest::controller_start() {
	print_method_entry( "controller_start()" );

	MeCtUnary::controller_start();
	update_timing_from_child();
}

void MeCtLifecycleTest::controller_stop() {
	print_method_entry( "controller_stop()" );

	MeCtUnary::controller_stop();
}

void MeCtLifecycleTest::controller_map_updated() {
	print_method_entry( "controller_map_updated()" );

	MeCtUnary::controller_map_updated();
	update_timing_from_child();
}

bool MeCtLifecycleTest::controller_evaluate( double t, MeFrameData& frame ) {
	if( evaluate_count < EVALUATION_COUNT_MAX )
		print_method_entry( "controller_evaluate()", false );
	++evaluate_count;

	MeController* child = this->child();
	if( child ) {
		child->evaluate( t, frame );

		return child->active();
	} else {
		return false;
	}
}

SkChannelArray& MeCtLifecycleTest::controller_channels() {
	if( evaluate_count < EVALUATION_COUNT_MAX )
		print_method_entry( "controller_channels()", false );

	return MeCtUnary::controller_channels();
}

double MeCtLifecycleTest::controller_duration() {
	print_method_entry( "controller_duration()" );

	MeController* child = this->child();
	if( child ) {
		return child->controller_duration();
	} else {
		return 0;
	}
}

bool MeCtLifecycleTest::remove_child( MeController* child ) {
	print_method_entry( "remove_child()" );

	return MeCtUnary::remove_child( child );
}

void MeCtLifecycleTest::print_state( int tab_count ) {
	print_method_entry( "print_state()" );

	MeCtUnary::print_state( tab_count );
}

void MeCtLifecycleTest::print_children( int tab_count ) {
	print_method_entry( "print_children()" );

	MeCtUnary::print_children( tab_count );
}

void MeCtLifecycleTest::print_method_entry( string method, bool reset_eval_count ) const {
	if( out ) {
		string state_descrip = "_context is ";
		if( _context ) {
			state_descrip += "set";
			if( _context->channels().skeleton() ) {
				state_descrip += " but skeleton is NULL";
			} else {
				state_descrip += " with skeleton";
			}
		} else {
			state_descrip += "NULL";
		}

	/*	(*out) << "MeCtLifecycleTest "
		       << '\"' << getName() << "\" "
			   << method << ":\t\t" << state_descrip << endl;
			   */
	}
	if( reset_eval_count )
		(const_cast<MeCtLifecycleTest*>(this))->evaluate_count = 0;
}
