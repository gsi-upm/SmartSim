/*
 *  me_ct_tether.cpp - part of SmartBody-lib
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
 *      Marcus Thiebaux, USC
 *      Andrew n marshall, USC
 */

#include "sbm/lin_win.h"
#include "sbm/gwiz_math.h"
using namespace gwiz;

#include "controllers/me_ct_tether.h"
#include <vhcl_log.h>

//////////////////////////////////////////////////////////////////////////////////

std::string MeCtTether::type_name = "Tether";

MeCtTether::MeCtTether( void )	{

   _duration = -1.0;
   _skeleton_ref_p = NULL;
}

MeCtTether::~MeCtTether( void )	{
	
	clear();
}

void MeCtTether::clear( void )	{
	
   _duration = -1.0;
   _skeleton_ref_p = NULL;

	if( source_ref_joint_str ) {
		free( source_ref_joint_str );
		source_ref_joint_str = NULL;
	}
}

void MeCtTether::init(SbmPawn* pawn,  char *channel_name) {
	
	clear();

	_channels.add( channel_name, SkChannel::XPos );
	_channels.add( channel_name, SkChannel::YPos );
	_channels.add( channel_name, SkChannel::ZPos );
	_channels.add( channel_name, SkChannel::Quat );
	
	MeController::init(pawn);
}

///////////////////////////////////////////////////////////////////////////////////

void MeCtTether::set_source_joint( char *joint_name )	{
	
	if( source_ref_joint_str ) free( source_ref_joint_str );
	source_ref_joint_str = _strdup( joint_name );
	source_ref_joint_p = NULL;
}

void MeCtTether::set_source_joint( SkJoint* joint_p )	{

	if( source_ref_joint_str ) free( source_ref_joint_str );
	source_ref_joint_str = NULL;
	source_ref_joint_p = joint_p;
}

void MeCtTether::set_offset_pos( float x, float y, float z ) { 
	offset_pos = vector_t( x, y, z ); 
}

void MeCtTether::set_offset_euler( float p, float h, float r ) { 
	offset_rot = euler_t( p, h, r ); 
}

void MeCtTether::set_offset_swing( float sw_p, float sw_h, float tw ) {
	offset_rot = quat_t( sw_p, sw_h, tw );
}

void MeCtTether::set_offset_polar( float d, float a, float r ) {

	vector_t direction = 
		euler_t( 0.0, 0.0, d ) * 
		euler_t( -a, 0.0, 0.0 ) * 
		vector_t( 0.0, 0.0, 1.0 ); // NOTE: flip pitch
	offset_rot = euler_t( direction, r ); // lookat constructor
}

///////////////////////////////////////////////////////////////////////////////////

SkJoint* MeCtTether::get_joint( char *joint_str, SkJoint *joint_p )	{

	if( joint_str )	{
		if( joint_p == NULL )	{
			if( _skeleton_ref_p )	{
				joint_p = _skeleton_ref_p->search_joint( joint_str );
				if( joint_p == NULL )	{
					fprintf( stderr, "MeCtTether::get_joint ERR: joint '%s' NOT FOUND in skeleton\n", joint_str );
					free( joint_str );
					joint_str = NULL;
				}
			}
			else	{
				fprintf( stderr, "MeCtTether::get_joint ERR: skeleton NOT FOUND\n" );
			}
		}
	}
	return( joint_p );
}

SkJoint* MeCtTether::source_ref_joint( void ) {

	return( source_ref_joint_p = get_joint( source_ref_joint_str, source_ref_joint_p ) );
}

MeCtTether::joint_state_t MeCtTether::capture_joint_state( SkJoint *joint_p ) {
	SrMat sr_M;
	gwiz::matrix_t M;
	int i, j;
	joint_state_t state;

	if( joint_p )	{

		sr_M = joint_p->lmat();
		for( i=0; i<4; i++ )	{
			for( j=0; j<4; j++ )	{
				M.set( i, j, sr_M.get( i, j ) );
			}
		}
		state.local_pos = M.translation( gwiz::COMP_M_TR );
		state.local_rot = M.quat( gwiz::COMP_M_TR );

		sr_M = joint_p->gmat();
		for( i=0; i<4; i++ )	{
			for( j=0; j<4; j++ )	{
				M.set( i, j, sr_M.get( i, j ) );
			}
		}
		state.world_pos = M.translation( gwiz::COMP_M_TR );
		state.world_rot = M.quat( gwiz::COMP_M_TR );
	}
	return( state );
}

MeCtTether::joint_state_t MeCtTether::calc_channel_state( MeCtTether::joint_state_t source )	{
	joint_state_t state;

	state.world_pos = source.world_pos + source.world_rot * offset_pos;
	state.world_rot = source.world_rot * offset_rot;
	
	// if channel has a skeletal parent: subtract parent from world
	//   state.local_pos = 
	//   state.local_rot = 
	// else assume same
	state.local_pos = state.world_pos;
	state.local_rot = state.world_rot;

	return( state );
}

///////////////////////////////////////////////////////////////////////////////////

void MeCtTether::context_updated( void ) {
}

void MeCtTether::controller_map_updated( void ) {
}

void MeCtTether::controller_start( void )	{

	if( _context->channels().size() > 0 )	{
		_skeleton_ref_p = _context->channels().skeleton();
	}
}

bool MeCtTether::controller_evaluate( double t, MeFrameData& frame ) {

	bool continuing = true;
	continuing = t < _duration;
	if( t < 0.0 )	{
		return( continuing );
	}

	float *fbuffer = &( frame.buffer()[0] );
	int n_chan = _channels.size();
// _channels.name( i )
// _channels.type( i )

	SkJoint* joint_p = source_ref_joint();
	joint_state_t state_in = capture_joint_state( joint_p );
	joint_state_t state_out = calc_channel_state( state_in );
	
	for( int i=0; i<n_chan; i++ ) {

		int index = frame.toBufferIndex( _toContextCh[ i ] );

		if( _channels.type( i ) == SkChannel::XPos )	{
			fbuffer[ index ] = (float)state_out.local_pos.x();
		}
		else
		if( _channels.type( i ) == SkChannel::YPos )	{
			fbuffer[ index ] = (float)state_out.local_pos.y();
		}
		else
		if( _channels.type( i ) == SkChannel::ZPos )	{
			fbuffer[ index ] = (float)state_out.local_pos.z();
		}
		else
		if( _channels.type( i ) == SkChannel::Quat )	{
			fbuffer[ index + 0 ] = (float) state_out.local_rot.w();
			fbuffer[ index + 1 ] = (float) state_out.local_rot.x();
			fbuffer[ index + 2 ] = (float) state_out.local_rot.y();
			fbuffer[ index + 3 ] = (float) state_out.local_rot.z();
		}

		// Mark channel changed
		frame.channelUpdated( i );
	}

	return continuing;
}

SkChannelArray& MeCtTether::controller_channels( void )	{
	return( _channels );
}

double MeCtTether::controller_duration( void ) {
	return( _duration );
}

const std::string& MeCtTether::controller_type( void )	const {
	return( type_name );
}


