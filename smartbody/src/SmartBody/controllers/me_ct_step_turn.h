/*
 *  me_ct_step_turn.h - part of SmartBody-lib
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

# ifndef ME_CT_STEP_TURN_H
# define ME_CT_STEP_TURN_H

//////////////////////////////////////////////////////////////////////////////////

#include <sr/sr_hash_table.h>
#include <sr/sr_buffer.h>
#include <sk/sk_motion.h>
#include <sk/sk_skeleton.h>
#include <sb/SBController.h>

#include "sbm/gwiz_math.h"

/*
SBM commands:

        ctrl <> stepturn <step-motion-name>
        stepturn <> dur|time|speed <sec|dps> local|world <heading-deg>

 * The input stepping motion can be either direction, any reasonable magnitude.
 * Tokens 'dur' and 'time' are synonymous.

Class API members:

        void init( SkMotion* mot_p );
        void set_time( float sec );
        void set_speed( float dps );
        void set_heading_local( float h );
        void set_heading_world( float h );

Step motion:

        smartbody/testdata/doctor/step/StepTurn_Right.skm

Example sequence:

        0.0 load motions ../../testdata/doctor/step
        0.0 ctrl ST stepturn StepTurn_Right
        0.0 stepturn ST dur 0.9 local 45.0
        0.0 char doctor ctrl ST begin

 * This will induce the doctor to step 45 degrees to his left.

Testing sequence:

        seq sbm-stepturn-init

 * This runs the doctor through a few hoops.
*/

class MeCtStepTurn : public SmartBody::SBController	{ 

	private:
		enum timing_mode_enum_set	{
			TASK_SPEED,
			TASK_TIME
		};
		enum coord_coord_enum_set	{
			HEADING_LOCAL,
			HEADING_WORLD
		};

		struct joint_param_index_t {
			int x, y, z, q;
		};

		SkMotion*            _left_motion;
		SkMotion*            _right_motion;
		SkMotion*            _motion;    // the current motion, based on turn heading
		SkMotion::InterpType _play_mode; // its play mode
		double               _duration;  // the time-warped duration
		//    bool                 _loop;      // if the motion is to be played in loop
		int                  _last_apply_frame; // to optimize shared motion evaluation
		SrBuffer<int>        _mChan_to_buff; // motion's channels to context's buffer index
		SkChannelArray		_channels; // override motion channels, to include world_offset

		SkSkeleton* 	skeleton_ref_p;
		float * interim_pose_buff_p;
		
		gwiz::vector_t world_offset_pos; // joint state at controller-start
		gwiz::quat_t   world_offset_rot;
		
		joint_param_index_t world_offset_chan;
		joint_param_index_t world_offset_idx;
		joint_param_index_t base_joint_chan;
		joint_param_index_t base_joint_idx;
		
		int timing_mode;
		int heading_mode;
		int dirty_action_bit;
		
		float raw_angle;
		float world_turn_angle;
		float turn_angle;
		float turn_angle_scale;

		float raw_time;
		float turn_speed;
		float turn_time;
		float turn_time_scale;
		
		SkMotion* build_mirror_motion( SkMotion* ref_motion_p );
		void capture_world_offset_state( void );
		float calc_raw_turn_angle( SkMotion* mot_p, char *joint_name );
		void update_action_params( void );

	public:
		static std::string type_name;

		/*! Constructor */
		MeCtStepTurn ();

		/*! Destructor is public but pay attention to the use of ref()/unref() */
		virtual ~MeCtStepTurn ();

		/*! Set the motion to be used. A valid motion must be set using
    		this method before calling any other method.
    		The old motion is unreferenced, and the new one is referenced.
    		(SkMotion derives SrSharedClass and has ref/unref methods)
    		The keytimes of m are translated to ensure start from zero. 
    		MeController::init() is automatically called. */
		void init( SbmPawn* pawn, SkMotion* mot_p);

		void set_time( float sec );
		void set_duration( float sec );
		void set_speed( float dps );
		
		void set_heading_local( float h );
		void set_heading_world( float h );

		/*! Returns a pointer to the current motion of this controller */
		SkMotion* motion () { return _motion; }

		/*! Set the play mode, default is linear */
		void play_mode ( SkMotion::InterpType it ) { _play_mode=it; }

		/*! Returns the current play mode */
		SkMotion::InterpType play_mode () const { return _play_mode; }

//	See notes below: CALCULATING DURATION
		virtual double controller_duration ();

	private:

		// callbacks for the base class
		virtual void context_updated( void );
		virtual void controller_start();
		virtual void controller_map_updated();
		virtual bool controller_evaluate ( double t, MeFrameData& frame );
		virtual SkChannelArray& controller_channels ();
		virtual const std::string& controller_type () const;
		virtual void print_state( int tabCount );
};

/* ***************************************************************

NOTE: CALCULATING DURATION with world-heading and speed requires a skeleton

>  - when is the earliest that a controller can query the skeleton?

The MeController method context_updated() is called every time the
_context variable is set.  If _context is not NULL, then you should be
able to query the skeleton connected to the context's channels.  All
this happens when context( MeControllerContext* ) is called, either by
adding the controller to a MeControllerTreeRoot, or a meta-controller.

****************************************************************** */

//////////////////////////////////////////////////////////////////////////////////
# endif // ME_CT_STEP_TURN_H

