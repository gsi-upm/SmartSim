/*
 *  me_ct_locomotion_pawn.hpp - part of SmartBody-lib's Motion Engine
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

#ifndef ME_CT_LOCOMOTION_PAWN_HPP
#define ME_CT_LOCOMOTION_PAWN_HPP

#include <controllers/me_controller.h>

#include "controllers/me_ct_locomotion_limb.hpp"
#include "controllers/me_ct_locomotion_speed_accelerator.hpp"
#include "controllers/me_ct_locomotion_navigator.hpp"
#include "controllers/me_ct_locomotion_func.hpp"
#include "controllers/me_ct_locomotion_joint_info.hpp"
#include "controllers/me_ct_locomotion_height_offset.hpp"
#include "controllers/me_ct_locomotion_IK.hpp"
#include "controllers/me_ct_locomotion_balance.hpp"

class MeCtLocomotionLimb;

#pragma once


class MeCtLocomotionPawn
{
public:
	// Public Constants
	static const char* TYPE;

public: // constants

	static const char* LOCOMOTION_ROTATION;
	//! Channel name for instantaneous locomotion rotation, uses YPos
	static const char* LOCOMOTION_GLOBAL_ROTATION;
	static const char* LOCOMOTION_LOCAL_ROTATION;
	static const char* LOCOMOTION_LOCAL_ROTATION_ANGLE;
	static const char* LOCOMOTION_TIME;
	static const char* LOCOMOTION_ID;
	//! Channel name for immediate locomotion speed and trajectory, stored in the joint position channels
	static const char* LOCOMOTION_VELOCITY;
	
	//! Channel name for the body orientation target, stored in the joint position channels
	static const char* ORIENTATION_TARGET;

public:

	std::string translation_joint_name;
	float translation_joint_height;
	SrArray<MeCtLocomotionAnimGlobalInfo*> anim_global_info;

protected:
	bool valid;

	MeCtLocomotionNavigator navigator;

	MeCtLocomotionIK ik;

	MeCtLocomotionHeightOffset height_offset;

	MeCtLocomotionBalance balance;

	MeCtLocomotionSpeedAccelerator speed_accelerator; // to be moved to MeCtLocomotionLimb

	MeCtLocomotionTerrain terrain;

	MeCtLocomotionJointInfo nonlimb_joint_info;

	std::string base_name;

	//temp, to be deleted=================
public:
	SkSkeleton* walking_skeleton;
	SkSkeleton* standing_skeleton;

	SrArray<MeCtLocomotionLimb*> limb_list; //limbs
	SrArray<SkMotion*> locomotion_anims;


public:
	/** Constructor */
	MeCtLocomotionPawn();

	/** Destructor */
	virtual ~MeCtLocomotionPawn();

	void set_valid(bool valid);

	SrArray<MeCtLocomotionAnimGlobalInfo*>* get_anim_global_info();

	void init_nonlimb_joint_info();

	SrArray<MeCtLocomotionLimb*>* get_limb_list();

	void set_translation_joint_name(const std::string& name);

	void set_base_name(const std::string& name);

	MeCtLocomotionSpeedAccelerator* get_speed_accelerator();

};

#endif // ME_CT_LOCOMOTION_PAWN_HPP
