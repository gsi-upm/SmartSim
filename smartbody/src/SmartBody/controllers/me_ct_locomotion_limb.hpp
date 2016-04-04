/*
 *  me_ct_locomotion_limb.hpp - part of SmartBody-lib's Motion Engine
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

#ifndef ME_CT_LOCOMOTION_LIMB_HPP
#define ME_CT_LOCOMOTION_LIMB_HPP

#include "controllers/me_ct_locomotion_limb_anim.hpp"
#include "controllers/me_ct_locomotion_limb_direction_planner.hpp"
#include "controllers/me_ct_locomotion_joint_info.hpp"
#include "controllers/me_ct_IK_scenario.hpp"

class MeCtLocomotionLimbAnim;

#pragma once

class MeCtLocomotionLimb{
public:
	// Public Constants
	static const char* TYPE;

protected:
	// Data
	float							height_bound;
	float							space_time;
	bool							is_valid;  // All necessary channels are present

public:
	int								joint_num;
	SrArray<SrString*>				support_joint_list; // joints that touch the ground to support body when walking
	SrArray<float>					support_height; // distance from each support joint to ground
	float							ground_height; // ground height in Y direction
	char*							skeleton_name;
	MeCtLocomotionJointInfo			limb_joint_info;
	SrString						limb_name;


public:
	//temp, to be deleted=================
	SkSkeleton*						walking_skeleton;
	SkSkeleton*						standing_skeleton;

public://for IK
	MeCtIKScenario					ik;
	SrVec							ik_offset;
	SrVec							ik_offset_record;
	SrVec							ik_terrain_normal;
	SrVec							ik_terrain_target_normal;
	float							ik_target_comp;

public:
	SrArray<MeCtLocomotionLimbAnim*>	walking_list; // originally provided artist-made animation. for each provided walking animation, do analysis in MeCtLocomotionLimbAnim.

	
	SrArray<SrVec>					pos_buffer;
	MeCtLocomotionLimbAnim			blended_anim; // blended run-time animation
	MeCtLocomotionLimbDirectionPlanner direction_planner; 
	
	SrVec							pos;
	std::string						limb_base_name;
	float							rotation_record;
	float							curr_rotation;
	float							pre_rotation;
	SrVec							displacement;

	float							max_rotation_left; // rotation limit left;
	float							max_rotation_right; // rotation limit right;

public:
	MeCtLocomotionLimb();
	~MeCtLocomotionLimb();

	MeCtLocomotionLimb(char* name);

	void set_space_time(float space_time);
	float get_space_time();
	float* get_space_time_pt();

	void	set_height_bound(float bound);
	float	get_height_bound();

	SrVec	get_orientation();

	float	get_off_ground_height(int joint_index)
	{
		if(walking_list.size()<2) return pos_buffer.get(joint_index).y-support_height.get(joint_index-2);
		return ik.joint_info_list.get(joint_index).support_joint_comp;
	}

	float	get_ground_height();
	void	set_ground_height(float ground_height);

	void	init_skeleton(SkSkeleton* standing_skeleton, SkSkeleton* walking_skeleton);

	int		add_support_joint(char* joint_name);
	int		set_limb_base(const std::string& name);

	int		get_descendant_num(SkJoint* joint);
	int		get_support_joint_num();

	void	set_skeleton_name(char* name);

	std::string	get_limb_base_name();

	void	calc_blended_anim_speed(MeCtLocomotionLimbAnim* anim1, MeCtLocomotionLimbAnim* anim2, float weight);

	void	blend_anim(float space_time, int anim_index1, int anim_index2, float weight, SrArray<int>* index_buff);

	void	blend_standing(MeCtLocomotionLimbAnim* standing, float weight);

	void	manipulate_turning(SrMat& root_mat);

	void	set_joint_type(int index, int type);

	int		get_joint_type(int index);

	void	set_joint_rotation_axis(int index, SrVec* axis);

	SrArray<MeCtLocomotionLimbAnim*>* get_walking_list();

	//temp function for test, to be deleted......
	void	print_info();




};

#endif // ME_CT_LOCOMOTION_LIMB_HPP
