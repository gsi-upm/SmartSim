/*  sr_sa_render_mode.cpp - part of Motion Engine and SmartBody-lib
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

# include <sr/sr_sa_render_mode.h>

# include <sr/sr_sn_shape.h>

//# define SR_USE_TRACE1 // constructor / destructor
//# include <sr/sr_trace.h>

//=============================== SrSaRenderMode ====================================

bool SrSaRenderMode::shape_apply ( SrSnShapeBase* s )
 {
   if ( _override )
     s->override_render_mode ( _render_mode );
   else
     s->restore_render_mode ();

   return true;
 }

//======================================= EOF ====================================
