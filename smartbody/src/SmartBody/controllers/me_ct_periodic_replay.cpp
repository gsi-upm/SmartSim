/*
 *  me_ct_periodic_replay.cpp - part of SmartBody-lib's Motion Engine
 *  Copyright (C) 2010  University of Southern California
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
 *      Ed Fast, USC
 */

#include "vhcl.h"
#include <controllers/me_ct_periodic_replay.hpp>


#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>


std::string MeCtPeriodicReplay::CONTROLLER_TYPE = "MeCtPeriodicReplay";

MeCtPeriodicReplay::MeCtPeriodicReplay( MeController* child )
:	MeCtUnary( new MeCtUnary::Context(this), child ),
	period( 0 ),
	period_offset( 0 ),
	child_time_offset( 0 )
{
	if( child ) {
		_sub_context->add_controller( child );
	}
}

const std::string& MeCtPeriodicReplay::controller_type() const {
	return CONTROLLER_TYPE;
}

void MeCtPeriodicReplay::init( double period ) {
	init( period, 0, 0 );
}

void MeCtPeriodicReplay::init( double period, double period_offset, double child_offset ) {
	this->period = period;
	this->period_offset = period_offset;
	this->child_time_offset = child_offset;
}

double MeCtPeriodicReplay::controller_duration() {
	return -1;  // indefinite, regardless of child duration
}

bool MeCtPeriodicReplay::controller_evaluate( double t, MeFrameData & frame ) {
	if( child() ) {
		double temp1 = t - period_offset;
		double temp2 = fmod( temp1, period );
		double temp3 = temp2+child_time_offset;
		//t = fmod( t-period_offset, period ) + child_time_offset;;
		child()->evaluate( temp3, frame );
		return true;
	} else {
		return false;
	}
}

