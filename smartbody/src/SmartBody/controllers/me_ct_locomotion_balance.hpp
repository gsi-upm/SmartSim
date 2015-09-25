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

#ifndef ME_CT_LOCOMOTION_BALANCE_HPP
#define ME_CT_LOCOMOTION_BALANCE_HPP

#include "controllers/me_ct_locomotion_joint_info.hpp"
#include "controllers/me_ct_locomotion_limb.hpp"

#pragma once

class MeCtLocomotionBalance
{
protected:
	SrVec target_normal;
	SrVec normal;
	float rotational_speed;
	float factor;

public:
	MeCtLocomotionBalance();
	~MeCtLocomotionBalance();

public:
	void update(SrArray<MeCtLocomotionLimb*>& limb_list, SrVec& orientation, MeCtLocomotionJointInfo* nonlimb_joint_info, float facing, int translation_joint_index, float time);
	void calc_target_normal(SrArray<MeCtLocomotionLimb*>& limb_list, SrVec& orientation);
	void calc_normal(float time);
	void set_factor(float factor);
};



#endif // ME_CT_LOCOMOTION_BALANCE_HPP
