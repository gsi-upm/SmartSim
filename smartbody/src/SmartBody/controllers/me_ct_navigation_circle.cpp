/*
 *  me_ct_navigation_circle.cpp - part of SmartBody-lib's Motion Engine
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

#include <math.h>

#include <controllers/me_ct_navigation_circle.hpp>
#include <sbm/sbm_character.hpp>
#include "sbm/gwiz_math.h"

std::string MeCtNavigationCircle::TYPE = "MeCtNavigationCircle";


MeCtNavigationCircle::MeCtNavigationCircle()
:	is_valid( false ),
	start_time( std::numeric_limits<double>::quiet_NaN() ),
	last_time( std::numeric_limits<double>::quiet_NaN() ),
	radians_per_second( 0 ),
	forward_velocity( 0 ),
//	radius( 0 ),
	bi_world_rot(-1), bi_loco_vel_x(-1), bi_loco_vel_y(-1), bi_loco_vel_z(-1), bi_loco_rot_global_y(-1), bi_loco_rot_local_y(-1), bi_loco_rot_local_angle(-1), bi_id(-1), bi_loco_time(-1)
{}

void MeCtNavigationCircle::setRadius( float forward_velocity, float radius ) {
	this->forward_velocity   = forward_velocity;
	this->radians_per_second = forward_velocity / radius;
//	this->radius             = radius;
}

void MeCtNavigationCircle::setRadiansPerSecond( float forward_velocity, float radians_per_second ) {
	this->forward_velocity   = forward_velocity;
	this->radians_per_second = radians_per_second;
//	this->radius             = forward_velocity / radians_per_second;
}

const std::string& MeCtNavigationCircle::controller_type() const {
	return TYPE;
}

// Implements MeController::context_updated(..)
void MeCtNavigationCircle::context_updated() {
	if( _context == NULL ) {
		is_valid = false;
		last_time = start_time = std::numeric_limits<double>::quiet_NaN();
	}
}

// Implements MeController::controller_channels().
SkChannelArray& MeCtNavigationCircle::controller_channels() {
	return request_channels;
}

// Look up the context indices, and check to make sure it isn't -1
#define LOOKUP_BUFFER_INDEX( var_name, ct_index ) \
	var_name = _context->toBufferIndex( _toContextCh[ ( ct_index ) ] );  \
	is_valid &= ( var_name != -1 );

void MeCtNavigationCircle::controller_map_updated() {
	is_valid = true;

	if( _context != NULL ) {
		// request_channel indices (second param) come from the order of request_channels.add(..) calls in controller_channels()
		LOOKUP_BUFFER_INDEX( bi_world_rot,  0 );

		LOOKUP_BUFFER_INDEX( bi_loco_vel_x, 1 );
		LOOKUP_BUFFER_INDEX( bi_loco_vel_y, 2 );
		LOOKUP_BUFFER_INDEX( bi_loco_vel_z, 3 );

		LOOKUP_BUFFER_INDEX( bi_loco_rot_global_y, 4 );
		LOOKUP_BUFFER_INDEX( bi_loco_rot_local_y, 5 );
		LOOKUP_BUFFER_INDEX( bi_loco_rot_local_angle, 6 );
		LOOKUP_BUFFER_INDEX( bi_loco_time, 7 );

		LOOKUP_BUFFER_INDEX( bi_id, 8 );
	} else {
		// This shouldn't get here
		is_valid = false;
	}
}

double MeCtNavigationCircle::controller_duration() {
	return -1;
}

void MeCtNavigationCircle::init()
{
	// Initialize Requested Channels                                                           // Indices
	request_channels.add(SbmPawn::WORLD_OFFSET_JOINT_NAME , SkChannel::Quat );  //  0

	request_channels.add( MeCtLocomotionPawn::LOCOMOTION_VELOCITY , SkChannel::XPos ); //  1
	request_channels.add( MeCtLocomotionPawn::LOCOMOTION_VELOCITY , SkChannel::YPos ); //  2
	request_channels.add( MeCtLocomotionPawn::LOCOMOTION_VELOCITY , SkChannel::ZPos ); //  3

	request_channels.add(MeCtLocomotionPawn::LOCOMOTION_GLOBAL_ROTATION , SkChannel::YPos ); //  4
	request_channels.add(MeCtLocomotionPawn::LOCOMOTION_LOCAL_ROTATION , SkChannel::YPos ); //  5
	request_channels.add(MeCtLocomotionPawn::LOCOMOTION_LOCAL_ROTATION_ANGLE , SkChannel::YPos ); //  6
	request_channels.add(MeCtLocomotionPawn::LOCOMOTION_TIME , SkChannel::YPos ); //  7

	request_channels.add(MeCtLocomotionPawn::LOCOMOTION_ID , SkChannel::YPos ); //  8
}


void MeCtNavigationCircle::set( float dx, float dy, float dz, float g_angular, float l_angular, float l_angle, int id, int has_destination, float tx, float tz, float time)
{
	new_routine = true;
	velocity.x = dx;
	velocity.y = dy;
	velocity.z = dz;
	destination.x = tx;
	destination.y = 0.0f;
	destination.z = tz;
	this->g_angular = g_angular;
	this->l_angular = l_angular;
	this->l_angle = l_angle;
	this->id = id;
	this->time = time;
}


/*bool MeCtNavigationCircle::controller_evaluate( double time, MeFrameData& frame ) {
	if( is_valid ) {
		float time_delta = 0.033f;   // assume a 30fps
		if( last_time == last_time ) {  // false if NaN (after first run)
			time_delta = (float)( time - last_time );
		}
		if( start_time != start_time ) {  // true if NaN (on first run)
			start_time = time - time_delta;
		}
		last_time = time;

		SrBuffer<float>& buffer = frame.buffer(); // convenience reference

		// Read inputs
		
		quat_t world_quat( buffer[ bi_world_rot ], buffer[ bi_world_rot+1 ], buffer[ bi_world_rot+2 ], buffer[ bi_world_rot+3 ] );
		gwiz::float_t world_roty_deg = euler_t( world_quat ).y();
		euler_t world_roty( 0, world_roty_deg, 0 );

		float delta_radians = radians_per_second * time_delta;

		vector_t local_velocity = vector_t( sin( delta_radians ), 0, cos( delta_radians ) )*forward_velocity;
		vector_t world_velocity = world_roty * local_velocity;


		if(!new_routine) return;


		// Write Results
		//buffer[ bi_loco_vel_x ] = (float)world_velocity.x();
		//buffer[ bi_loco_vel_y ] = (float)world_velocity.y();
		//buffer[ bi_loco_vel_z ] = (float)world_velocity.z();

		//buffer[ bi_loco_rot_y ] = radians_per_second;
	}

	return is_valid;
}*/

bool MeCtNavigationCircle::controller_evaluate( double time, MeFrameData& frame ) {
	if (!is_valid)
	{
		remap();
		controller_map_updated();
	}

	if( is_valid ) 
	{
		SrBuffer<float>& buffer = frame.buffer(); // convenience reference
		if(!new_routine) 
		{
			buffer[ bi_id ] = 0;
			return is_valid;
		}

		// Write Results
		buffer[ bi_loco_vel_x ] = velocity.x;
		buffer[ bi_loco_vel_y ] = velocity.y;
		buffer[ bi_loco_vel_z ] = velocity.z;

		buffer[ bi_loco_rot_global_y ] = g_angular;
		buffer[ bi_loco_rot_local_y ] = l_angular;
		buffer[ bi_loco_rot_local_angle ] = l_angle;
		buffer[ bi_loco_time ] = this->time;
		buffer[ bi_id ] = (float)id;

		//buffer[ bi_has_destination ] = has_destination;

		//buffer[ bi_loco_dest_x ] = destination.x;
		//buffer[ bi_loco_dest_y ] = destination.y;
		//buffer[ bi_loco_dest_z ] = destination.z;
		new_routine = false;
	}

	return is_valid;
}

void MeCtNavigationCircle::print_state( int tab_count ) {
	// TODO
	MeController::print_state( tab_count );
}

void MeCtNavigationCircle::setContext(MeControllerContext* context)
{
	_context = context;
}
