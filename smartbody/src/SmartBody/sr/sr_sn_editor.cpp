/*  sr_sn_editor.cpp - part of Motion Engine and SmartBody-lib
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

# include <sr/sr_sn_editor.h>

//# define SR_USE_TRACE1  // SrSn Const/Dest
//# define SR_USE_TRACE2  // SrSceneGroup Const/Dest
//# define SR_USE_TRACE3  // SrSceneGroup children management
//# define SR_USE_TRACE4  // SrSceneMatrix Const/Dest
//# define SR_USE_TRACE5  // SrSceneShapeBase Const/Dest
//# include <sr/sr_trace.h>


//======================================= SrSnEditor ====================================

SrSnEditor::SrSnEditor ( const char* name )
                       :SrSn ( SrSn::TypeEditor, name )
 {
//   SR_TRACE2 ( "Constructor" );
   _child = 0;
   _helpers = new SrSnGroup;
   _helpers->ref ();
 }

SrSnEditor::~SrSnEditor ()
 {
//   SR_TRACE2 ( "Destructor" );
   _helpers->unref ();
   child ( 0 );
 }

// protected :

void SrSnEditor::child ( SrSn *sn )
 {
   if ( _child ) _child->unref();
   if ( sn ) sn->ref(); // Increment reference counter
   _child = sn;
 }

int SrSnEditor::handle_event ( const SrEvent &e )
 {
   return 0;
 }

//======================================= EOF ====================================

