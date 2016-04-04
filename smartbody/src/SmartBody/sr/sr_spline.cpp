/*
 *  sr_spline.cpp - part of Motion Engine and SmartBody-lib
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
 *      Marcelo Kallmann, USC (currently UC Merced)
 */

//# include <math.h>
# include <sr/sr_spline.h> 

//============================== SrSpline ===============================

SrSpline::SrSpline ( const SrSpline& c )
 {
 }

void SrSpline::init ( int d, int k )
 {
   _pieces = k-1;
   _dim = d;
   if ( _pieces<0 ) _pieces=0;
   if ( _dim<0 ) _dim=0;
   _spline.size ( _pieces*(_dim*3) + _dim );
   _spline.setall ( 0.0f );
 }
 

//============================== end of file ===============================

