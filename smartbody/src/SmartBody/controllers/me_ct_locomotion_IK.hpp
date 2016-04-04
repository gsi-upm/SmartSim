/*
 *  me_ct_locomotion_IK.hpp - part of SmartBody-lib's Motion Engine
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

#ifndef ME_CT_LOCOMOTION_IK_HPP
#define ME_CT_LOCOMOTION_IK_HPP

#include "controllers/me_ct_IK.hpp"
#include "controllers/me_ct_locomotion_terrain.hpp"



#pragma once

class MeCtLocomotionIK : public MeCtIK
{
protected:
	MeCtLocomotionTerrain* terrain;

public:
	MeCtLocomotionIK();
	~MeCtLocomotionIK();

public:

	void calc_target(SrVec& orientation, SrVec& offset);
	void set_terrain(MeCtLocomotionTerrain* terrain);

};



#endif // ME_CT_LOCOMOTION_TERRAIN_HPP
