/*
 *  me_ct_navigation_circle.hpp - part of SmartBody-lib's Motion Engine
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

#ifndef ME_CT_NAVIGATION_CIRCLE_HPP
#define ME_CT_NAVIGATION_CIRCLE_HPP

#include <sb/SBController.h>



/**
 *  Controls the locomotions inputs to drive a character in a circle
 */
class MeCtNavigationCircle : public SmartBody::SBController {
public:
	// Public Constants
	static std::string TYPE;

protected:
	// Data
	bool is_valid;  // All necessary channels are present

	SkChannelArray  request_channels;

	// init parameters
	double start_time;
	double last_time;
	float forward_velocity;
	float radians_per_second;
	//float radius;

	// Buffer indices ("bi_") to the requested channels
	int bi_world_rot;                                 // Input: World Orientation
	int bi_loco_vel_x, bi_loco_vel_y, bi_loco_vel_z;  // Output: Locomotion velocity
	int bi_loco_rot_global_y;                             // Rotational velocity around Y
	int bi_loco_rot_local_y;                             // Rotational velocity around Y
	int bi_loco_rot_local_angle;
	int bi_loco_time;
	int bi_id;											 // ID
	int bi_has_destination;
	int bi_loco_dest_x, bi_loco_dest_y, bi_loco_dest_z;
	int bi_facing_w, bi_facing_x, bi_facing_y, bi_facing_z;
	int bi_reset_flag;

	bool new_routine;
	SrVec velocity;
	SrVec destination;
	int has_destination;
	float g_angular;
	float l_angular;
	float l_angle;
	int id;
	float time;


public:
	/** Constructor */
	MeCtNavigationCircle();

	const std::string& controller_type() const;

	/**
	 *  Initializes the controller.
	 */
	void init();

	void set( float dx, float dy, float dz, float g_angular, float l_angular, float l_angle, int id, int has_destination, float tx, float tz, float time);
	void setRadiansPerSecond( float forward_vel, float radians_per_sec );
	void setRadius( float forward_vel, float radius );

	void setContext(MeControllerContext* context);
	/**
	 *  Implements MeController::controller_channels().
	 */
	SkChannelArray& controller_channels();

	/*!
	 *  Implements MeController::context_updated(..).
	 */
	virtual void context_updated();

	/*!
	 *  Implements MeController::controller_map_updated().
	 */
	virtual void controller_map_updated();

	/*!
	 *  Implements MeController::controller_duration().
	 */
	virtual double controller_duration();

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
#endif // ME_CT_NAVIGATION_CIRCLE_HPP
