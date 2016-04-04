/*
 *  me_ct_navigation_waypoint.cpp - part of SmartBody-lib's Motion Engine
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

#include <controllers/me_ct_navigation_waypoint.hpp>
#include <sbm/sbm_character.hpp>



std::string MeCtNavigationWaypoint::TYPE = "MeCtNavigationWaypoint";


MeCtNavigationWaypoint::MeCtNavigationWaypoint()
:	is_valid( false ),
	last_time( std::numeric_limits<float>::quiet_NaN() )
{}

const std::string& MeCtNavigationWaypoint::controller_type() {
	return TYPE;
}

// Implements MeController::context_updated(..)
void MeCtNavigationWaypoint::context_updated() {
	if( _context == NULL ) {
		is_valid = false;
		last_time = std::numeric_limits<double>::quiet_NaN();
	}
}

// Implements MeController::controller_channels().
SkChannelArray& MeCtNavigationWaypoint::controller_channels() {
	if( request_channels.size() == 0 ) {
		// Initialize Requested Channels                                                           // Indices
		request_channels.add(SbmPawn::WORLD_OFFSET_JOINT_NAME , SkChannel::XPos );  // #0
		request_channels.add(SbmPawn::WORLD_OFFSET_JOINT_NAME, SkChannel::YPos );  //  1
		request_channels.add(SbmPawn::WORLD_OFFSET_JOINT_NAME , SkChannel::ZPos );  //  2
		request_channels.add(SbmPawn::WORLD_OFFSET_JOINT_NAME , SkChannel::Quat );  //  3

		request_channels.add( MeCtLocomotionPawn::LOCOMOTION_VELOCITY , SkChannel::XPos ); //  4
		request_channels.add( MeCtLocomotionPawn::LOCOMOTION_VELOCITY , SkChannel::YPos ); //  5
		request_channels.add( MeCtLocomotionPawn::LOCOMOTION_VELOCITY , SkChannel::ZPos ); //  6

		request_channels.add( MeCtLocomotionPawn::LOCOMOTION_GLOBAL_ROTATION , SkChannel::YPos ); //  7
		request_channels.add( MeCtLocomotionPawn::LOCOMOTION_LOCAL_ROTATION, SkChannel::YPos ); //  8
		request_channels.add( MeCtLocomotionPawn::LOCOMOTION_LOCAL_ROTATION_ANGLE , SkChannel::YPos ); //  9
		request_channels.add( MeCtLocomotionPawn::LOCOMOTION_TIME , SkChannel::YPos ); //  9
		request_channels.add( MeCtLocomotionPawn::LOCOMOTION_ID , SkChannel::YPos ); //  10

		//request_channels.add( SkJointName( SbmCharacter::LOCOMOTION_HAS_DESTINATION ), SkChannel::YPos ); //  10

		//request_channels.add( SkJointName( SbmCharacter::LOCOMOTION_DESTINATION ), SkChannel::XPos ); //  11
		//request_channels.add( SkJointName( SbmCharacter::LOCOMOTION_DESTINATION ), SkChannel::YPos ); //  12
		//request_channels.add( SkJointName( SbmCharacter::LOCOMOTION_DESTINATION ), SkChannel::ZPos ); //  13
	}

	return request_channels;
}

bool MeCtNavigationWaypoint::controller_evaluate( double time, MeFrameData& frame ) {
	if( is_valid ) {
		float time_delta = 0.033f;
		if( last_time == last_time ) {  // false if NaN
			time_delta = (float)( time - last_time );
		}
		last_time = time;
	}

	return is_valid;
}

void MeCtNavigationWaypoint::print_state( int tab_count ) {
	// TODO
	MeController::print_state( tab_count );
}

double MeCtNavigationWaypoint::controller_duration()
{
	return -1;
}