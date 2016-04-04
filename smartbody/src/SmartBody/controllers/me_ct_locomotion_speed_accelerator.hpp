/*
 *  me_ct_locomotion_speed_accelerator.hpp - part of SmartBody-lib's Motion Engine
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
 *      Jingqiao Fu, USC
 */

#ifndef ME_CT_LOCOMOTION_SPEED_ACCELERATOR_HPP
#define ME_CT_LOCOMOTION_SPEED_ACCELERATOR_HPP

#include "controllers/me_ct_locomotion_limb.hpp"

#pragma once

class MeCtLocomotionSpeedAccelerator{
public:
	// Public Constants
	static const char*	TYPE;

	float				target_speed;
	float				curr_speed;
	float				target_acceleration;
	float				acceleration_factor;
	float				acceleration;

	float				max_acceleration_pos; // max acceleration limit. 0: not used; > 0: limit
	float				min_acceleration_pos; // min acceleration limit. 0: not used; > 0: limit
	float				max_acceleration_neg; // max acceleration limit. 0: not used; < 0: limit
	float				min_acceleration_neg; // min acceleration limit. 0: not used; < 0: limit

	float				speed_limit;

	SrArray<int>		anim_indices;

	bool				auto_accelerated; // acceleration is automatically calculated / mannually set.
	bool				proceed_acceleration; // whether acceleration is usced at every cycle.
	float				frame_interval;


public:
	/** Constructor */
	MeCtLocomotionSpeedAccelerator();

	/** Destructor */
	virtual ~MeCtLocomotionSpeedAccelerator();

	float		update(SrVec* direction, MeCtLocomotionLimb* limb);
	void		update_speed(double time_interval);
	void		set_target_speed(float target_speed);
	float		get_target_speed();
	float		get_target_acceleration();
	float		get_curr_acceleration();
	float		get_curr_speed();

	float		get_max_acceleration_neg();
	float		get_min_acceleration_neg();

	void		set_acceleration(float acc);
	void		set_max_acceleration(float max);
	void		update_acceleration(MeCtLocomotionLimb* limb, MeCtLocomotionTimingSpace* timing_space);
	void		clear_acceleration();

};

#endif // ME_CT_LOCOMOTION_SPEED_ACCELERATOR_HPP
