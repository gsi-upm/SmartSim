/*
 *  me_ct_locomotion_terrain.hpp - part of SmartBody-lib's Motion Engine
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

#include "controllers/me_ct_locomotion_terrain.hpp"
#include "sbm/mcontrol_util.h"


MeCtLocomotionTerrain::MeCtLocomotionTerrain()
{
	ground_height = 0.0f;
}

MeCtLocomotionTerrain::~MeCtLocomotionTerrain()
{
}

float MeCtLocomotionTerrain::get_height(float x, float z, float* normal)
{
	mcuCBHandle& mcu = mcuCBHandle::singleton();
	if(mcu.height_field_p == NULL)
	{
		if(normal != NULL)
		{
			normal[0] = 0.0f;
			normal[1] = 1.0f;
			normal[2] = 0.0f;
		}
		return ground_height;
	}
	return mcu.query_terrain(x, z, normal);
}

void MeCtLocomotionTerrain::set_ground_height(float height)
{
	ground_height = height;
}
