/*  sr_sa_event.cpp - part of Motion Engine and SmartBody-lib
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

# include <sr/sr_sn_editor.h>
# include <sr/sr_sa_event.h>

//# define SR_USE_TRACE1 // constructor / destructor
//# include <sr/sr_trace.h>

//================================== SrSaEvent ====================================

bool SrSaEvent::editor_apply ( SrSnEditor* m )
 {
   bool b=true;
   if ( !m->visible() ) return b;

   push_matrix ();
   mult_matrix ( m->mat() );

   SrMat mat = _matrix_stack.top();
   mat.invert();
//sr_out<<mat<<srnl;
//sr_out << "pixel_size AA: " << _ev.pixel_size <<srnl;

   SrEvent e = _ev;
//sr_out << "pixel_size BB: " << e.pixel_size <<srnl;
   e.ray.p1 = e.ray.p1*mat;
   e.ray.p2 = e.ray.p2*mat;
   e.lray.p1 = e.lray.p1*mat;
   e.lray.p2 = e.lray.p2*mat;
   e.mousep = e.mousep*mat;
   e.lmousep = e.lmousep*mat;

   _result = m->handle_event ( e );
   if ( _result ) b = false; // event used: stop the scene traversing

   pop_matrix ();
   return b;
 }

//======================================= EOF ====================================
