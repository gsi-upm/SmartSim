/*
 *  me_ct_locomotion_anim_global_info.cpp - part of SmartBody-lib's Motion Engine
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

#include "controllers/me_ct_locomotion_anim_global_info.hpp"


const char* MeCtLocomotionAnimGlobalInfo::TYPE = "MeCtLocomotionAnimGlobalInfo";

/** Constructor */
MeCtLocomotionAnimGlobalInfo::MeCtLocomotionAnimGlobalInfo() 
{
	_motion = NULL;
	direction.set(0.0f, 0.0f, 0.0f);		//global direction
	speed = 0.0f;			//global average walking speed of the animation.
	displacement = 0.0f;   //global displacement absolute value
	height_offset = 0.0f;  //global user-defined height offset
}

/** Destructor */
MeCtLocomotionAnimGlobalInfo::~MeCtLocomotionAnimGlobalInfo() 
{
	// Nothing allocated to the heap
}
