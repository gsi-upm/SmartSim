/*
 *  me_ct_locomotion_direction_planner.hpp - part of SmartBody-lib's Motion Engine
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

#ifndef ME_CT_LOCOMOTION_DIRECTION_PLANNER_HPP
#define ME_CT_LOCOMOTION_DIRECTION_PLANNER_HPP

#include <controllers/me_controller.h>
#include "controllers/me_ct_locomotion_limb_anim.hpp"

#pragma once

class MeCtLocomotionLimbDirectionPlanner{
public:
	// Public Constants
	static const char* TYPE;

	int			turning_mode;	//0: default. when angle between target direction and current direction < 90, make direct turn;
								//            otherwis, make opposite turn, and reverse the direction.
								//1: direction turn.
								//2: opposite turn and reverse.

	float		curr_ratio;

	float		turning_speed; // in radians
	float		target_turning_speed; // in radians

	SrVec		curr_direction;
	SrVec		target_direction;
	bool		direction_inversed;

public:
	/** Constructor */
	MeCtLocomotionLimbDirectionPlanner();

	/** Destructor */
	virtual ~MeCtLocomotionLimbDirectionPlanner();

	float		get_ratio(MeCtLocomotionLimbAnim* anim1, MeCtLocomotionLimbAnim* anim2);

	void		set_target_direction(SrVec& direction);
	void		set_target_direction(SrVec* direction);

	SrVec		get_target_direction();

	void		set_turning_speed(float speed);

	void		set_turning_mode(int mode);

	float		get_ratio();

	void		update_direction(double time_interval, float* space_time, int ref_time_num, bool dominant_limb);

	void		update_anim_mode(MeCtLocomotionLimbAnim* anim);

	void		update_space_time(float* space_time, int ref_time_num);

	SrVec		get_curr_direction();

	void		set_curr_direction(SrVec* direction);

	void		reset();

};

#endif // ME_CT_LOCOMOTION_DIRECTION_PLANNER_HPP
