/*
 *  sr_light.cpp - part of Motion Engine and SmartBody-lib
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

# include <sr/sr_light.h>

//===================================== SrLight ================================


SrLight::SrLight ()
 {
   init ();
 }

void SrLight::init ()
 {
   spot_exponent = 0;
   spot_direction.set ( 0, 0, -1.0f );
   spot_cutoff = 180;
   constant_attenuation = 1.0f;
   linear_attenuation = 0;
   quadratic_attenuation = 0;
   ambient = SrColor::black;
   diffuse = SrColor::white;
   specular = SrColor::black;
   position = SrVec::k;
   directional = true;
 }



//================================ End of File =================================================
