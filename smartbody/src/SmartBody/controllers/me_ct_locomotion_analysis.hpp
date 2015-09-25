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
 *      Jingqiao Fu, USC
 */

#ifndef ME_CT_LOCOMOTION_ANALYSIS_HPP
#define ME_CT_LOCOMOTION_ANALYSIS_HPP

#include <controllers/me_controller.h>
#include "controllers/me_ct_locomotion_pawn.hpp"
#include "controllers/me_ct_IK.hpp"
//#include "sbm/sbm_character.hpp"
#include "sbm/sr_path_list.h"

#pragma once

//class MeCtLocomotionPawn;

class MeCtLocomotionAnalysis{
public:
	// Public Constants
	static const char* TYPE;

protected:
	// Data
	//SrArray<MeCtLocomotionLimb*> limb_list;

	MeCtLocomotionPawn* _ct_locomotion_pawn;

	MeCtLocomotionLimb*  _limb;

//	MeCtLocomotion* _locomotion;

	SkChannelArray  request_channels;

	SrArray<int>		stance_frame;

	SkMotion *motion_locomotion;
	SkMotion *motion_standing;
	
	//bool is_valid;  // All necessary channels are present

	SkSkeleton* walking_skeleton;
	SkSkeleton* standing_skeleton;

	bool standing_initialized;

public:
	MeCtLocomotionAnalysis();

	~MeCtLocomotionAnalysis();

	void set_skeleton(SkSkeleton* walking_skeleton, SkSkeleton* standing_skeleton);

	bool get_standing_initialized();

	void set_ct_pawn(MeCtLocomotionPawn* controller);

	MeCtLocomotionPawn* get_ct_pawn();

	void init(SkMotion* standing, srPathList &me_paths); //temp hard-coded init for human characters

	void add_locomotion(SkMotion* motion_locomotion, int type, int walking_style, float translation_scale, float base_height_offset);

	//void add_locomotion(SkMotion* motion_locomotion, float land_time, float stance_time, float lift_time);
	void add_locomotion(SkMotion* motion_locomotion, float l_land_time, float l_stance_time, float l_lift_time, float r_land_time, float r_stance_time, float r_lift_time, float translation_scale);
	//void calc_velocity();
	//void calc_velocity(int index);

	//void analyze_locomotion(MeCtLocomotion* locomotion, SbmCharacter* character, SkMotion* motion, mcuCBHandle *mcu_p );

	void analyze_limb_anim(MeCtLocomotionLimbAnim* anim, SkMotion* walking, SkMotion* standing, std::string limb_base, SrArray<float>* support_height, float ground_height, float height_bound, float base_height_offset);

	void analyze_limb_anim(MeCtLocomotionLimbAnim* anim, SkMotion* walking, SkMotion* standing, std::string limb_base, float land_time, float stance_time, float lift_time);

	void analyze_walking_limb(MeCtLocomotionLimb* limb, SkMotion* walking, SkMotion* standing, int walking_style, float translation_scale, float base_height_offset);
	void analyze_walking_limb(MeCtLocomotionLimb* limb, SkMotion* walking, SkMotion* standing, float land_time, float stance_time, float lift_time, int walking_style, float translation_scale);

	void analyze_standing(MeCtLocomotionLimb* limb, SkMotion* standing);

	void analyze_standing_core(MeCtLocomotionLimb* limb, SkSkeleton* skeleton);

	void calc_stance_time(MeCtLocomotionLimbAnim* anim, std::string limb_base);

	float iterate_sub_joints(SkJoint* walking_joint, SkJoint* standing_joint);

	int get_descendant_num(const std::string& base_name);

	int get_descendant_num(SkJoint* base);

	void add_ref_times(MeCtLocomotionLimbAnim* anim, int* count);

	void estimate_direction(MeCtLocomotionLimbAnim* anim, int* count);

	void filter_displacement(MeCtLocomotionLimbAnim* anim, int* count);

	void init_blended_anim();

	int get_translation_base_joint_name(SkSkeleton* skeleton);

	//temp funcs.............
	void test_facing(SkMotion* walking);



	void print_info();

};

#endif // ME_CT_LOCOMOTION_ANALYSIS_HPP
