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

#include "controllers/me_ct_locomotion_limb_direction_planner.hpp"


const char* MeCtLocomotionLimbDirectionPlanner::TYPE = "MeCtLocomotionLimbDirectionPlanner";


/** Constructor */
MeCtLocomotionLimbDirectionPlanner::MeCtLocomotionLimbDirectionPlanner() 
{
	curr_ratio = 0.0f;
	set_turning_speed(4.0f);
	curr_direction.set(0.0f, 0.0f, 1.0f);
	target_direction.set(0.0f, 0.0f, 0.0f);
	turning_mode = 0;
}

/** Destructor */
MeCtLocomotionLimbDirectionPlanner::~MeCtLocomotionLimbDirectionPlanner() 
{
	// Nothing allocated to the heap

}

SrVec MeCtLocomotionLimbDirectionPlanner::get_curr_direction()
{
	return curr_direction;
}

/*float MeCtLocomotionLimbDirectionPlanner::get_ratio(MeCtLocomotionLimbAnim* anim1, MeCtLocomotionLimbAnim* anim2)
{
	SrVec v1 = anim1->global_info->direction*(float)anim1->get_timing_space()->get_mode();
	SrVec v2 = anim2->global_info->direction*(float)anim2->get_timing_space()->get_mode();
	SrVec dir = curr_direction;
	float len1 = anim1->global_info->displacement;
	float len2 = anim2->global_info->displacement;
	dir.normalize();
	float p1 = dir.z*v1.x - dir.x*v1.z;
	float p2 = dir.x*v2.z - dir.z*v2.x;
	float s = (p2*len2+p1*len1);
	if(s == 0.0f) return 0.0f;
	s = len2*p2/s;
	if(s > 1.0f) s = 1.0f;
	return s;
}*/

float MeCtLocomotionLimbDirectionPlanner::get_ratio(MeCtLocomotionLimbAnim* anim1, MeCtLocomotionLimbAnim* anim2)
{

	if(anim1 && ! anim2) return 1.0f;
	else if(!anim1 && anim2) return 0.0f;
	else if(!anim1 && !anim2) return 0.0f;
	SrVec dir = curr_direction;
	dir.normalize();

	SrVec v1(0,0,0);
	SrVec v2(0,0,0);
	float len1 = 1.0f, len2 = 1.0f, cos1 = 0.0f, cos2 = 0.0f, sin1 = 1.0f, sin2 = 1.0f;

	if(anim1)
	{
		v1 = anim1->global_info->direction*(float)anim1->get_timing_space()->get_mode();
		len1 = anim1->global_info->displacement;
		cos1 = dot(anim1->global_info->direction, dir);
		if(cos1 >= 1.0f)
		{
			sin1 = 0.0f;
		}
		else sin1 = sqrt(1.0f-cos1*cos1)*len1;
	}

	if(anim2)
	{
		v2 = anim2->global_info->direction*(float)anim2->get_timing_space()->get_mode();
		len2 = anim2->global_info->displacement;
		cos2 = dot(anim2->global_info->direction, dir);
		if(cos2 >= 1.0f)
		{
			sin2 = 0.0f;
		}
		else sin2 = sqrt(1.0f-cos2*cos2)*len2;
	}

	return sin2/(sin1+sin2);
}

void MeCtLocomotionLimbDirectionPlanner::reset()
{
	curr_direction.set(0.0f, 0.0f, 0.0f);
	target_direction.set(0.0f, 0.0f, 0.0f);
}

void MeCtLocomotionLimbDirectionPlanner::set_target_direction(SrVec& direction)
{
	set_target_direction(&direction);
}

void MeCtLocomotionLimbDirectionPlanner::set_target_direction(SrVec* direction)
{
	target_direction = *direction;
	if(!direction->iszero())
	{
		if(curr_direction.iszero()) curr_direction = target_direction;
		target_direction.normalize();
		SrVec vec = cross(curr_direction, target_direction);
		if(vec.y > 0.0f)
		{
			turning_speed = target_turning_speed;
		}
		else turning_speed = -target_turning_speed;
	}
}

void MeCtLocomotionLimbDirectionPlanner::set_curr_direction(SrVec* direction)
{
	curr_direction = *direction;
	if(!target_direction.iszero())
	{
		SrVec vec = cross(curr_direction, target_direction);
		if(vec.y >= 0.0f)
		{
			turning_speed = target_turning_speed;
		}
		else turning_speed = -target_turning_speed;
	}
}

SrVec MeCtLocomotionLimbDirectionPlanner::get_target_direction()
{
	return target_direction;
}

void MeCtLocomotionLimbDirectionPlanner::set_turning_speed(float speed)
{
	if(speed < 0.0f) speed = -speed;
	target_turning_speed = speed;
	if(turning_speed < 0) turning_speed = -target_turning_speed;
	else turning_speed = target_turning_speed;
}

void MeCtLocomotionLimbDirectionPlanner::set_turning_mode(int mode)
{
	turning_mode = mode;
}

void MeCtLocomotionLimbDirectionPlanner::update_anim_mode(MeCtLocomotionLimbAnim* anim)
{
	int mode = 0;
	if(dot(anim->global_info->direction, curr_direction) < 0.0f) mode = -1;
	else mode = 1;
	anim->get_timing_space()->set_mode(mode);
}

void MeCtLocomotionLimbDirectionPlanner::update_direction(double time_interval, float* space_time, int ref_time_num, bool dominant_limb)
{
	if(time_interval == 0.0f || curr_direction == target_direction) return;

	SrMat mat;
	float angle = 0.0f;
	int p = 0;
	SrVec vec1;
	if(!target_direction.iszero())
	{
		vec1 = cross(curr_direction, target_direction);
		if(turning_mode == 0 && dot(target_direction, curr_direction) >= 0.0f || turning_mode == 1)
		//if(turning_mode == 0 || turning_mode == 1)
		{
			angle = (float)(turning_speed*time_interval);
			p = 1;
		}
		else if(turning_mode == 0 && dot(target_direction, curr_direction) < 0.0f || turning_mode == 2)
		//else if(turning_mode == 2)
		{ 
			angle = -(float)(turning_speed*time_interval);
			p = -1;
		}
	}

	mat.roty(angle);
	curr_direction = curr_direction*mat;

	if(!target_direction.iszero())
	{
		SrVec vec2 = cross(curr_direction, target_direction);
		if(vec1.y*vec2.y <= 0.0f)
		{
			float angle = dot(target_direction, curr_direction);
			if(angle > 0.0f && p == 1) 
				curr_direction = target_direction;
			else if(angle < 0.0f && p == -1)
			{
				curr_direction = target_direction;
				if(dominant_limb) direction_inversed = true;
			}
		}
	}

	update_space_time(space_time, ref_time_num);

}

void MeCtLocomotionLimbDirectionPlanner::update_space_time(float* space_time, int ref_time_num)
{
	if(direction_inversed) 
	{
		*space_time = ref_time_num-*space_time;
		if(*space_time == ref_time_num) *space_time = 0.0f;
		direction_inversed = false;
	}
}