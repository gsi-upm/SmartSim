/*
 *  me_ct_locomotion_balance.hpp - part of SmartBody-lib's Motion Engine
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

#include "controllers/me_ct_locomotion_balance.hpp"
#include <cstdlib>

MeCtLocomotionBalance::MeCtLocomotionBalance()
{
	rotational_speed = 0.5f;
	factor = 1.0f;
	target_normal.set(0.0f, 1.0f, 0.0f);
}

MeCtLocomotionBalance::~MeCtLocomotionBalance()
{
}

void MeCtLocomotionBalance::set_factor(float factor)
{
	this->factor = factor;
}

void MeCtLocomotionBalance::update(SrArray<MeCtLocomotionLimb*>& limb_list, SrVec& orientation, MeCtLocomotionJointInfo* nonlimb_joint_info, float facing, int translation_joint_index, float time)
{
	calc_target_normal(limb_list, orientation);

	calc_normal(time);

	float angle = acos(dot(normal, orientation)/(normal.len()*orientation.len()));

	angle = factor*angle/4.0f;

	SrVec axis;
	SrVec up(0.0f, 1.0f, 0.0f);
	axis = cross(orientation, normal);

	SrMat mat;

	mat.rot(up, -facing);

	axis = axis*mat;

	mat.rot(axis, -angle);

	SrQuat quat = nonlimb_joint_info->quat.get(translation_joint_index+1);
	quat = quat * mat;
	nonlimb_joint_info->quat.set(translation_joint_index+1, quat);
}

void MeCtLocomotionBalance::calc_target_normal(SrArray<MeCtLocomotionLimb*>& limb_list, SrVec& orientation)
{
	target_normal.set(0.0f, 0.0f, 0.0f);
	for(int i= 0; i < limb_list.size(); ++i)
	{
		target_normal += limb_list.get(i)->ik_terrain_normal;
	}
	target_normal.normalize();
	
}

void MeCtLocomotionBalance::calc_normal(float time)
{
	if(normal.iszero()) 
	{
		normal = target_normal;
		return;
	}
	float angle = time*rotational_speed;
	float c_angle = acos(dot(normal, target_normal));
	if(c_angle <= angle) normal = target_normal;

	if(normal == target_normal) return;

	SrVec axis = cross(normal, target_normal);
	SrMat mat;

	mat.rot(axis, angle);
	normal = normal*mat;
	normal.normalize();
}
