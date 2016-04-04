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

#include "controllers/me_ct_lilt_try.h"
#include <vhcl_log.h>

//#include <iostream>
using namespace std;



////////////////////////////////////////////////////
//Helpful comments in the .h file/ and here in the evaluate method
std::string MeCtAnkleLilt::type_name= "AnkleLilt";


MeCtAnkleLilt::MeCtAnkleLilt( void ) { 
		 _duration=0;
		 _magnitude=0;
		 _angle_offset = 0;
		 _phase_offset= 0;
}

MeCtAnkleLilt::~MeCtAnkleLilt(void){}

void MeCtAnkleLilt::init(SbmPawn* pawn,SkSkeleton* skeleton_p){ //attaches skeleton and defines each joint that will be altered as a seperate channel
	int err = 0;
	_channels.init(); 

	SkJoint* rAnkle= skeleton_p->search_joint ("r_ankle");
	SkJoint* lAnkle= skeleton_p->search_joint("l_ankle");
	SkJoint* base= skeleton_p->search_joint("base"); //base must be defined because elevation of doctor must be done manually

	if (rAnkle == NULL){
		fprintf( stderr, "MeCtAnkleLilt:: init ERR: joint 'r_ankle' NOT FOUND\n" );
		err=1;
	}
	if (lAnkle == NULL){
		fprintf( stderr, "MeCtAnkleLilt:: init ERR: joint 'l_ankle' NOT FOUND\n" );
		err=1;
	}
	if (base== NULL){
		fprintf( stderr, "MeCtAnkleLilt:: init ERR: joint 'base' NOT FOUND\n" );
	}
	if ( err == 0 ){
		_channels.add( rAnkle, SkChannel::Quat );
		_channels.add( lAnkle, SkChannel::Quat );
		_channels.add( base, SkChannel::YPos);
	}
	MeController::init(pawn);

}

void MeCtAnkleLilt::set_lilt(float dur,float mag, float angle_offset){ //allows you to set duration and magnitude, increased duration will just lead to a "roll" back and forth
	 _duration=dur;
	 _magnitude= mag;  
}

void MeCtAnkleLilt::controller_start(){
	_currentAngle = 0.0;
	_prevT = 0.0;
	_dt = 0.0;
}

bool MeCtAnkleLilt::controller_evaluate( double t, MeFrameData& frame ){ //add peak duration capability
	double r= t+.5*(double)(_duration);  //gives you only the positive half of the sine wave
	if ( r> (double) _duration ){return FALSE; }
	float x=  (float)((t/(double)_duration)+.5);
	float angle= (float)(-1*M_PI/6)+(float)(-1*RAD(_magnitude )*sin(2.0*M_PI*x)); // -1*M_PI/6 term is required because the bp-neutral ankle angle is approx that angle  
	if( t > (double)_duration )	{                                                 // should be noted that both ankles are NOT the same angle in bp-neutral
		return( FALSE );
	}
	
	SrVec axis;
	axis.set(1.0, 0.0, 0.0);

	_currentAngle = angle;
	SrQuat q( axis, _currentAngle * (float)0.333 );
	
	SR_ASSERT( _context != NULL );  // Since we only use the new controller APIS.

	//the global buffer accessed/written to by all controllers
	SrBuffer<float>& buff = frame.buffer();
	int channels_size= _channels.size();
	int i=0;
	//the while loop takes care of the ankle rotations
	while(i<channels_size-1){
		//get buffer index
		int index= frame.toBufferIndex(_toContextCh[i]);
		buff[ index + 1 ] = q.x;
		//Mark channel changed
		frame.channelUpdated(i);
		i++;
	}
	//takes care of translating Y position
	int index= frame.toBufferIndex(_toContextCh[i]); 
	buff[ index + 0 ]= float(-.9*sin(2.0*M_PI*x));

	return ( true );
}

SkChannelArray& MeCtAnkleLilt::controller_channels(){
	return ( _channels );
}

double MeCtAnkleLilt::controller_duration(){
	return ( (double)_duration);
}

const std::string& MeCtAnkleLilt::controller_type() const {
	return( type_name );
}

void MeCtAnkleLilt::print_state( int tabs ){
	LOG("MeCtAnkleLilt\n" );
	LOG("currentAngle :", _currentAngle);
	LOG("duration :",_duration); 

}