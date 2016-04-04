/*  sr_sa_bbox.cpp - part of Motion Engine and SmartBody-lib
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
 *  License along with SmarBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Marcelo Kallmann, USC (currently UC Merced)
 */

//# include <stdlib.h>

# include <sr/sr_sa_bbox.h>
# include <sr/sr_sn_shape.h>

//# define SR_USE_TRACE1 // constructor / destructor
//# include <sr/sr_trace.h>

//================================== SrSaBBox ====================================

bool SrSaBBox::shape_apply ( SrSnShapeBase* s )
 {
   if ( !s->visible() ) return true;
   SrBox b;
   SrMat m;
   s->get_bounding_box ( b );
   get_top_matrix ( m );
   _box.extend ( b * m );
   return true;
 }
/*
bool SrSaBBox::manipulator_apply ( SrSceneShapeBase* s )
 {
   SrBox b;
   s->get_bounding_box ( b );
   _box.extend ( b * top_matrix() );
   return true;
 }
*/

//======================================= EOF ====================================
