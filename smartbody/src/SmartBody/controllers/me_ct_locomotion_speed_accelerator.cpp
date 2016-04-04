/*
 *  me_ct_locomotion_speed_planner.hpp - part of SmartBody-lib's Motion Engine
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

#include "controllers/me_ct_locomotion_speed_accelerator.hpp"

#include "sbm/sbm_character.hpp"
#include "sbm/gwiz_math.h"
#include "limits.h"


const char* MeCtLocomotionSpeedAccelerator::TYPE = "MeCtLocomotionSpeedAccelerator";


/** Constructor */
MeCtLocomotionSpeedAccelerator::MeCtLocomotionSpeedAccelerator() {
	curr_speed = 0.0f;
	target_speed = 0.0f;
	target_acceleration = 10.0f;
	acceleration = 10.0f;
	acceleration_factor = 8800.0f;
	//automate = true;
	proceed_acceleration = true;
	auto_accelerated = true;
	speed_limit = 200.0f;
	frame_interval = 0.03333333f;
	max_acceleration_pos = 100.0f;
	min_acceleration_pos = 0.0f;
	max_acceleration_neg = -1.0f;
	min_acceleration_neg = -50.0f;
}

/** Destructor */
MeCtLocomotionSpeedAccelerator::~MeCtLocomotionSpeedAccelerator() {
	// Nothing allocated to the heap

}

void MeCtLocomotionSpeedAccelerator::update_speed(double time_interval)
{
	if(!proceed_acceleration)
	{
		curr_speed = target_speed;
		return;
	}
	if(target_acceleration > acceleration)
	{
		acceleration += (float)(acceleration_factor*time_interval);
		if(acceleration > target_acceleration) acceleration = target_acceleration;
	}
	else
	{
		acceleration -= (float)(acceleration_factor*time_interval);
		if(acceleration < target_acceleration) acceleration = target_acceleration;
	}
	if(curr_speed == target_speed) return;
	curr_speed += acceleration * (float)time_interval;
	if(acceleration != 0.0f && (curr_speed - target_speed)*acceleration >= 0.0f)
	{
		curr_speed = target_speed;
		//acceleration = 0.0f;
	}
}

void MeCtLocomotionSpeedAccelerator::clear_acceleration()
{
	target_acceleration = 0.0f;
	acceleration = 0.0f;
}

float MeCtLocomotionSpeedAccelerator::update(SrVec* direction, MeCtLocomotionLimb* limb)
{
	if(direction->iszero()) return 0.0f;
	anim_indices.capacity(0);
	anim_indices.push() = 1;
	anim_indices.push() = 2;
	MeCtLocomotionLimbAnim* anim1 = limb->walking_list.get(1);
	MeCtLocomotionLimbAnim* anim2 = limb->walking_list.get(2);
	SrVec m = *direction;
	SrVec d1 = anim1->global_info->direction*(float)anim1->get_timing_space()->get_mode();
	SrVec d2 = anim2->global_info->direction*(float)anim2->get_timing_space()->get_mode();
	d1.normalize();
	d2.normalize();
	d1 *= anim1->global_info->speed;
	d2 *= anim2->global_info->speed;
	m.normalize();
	SrVec dir = d2- d1;
	float a = dir.x/dir.z;

	limb->blended_anim.global_info->speed = (d1.x - a * d1.z)/(m.x - a * m.z);
	//float len = (d1.x - a * d1.z)/(m.x - a * m.z);
	return curr_speed/limb->blended_anim.global_info->speed;
}

void MeCtLocomotionSpeedAccelerator::set_target_speed(float target_speed)
{
	if(target_speed > speed_limit) target_speed = speed_limit;
	this->target_speed = target_speed;
	if((target_speed - curr_speed)*acceleration < 0.0f) 
	{
		target_acceleration = -target_acceleration;
		acceleration = -acceleration;
	}

}

float MeCtLocomotionSpeedAccelerator::get_target_speed()
{
	return target_speed;
}

float MeCtLocomotionSpeedAccelerator::get_curr_acceleration()
{
	return acceleration;
}

float MeCtLocomotionSpeedAccelerator::get_target_acceleration()
{
	return target_acceleration;
}

float MeCtLocomotionSpeedAccelerator::get_curr_speed()
{
	return curr_speed;
}

/*void MeCtLocomotionSpeedAccelerator::set_acceleration(float acc)
{
	if(acc < 0.0f) acc = -acc;
	target_acceleration = acc;
	if(this->acceleration > 0) this->acceleration = target_acceleration;
	else this->acceleration = -target_acceleration;
}*/

void MeCtLocomotionSpeedAccelerator::set_max_acceleration(float max)
{
	if(max < 0.0f) 
	{
		max_acceleration_neg = max;
	}
	else
	{
		max_acceleration_pos = max;
	}
}

void MeCtLocomotionSpeedAccelerator::update_acceleration(MeCtLocomotionLimb* limb, MeCtLocomotionTimingSpace* timing_space)
{
	if(proceed_acceleration == false || auto_accelerated == false) return;
	if(target_speed == 0.0f && curr_speed == 0.0f) return;
	float length = 0.0f;

	if(target_speed >= curr_speed) 
		length = timing_space->get_section_length(limb->get_space_time(), 1.0f);
	else
		length = timing_space->get_section_length(limb->get_space_time(), 0.0f);
	float time;
		
	time = length * frame_interval;
	target_acceleration = (target_speed - curr_speed)/time;
	
	if(target_acceleration >= 0.0f)
	{
		if(max_acceleration_pos > 0.0f && target_acceleration > max_acceleration_pos) target_acceleration = max_acceleration_pos;
		if(min_acceleration_pos > 0.0f && target_acceleration < min_acceleration_pos) target_acceleration = min_acceleration_pos;
	}
	else
	{
		if(max_acceleration_neg < 0.0f && target_acceleration > max_acceleration_neg) target_acceleration = max_acceleration_neg;
		if(min_acceleration_neg < 0.0f && target_acceleration < min_acceleration_neg) target_acceleration = min_acceleration_neg;
	}

	//if(curr_speed > 0.0f) printf("\n%f %f %f", target_acceleration, target_speed, curr_speed);
}

float MeCtLocomotionSpeedAccelerator::get_max_acceleration_neg()
{
	return max_acceleration_neg;
}

float MeCtLocomotionSpeedAccelerator::get_min_acceleration_neg()
{
	return min_acceleration_neg;
}
