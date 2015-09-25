/*
 *  me_ct_navigation_waypoint.hpp - part of SmartBody-lib's Motion Engine
 *  Copyright (C) 2009  University of Southern California
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

#ifndef ME_CT_NAVIGATION_WAYPOINT_HPP
#define ME_CT_NAVIGATION_WAYPOINT_HPP

#include <sb/SBController.h>


/**
 *  Controls the locomotion vector to approach a single waypoint at a given speed.
 */
class MeCtNavigationWaypoint : public SmartBody::SBController {
public:
	// Public Constants
	static std::string TYPE;

protected:
	// Data
	SkChannelArray  request_channels;

	bool is_valid;

	double last_time;


public:
	/** Constructor */
	MeCtNavigationWaypoint();

	const std::string& controller_type();

	/**
	 *  Initializes the controller with a set of channels to write.
	 */
	void init();

	/**
	 *  Implements MeController::controller_channels().
	 */
	SkChannelArray& controller_channels();

	/**
	 *  Implements MeController::controller_duration()
	 *  Returns -1, undefined duration.
	 */
	double controller_duration();

	/*!
	 *  Implements MeController::context_updated(..).
	 */
	virtual void context_updated();

	/**
	 *  Implements MeController::controller_evaluate(..).
	 */
	bool controller_evaluate( double time, MeFrameData& frame );

	/*! Implements MeController::printe_state.
	    Print the info about the controller and its state to stdout.  The first 
	    line of output should begin immediately, and second and following lines 
		should be indented by tabCount number of tabs.  Child controller should 
		be indented by an additional tab. All output should end with a new line.  */
	virtual void print_state( int tab_count );

};

#endif // ME_CT_NAVIGATION_WAYPOINT_HPP
