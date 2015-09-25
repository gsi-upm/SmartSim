/*
 *  me_ct_gaze_sensor.h - part of SmartBody-lib
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
 *      Marcus Thiebaux, USC
 */

#ifndef ME_CT_GAZE_SENSOR_H
#define ME_CT_GAZE_SENSOR_H

#include "controllers/me_ct_gaze.h"

///////////////////////////////////////////////////////////////////////////

class MeCtGazeSensor	{
	
	private:
		float threshold;
		float timeout;
		int id;
		MeCtGaze* gaze_p;
		void (*callback_fn)( int, int );
		bool pending;
	
	public:
		enum status_enum_set	{
			STATUS_SUCCESS,
			STATUS_TIMEOUT,
			STATUS_FAILURE,
			STATUS_ERROR
		};

		MeCtGazeSensor( void )	{
			threshold = 0.0;
			timeout = 0.0;
			id = -1;
			gaze_p = NULL;
			callback_fn = NULL;
			pending = false;
		}
		~MeCtGazeSensor( void );
		
		void init( 
			MeCtGaze* gaze_ref_p,
			float threshold_deg,
			float timeout_sec,
			int sensor_id,
			void (*callback_p)( int, int )
		)	{
			threshold = threshold_deg;
			timeout = timeout_sec;
			id = sensor_id;
			gaze_p = gaze_ref_p;
			callback_fn = callback_p;
		}

		void init( 
			float threshold_deg,
			float timeout_sec,
			int sensor_id,
			MeCtGaze* gaze_ref_p,
			void (*callback_p)( int, int )
		)	{
			threshold = threshold_deg;
			timeout = timeout_sec;
			id = sensor_id;
			gaze_p = gaze_ref_p;
			callback_fn = callback_p;
		}

		virtual void controller_start( void ) {
			pending = true;
		}
		virtual bool controller_evaluate( double t, MeFrameData& frame )	{
			
			if( !pending )	{
				return( false );
			}
			if( !callback_fn )	{
				pending = false;
				return( false );
			}
			if( !gaze_p )	{
				callback_fn( id, STATUS_ERROR );
				pending = false;
				return( false );
			}
			
			float angle = 0.0;
			bool report = gaze_p->calc_real_angle_to_target( angle );
			if( !report )	{
				pending = false;
				return( false );
			}
			
			if( angle < threshold ) {
				callback_fn( id, STATUS_SUCCESS );
				pending = false;
			}
			else
			if( t > timeout )	{
				callback_fn( id, STATUS_TIMEOUT );
				pending = false;
			}
			return( pending );
		}
		
#if 0
		virtual SkChannelArray& controller_channels()	{ return( _channels ); }
		virtual double controller_duration()			{ return( (double)_duration ); }
		virtual const std::string& controller_type() const		{ return( type_name ); }
#endif
};

///////////////////////////////////////////////////////////////////////////
#endif
