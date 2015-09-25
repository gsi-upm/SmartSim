/*
 *  me_ct_locomotion_height_offset.hpp - part of SmartBody-lib's Motion Engine
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

#include "controllers/me_ct_locomotion_height_offset.hpp"


MeCtLocomotionHeightOffset::MeCtLocomotionHeightOffset()
{
	target_height_offset = 0.0f;
	translation_base_joint_height = 0.0f;
	acceleration = 20.0f;
	height_offset = 0.0f;
	height_offset_initialized = false;
}

MeCtLocomotionHeightOffset::~MeCtLocomotionHeightOffset()
{
}

void MeCtLocomotionHeightOffset::set_translation_base_joint_height(float height)
{
	translation_base_joint_height = height;
}

void MeCtLocomotionHeightOffset::set_limb_list(SrArray<MeCtLocomotionLimb*>* limb_list)
{
	this->limb_list = limb_list;
}

void MeCtLocomotionHeightOffset::update_supporting_joint_orientation()
{
	MeCtLocomotionLimb* limb = NULL;
	SrVec axis;
	SrMat mat;
	for(int i = 0; i < limb_list->size(); ++i)
	{
		limb = limb_list->get(i);
		if(limb->ik_terrain_normal == limb->ik_terrain_target_normal) continue;
		if(limb->ik_terrain_normal.iszero())
		{
			limb->ik_terrain_normal = limb->ik_terrain_target_normal;
			continue;
		}
		if(dot(limb->ik_terrain_normal, limb->ik_terrain_target_normal) >= cos(0.02f))
		{
			limb->ik_terrain_normal = limb->ik_terrain_target_normal;
			continue;
		}
		axis = cross(limb->ik_terrain_normal, limb->ik_terrain_target_normal);
		mat.rot(axis, 0.02f);
		limb->ik_terrain_normal = limb->ik_terrain_normal * mat;
		limb->ik_terrain_normal.normalize();
	}
}

void MeCtLocomotionHeightOffset::update_height_offset(SrMat& parent_mat, float base_height_displacement, float time)
{
	SrVec wpos;
	float normal[3];
	float* height = (float*)malloc(limb_list->size()*sizeof(float));
	
	int min_index = -1;
	float min_height = 0.0f;
	target_height_offset = 0.0f;

	SrVec pos;
	SrVec base_pos;
	base_pos.set(parent_mat.get(12), parent_mat.get(13), parent_mat.get(14));

	MeCtLocomotionLimb* limb = NULL;

	for(int i = 0; i < limb_list->size(); ++i)
	{
		limb = limb_list->get(i);
		pos = limb->pos_buffer.get(2);
		wpos = pos * parent_mat;
		wpos += limb->ik_offset;

		height[i] = terrain->get_height(wpos.x, wpos.z, normal);

		limb->ik_terrain_target_normal.set(normal[0], normal[1], normal[2]);
		
		if(min_index < 0 || min_height > height[i]) 
		{
			min_index = i;
			min_height = height[i];
		}
	}

	update_supporting_joint_orientation();

	target_height_offset = -(base_pos.y - min_height - translation_base_joint_height - base_height_displacement);

	synchronizer.set_target_flag(QUAD_SYNC_SPEED, QUAD_SYNC_DISTANCE);
	synchronizer.update_target(QUAD_SYNC_DISTANCE, target_height_offset);
	synchronizer.update_target(QUAD_SYNC_SPEED, 0.0f);
	synchronizer.set_max_acceleration(600.0f);
	synchronizer.update(time);
	height_offset = synchronizer.get_delta_distance();

	//height_offset = target_height_offset;

	free(height);
}

/*void MeCtLocomotionHeightOffset::update(SrMat& parent_mat, float base_height_displacement)
{
	SrVec wpos;
	//mcuCBHandle& mcu = mcuCBHandle::singleton();
	float normal[3];
	float* height = (float*)malloc(limb_list->size()*sizeof(float));
	
	int min_index = -1;
	float min_height = 0.0f;
	height_offset = 0.0f;

	SrVec pos;
	SrVec base_pos;
	base_pos.set(parent_mat.get(12), parent_mat.get(13), parent_mat.get(14));

	for(int i = 0; i < limb_list->size(); ++i)
	{
		pos = limb_list->get(i)->pos_buffer.get(2);
		wpos = pos * parent_mat;
		height[i] = terrain.get_height(wpos.x, wpos.z, normal);
		//mcu.query_terrain(wpos.x, wpos.z, normal);
		
		if(min_index < 0 || min_height > height[i]) 
		{
			min_index = i;
			min_height = height[i];
		}
	}
	height_offset = -(base_pos.y - min_height - translation_base_joint_height - base_height_displacement);

	//printf("\n%f", height_offset);
	free(height);
}*/

float MeCtLocomotionHeightOffset::get_height_offset()
{
	return height_offset;
}

void MeCtLocomotionHeightOffset::set_target_height_offset(float offset)
{
	target_height_offset = offset;
}

void MeCtLocomotionHeightOffset::set_terrain(MeCtLocomotionTerrain* terrain)
{
	this->terrain = terrain;
}