/*
 *  me_ct_step_turn.cpp - part of SmartBody-lib
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

#include "sbm/sbm_pawn.hpp"
#include "controllers/me_ct_step_turn.h"
using namespace gwiz;

#include <sb/SBMotion.h>
#include <vhcl_log.h>

//////////////////////////////////////////////////////////////////////////////////

std::string MeCtStepTurn::type_name = "StepTurn";

MeCtStepTurn::MeCtStepTurn( void )	{

   _left_motion = NULL;
   _right_motion = NULL;
   _motion = NULL;

   _play_mode = SkMotion::Linear;
   _duration = -1.0;
   _last_apply_frame = 0;
   
   skeleton_ref_p = NULL;
   interim_pose_buff_p = NULL;
   
   timing_mode = TASK_TIME;
   heading_mode = HEADING_LOCAL;
   dirty_action_bit = 0;
}

MeCtStepTurn::~MeCtStepTurn( void )	{

	if( interim_pose_buff_p )	{
		delete [] interim_pose_buff_p;
		interim_pose_buff_p = NULL;
	}
}

void MeCtStepTurn::init( SbmPawn* pawn, SkMotion* mot_p ) {

	if( _motion ) {
		if( mot_p == _motion ) {
			// Minimal init()
			_last_apply_frame = 0;
			MeController::init (pawn);
			return;
		}
		// else new motion
	}
	_last_apply_frame = 0;
	
	raw_time = mot_p->duration();
	raw_angle = calc_raw_turn_angle( mot_p, (char*)"base" );

	SkMotion* mirr_p = build_mirror_motion( mot_p );
	if( raw_angle > 0.0 )	{
		_left_motion = mot_p;
		_right_motion = mirr_p;
	}
	else	{
		_left_motion = mirr_p;
		_right_motion = mot_p;
	}

	_left_motion->move_keytimes( 0 ); // make sure motion starts at 0
	_right_motion->move_keytimes( 0 ); 
	_motion = _left_motion;

//	set_time( raw_time );
	set_duration( raw_time );
	set_heading_local( raw_angle );
	
	SkChannelArray& mchan_arr = mot_p->channels();

	if( interim_pose_buff_p )	{
		delete [] interim_pose_buff_p;
	}
	interim_pose_buff_p = new float[ mchan_arr.count_floats() ];
	
#if 0
// NOTE: creates erroneous playback:
	_channels.add( SkJointName( "world_offset" ), SkChannel::XPos );
	_channels.add( SkJointName( "world_offset" ), SkChannel::YPos );
	_channels.add( SkJointName( "world_offset" ), SkChannel::ZPos );
	_channels.add( SkJointName( "world_offset" ), SkChannel::Quat );
#endif

	int size = mchan_arr.size();
	for( int i = 0; i < size; i++ )	{
		_channels.add( mchan_arr.name(i), mchan_arr.type(i) );
	}

#if 1
// NOTE: creates correct playback:
	_channels.add( "world_offset" , SkChannel::XPos );
	_channels.add( "world_offset" , SkChannel::YPos );
	_channels.add( "world_offset" , SkChannel::ZPos );
	_channels.add( "world_offset" , SkChannel::Quat );
#endif
	
	MeController::init(pawn);

	if( _context ) {
		// Notify _context of channel change.
		_context->child_channels_updated( this );
	}
}

///////////////////////////////////////////////////////////////////////////////////

#define NUM_STEPTURN_JOINTS 			11
#define MAX_STEPTURN_JOINT_LABEL_LEN	32

/*
	Build mirrored motion:
		flip X position
		flip Y, Z rotation
*/

SkMotion* MeCtStepTurn::build_mirror_motion( SkMotion* ref_motion_p )	{
	static char ref_labels[ NUM_STEPTURN_JOINTS ][ MAX_STEPTURN_JOINT_LABEL_LEN ] = {
		"base",
		"l_hip", "l_knee", "l_ankle", "l_forefoot", "l_toe",
		"r_hip", "r_knee", "r_ankle", "r_forefoot", "r_toe"
	};
	static char new_labels[ NUM_STEPTURN_JOINTS ][ MAX_STEPTURN_JOINT_LABEL_LEN ] = {
		"base",
		"r_hip", "r_knee", "r_ankle", "r_forefoot", "r_toe",
		"l_hip", "l_knee", "l_ankle", "l_forefoot", "l_toe"
	};
	int i, j;
	
	SkChannelArray& mchan_arr = ref_motion_p->channels();
	SkMotion *mirror_p = new SmartBody::SBMotion();
	mirror_p->init( mchan_arr );

	int num_f = ref_motion_p->frames();
	for( i=0; i<num_f; i++ )	{
		
		mirror_p->insert_frame( i, ref_motion_p->keytime( i ) );
		float *ref_p = ref_motion_p->posture( i );
		float *new_p = mirror_p->posture( i );
		int ref_i, new_i;

		ref_i = mchan_arr.float_position( mchan_arr.search("base" , SkChannel::XPos ) );
		new_p[ ref_i ] = -ref_p[ ref_i ];
		ref_i = mchan_arr.float_position( mchan_arr.search("base" , SkChannel::YPos ) );
		new_p[ ref_i ] = ref_p[ ref_i ];
		ref_i = mchan_arr.float_position( mchan_arr.search("base" , SkChannel::ZPos ) );
		new_p[ ref_i ] = ref_p[ ref_i ];

		for( j=0; j<NUM_STEPTURN_JOINTS; j++ )	{
			ref_i = mchan_arr.float_position( mchan_arr.search( ref_labels[ j ] , SkChannel::Quat ) );
			new_i = mchan_arr.float_position( mchan_arr.search( new_labels[ j ] , SkChannel::Quat ) );
			euler_t ref_e = quat_t( ref_p[ ref_i ], ref_p[ ref_i + 1 ], ref_p[ ref_i + 2 ], ref_p[ ref_i + 3 ] );
			quat_t new_q = euler_t( ref_e.x(), -ref_e.y(), -ref_e.z() );
			new_p[ new_i + 0 ] = (float)new_q.w();
			new_p[ new_i + 1 ] = (float)new_q.x();
			new_p[ new_i + 2 ] = (float)new_q.y();
			new_p[ new_i + 3 ] = (float)new_q.z();
		}
	}
	
	return( mirror_p );
}

float MeCtStepTurn::calc_raw_turn_angle( SkMotion* mot_p, char *joint_name )	{
	
	if( mot_p && joint_name ) {
	
		float * first_p = mot_p->posture( 0 );
		float * final_p = mot_p->posture( mot_p->frames() - 1 );

		SkChannelArray& mchan_arr = mot_p->channels();
		int i = mchan_arr.float_position( mchan_arr.search( joint_name, SkChannel::Quat ) );

		quat_t first_q( first_p[ i ], first_p[ i + 1 ], first_p[ i + 2 ], first_p[ i + 3 ] );
		quat_t final_q( final_p[ i ], final_p[ i + 1 ], final_p[ i + 2 ], final_p[ i + 3 ] );

		euler_t e = -first_q * final_q;
		return( (float)e.y() );
	}
	return( 0.0 );
}

void MeCtStepTurn::capture_world_offset_state( void )	{
	
	if( _context )	{
		if( _context->channels().size() > 0 )	{

			if( skeleton_ref_p == NULL )	{
				skeleton_ref_p = _context->channels().skeleton(); // WHY HERE?
			}
			if( skeleton_ref_p )	{
				
				SkJoint* joint_p = skeleton_ref_p->search_joint( SbmPawn::WORLD_OFFSET_JOINT_NAME );
				if( joint_p )	{
					joint_p->update_gmat_up();

					SrMat sr_M;
					gwiz::matrix_t M;
					int i, j;

					sr_M = joint_p->gmat();
					for( i=0; i<4; i++ )	{
						for( j=0; j<4; j++ )	{
							M.set( i, j, sr_M.get( i, j ) );
						}
					}
					
					world_offset_pos = M.translation( gwiz::COMP_M_TR );
					world_offset_rot = M.quat( gwiz::COMP_M_TR );
					return;
				}
				LOG( "MeCtStepTurn::capture_world_offset_state ERR: '%s' joint is NULL\n", SbmPawn::WORLD_OFFSET_JOINT_NAME );
				return;
			}
			LOG( "MeCtStepTurn::capture_world_offset_state ERR: skeleton reference is still NULL\n" );
			return;
		}
		LOG( "MeCtStepTurn::capture_world_offset_state ERR: context channels have no size\n" );
		return;
	}
	LOG( "MeCtStepTurn::capture_world_offset_state ERR: context is NULL\n" );
}

void MeCtStepTurn::update_action_params( void )	{
	
	if( heading_mode == HEADING_WORLD )	{
		euler_t w_e = world_offset_rot;
		euler_t l_e = euler_t( 0.0, world_turn_angle, 0.0 ) * -euler_t( 0.0, w_e.y(), 0.0 );
		turn_angle = (float)l_e.y();
	}
	
	if( turn_angle < 0.0 )	{
		_motion = _right_motion;
	}
	else	{
		_motion = _left_motion;
	}

	if( timing_mode == TASK_SPEED )	{
		turn_time = fabs( raw_angle / turn_speed );
	}

	turn_angle_scale = fabs( turn_angle / raw_angle );
	turn_time_scale = raw_time / turn_time;

	_duration = turn_time;
	dirty_action_bit = 0;
}

void MeCtStepTurn::set_time( float sec )	{
	
	static int once = 1;
	if( once )	{
		once = 0;
		LOG( "MeCtStepTurn::set_time NOTE: CALL DEPRECATED: use set_duration()\n" );
	}
	set_duration( sec );
}

void MeCtStepTurn::set_duration( float sec )	{
	
	timing_mode = TASK_TIME;
	turn_time = sec;
	dirty_action_bit = 1;
}

void MeCtStepTurn::set_speed( float dps ) {

	timing_mode = TASK_SPEED;
	turn_speed = dps;
	dirty_action_bit = 1;
}

void MeCtStepTurn::set_heading_local( float h )	{

	heading_mode = HEADING_LOCAL;
	turn_angle = h;
	dirty_action_bit = 1;
}

void MeCtStepTurn::set_heading_world( float h )	{
	
	heading_mode = HEADING_WORLD;
	world_turn_angle = h;
	dirty_action_bit = 1;
}

///////////////////////////////////////////////////////////////////////////////////

void MeCtStepTurn::context_updated( void ) {

#if 0
	if( _context ) {
		skeleton_ref_p = _context->channels().skeleton(); // WHY HERE?
		if( skeleton_ref_p == NULL )	{
			LOG( "MeCtStepTurn::context_updated ERR: skeleton_ref_p is NULL\n" );
		}
	}
	else	{
		LOG( "MeCtStepTurn::context_updated ERR: context is NULL\n" );
	}
#endif
}
	
void MeCtStepTurn::controller_map_updated( void ) {

	// Map motion channel index to context float buffer index
	SkChannelArray& mChannels = _motion->channels();
	const int size = mChannels.size();

	_mChan_to_buff.size( size );

	if( _context ) {

		SkChannelArray& cChannels = _context->channels();
		for( int i=0; i<size; ++i ) {
			int chanIndex = cChannels.search( mChannels.name(i), mChannels.type(i) );
			_mChan_to_buff[ i ] = _context->toBufferIndex( chanIndex );
		}

		world_offset_chan.x = cChannels.search( "world_offset" , SkChannel::XPos );
		world_offset_chan.y = cChannels.search( "world_offset" , SkChannel::YPos );
		world_offset_chan.z = cChannels.search( "world_offset" , SkChannel::ZPos );
		world_offset_chan.q = cChannels.search( "world_offset" , SkChannel::Quat );

		world_offset_idx.x = _context->toBufferIndex( world_offset_chan.x );
		world_offset_idx.y = _context->toBufferIndex( world_offset_chan.y );
		world_offset_idx.z = _context->toBufferIndex( world_offset_chan.z );
		world_offset_idx.q = _context->toBufferIndex( world_offset_chan.q );

		base_joint_chan.x = cChannels.search( "base" , SkChannel::XPos );
		base_joint_chan.y = cChannels.search( "base" , SkChannel::YPos );
		base_joint_chan.z = cChannels.search( "base" , SkChannel::ZPos );
		base_joint_chan.q = cChannels.search( "base" , SkChannel::Quat );

		base_joint_idx.x = _context->toBufferIndex( base_joint_chan.x );
		base_joint_idx.y = _context->toBufferIndex( base_joint_chan.y );
		base_joint_idx.z = _context->toBufferIndex( base_joint_chan.z );
		base_joint_idx.q = _context->toBufferIndex( base_joint_chan.q );
#if 0
		LOG( "world_offset chan: %d index: %d\n", world_offset_chan.x, world_offset_idx.x );
		LOG( "base_joint   chan: %d index: %d\n", base_joint_chan.x, base_joint_idx.x );
#endif
	} 
	else {
		_mChan_to_buff.setall( -1 );
	}
}

void MeCtStepTurn::controller_start( void )	{

	capture_world_offset_state();
	update_action_params();
}

bool MeCtStepTurn::controller_evaluate( double t, MeFrameData& frame ) {

	bool continuing = true;
	continuing = t < _duration;

	if( t < 0.0 )	{
		return( continuing );
	}

#if 0 // RAW STEPPING
	_motion->apply( 
		float( t * turn_time_scale ),
		&( frame.buffer()[0] ),  // pointer to buffer's float array
		&_mChan_to_buff,
		_play_mode, 
		&_last_apply_frame 
	);

#else // SCALED STEPPING

	_motion->apply( 
		float( t * turn_time_scale ),
		interim_pose_buff_p,
		NULL, // same order in interim_buffer
		_play_mode, 
		&_last_apply_frame 
	);

	float *fbuffer = &( frame.buffer()[0] );
	SkChannelArray& mChannels = _motion->channels();
	int nchan = mChannels.size();

	vector_t world_offset_delta_pos;
	quat_t world_offset_delta_rot;
	
	int index = 0;
	for( int i=0; i<nchan; i++ )	{

		int ch_size = mChannels[ i ].size();
		SkChannel::Type ch_type = mChannels[ i ].type;
		
		float tmp_f[ 4 ];
		for( int j=0; j<ch_size; j++ ) {
			tmp_f[ j ] = interim_pose_buff_p[ index + j ];
		}

		if( 
			( ch_type == SkChannel::XPos ) ||
			( ch_type == SkChannel::YPos ) ||
			( ch_type == SkChannel::ZPos )
			)	{

			// take difference between first and current frame, then scale
			// TODO: the float-pos index of the first frame posture for each channel should be pre-stored

			float * first_p = _motion->posture( 0 );
			int f_pos = mChannels.float_position( mChannels.search( mChannels.name( i ), ch_type ) );

			tmp_f[ 0 ] = first_p[ f_pos ] + ( tmp_f[ 0 ] - first_p[ f_pos ] )* turn_angle_scale - first_p[ f_pos ];
		}

		if( ch_type == SkChannel::Quat )	{
		
			quat_t curr_q = quat_t( tmp_f[ 0 ], tmp_f[ 1 ], tmp_f[ 2 ], tmp_f[ 3 ] );

			// take difference between first and current frame, then scale
			// TODO: the float-pos index of the first frame posture for each channel should be pre-stored
			
			float * first_p = _motion->posture( 0 );
			int f_pos = mChannels.float_position( mChannels.search( mChannels.name( i ), SkChannel::Quat ) );
			quat_t first_q( 
				first_p[ f_pos ], 
				first_p[ f_pos + 1 ], 
				first_p[ f_pos + 2 ], 
				first_p[ f_pos + 3 ] 
			);

			quat_t q = ( ( curr_q * -first_q ) * turn_angle_scale ) * first_q;
			tmp_f[ 0 ] = (float)q.w();
			tmp_f[ 1 ] = (float)q.x();
			tmp_f[ 2 ] = (float)q.y();
			tmp_f[ 3 ] = (float)q.z();
		}
		
		if( i == base_joint_chan.x )	{
			world_offset_delta_pos.x( tmp_f[ 0 ] );
			tmp_f[ 0 ] = 0.0;
		}
		else
		if( i == base_joint_chan.y )	{
			world_offset_delta_pos.y( tmp_f[ 0 ] );
			tmp_f[ 0 ] = 0.0;
		}
		else
		if( i == base_joint_chan.z )	{
			world_offset_delta_pos.z( tmp_f[ 0 ] );
			tmp_f[ 0 ] = 0.0;
		}
		else
		if( i == base_joint_chan.q )	{
			world_offset_delta_rot = quat_t( tmp_f[ 0 ], tmp_f[ 1 ], tmp_f[ 2], tmp_f[ 3 ] );
			tmp_f[ 0 ] = 1.0;
			tmp_f[ 1 ] = 0.0;
			tmp_f[ 2 ] = 0.0;
			tmp_f[ 3 ] = 0.0;
		}

		for( int j=0; j<ch_size; j++ ) {
			fbuffer[ _mChan_to_buff[ i ] + j ] = tmp_f[ j ];
		}
		
		index += ch_size;
	}
	
	vector_t tmp_v = world_offset_pos + world_offset_delta_pos;
	quat_t tmp_q = world_offset_rot * world_offset_delta_rot;

	fbuffer[ world_offset_idx.x ] = (float)tmp_v.x();
	fbuffer[ world_offset_idx.y ] = (float)tmp_v.y();
	fbuffer[ world_offset_idx.z ] = (float)tmp_v.z();
	fbuffer[ world_offset_idx.q + 0 ] = (float)tmp_q.w();
	fbuffer[ world_offset_idx.q + 1 ] = (float)tmp_q.x();
	fbuffer[ world_offset_idx.q + 2 ] = (float)tmp_q.y();
	fbuffer[ world_offset_idx.q + 3 ] = (float)tmp_q.z();

#endif // SCALED STEPPING

	return continuing;
}

SkChannelArray& MeCtStepTurn::controller_channels( void )	{
	return _channels;
}

double MeCtStepTurn::controller_duration( void ) {

// THIS IS CALLED PRIOR TO controller_start().
#if 0
	if( dirty_action_bit )	{
		capture_world_offset_state(); // skeleton not available
		update_action_params();
	}
	return _duration;
#else
	return( -1.0 );
#endif
}

const std::string& MeCtStepTurn::controller_type( void ) const	{
	return type_name;
}

void MeCtStepTurn::print_state( int tabCount ) {

	LOG("MeCtStepTurn" );

	const char* str = getName().c_str();
	if( str )
		LOG(" \"%s\"", str );

	LOG(", motion" );
	if( _motion ) {

		// motion name
		str = _motion->getName().c_str();
		if( str )
			LOG(" \"%s\"", str );

		// motion filename
		str = _motion->filename().c_str();
		if( str )
			LOG(" file \"%s\"", str );
	} 
	else {
		LOG("=NULL" );
	}
	LOG("\n" );
}

//////////////////////////////////////////////////////////////////////////////////

