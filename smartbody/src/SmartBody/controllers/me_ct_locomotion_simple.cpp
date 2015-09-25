/*
 *  me_ct_locomotion_simple.hpp - part of SmartBody-lib's Motion Engine
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
 *      
 */

#include "controllers/me_ct_locomotion_simple.hpp"
#include "sbm/sbm_character.hpp"
#include "limits.h"
#include "sbm/gwiz_math.h"

#if 0
using namespace gwiz;
#endif


std::string MeCtLocomotionSimple::TYPE = "MeCtLocomotionSimple";

/** Constructor */
MeCtLocomotionSimple::MeCtLocomotionSimple()
:	is_valid( false ),
	last_time( std::numeric_limits<float>::quiet_NaN() )
{}

/** Destructor */
MeCtLocomotionSimple::~MeCtLocomotionSimple() {
	// Nothing allocated to the heap
}

const std::string& MeCtLocomotionSimple::controller_type() {
	return TYPE;
}

// Implements MeController::controller_channels().
SkChannelArray& MeCtLocomotionSimple::controller_channels() {
	if( request_channels.size() == 0 ) {
		// Initialize Requested Channels                                                           // Indices
		request_channels.add(  SbmPawn::WORLD_OFFSET_JOINT_NAME, SkChannel::XPos );  // #0
		request_channels.add(  SbmPawn::WORLD_OFFSET_JOINT_NAME , SkChannel::YPos );  //  1
		request_channels.add( SbmPawn::WORLD_OFFSET_JOINT_NAME , SkChannel::ZPos );  //  2
		request_channels.add(  SbmPawn::WORLD_OFFSET_JOINT_NAME , SkChannel::Quat );  //  3

		request_channels.add( MeCtLocomotionPawn::LOCOMOTION_VELOCITY, SkChannel::XPos ); //  4
		request_channels.add( MeCtLocomotionPawn::LOCOMOTION_VELOCITY, SkChannel::YPos ); //  5
		request_channels.add(  MeCtLocomotionPawn::LOCOMOTION_VELOCITY , SkChannel::ZPos ); //  6

	}

	return request_channels;
}

// Implements MeController::context_updated(..)
void MeCtLocomotionSimple::context_updated() {
	if( _context == NULL ) {
		is_valid = false;
		last_time = std::numeric_limits<double>::quiet_NaN();
	}
}

// Look up the context indices, and check to make sure it isn't -1
#define LOOKUP_BUFFER_INDEX( var_name, ct_index ) \
	var_name = _context->toBufferIndex( _toContextCh[ ( ct_index ) ] );  \
	is_valid &= ( var_name != -1 );

void MeCtLocomotionSimple::controller_map_updated() {
	is_valid = true;

	if( _context != NULL ) {
		// request_channel indices (second param) come from the order of request_channels.add(..) calls in controller_channels()
		LOOKUP_BUFFER_INDEX( bi_world_x,    0 );
		LOOKUP_BUFFER_INDEX( bi_world_y,    1 );
		LOOKUP_BUFFER_INDEX( bi_world_z,    2 );
		LOOKUP_BUFFER_INDEX( bi_world_rot,  3 );

		LOOKUP_BUFFER_INDEX( bi_loco_vel_x, 4 );
		LOOKUP_BUFFER_INDEX( bi_loco_vel_y, 5 );
		LOOKUP_BUFFER_INDEX( bi_loco_vel_z, 6 );

	} else {
		// This shouldn't get here
		is_valid = false;
	}
}


bool MeCtLocomotionSimple::controller_evaluate( double time, MeFrameData& frame ) {
	float time_delta = 0.033f;
	if( last_time == last_time ) {  // false if NaN
		time_delta = (float)( time - last_time );
	}
	last_time = time;

	const gwiz::vector3_t UP_VECTOR( 0, 1, 0 );

	if( is_valid ) {
		SrBuffer<float>& buffer = frame.buffer(); // convenience reference

		// Read inputs
		gwiz::vector3_t world_pos( buffer[ bi_world_x ], buffer[ bi_world_y ], buffer[ bi_world_z ] );
		gwiz::quat_t    world_rot( buffer[ bi_world_rot ], buffer[ bi_world_rot+1 ], buffer[ bi_world_rot+2 ], buffer[ bi_world_rot+3 ] );

		gwiz::vector3_t loco_vel( buffer[ bi_loco_vel_x ], buffer[ bi_loco_vel_y ], buffer[ bi_loco_vel_z ] );
		gwiz::euler_t   loco_drot( 0, DEG( buffer[ bi_loco_rot_y ] ), 0 );

		// Position Calc
		world_pos += ( loco_vel * time_delta );

		// Rotation Calc
		loco_drot *= time_delta;
		world_rot = gwiz::quat_t( loco_drot ) * world_rot;

		// Write Results
		buffer[ bi_world_x ] = (float)world_pos.x();
		buffer[ bi_world_y ] = (float)world_pos.y();
		buffer[ bi_world_z ] = (float)world_pos.z();

		buffer[ bi_world_rot+0 ] = (float)world_rot.w();
		buffer[ bi_world_rot+1 ] = (float)world_rot.x();
		buffer[ bi_world_rot+2 ] = (float)world_rot.y();
		buffer[ bi_world_rot+3 ] = (float)world_rot.z();
	}

	return is_valid;
}
