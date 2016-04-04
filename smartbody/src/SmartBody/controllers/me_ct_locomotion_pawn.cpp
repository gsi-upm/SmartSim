/*
 *  me_ct_locomotion_pawn.cpp - part of SmartBody-lib's Motion Engine
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

#include "vhcl.h"

#include "controllers/me_ct_locomotion_pawn.hpp"
#include "sbm/mcontrol_util.h"
#include "sbm/sbm_character.hpp"
#include "sbm/gwiz_math.h"
#include "limits.h"

#include <iostream>
#include <string>


const char* MeCtLocomotionPawn::TYPE = "MeCtLocomotionPawn";

const char* MeCtLocomotionPawn::LOCOMOTION_VELOCITY = "locomotion_velocity";
const char* MeCtLocomotionPawn::LOCOMOTION_ROTATION = "locomotion_rotation";
const char* MeCtLocomotionPawn::LOCOMOTION_GLOBAL_ROTATION = "locomotion_global_rotation";
const char* MeCtLocomotionPawn::LOCOMOTION_LOCAL_ROTATION = "locomotion_local_rotation";
const char* MeCtLocomotionPawn::LOCOMOTION_LOCAL_ROTATION_ANGLE = "locomotion_local_rotation_angle";
const char* MeCtLocomotionPawn::LOCOMOTION_TIME = "locomotion_time";
const char* MeCtLocomotionPawn::LOCOMOTION_ID = "locomotion_id";

/////////////////////////////////////////////////////////////
const char* MeCtLocomotionPawn::ORIENTATION_TARGET  = "orientation_target";



/** Constructor */
MeCtLocomotionPawn::MeCtLocomotionPawn() {

	walking_skeleton = NULL;
	standing_skeleton = NULL;
	limb_list.capacity(0);
	locomotion_anims.capacity(0);
}

/** Destructor */
MeCtLocomotionPawn::~MeCtLocomotionPawn() 
{
	// Nothing allocated to the heap

	int num = limb_list.size();
	for (int i = 0; i < num; i++)
	{
		delete limb_list.pop();
	}

	num = anim_global_info.size();
	for (int i = 0; i < num; i++)
	{
		delete anim_global_info.pop();
	}
}

void MeCtLocomotionPawn::set_valid(bool valid)
{
	this->valid = valid;
}

SrArray<MeCtLocomotionAnimGlobalInfo*>* MeCtLocomotionPawn::get_anim_global_info()
{
	return &anim_global_info;
}

void MeCtLocomotionPawn::init_nonlimb_joint_info()
{
	std::vector<std::string> limb_base_name;
	limb_base_name.resize(limb_list.size());
	for(int i = 0; i < limb_list.size(); ++i)
	{
		limb_base_name[i] = limb_list.get(i)->get_limb_base_name();
	}
	std::string base_name = walking_skeleton->root()->name();
	nonlimb_joint_info.Init(walking_skeleton, base_name.c_str(), &limb_base_name);
	SkJoint* joint = NULL;
	int index = -1;
	for(int i = 0; i < limb_list.size(); ++i)
	{
		joint = walking_skeleton->search_joint(limb_list.get(i)->get_limb_base_name().c_str());
		while(true)
		{
			joint = joint->parent();
			if(joint == NULL) break;
			index = nonlimb_joint_info.get_index_by_name(joint->name().c_str());
			nonlimb_joint_info.mat_valid.set(index, 1);
		}
	}
}

SrArray<MeCtLocomotionLimb*>* MeCtLocomotionPawn::get_limb_list()
{
	return &limb_list;
}

void MeCtLocomotionPawn::set_translation_joint_name(const std::string& name)
{
	translation_joint_name = name;
	navigator.set_translation_joint_name(translation_joint_name);
}

void MeCtLocomotionPawn::set_base_name(const std::string& name)
{
	base_name = name;
}

MeCtLocomotionSpeedAccelerator* MeCtLocomotionPawn::get_speed_accelerator()
{
	return &speed_accelerator;
}