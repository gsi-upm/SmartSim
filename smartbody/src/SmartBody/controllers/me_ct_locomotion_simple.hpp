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
 *      Andrew n marshall, USC
 */

#ifndef ME_CT_LOCOMOTION_SIMPLE_HPP
#define ME_CT_LOCOMOTION_SIMPLE_HPP

#include <sb/SBController.h>

/**
 *  Calculate new world_offset each frame, given the
 *  locomotion vector and orientation target channels.
 *  These channels should be set by an earlier controller
 *  in the pipeline.
 *
 *  Does not play a walk animation or otherwise account for
 *  foot steps.  The character will just glide into position.
 */
// TODO: Replace too generic controllers (after MotionEngine v2's generic channel types?)
//   Integrator controller (locomotion_vector X/Z -> world_offset X/Z )
//   Rotational Tracking controller (given positions world_offset and orientation_target, calc new world_offset rotation)
class MeCtLocomotionSimple : public SmartBody::SBController {
public:
	// Public Constants
	static std::string TYPE;

protected:
	// Data
	SkChannelArray  request_channels;

	bool is_valid;  // All necessary channels are present

	// Buffer indices ("bi_") to the requested channels
	int bi_world_x, bi_world_y, bi_world_z, bi_world_rot; // World offset position and rotation
	int bi_loco_vel_x, bi_loco_vel_y, bi_loco_vel_z;      // Locomotion velocity
	int bi_loco_rot_y;                                    // Rotational velocity around Y

	double last_time;

public:
	/** Constructor */
	MeCtLocomotionSimple();

	/** Destructor */
	virtual ~MeCtLocomotionSimple();

	const std::string& controller_type();

	/**
	 *  Implements MeController::controller_channels().
	 */
	SkChannelArray& controller_channels();

	/**
	 *  Implements MeController::controller_duration().  -1 means indefinite.
	 */
	double controller_duration() { return -1; }

	/*!
	 *  Implements MeController::context_updated(..).
	 */
	virtual void context_updated();

	/*!
	 *  Implements MeController::controller_map_updated(..).
	 *  Save channel indices after context remap.
	 */
	virtual void controller_map_updated();

	/**
	 *  Implements MeController::controller_evaluate(..).
	 */
	bool controller_evaluate( double time, MeFrameData& frame );
};

#endif // ME_CT_LOCOMOTION_SIMPLE_HPP
