/*  sr_sa.cpp - part of Motion Engine and SmartBody-lib
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
#include "vhcl.h"
# include <sr/sr_sa.h>
# include <sr/sr_sn_group.h>
# include <sr/sr_sn_editor.h>
# include <sr/sr_sn_matrix.h>

//# define SR_USE_TRACE1 // constructor / destructor
//# include <sr/sr_trace.h>

//=============================== SrSa ====================================

SrSa::SrSa () 
 { 
   //SR_TRACE1 ( "Constructor" );

   _matrix_stack.capacity ( 8 );
   _matrix_stack.push ( SrMat::id );
 }

SrSa::~SrSa ()
 {
   //SR_TRACE1 ( "Destructor" );
 }

bool SrSa::apply ( SrSn* n, bool init )
 {  
   //LOG("SrSs::apply");
   if ( init ) init_matrix ();
	
   apply_begin ();

   bool result;
   int t = n->type();

   if ( t==SrSn::TypeMatrix )
    {
      //LOG("TypeMatrix");
      result = matrix_apply ( (SrSnMatrix*) n );
    }
   else if ( t==SrSn::TypeGroup )
    {
      //LOG("TypeGroup");
      result = group_apply ( (SrSnGroup*) n );
    }
   else if ( t==SrSn::TypeShape )
    {
      //LOG("TypeShape");
      result = shape_apply ( (SrSnShapeBase*) n );
    }
   else if ( t==SrSn::TypeEditor )
    {
      result = editor_apply ( (SrSnEditor*) n );
    }
   else
    { sr_out.fatal_error ( "Undefined type &d in SrSa::apply()!", t );
      result = false;
    }

   apply_end ();
   return result;
 }

//--------------------------------- virtuals -----------------------------------

void SrSa::get_top_matrix ( SrMat& mat )
 {
   mat = _matrix_stack.top();
 }

int SrSa::matrix_stack_size ()
 {
   return _matrix_stack.size();
 }

void SrSa::init_matrix ()
 {
   _matrix_stack.size ( 1 );
   _matrix_stack[0] = SrMat::id;
 }

void SrSa::mult_matrix ( const SrMat& mat )
 {
   SrMat stackm = _matrix_stack.top(); 
   _matrix_stack.top().mult ( mat, stackm ); // top = mat * top
 }

void SrSa::push_matrix ()
 {
   _matrix_stack.push();
   _matrix_stack.top() = _matrix_stack[ _matrix_stack.size()-2 ];
 }

void SrSa::pop_matrix ()
 {
   _matrix_stack.pop();
 }

bool SrSa::matrix_apply ( SrSnMatrix* m )
 {
   if ( !m->visible() ) return true;
   mult_matrix ( m->get() );
   return true;
 }

bool SrSa::group_apply ( SrSnGroup* g )
 {
   bool b=true;
   int i, s;

   if ( !g->visible() ) return true;

   if ( g->separator() ) push_matrix();

   s = g->size();
   //LOG("group size = %d",s);
   for ( i=0; i<s; i++ )
    { b = apply ( g->get(i), false );
      if ( !b ) break;
    }

   if ( g->separator() ) pop_matrix();
   return b;
 }

bool SrSa::shape_apply ( SrSnShapeBase* s )
 {
   return true; // implementation specific to the derived class
 }

bool SrSa::editor_apply ( SrSnEditor* e )
 {
   SrSnGroup* h = e->helpers();
   SrSn* c = e->child();
   if ( !c ) return true;

   push_matrix ();
   mult_matrix ( e->mat() );

   bool b = apply ( c, false );

   if ( b && h && e->visible() )
    { int i, s = h->size();
      for ( i=0; i<s; i++ )
       { b = apply ( h->get(i), false );
         if ( !b ) break;
       }
    }

   pop_matrix();
   return b;
 }

//======================================= EOF ====================================
