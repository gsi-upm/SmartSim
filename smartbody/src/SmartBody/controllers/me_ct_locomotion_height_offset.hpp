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

#ifndef ME_CT_LOCOMOTION_HEIGHT_OFFSET_HPP
#define ME_CT_LOCOMOTION_HEIGHT_OFFSET_HPP

#include "controllers/me_ct_locomotion_limb.hpp"
#include "controllers/me_ct_locomotion_terrain.hpp"
#include "controllers/me_ct_locomotion_quadratic_synchronizer.hpp"

#pragma once

class MeCtLocomotionHeightOffset
{
protected:
	MeCtLocomotionQuadraticSynchronizer synchronizer;
	SrArray<MeCtLocomotionLimb*>* limb_list;
	float acceleration;

	float acceleration_max;
	float acceleration_min;

	float height_offset;
	float target_height_offset;
	bool height_offset_initialized;

	float translation_base_joint_height;

	SrArray<SrVec> joint_pos_prev;
	SrArray<SrVec> joint_pos_curr;

	MeCtLocomotionTerrain* terrain;

public:
	MeCtLocomotionHeightOffset();
	~MeCtLocomotionHeightOffset();

public:
	
	void set_limb_list(SrArray<MeCtLocomotionLimb*>* limb_list);
	void set_translation_base_joint_height(float height);
	void update_height_offset(SrMat& parent_mat, float base_height_displacement, float time);
	void update_supporting_joint_orientation();
	float get_height_offset();
	void set_target_height_offset(float offset);

	void set_terrain(MeCtLocomotionTerrain* terrain);

};



#endif // ME_CT_LOCOMOTION_HEIGHT_OFFSET_HPP
