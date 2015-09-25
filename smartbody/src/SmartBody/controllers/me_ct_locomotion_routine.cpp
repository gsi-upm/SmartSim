/*
 *  me_ct_locomotion_routine.cpp - part of SmartBody-lib's Motion Engine
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

#include "controllers/me_ct_locomotion_routine.hpp"

/** Constructor */
MeCtLocomotionRoutine::MeCtLocomotionRoutine() 
{
	life_time = 0.0f;
	elapsed_time = 0.0f;
	direction.set(0.0f, 0.0f, 0.0f);
	speed = 0.0f;
	type = ME_CT_LOCOMOTION_ROUTINE_TYPE_UNKNOWN; 
	global_rps = 0.0f;
	local_rps = 0.0f;
	local_angle = 0.0f;
	SrVec target;
}

/** Destructor */
MeCtLocomotionRoutine::~MeCtLocomotionRoutine() 
{
	// Nothing allocated to the heap
}
