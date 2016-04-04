/*
 *  me_utilities.hpp - part of SmartBody-lib
 *  Copyright (C) 2008  University of Southern California
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
 *      Andrew n marshall, USC
 *      Marcus Thiebaux, USC
 */

#ifndef ME_UTILITIES_HPP
#define ME_UTILITIES_HPP


#include <sk/sk_skeleton.h>
#include <sk/sk_motion.h>
#include <sk/sk_posture.h>

#include "sr_path_list.h"
#include "sr_hash_map.h"
#include "sbm/mcontrol_util.h"
#include "mcontrol_callbacks.h"



namespace SmartBody {

class SBMotion;
class SBSkeleton;

}



void print_joint( const SkJoint* joint );


#endif // ME_UTILITIES_HPP
