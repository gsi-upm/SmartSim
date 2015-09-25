/*
 *  me_ct_simple_gaze.h - part of SmartBody-lib
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
 */

#ifndef ME_CT_SIMPLE_GAZE_H
#define ME_CT_SIMPLE_GAZE_H

#include <vhcl.h>
#include <sk/sk_skeleton.h>
#include <sb/SBController.h>

#include "sbm/gwiz_math.h"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

///////////////////////////////////////////////////////////////////////////

class MeCtSimpleGazeJoint	{

	public:
		MeCtSimpleGazeJoint();
		virtual ~MeCtSimpleGazeJoint() {}

		float limit;
		float weight;
		float speed;
		float duration;
		float smooth;
		
		void init( SkJoint* j_p );
		void begin( void );
		
		gwiz::vector_t	forward_pos;
		gwiz::vector_t	forward_ref; // default forward direction
		gwiz::quat_t		forward_rot;
		
		gwiz::vector_t	local_pos;
		gwiz::quat_t		local_rot;
		gwiz::quat_t		prev_local_rot;
		
		gwiz::vector_t	world_pos;
		gwiz::quat_t		world_rot;
		
		gwiz::vector_t	parent_pos; // world coord of immediate parent joint
		gwiz::quat_t		parent_rot;

		gwiz::quat_t	evaluate( float dt, gwiz::vector_t target_pos, gwiz::quat_t off_rot, float scale_factor = 1.0 );
		gwiz::quat_t	evaluate( float dt, gwiz::quat_t target_rot, gwiz::quat_t off_rot, float scale_factor = 1.0 );
		
	private:
		void	capture_joint_state( void );
		gwiz::quat_t	rotation_to_target( gwiz::vector_t target_pos );
		gwiz::quat_t	rotation_to_target( gwiz::quat_t target_rot );
		
		gwiz::quat_t	constrain_ellipse( gwiz::quat_t task_rot );
		gwiz::quat_t	constrain_quat( gwiz::quat_t task_rot );
		gwiz::quat_t	constrain_box( gwiz::quat_t task_rot );
		
		gwiz::quat_t	constrain_quat_speed( float dt, gwiz::quat_t task_rot );
		gwiz::quat_t	constrain_smooth( float dt, gwiz::quat_t task_rot );

		gwiz::quat_t	constrain( float dt, gwiz::quat_t task_rot );

		SkJoint* joint_p;
};

class MeCtSimpleGaze : public SmartBody::SBController	{
	
	public:
		static std::string type_name;

		MeCtSimpleGaze();
		virtual ~MeCtSimpleGaze();
		
		void init( SbmPawn* pawn );

		void set_target_joint( float x, float y, float z, SkJoint* ref_joint_p = NULL );
		void set_target( float x, float y, float z, char *ref_joint_name = NULL ); // world-coord if NULL
		void set_orient_joint( float p, float h, float r, SkJoint* ref_joint_p = NULL );
		void set_orient( float p, float h, float r, char *ref_joint_name = NULL );

		void set_offset_swing( float off_p, float off_h, float off_r ); // swing-twist: pitch, heading, roll
		void set_offset_polar( float off_d, float off_a, float off_r ); // direction, radial angle, roll

		void set_speed( float back_dps, float neck_dps, float eyes_dps );
		void set_smooth( float back_sm, float neck_sm, float eyes_sm );

	private:

		enum target_mode_enum_set	{
			TARGET_POINT,
			TARGET_ORIENT
		};

		double	prev_time;
		int 	start;	// to initialize prev_time, dt

		int 	target_mode;
		int 	flexible_joint;
		int 	priority_joint;

		float 			_duration;
		SkChannelArray	_channels;
		SkSkeleton* 	skeleton_ref_p;

		char*			ref_joint_str;
		SkJoint* 		ref_joint_p;
		gwiz::vector_t 		point_target_pos;
		gwiz::quat_t  		orient_target_rot;
		gwiz::quat_t  		offset_rot;

		int 			joint_count;   
		MeCtSimpleGazeJoint*	joint_arr;

		void		inspect_skeleton( SkJoint* joint_p, int depth = 0 );
		void		inspect_skeleton_local_transform( SkJoint* joint_p, int depth = 0 );
		void		inspect_skeleton_world_transform( SkJoint* joint_p, int depth = 0 );

		void		update_skeleton_gmat( void );
		SkJoint*	reference_joint( void );
		gwiz::vector_t	world_target_point( void );
		gwiz::quat_t		world_target_orient( void );
		
		virtual void controller_start();
		virtual bool controller_evaluate( double t, MeFrameData& frame );
		virtual SkChannelArray& controller_channels()	{ return( _channels ); }
		virtual double controller_duration()			{ return( (double)_duration ); }
		virtual const std::string& controller_type() const		{ return( type_name ); }
};

///////////////////////////////////////////////////////////////////////////
#endif
