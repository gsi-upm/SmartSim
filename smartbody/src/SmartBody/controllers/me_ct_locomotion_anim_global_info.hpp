/*
 *  me_ct_locomotion_anim_global_info.hpp - part of SmartBody-lib's Motion Engine
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

#ifndef ME_LOCOMOTION_ANIM_GLOBAL_INFO_HPP
#define ME_LOCOMOTION_ANIM_GLOBAL_INFO_HPP

#include <sk/sk_motion.h>

#pragma once

class MeCtLocomotionAnimGlobalInfo{
public:
	// Public Constants
	static const char* TYPE;

public:
	// Data
	SkMotion*			_motion;
	//SrArray<SrVec*>		velocity_list;
	SrVec				direction;		//global direction
	float				speed;			//global average walking speed of the animation.
	float				displacement;   //global displacement absolute value
	float				height_offset;  //global user-defined height offset

public:
	MeCtLocomotionAnimGlobalInfo();
	~MeCtLocomotionAnimGlobalInfo();

};

#endif // ME_LOCOMOTION_ANIM_GLOBAL_INFO_HPP
