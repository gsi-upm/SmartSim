/*
 *  me_ct_locomotion.hpp - part of SmartBody-lib's Motion Engine
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

#ifndef ME_CT_LOCOMOTION_HPP
#define ME_CT_LOCOMOTION_HPP

#include <sb/SBController.h>
#include "controllers/me_ct_locomotion_pawn.hpp"
#include "controllers/me_ct_locomotion_analysis.hpp"
#include "controllers/me_ct_locomotion_limb.hpp"
#include "controllers/me_ct_locomotion_speed_accelerator.hpp"
#include "controllers/me_ct_locomotion_navigator.hpp"
#include "controllers/me_ct_locomotion_func.hpp"
#include "controllers/me_ct_locomotion_joint_info.hpp"
#include "controllers/me_ct_locomotion_height_offset.hpp"
#include "controllers/me_ct_locomotion_IK.hpp"
#include "controllers/me_ct_locomotion_balance.hpp"
#include "controllers/me_ct_navigation_circle.hpp"

class MeCtLocomotionLimb;

#pragma once


class MeCtLocomotion : public SmartBody::SBController, public MeCtLocomotionPawn {
public:
	// Public Constants
	static std::string TYPE;

public:

	//SrArray<char*> limb_base_name;
	float pre_blended_base_height;
	float r_blended_base_height;

	SrVec world_offset_to_base;
	SrVec pre_world_offset_to_base;

	int translation_joint_index;
	//SrString translation_joint_name;
	//float translation_joint_height;

	SrVec displacement;

	int style;

protected: // flags
	bool channels_valid;  // All necessary channels are present
	//bool valid;
	bool nonlimb_joints_indexed;
	bool dis_initialized; // limb joint position calculated
	bool initialized;
	bool enabled;
	bool idle;

public:
	bool motions_loaded;
	bool reset;
	bool ik_enabled;
	bool freezed;

	double freeze_delta_time;
	
protected:
	//MeCtLocomotionJointInfo nonlimb_joint_info;
	int r_anim1_index;
	int r_anim2_index;
	int r_anim1_index_dominant;
	int r_anim2_index_dominant;

protected:


	MeCtLocomotionAnalysis analyzer;
	MeCtNavigationCircle navigation_circle;

	SkChannelArray  request_channels;

	//SrArray<MeCtLocomotionAnimGlobalInfo*> anim_global_info;

	int dominant_limb;

	//temp
	SrArray<SrQuat> t_joint_quats1;
	SrArray<SrQuat> t_joint_quats2;
	SrArray<SrQuat> joint_quats1;
	SrArray<SrQuat> joint_quats2;

	double last_time;
	double delta_time;

	//char* base_name;


protected: // channel initialization related
	int joint_channel_start_ind;
	int limb_joint_num;

	//temp, to be deleted=================
public:
	//SkSkeleton* walking_skeleton;
	//SkSkeleton* standing_skeleton;

	//SrArray<MeCtLocomotionLimb*> limb_list; //limbs
	//SrArray<SkMotion*> locomotion_anims;


public:
	/** Constructor */
	MeCtLocomotion();

	/** Destructor */
	virtual ~MeCtLocomotion();

	const std::string& controller_type() const;

	/**
	 *  Implements MeController::controller_channels().
	 */
	SkChannelArray& controller_channels();

	int iterate_limb_joints(SkJoint* base);

	void iterate_joints(MeCtLocomotionJointInfo* joint_info);

	int iterate_limb_joints(SkJoint* base, int depth);

	int iterate_nonlimb_joints(SkJoint* base, int depth);

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


protected:
	int check_limb_controller_map_updated();

public:

	MeCtLocomotionAnalysis* get_analyzer();

	MeCtLocomotionNavigator* get_navigator();

	MeCtNavigationCircle* get_navigation_circle();

	void add_locomotion_anim(SkMotion* anim);

	//SrArray<MeCtLocomotionAnimGlobalInfo*>* get_anim_global_info();

	SrVec get_facing_vector();

	void set_motion_time(float time);

	//void set_translation_joint_name(const char* name);
	std::string& get_translation_joint_name();

	void set_freeze(bool freeze);

	void init_skeleton(SkSkeleton* standing, SkSkeleton* walking);
	
	bool is_initialized();

	bool is_freezed();

	bool is_enabled();

	bool is_valid();

	bool is_motions_loaded();

	bool is_channels_valid();

	SrArray<MeCtLocomotionLimb*>* get_limb_list();

	//void init_nonlimb_joint_info();

	void set_target_height_displacement(float displacement);

	//void set_base_name(const char* name);

	void set_enabled(bool enable){enabled = enable;}

	void print_info(const std::string& name);

	void set_balance_factor(float factor);

	SrVec get_base_pos();

	float get_current_speed();

	int get_dominant_limb_index();

	//void set_valid(bool valid);

	MeCtLocomotionTerrain* get_terrain();

	//temp functions
	SrVec get_supporting_joint_pos(int joint_index, int limb_index, SrVec* orientation, SrVec* normal);

	void temp_update_for_footprint(MeFrameData& frame);

protected:

	int determine_dominant_limb_index();

	void get_translation_base_joint_index();

	void get_anim_indices(int limb_index, SrVec direction, int* anim1_index, int* anim2_index);

	void update_pos();

	SrVec get_limb_pos(MeCtLocomotionLimb* limb);

	void update_facing();

	void update_heading();

	char* get_base_name();

	//SrVec get_heading_direction();

	void set_turning_speed(float radians);

	bool controller_evaluate( double time, MeFrameData& frame );

	void set_skeleton(SkSkeleton* skeleton);

	void analyze_motion( SkMotion* motion );

	void update(float inc_frame, MeFrameData& frame);

	void update_facing(MeCtLocomotionLimb* limb);

	void normalize_proportion();

	void blend_base_joint(MeFrameData& frame, float space_time, int anim_index1, int anim_index2, float weight);

	SrVec get_local_direction(SrVec* direction);

	int determine_dominant_limb();

	void apply_IK();

	int LOOKUP_BUFFER_INDEX(int var_name, int index );

	void update_limb_anim_standing();

	void update_limb_anim_standing(MeCtLocomotionLimbAnim* anim, int index, MeFrameData& frame);

	void update_nonlimb_mat(SkJoint* joint, SrMat* mat, int depth);

	void blend_standing(MeFrameData& frame);

	SrVec calc_rotational_displacement();

	void update_nonlimb_mat_with_global_info();

	void update_limb_mat_with_global_info();



};

#endif // ME_CT_LOCOMOTION_HPP
