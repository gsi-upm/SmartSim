/*  sr_points.cpp - part of Motion Engine and SmartBody-lib
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

# include <sr/sr_box.h>
# include <sr/sr_vec2.h>
# include <sr/sr_points.h>

//# define SR_USE_TRACE1 // Constructor and Destructor
//# include <sr/sr_trace.h>

//======================================= SrPoints ====================================

const char* SrPoints::class_name = "Points";


SrPoints::SrPoints ()
 {
   //SR_TRACE1 ( "Constructor" );
   A = 0;
 }

SrPoints::~SrPoints ()
 {
   //SR_TRACE1 ( "Destructor" );
   delete A;
 }

void SrPoints::init ()
 {
   P.size(0);
 }

void SrPoints::init_with_attributes ()
 {
   P.size(0);
   if ( A )
    A->size(0);
   else A = new SrArray<Atrib>;
 }

void SrPoints::compress ()
 {
   P.compress();
   if ( A ) A->compress();
 }

void SrPoints::push ( const SrPnt& p )
 { 
   P.push() = p;
   if ( A ) A->push().s=1;
 }

void SrPoints::push ( const SrPnt2& p )
 { 
   P.push().set ( p.x, p.y, 0 ); 
   if ( A ) A->push().s=1;
 }

void SrPoints::push ( float x, float y, float z )
 { 
   P.push().set(x,y,z); 
   if ( A ) A->push().s=1;
 }

void SrPoints::push ( const SrPnt& p, SrColor c, float size )
 { 
   if ( !A ) return;
   P.push() = p;
   A->push().s=size;
   A->top().c = c;
 }

void SrPoints::push ( const SrPnt2& p, SrColor c, float size )
 { 
   if ( !A ) return;
   P.push().set ( p.x, p.y, 0 ); 
   A->push().s=size;
   A->top().c = c;
 }

void SrPoints::push ( float x, float y, SrColor c, float size )
 { 
   if ( !A ) return;
   P.push().set(x,y,0); 
   A->push().s=size;
   A->top().c = c;
 }

void SrPoints::push ( float x, float y, float z, SrColor c, float size )
 { 
   if ( !A ) return;
   P.push().set(x,y,z); 
   A->push().s=size;
   A->top().c = c;
 }

void SrPoints::get_bounding_box ( SrBox& b ) const
 { 
   int i;
   b.set_empty ();
   for ( i=0; i<P.size(); i++ ) 
    { b.extend ( P[i] );
    }
 }

//================================ EOF =================================================
