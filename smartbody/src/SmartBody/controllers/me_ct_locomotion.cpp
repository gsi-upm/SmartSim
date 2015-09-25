/*
 *  me_ct_locomotion.cpp - part of SmartBody-lib's Motion Engine
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

#include "controllers/me_ct_locomotion.hpp"
#include "sbm/mcontrol_util.h"
#include "sbm/sbm_character.hpp"
#include "sbm/gwiz_math.h"
#include "limits.h"

#include <iostream>
#include <string>


std::string MeCtLocomotion::TYPE = "MeCtLocomotion";


/** Constructor */
MeCtLocomotion::MeCtLocomotion() {
	channels_valid = false;
	limb_joint_num = 0;
	last_time = std::numeric_limits<float>::quiet_NaN();
	nonlimb_joint_info.joint_num = 0;
	nonlimb_joint_info.buff_index.capacity(0);
	nonlimb_joint_info.joint_index.capacity(0);
	nonlimb_joint_info.quat.capacity(0);
	last_time = 0.0f;
	dominant_limb = 0;
	reset = false;
	dis_initialized = false;
	initialized = false;
	ik_enabled = true;
	enabled = false;
	nonlimb_joints_indexed = false;
	motions_loaded = true;
	base_name = "";
	locomotion_anims.capacity(0);
	pre_blended_base_height = 0.0f;
	r_blended_base_height = 0.0f;
	style = 0;
	last_time = 0.0f;
	translation_joint_height = 0.0f;
	valid = false;
	freezed = false;
	idle = false;
	freeze_delta_time = 0.0f;
	ik.set_terrain(&terrain);
	height_offset.set_terrain(&terrain);
	analyzer.set_ct_pawn((MeCtLocomotionPawn*)this);

	navigation_circle.init();
}

/** Destructor */
MeCtLocomotion::~MeCtLocomotion() {
	// Nothing allocated to the heap

}

// Look up the context indices, and check to make sure it isn't -1
/*#define LOOKUP_BUFFER_INDEX( var_name, index ) \
	var_name = _context->toBufferIndex( _toContextCh[ ( index ) ] );  \
	is_valid &= ( var_name != -1 );*/

int MeCtLocomotion::LOOKUP_BUFFER_INDEX(int var_name, int index )
{
	var_name = _context->toBufferIndex( _toContextCh[ ( index ) ] );
	channels_valid &= ( var_name != -1 );
	return var_name;
}

// Implements MeController::controller_channels().
SkChannelArray& MeCtLocomotion::controller_channels() {
	if( request_channels.size() == 0 ) 
	{
		get_translation_base_joint_index();
		joint_channel_start_ind = navigator.controller_channels(&request_channels);

		limb_joint_num = 0;
		for(int i = 0; i < limb_list.size(); ++i)
		{
			MeCtLocomotionLimb* limb = limb_list.get(i);
			SkJoint* base = standing_skeleton->search_joint(limb->get_limb_base_name().c_str());
			//limb_joint_num += iterate_limb_joints(base, 0); // starting from joint_channel_start_ind
			iterate_joints(&(limb->limb_joint_info));
			limb_joint_num += limb->limb_joint_info.joint_num;
		}

		iterate_joints(&nonlimb_joint_info);

		t_joint_quats1.capacity(nonlimb_joint_info.joint_num);
		t_joint_quats2.capacity(nonlimb_joint_info.joint_num);
		t_joint_quats1.size(nonlimb_joint_info.joint_num);
		t_joint_quats2.size(nonlimb_joint_info.joint_num);
		joint_quats1.capacity(nonlimb_joint_info.joint_num);
		joint_quats2.capacity(nonlimb_joint_info.joint_num);
		joint_quats1.size(nonlimb_joint_info.joint_num);
		joint_quats2.size(nonlimb_joint_info.joint_num);
	}
	return request_channels;
}

void MeCtLocomotion::get_translation_base_joint_index()
{
	for(size_t i = 0 ; i < nonlimb_joint_info.joint_name.size(); ++i)
	{
		if(nonlimb_joint_info.joint_name[i] == translation_joint_name)
		{
			translation_joint_index = i;
			return;
		}
	}
	translation_joint_index = 0;
}

void MeCtLocomotion::iterate_joints(MeCtLocomotionJointInfo* joint_info)
{
	SrQuat quat;
	for(int i = 0; i < joint_info->joint_num; ++i)
	{
		std::string name = joint_info->joint_name[i];
		if(joint_info->quat_valid.get(i) == 1)
		{
			request_channels.add( name, SkChannel::Quat );
		}
		else joint_info->quat.set(i, quat);
	}
}

// Implements MeController::context_updated(..).
void MeCtLocomotion::context_updated() 
{
	if( _context == NULL )
		channels_valid = false;
	else
		navigation_circle.setContext(_context);
}

int MeCtLocomotion::check_limb_controller_map_updated()
{
	int index = 0;
	int k = 0;
	for(int j = 0; j < limb_list.size(); ++j)
	{
		MeCtLocomotionLimb* limb = limb_list.get(j);

		if(!limb->limb_joint_info.joints_indexed)
		{
			for(int i = 0; i < limb->limb_joint_info.joint_num; ++i)
			{
				if(limb->limb_joint_info.quat_valid.get(i) == 0) 
				{
					limb->limb_joint_info.buff_index.set(i, -1);
					continue;
				}
				index = LOOKUP_BUFFER_INDEX( index,  k+joint_channel_start_ind );
				if(index < 0)
				{
					LOG("\ni=%d failed to look up buffer index", k);
				}
				limb->limb_joint_info.buff_index.set(i, index);
				++k;
			}
			limb->limb_joint_info.joints_indexed = true;
		}
	}
	return k;
}

void MeCtLocomotion::controller_map_updated() 
{
	int index = 0;
	int k = 0;

	k = check_limb_controller_map_updated();

	if(!nonlimb_joints_indexed)
	{
		channels_valid = navigator.controller_map_updated(_context, &_toContextCh);

		for(int i = 0; i < nonlimb_joint_info.joint_num; ++i)
		{
			if(nonlimb_joint_info.quat_valid.get(i) == 0) 
			{
				nonlimb_joint_info.buff_index.set(i, -1);
				continue;
			}
			index = LOOKUP_BUFFER_INDEX( index,  k+joint_channel_start_ind);
			if(index < 0)
			{
				LOG("\ni=%d failed to look up buffer index", i);
			}
			nonlimb_joint_info.buff_index.set(i, index);
			++k;
		}
		nonlimb_joints_indexed = true;
	}

}

//temp function
void MeCtLocomotion::temp_update_for_footprint(MeFrameData& frame)
{
	MeCtLocomotionLimb* limb = NULL;
	int index;
	SrQuat quat;
	SrVec pos;
	SrQuat quat_buff;
	SrBuffer<float>& buffer = frame.buffer(); // convenience reference;

	navigator.controller_evaluate(0.0f, frame);

	for(int i = 0; i < nonlimb_joint_info.buff_index.size(); ++i)
	{
		index = nonlimb_joint_info.buff_index.get(i);
		if(index < 0) continue;
		quat = nonlimb_joint_info.quat.get(i);

		quat_buff.w = buffer[index+0];
		quat_buff.x = buffer[index+1];
		quat_buff.y = buffer[index+2];
		quat_buff.z = buffer[index+3];

		nonlimb_joint_info.quat.set(i, quat_buff);
	}

	update_nonlimb_mat(NULL, NULL, 0);

	for(int j = 0; j < limb_list.size(); ++j)
	{
		limb = limb_list.get(j);
		for(int i = 0; i < limb->limb_joint_info.buff_index.size(); ++i)
		{
			index = limb->limb_joint_info.buff_index.get(i);
			if(index < 0) continue;
			quat = limb->limb_joint_info.quat.get(i);

			quat_buff.w = buffer[index+0];
			quat_buff.x = buffer[index+1];
			quat_buff.y = buffer[index+2];
			quat_buff.z = buffer[index+3];

			limb->limb_joint_info.quat.set(i, quat_buff);
		}
		get_limb_pos(limb);
	}
	calc_rotational_displacement();
	for(int j = 0; j < limb_list.size(); ++j)
	{
		limb = limb_list.get(j);
		for(int i = 0; i < limb->pos_buffer.size(); ++i)
		{
			pos = limb->pos_buffer.get(i);
			pos += navigator.get_world_pos();
			pos -= world_offset_to_base;
			limb->pos_buffer.set(i, pos);
		}
	}

}

bool MeCtLocomotion::is_motions_loaded()
{
	return motions_loaded;
}

bool MeCtLocomotion::controller_evaluate( double time, MeFrameData& frame ) 
{
	return false;

	navigation_circle.controller_evaluate(time, frame);

	if(!valid) return false;
	if(!channels_valid ) return false;

	if(!enabled) return false;
	if(!motions_loaded) return motions_loaded;

	if(anim_global_info.size() < 2)// if the locomotion is enabled but no walking animation is loaded, update for kinematic footprints
	{
		temp_update_for_footprint(frame);
		return false;
	}

	check_limb_controller_map_updated();

	delta_time = time - last_time;

	if(delta_time > 0.03333333f) delta_time = 0.03333333f;

	if(freezed) 
	{
		delta_time = freeze_delta_time;
		freeze_delta_time = 0.00f;
	}

	SrBuffer<float>& buffer = frame.buffer(); // convenience reference

	navigator.controller_evaluate(delta_time, frame);

	float inc_frame = (float)(delta_time/0.03333333f);

	//if current destination is not the final target
	if(navigator.has_destination && navigator.get_destination_count() > navigator.get_curr_destination_index() && navigator.get_curr_destination_index() >= 0)
	{
		SrVec dis_to_dest = navigator.get_dis_to_dest();
		if(2.0f * dis_to_dest.len() * abs(speed_accelerator.get_min_acceleration_neg()) <= speed_accelerator.get_curr_speed()*speed_accelerator.get_curr_speed())
		{
			if(navigator.get_destination_count() == navigator.get_curr_destination_index()+1) 
				navigator.set_reached_destination(frame);
			else navigator.next_destination(frame);
		}
	}

	MeCtLocomotionLimb* limb;
	if(navigator.get_local_velocity().len() != 0.0f)
	{
		for(int i = 0; i < limb_list.size(); ++i)
		{
			limb = limb_list.get(i);
			if(navigator.has_destination) 
			{
				SrVec v = navigator.get_dis_to_dest_local();
				limb->direction_planner.set_target_direction( &v );
			}
			else 
			{
				SrVec v = navigator.get_target_local_velocity();
				limb->direction_planner.set_target_direction( &v );
			}
		}
	}
	speed_accelerator.set_target_speed(navigator.get_target_local_velocity().len());

	update(inc_frame, frame);
	if(ik_enabled) apply_IK(); 

	//balance control
	SrVec sr_tmp_vec(0.0f,1.0f,0.0f);
	balance.update(limb_list, sr_tmp_vec, &nonlimb_joint_info, navigator.get_orientation_angle(), translation_joint_index, (float)delta_time);

	navigator.post_controller_evaluate(frame, limb_list.get(dominant_limb), reset);

	reset = false;

	int index = 0;
	SrQuat quat;
	SrQuat quat_buff;

	// update joint values in channel buffer
	for(int i = 0; i < limb_list.size(); ++i)
	{
		MeCtLocomotionJointInfo* info = &(limb_list.get(i)->limb_joint_info);
		for(int j = 0; j < info->quat.size(); ++j)
		{
			index = info->buff_index.get(j);
			if(index < 0) continue;
			quat = info->quat.get(j);

			buffer[index+0] = (float)quat.w;
			buffer[index+1] = (float)quat.x;
			buffer[index+2] = (float)quat.y;
			buffer[index+3] = (float)quat.z;
		}
	}

	for(int i = 0; i < nonlimb_joint_info.joint_num; ++i)
	{
		index = nonlimb_joint_info.buff_index.get(i);
		if(index < 0) continue;
		quat = nonlimb_joint_info.quat.get(i);

		buffer[index+0] = (float)quat.w;
		buffer[index+1] = (float)quat.x;
		buffer[index+2] = (float)quat.y;
		buffer[index+3] = (float)quat.z;
	}
	
	last_time = time;
	return true;
}

bool MeCtLocomotion::is_initialized()
{
	return initialized;
}

bool MeCtLocomotion::is_enabled()
{
	return enabled;
}

bool MeCtLocomotion::is_valid()
{
	return valid;
}

bool MeCtLocomotion::is_channels_valid()
{
	return channels_valid;
}

SrArray<MeCtLocomotionLimb*>* MeCtLocomotion::get_limb_list()
{
	return &limb_list;
}

void MeCtLocomotion::set_balance_factor(float factor)
{
	balance.set_factor(factor);
}

SrVec MeCtLocomotion::calc_rotational_displacement()
{
	SrVec v;
	SkJoint* tjoint = walking_skeleton->search_joint(nonlimb_joint_info.joint_name[0].c_str());
	SrMat pmat;

	v = -get_offset(walking_skeleton->root(), translation_joint_index+1, nonlimb_joint_info.quat);

	v -= navigator.get_base_pos();

	pmat.roty(navigator.get_orientation_angle());

	world_offset_to_base = v*pmat;
	v = world_offset_to_base - pre_world_offset_to_base;
	v.y = 0.0f;

	pre_world_offset_to_base = world_offset_to_base;

	if(!dis_initialized) 
	{
		v.set(0.0f, 0.0f, 0.0f);
		return v;
	}
	return v;
}

SrVec MeCtLocomotion::get_base_pos()
{
	return navigator.get_world_pos()-world_offset_to_base;
}

void MeCtLocomotion::init_skeleton(SkSkeleton* standing, SkSkeleton* walking)
{
	walking_skeleton = walking;
	standing_skeleton = standing;
	analyzer.set_skeleton(walking_skeleton, standing_skeleton);
	initialized = true;
}

const std::string& MeCtLocomotion::controller_type( void )	const {

	return TYPE;
}

std::string& MeCtLocomotion::get_translation_joint_name()
{
	return translation_joint_name;
}

MeCtLocomotionTerrain* MeCtLocomotion::get_terrain()
{
	return &terrain;
}

void MeCtLocomotion::set_turning_speed(float radians)
{
	for(int i = 0; i < limb_list.size(); ++i)
	{
		limb_list.get(i)->direction_planner.set_turning_speed(radians);
	}
}

int MeCtLocomotion::determine_dominant_limb()
{
	SrVec pos;
	SrMat mat;
	mat.rot(navigator.get_local_velocity(), SrVec(0,0,1));
	int min_ind = -1;
	float min = 0.0f;
	for(int i = 0; i < limb_list.size(); ++i)
	{
		pos = get_limb_pos(limb_list.get(i));
		pos = pos*mat;
		if(min_ind == -1 || pos.z < min) 
		{
			min_ind = i;
			min = pos.z;
		}
		/*else
		{
			if(pos.z < min) 
			{
				min_ind = i;
				min = pos.z;
			}
		}*/
	}
	return min_ind;
}

// blend the height of base joint from the source animations, no IK taken into consideration
void MeCtLocomotion::blend_base_joint(MeFrameData& frame, float space_time, int anim_index1, int anim_index2, float weight)
{
	pre_blended_base_height = r_blended_base_height;

	SrQuat rot1, rot2, rot3, rot4;
	float ratio = 0.0f;
	float pheight = 0.0f;
	std::string translate_base;
	SkJoint* base;

	MeCtLocomotionLimbAnim* anim1 = limb_list.get(0)->walking_list.get(anim_index1);
	MeCtLocomotionLimbAnim* anim2 = limb_list.get(0)->walking_list.get(anim_index2);
	float frame1 = anim1->get_timing_space()->get_virtual_frame(space_time);
	float frame2 = anim2->get_timing_space()->get_virtual_frame(space_time);

	int t_frame1;
	int t_frame2;

	translate_base = nonlimb_joint_info.joint_name[translation_joint_index];
	base = walking_skeleton->search_joint(translate_base.c_str());

	t_frame1 = (int)frame1;
	t_frame2 = (int)frame1+1;
	if(t_frame2 >= anim1->walking->frames()) t_frame2 = 0;

	anim1->walking->connect(walking_skeleton);
	ratio = frame1 - t_frame1;

	anim1->apply_frame(t_frame1);
	pheight = base->pos()->value(1);
	r_blended_base_height = pheight*(1.0f-ratio)*(weight);

	anim1->apply_frame(t_frame2);
	pheight = base->pos()->value(1);
	r_blended_base_height += pheight*ratio*(weight);

	r_blended_base_height += anim_global_info.get(anim_index1)->height_offset * weight;

	t_frame1 = (int)frame2;
	t_frame2 = (int)frame2+1;
	if(t_frame2 >= anim2->walking->frames()) t_frame2 = 0;

	anim2->walking->connect(walking_skeleton);
	ratio = frame2 - t_frame1;

	anim2->apply_frame(t_frame1);
	pheight = base->pos()->value(1);
	r_blended_base_height += pheight*(1.0f-ratio)*(1.0f-weight);

	anim2->apply_frame(t_frame2);
	pheight = base->pos()->value(1);
	r_blended_base_height += pheight*ratio*(1.0f-weight);

	r_blended_base_height += anim_global_info.get(anim_index2)->height_offset * (1.0f - weight);

	SrBuffer<float>& buffer = frame.buffer();

	SrVec base_pos = navigator.get_base_pos();
	base->pos()->value(0, base_pos.x);
	base->pos()->value(1, base_pos.y);
	base->pos()->value(2, base_pos.z);

	r_blended_base_height = r_blended_base_height * (navigator.limb_blending_factor) + base_pos.y * (1.0f-navigator.limb_blending_factor);

}

void MeCtLocomotion::set_freeze(bool freeze)
{
	freezed = freeze;
}

bool MeCtLocomotion::is_freezed()
{
	return freezed;
}

// main stream
void MeCtLocomotion::update(float inc_frame, MeFrameData& frame)
{
	if(inc_frame < 0) inc_frame = 0.0f;
	float frame_num = 0.0f; 
	float ratio = 0.0f;
	float dom_ratio = 0.0f;

	// set the limb behind the dominant limb, clear out dirty data and the the space value to 0 when starting the locomotion.
	if(navigator.limb_blending_factor == 0.0f)
	{
		//if starting locomotion from standing pose
		if(speed_accelerator.get_target_speed() != 0.0f)
		{
			dominant_limb = determine_dominant_limb();
			limb_list.get(dominant_limb)->set_space_time(0.0f);
		}
		else
		{
			for(int i = 0; i < limb_list.size(); ++i)
			{
				limb_list.get(i)->direction_planner.reset();
			}
		}
		speed_accelerator.clear_acceleration();
		idle = true;

	}

	//get current direction
	limb_list.get(dominant_limb)->direction_planner.update_direction(delta_time, limb_list.get(dominant_limb)->get_space_time_pt(), 3, true);

	// set r_anim1_index_dominant amd r_anim2_index_dominant
	get_anim_indices(dominant_limb, limb_list.get(dominant_limb)->direction_planner.get_curr_direction(), &r_anim1_index_dominant, &r_anim2_index_dominant);

	if(r_anim1_index_dominant < 0 && r_anim2_index_dominant < 0) return;

	MeCtLocomotionLimbAnim* anim1 = NULL;
	if(r_anim1_index_dominant >= 0) anim1 = limb_list.get(dominant_limb)->get_walking_list()->get(r_anim1_index_dominant);
	MeCtLocomotionLimbAnim* anim2 = NULL;
	if(r_anim2_index_dominant >= 0) anim2 = limb_list.get(dominant_limb)->get_walking_list()->get(r_anim2_index_dominant);
	MeCtLocomotionLimbAnim* blended_anim = &limb_list.get(dominant_limb)->blended_anim;

	MeCtLocomotionTimingSpace* space1 = NULL;
	if(anim1) space1 = anim1->get_timing_space();
	MeCtLocomotionTimingSpace* space2 = NULL;
	if(anim2) space2 = anim2->get_timing_space();

	// get the ratio of the two animations
	dom_ratio = limb_list.get(dominant_limb)->direction_planner.get_ratio(anim1, anim2);
	get_blended_timing_space(blended_anim->get_timing_space(), space1, space2, dom_ratio);

	if(navigator.limb_blending_factor == 0.0f)
	{
		speed_accelerator.update_acceleration(limb_list.get(dominant_limb), blended_anim->get_timing_space());
	}

	//if(speed_accelerator.get_target_speed() > speed_accelerator.get_curr_speed() || limb_list.get(dominant_limb)->space_time > 2.0f) 
	speed_accelerator.update_speed(delta_time);

	// get current acceleration
	SrVec v = limb_list.get(dominant_limb)->direction_planner.get_curr_direction();
	float acc = speed_accelerator.update(&v, limb_list.get(dominant_limb));

	navigator.update_framerate_accelerator(acc, &limb_list);

	//the current frame number is the addition of previous frame number + increased frame num * acceleration
	frame_num = blended_anim->get_timing_space()->get_virtual_frame(limb_list.get(dominant_limb)->get_space_time()) + inc_frame * navigator.framerate_accelerator;
	frame_num = blended_anim->get_timing_space()->get_normalized_frame(frame_num); // in case new frame number is beyond the range. 


	//space time was computed with current frame number
	if(navigator.limb_blending_factor != 0.0f) 
		limb_list.get(dominant_limb)->set_space_time(blended_anim->get_timing_space()->get_space_value(frame_num));
	
	// update the current orientation of dominant limb
	navigator.update_facing(limb_list.get(dominant_limb), true);



	// blend the two animations
	limb_list.get(dominant_limb)->blend_anim(limb_list.get(dominant_limb)->get_space_time(), r_anim1_index_dominant, r_anim2_index_dominant, dom_ratio, &(limb_list.get(dominant_limb)->limb_joint_info.joint_index));

	// update acceleration based on current timing space and space time.
	speed_accelerator.update_acceleration(limb_list.get(dominant_limb), blended_anim->get_timing_space());
 
	for(int i = 0; i < limb_list.size(); ++i)
	{
		if(i != dominant_limb)
		{
			//the following code computes the space value in accordance with that of the dominant limb:
			//get a subordinate timing space which has the same scale as the dominant timing space,
			//then map the frame number onto the space value of this timing space.
			//the reason for this is that provided the freedom of all the limbs, the animation can be 
			//unsmooth if frame number is not mapped properly onto the dominant limb's timing space
			get_anim_indices(i, limb_list.get(dominant_limb)->direction_planner.get_curr_direction(), &r_anim1_index, &r_anim2_index);
			anim1 = limb_list.get(i)->get_walking_list()->get(r_anim1_index);
			anim2 = limb_list.get(i)->get_walking_list()->get(r_anim2_index);
			blended_anim = &limb_list.get(i)->blended_anim;
			get_blended_timing_space(blended_anim->get_timing_space(), anim1->get_timing_space(), anim2->get_timing_space(), dom_ratio);
			
			if(navigator.limb_blending_factor != 0.0f) 
				limb_list.get(i)->set_space_time(blended_anim->get_timing_space()->get_space_value(frame_num));
			
			//compute the direction and orientation based on the real timing space
			limb_list.get(i)->direction_planner.update_direction(delta_time, limb_list.get(i)->get_space_time_pt(), blended_anim->get_timing_space()->get_ref_time_num(), false);
			get_anim_indices(i, limb_list.get(i)->direction_planner.get_curr_direction(), &r_anim1_index, &r_anim2_index);
			anim1 = limb_list.get(i)->get_walking_list()->get(r_anim1_index);
			anim2 = limb_list.get(i)->get_walking_list()->get(r_anim2_index);
			ratio = limb_list.get(i)->direction_planner.get_ratio(anim1, anim2);
			get_blended_timing_space(blended_anim->get_timing_space(), anim1->get_timing_space(), anim2->get_timing_space(), ratio);
			navigator.update_facing(limb_list.get(i), false);
			limb_list.get(i)->blend_anim(limb_list.get(i)->get_space_time(), r_anim1_index, r_anim2_index, ratio, &(limb_list.get(i)->limb_joint_info.joint_index));
		}
	}

	//blend non-limb joints
	anim1 = limb_list.get(0)->get_walking_list()->get(r_anim1_index_dominant);
	anim2 = limb_list.get(0)->get_walking_list()->get(r_anim2_index_dominant);
	get_frame(locomotion_anims.get(r_anim1_index_dominant - 1), walking_skeleton, anim1->get_timing_space()->get_virtual_frame(limb_list.get(0)->get_space_time()), base_name, &joint_quats1, &t_joint_quats1, &t_joint_quats2, &(nonlimb_joint_info.joint_index));
	get_frame(locomotion_anims.get(r_anim2_index_dominant - 1), walking_skeleton, anim2->get_timing_space()->get_virtual_frame(limb_list.get(0)->get_space_time()), base_name, &joint_quats2, &t_joint_quats1, &t_joint_quats2, &(nonlimb_joint_info.joint_index));
	get_blended_quat_buffer(&(nonlimb_joint_info.quat), &joint_quats1, &joint_quats2, dom_ratio);

	//recompute the dominant limb
	determine_dominant_limb_index();

	//blend with standing animation if standing factor > 0
	blend_standing(frame);

	for(int i = 0; i < limb_list.size(); ++i)
	{
		if(limb_list.get(i)->curr_rotation == 0.0f) continue;
		SkJoint* tjoint = walking_skeleton->search_joint(limb_list.get(i)->get_limb_base_name().c_str());
		int parent_ind = nonlimb_joint_info.get_index_by_name(tjoint->parent()->name().c_str());
		SrMat mat = nonlimb_joint_info.mat.get(parent_ind);
		limb_list.get(i)->manipulate_turning(mat);
	}

	last_time = limb_list.get(dominant_limb)->get_space_time();

	blend_base_joint(frame, limb_list.get(0)->get_space_time(), r_anim1_index_dominant, r_anim2_index_dominant, dom_ratio);

	update_nonlimb_mat(NULL, NULL, 0);

	update_pos();

	navigator.update_world_offset(displacement);

	navigator.update_world_mat();
	
	//if(ik_enabled)
	{
		height_offset.set_limb_list(&limb_list);
		height_offset.set_translation_base_joint_height(translation_joint_height);
		//SrVec displacement(0,0,0);
		SrMat w_mat = navigator.get_world_mat();

		w_mat.set(12, w_mat.get(12)-world_offset_to_base.x);
		w_mat.set(13, w_mat.get(13)-world_offset_to_base.y);
		w_mat.set(14, w_mat.get(14)-world_offset_to_base.z);
		//SrMat w_mat = get_gmat(walking_skeleton->root(), translation_joint_index+1, nonlimb_joint_info.quat);
		//w_mat = w_mat * navigator.get_world_mat();

		//height_offset.update_height_offset(nonlimb_joint_info.mat.get(translation_joint_index) * w_mat, r_blended_base_height);
		height_offset.update_height_offset(w_mat, r_blended_base_height, (float)delta_time);
		w_mat.set(13, navigator.get_world_mat().get(13) + height_offset.get_height_offset());
		navigator.set_world_mat(w_mat);
		navigator.world_pos.y += height_offset.get_height_offset();
	}


	update_nonlimb_mat_with_global_info();

	//printf("\ndominant: %d", dominant_limb);

}

float MeCtLocomotion::get_current_speed()
{
	return speed_accelerator.get_curr_speed();
}

void MeCtLocomotion::update_nonlimb_mat_with_global_info()
{
	SrMat mat = navigator.get_world_mat();
	for(size_t i = 0; i < nonlimb_joint_info.joint_name.size(); ++i)
	{
		nonlimb_joint_info.mat.set(i, nonlimb_joint_info.mat.get(i)*mat);
	}
}

int MeCtLocomotion::get_dominant_limb_index()
{
	return dominant_limb;
}

void MeCtLocomotion::update_limb_mat_with_global_info()
{
	SrMat mat = navigator.get_world_mat();
	for(int j = 0; j < limb_list.size(); ++j)
	{
		MeCtLocomotionLimb* limb = limb_list.get(j);
		for(size_t i = 0; i < nonlimb_joint_info.joint_name.size(); ++i)
		{
			limb->limb_joint_info.mat.set(i, limb->limb_joint_info.mat.get(i)*mat);
		}
	}
}

// blend with standing animation (channel buffer)
void MeCtLocomotion::blend_standing(MeFrameData& frame)
{
	if(navigator.limb_blending_factor == 1.0f) return;
	MeCtLocomotionLimb* limb = NULL;
	int index;
	SrQuat quat;
	SrQuat quat_buff;
	SrBuffer<float>& buffer = frame.buffer();
	for(int j = 0; j < limb_list.size(); ++j)
	{
		limb = limb_list.get(j);
		for(int i = 0; i < limb->limb_joint_info.buff_index.size(); ++i)
		{
			index = limb->limb_joint_info.buff_index.get(i);
			if(index < 0) continue;
			quat = limb->limb_joint_info.quat.get(i);

			quat_buff.w = buffer[index+0];
			quat_buff.x = buffer[index+1];
			quat_buff.y = buffer[index+2];
			quat_buff.z = buffer[index+3];

			quat_buff = slerp(quat_buff, quat, navigator.limb_blending_factor);
			limb->limb_joint_info.quat.set(i, quat_buff);
		}
	}
	for(int i = 0; i < nonlimb_joint_info.buff_index.size(); ++i)
	{
		index = nonlimb_joint_info.buff_index.get(i);
		if(index < 0) continue;
		quat = nonlimb_joint_info.quat.get(i);

		quat_buff.w = buffer[index+0];
		quat_buff.x = buffer[index+1];
		quat_buff.y = buffer[index+2];
		quat_buff.z = buffer[index+3];

		quat_buff = slerp(quat_buff, quat, navigator.limb_blending_factor);
		nonlimb_joint_info.quat.set(i, quat_buff);
	}
}

void MeCtLocomotion::update_nonlimb_mat(SkJoint* joint, SrMat* mat, int depth)
{
	if(joint == NULL) joint = walking_skeleton->root();
	//SkJoint* tjoint = NULL;
	int index = -1;
	SrMat lmat;
	SrMat gmat;
	SrQuat quat;
	if(mat == NULL) mat = &gmat;

	for(int j = 0; j < limb_list.size(); ++j)
	{
		if(joint->name() == limb_list.get(j)->limb_base_name) 
		{
			return;
		}
	}

	index = nonlimb_joint_info.get_index_by_name(joint->name().c_str());
	if(nonlimb_joint_info.mat_valid.get(index) != 1) return;
	if(nonlimb_joint_info.quat_valid.get(index) == 1) quat = nonlimb_joint_info.quat.get(index);
	lmat = get_lmat(joint, &quat);
	if(depth <= translation_joint_index)
	{
		lmat.set(12, 0.0f);
		lmat.set(13, 0.0f);
		lmat.set(14, 0.0f);
	}
	gmat = lmat * *mat;
	nonlimb_joint_info.mat.set(index, gmat);
	for(int i = 0; i < joint->num_children(); ++i)
	{
		update_nonlimb_mat(joint->child(i), &gmat, depth+1);
	}
}

void MeCtLocomotion::set_target_height_displacement(float displacement)
{
	navigator.target_height_adjustment = displacement;
}

// temp function
SrVec MeCtLocomotion::get_supporting_joint_pos(int joint_index, int limb_index, SrVec* orientation, SrVec* normal)
{
	//SrVec pos = limb_list.get(index)->pos_buffer.get(2) * navigator.get_world_mat();
	//pos.y -= limb_list.get(index)->support_height.get(0);

	int joint_index_plus = joint_index+1;
	if(joint_index_plus >= limb_list.get(limb_index)->support_joint_list.size())
	{
		joint_index_plus = joint_index-1;
	}
	float tnormal[3];
	mcuCBHandle& mcu = mcuCBHandle::singleton();
	SrVec pos = limb_list.get(limb_index)->pos_buffer.get(2+joint_index);
	//float height = mcu.query_terrain(pos.x, pos.z, tnormal);

	float height = terrain.get_height(pos.x, pos.z, tnormal);
	pos.y = height;
	SrVec pos1 = limb_list.get(limb_index)->pos_buffer.get(2+joint_index_plus);

	//height = mcu.query_terrain(pos1.x, pos1.z, NULL);
	height = terrain.get_height(pos1.x, pos1.z, NULL);

	pos1.y = height;
	if(joint_index_plus > joint_index) *orientation = pos1-pos;
	else *orientation = pos-pos1;
	normal->set(tnormal[0], tnormal[1], tnormal[2]);
	return pos;
}

// temp function
void MeCtLocomotion::apply_IK()
{
	//if(navigator.target_height_displacement == 0.0f) return;
	MeCtIKScenario* ik_scenario = NULL;
	MeCtIKScenarioJointInfo* info = NULL;
	mcuCBHandle& mcu = mcuCBHandle::singleton();

	SrMat global_mat;
	global_mat = navigator.get_world_mat();

	global_mat.set(12, global_mat.get(12) - world_offset_to_base.x);
	global_mat.set(13, global_mat.get(13) - world_offset_to_base.y);
	global_mat.set(14, global_mat.get(14) - world_offset_to_base.z);
	//global_mat = get_gmat(walking_skeleton->root(), translation_joint_index+1, nonlimb_joint_info.quat);

	//global_mat = global_mat * navigator.get_world_mat();

	for(int i = 0; i < limb_list.size(); ++i)
	{
		ik_scenario = &(limb_list.get(i)->ik);

		//temp...............
		//ik_scenario->joint_info_list.get(2).constraint.ball.max = 3.14159265f/4.0f;
		//temp...............

		SkJoint* tjoint = walking_skeleton->search_joint(limb_list.get(i)->get_limb_base_name().c_str());
		int parent_ind = nonlimb_joint_info.get_index_by_name(tjoint->parent()->name().c_str());
		ik_scenario->gmat = nonlimb_joint_info.mat.get(parent_ind);
		ik_scenario->gmat.set(12, ik_scenario->gmat.get(12) - world_offset_to_base.x);
		ik_scenario->gmat.set(13, ik_scenario->gmat.get(13) - world_offset_to_base.y);
		ik_scenario->gmat.set(14, ik_scenario->gmat.get(14) - world_offset_to_base.z);
		ik_scenario->start_joint = &(ik_scenario->joint_info_list.get(0));
		ik_scenario->end_joint = &(ik_scenario->joint_info_list.get(ik_scenario->joint_info_list.size()-1));
		
		SrVec pos = limb_list.get(i)->pos_buffer.get(2);
		pos  = pos * global_mat;

		float normal[3] = {0.0f, 0.0f, 0.0f};

		//float height = mcu.query_terrain(pos.x, pos.z, normal);

		float height = terrain.get_height(pos.x, pos.z, normal);

		ik_scenario->ik_offset = limb_list.get(i)->ik_offset;
		ik_scenario->joint_quat_list = limb_list.get(i)->limb_joint_info.quat;
		ik_scenario->ik_orientation.set(0.0f, -1.0f, 0.0f);
		ik_scenario->plane_normal = SrVec(limb_list.get(i)->ik_terrain_normal.x, limb_list.get(i)->ik_terrain_normal.y, limb_list.get(i)->ik_terrain_normal.z);
		ik_scenario->plane_point = SrVec(pos.x, height, pos.z);

		SrVec v1 = -limb_list.get(i)->ik_terrain_normal;
		ik_scenario->ik_compensate_factor = dot(v1, ik_scenario->ik_orientation);
		for(int j = 0; j < ik_scenario->joint_info_list.size(); ++j)
		{
			info = &(ik_scenario->joint_info_list.get(j));
			info->support_joint_comp = translation_joint_height + r_blended_base_height + limb_list.get(i)->pos_buffer.get(j).y - info->support_joint_height;
			if(info->support_joint_comp < 0.0f) info->support_joint_comp = 0.0f;// manually check if the compensation is valid
		}

		ik.update(ik_scenario);
		limb_list.get(i)->limb_joint_info.quat = ik_scenario->joint_quat_list;
		limb_list.get(i)->pos_buffer = ik_scenario->joint_pos_list;
	}
}

void MeCtLocomotion::get_anim_indices(int limb_index, SrVec direction, int* anim1_index, int* anim2_index)
{
	MeCtLocomotionLimb* limb = limb_list.get(limb_index);
	float angle1 = -1.0f, angle2 = -1.0f;
	float angle = 0.0f;
	SrVec dir1(0.0f, 1.0f, 0.0f);
	SrVec dir2(0.0f, -1.0f, 0.0f);
	SrVec d;
	int mode1 = 0, mode2 = 0;
	int index1  = -1, index2 = -1;
	for(int i = 1; i < limb->get_walking_list()->size(); ++i)
	{
		MeCtLocomotionLimbAnim* anim = limb->get_walking_list()->get(i);
		if(style != anim->style) continue;
		d = cross(direction, anim->global_info->direction);
		angle = dot(direction, anim->global_info->direction);
		if(dot(d, dir1) > 0.0f)
		{
			if(angle1 < angle)
			{
				angle1 = angle;
				mode1 = 1;
				index1 = i;
			}
			if(angle2 < -angle)
			{
				angle2 = -angle;
				mode2 = -1;
				index2 = i;
			}
		}
		else
		{
			if(angle1 < -angle)
			{
				angle1 = -angle;
				mode1 = -1;
				index1 = i;
			}
			if(angle2 < angle)
			{
				angle2 = angle;
				mode2 = 1;
				index2 = i;
			}
		}
	}

	*anim1_index = index1;
	*anim2_index = index2;
	if(index1 >= 0) limb->get_walking_list()->get(index1)->get_timing_space()->set_mode(mode1);
	if(index2 >= 0) limb->get_walking_list()->get(index2)->get_timing_space()->set_mode(mode2);
}

int MeCtLocomotion::determine_dominant_limb_index()
{
	if(navigator.limb_blending_factor == 0.0f)
	{
		++dominant_limb;
		if(dominant_limb >= limb_list.size()) dominant_limb = 0;
		return dominant_limb;
	}
	float remnant = 0.0f;
	float r = -1;
	for(int i = 0; i < limb_list.size(); ++i)
	{
		if(limb_list.get(i)->get_space_time() > 1.0f && limb_list.get(i)->get_space_time() < 2.0f) continue;
		else if(limb_list.get(i)->get_space_time() <= 1.0f) 
		{
			r = 1- limb_list.get(i)->get_space_time();
		}
		else if(limb_list.get(i)->get_space_time() >= 2.0f) 
		{
			r = limb_list.get(i)->get_space_time() - 1;
		}
		if(r < 0.0f) 
		{
			LOG("Error: can not determine dominant limb. \n space_time_limb1=%f\nspace_time_limb2=%f", limb_list.get(0)->get_space_time(), limb_list.get(1)->get_space_time());
			r = 0;
		}
		if(r > remnant) 
		{
			remnant = r;
			dominant_limb = i;
		}
	}

	return dominant_limb;
}

SrVec MeCtLocomotion::get_limb_pos(MeCtLocomotionLimb* limb)
{
	SrMat gmat;
	SrMat pmat;
	SrMat lmat;
	SrVec pos;
	SkJoint* tjoint = NULL;
	SkJoint* tjoint_base = NULL;
	//float* ppos;
	SkSkeleton* skeleton = limb->walking_skeleton;

	tjoint = skeleton->search_joint(limb->get_limb_base_name().c_str());
	tjoint_base = skeleton->search_joint(tjoint->parent()->name().c_str());
	int parent_ind = nonlimb_joint_info.get_index_by_name(tjoint->parent()->name().c_str());
	gmat = nonlimb_joint_info.mat.get(parent_ind);

	for(int j  = 0; j <= limb->limb_joint_info.quat.size()-1; ++j)
	{
		pmat = gmat;
		lmat = get_lmat(tjoint, &(limb->limb_joint_info.quat.get(j)));
		gmat = lmat * pmat;
		pos.set(*gmat.pt(12), *gmat.pt(13), *gmat.pt(14));
		limb->pos_buffer.set(j, pos);
		if(tjoint->num_children()>0)
		{ 
			tjoint = tjoint->child(0);
		}
		else break;
	}

	pos.set(*gmat.pt(12), *gmat.pt(13), *gmat.pt(14));

	return pos;
}

void MeCtLocomotion::update_pos()
{
	SkJoint* tjoint = NULL;
	SrVec currpos;
	
	SrVec dis[2];//temp;
	float ratio[2];//temp
	float sum = 0.0f;
	displacement.set(0,0,0);
	int y = 0;
	for(int i = 0; i < limb_list.size(); ++i)
	{
		MeCtLocomotionLimb* limb = limb_list.get(i);
		currpos = get_limb_pos(limb);
		ratio[i] = 0;
		dis[i].set(0,0,0);
		//if(i == dominant_limb)
		if((limb->get_space_time() >= 2.0f || limb->get_space_time() <= 1.0f))
		{
			if(limb->get_space_time() >= 2.0f) ratio[i] = limb->get_space_time() - 2.0f;
			else if(limb->get_space_time() <= 1.0f) ratio[i] = 1.0f - limb->get_space_time();
			//ratio[i] *= ratio[i];
			SrMat mat;

			mat.roty(navigator.get_pre_facing_angle());

			SrVec v1 = limb->pos * mat;

			mat.roty(navigator.get_orientation_angle());

			SrVec v2 = currpos * mat;

			dis[i] = v1 - v2;

			dis[i].y = 0.0f;
			sum += ratio[i];
			++y;
		}
		limb->pos = currpos;

	}

	if(dis_initialized)
	{
		if(sum != 0.0f)
		{
			for(int i = 0; i < 2; ++i)
			{
				displacement += dis[i]*ratio[i]/sum;
			}
			
			if(navigator.limb_blending_factor != 0.0f)
			{
				// update the ik_offset for each limb
				for(int i = 0; i < 2; ++i)
				{
					MeCtLocomotionLimb* limb = limb_list.get(i);
					if(limb->get_space_time() >= 2.0f || limb->get_space_time() <= 1.0f)
					{
						limb->ik_offset += (dis[i]-displacement)/3.0f;
						limb->ik_offset_record = limb->ik_offset;
						//printf("\n(%f, %f, %f)", limb->ik_offset.x, limb->ik_offset.y, limb->ik_offset.z);
					}
					else if(limb->get_space_time() > 1.0f && limb->get_space_time() <= 1.5f)
					{
						limb->ik_offset = limb->ik_offset_record * (1.5f - limb->get_space_time())*2.0f;
					}
					else
						limb->ik_offset.set(0,0,0);
				}
			}

			displacement += calc_rotational_displacement();
			//displacement.x = 0.0f;
			displacement.y = r_blended_base_height-pre_blended_base_height;
			//displacement.z = 0.0f;
		}
		else
		{
			LOG("No limb touches the ground");
		}
	}
	else dis_initialized = true;
	//navigator.update_displacement(&displacement);
}


MeCtLocomotionNavigator* MeCtLocomotion::get_navigator()
{
	return &navigator;
}

MeCtNavigationCircle* MeCtLocomotion::get_navigation_circle()
{
	return &navigation_circle;
}

MeCtLocomotionAnalysis* MeCtLocomotion::get_analyzer()
{
	return &analyzer;
}

void MeCtLocomotion::add_locomotion_anim(SkMotion* anim)
{
	locomotion_anims.push() = anim;
}

SrVec MeCtLocomotion::get_facing_vector()
{
	SrMat mat;
	float angle = navigator.get_orientation_angle();
	mat.roty(angle);
	SrVec direction(0.0, 0.0, 1.0f);
	direction = direction*mat;
	return direction;
}

void MeCtLocomotion::print_info(const std::string& name)
{
	LOG("Locomotion status of character: %s", name.c_str());
	LOG("Animations loaded:");
	for(int i = 0; i < locomotion_anims.size(); ++i)
	{
		LOG("\t[%d] %s", i, locomotion_anims.get(i)->getName().c_str());
	}

	LOG("Limbs:");
	LOG("  Total number: %d", limb_list.size());
	MeCtLocomotionLimb* limb;
	for(int i = 0; i < limb_list.size(); ++i)
	{
		limb = limb_list.get(i);
		LOG("\t%s:", limb->limb_name.get(0));
		//limb->print_info();
		LOG("\tSupport joints:");
		for(int j = 0; j < limb->get_support_joint_num(); ++j)
		{
			LOG("\t\t%s", (const char*)*(limb->support_joint_list.get(j)));
		}
	}

	if(initialized) LOG("Initialized: Yes");
	else LOG("Initialized: No");

	if(enabled) LOG("Enabled: Yes");
	else LOG("Enabled: No");
}
