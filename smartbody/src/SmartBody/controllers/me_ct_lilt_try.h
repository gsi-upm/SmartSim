/*
 *  me_ct_lilt_try.h - part of SmartBody-lib
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
 *      Ashok Basawapatna, USC (no longer)
 *      Andrew n marshall, USC
 */

#ifndef ME_CT_LILT_TRY_H
#define ME_CT_LILT_TRY_H

#include <sk/sk_skeleton.h>
#include <sb/SBController.h>

#include "sbm/gwiz_math.h"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

///////////////////////////////////////////////////////////////////////////
/*********************************READ ME********************************************************************************
* The Lilt controller Attempts to make a controller that allows the doctor to stand on his tiptoes at a certain magnitude for
a given duration as done by some people in the midst of conversation when making a point -- it can ideally be used when the doctor is 
emphasizing something. Note that the doctor body has to be translated to get the effect of moving upward.
I did not research if Iraqi people would actually do this- it was more to learn how to make a 
controller. Furthermore there are a few wierd issues dealing with BP-Neutral- mainly the ankles aren't at 0 angle and 
the left and right ankle are rotated a different amount leading to a an awkward ankle rotation to get the doctor 
back to the bp-neutral stance.


USE: launch SBM and at the SBM command line implement the following commads
   ctrl <> lilt <char-name>
    snod <> <dur-sec> <mag-deg>
    char <> ctrl <> begin [<ease-in> [<ease-out>]]
EXAMPLE: for example if I wanted the doctor's ankles to go to a 50 degree angle and the whole lilt to last 3 seconds I would do the 
			following

			ctrl Y lilt doctor
			lilt Y 3.0 50
			char doctor ctrl Y begin 0.3 0.3
	
	*/ 

class MeCtAnkleLilt : public SmartBody::SBController	{
	
	public:
		static std::string type_name;

		MeCtAnkleLilt();
		virtual ~MeCtAnkleLilt();
		
		void init( SbmPawn* pawn, SkSkeleton* skeleton_p );
		void set_lilt( float dur, float mag, float angle_offset); 
		
	private:
		virtual void controller_start();
		virtual bool controller_evaluate( double t, MeFrameData& frame );
		
		virtual SkChannelArray& controller_channels();
		virtual double controller_duration();
		virtual const std::string& controller_type() const;
		virtual void print_state( int tabs );
		
		SkChannelArray _channels;
		float _duration; 
		float _magnitude;
		float _angle_offset;
		float _phase_offset;
		
		float _currentAngle;
		float _prevT;
		float _dt;
};

///////////////////////////////////////////////////////////////////////////
#endif
