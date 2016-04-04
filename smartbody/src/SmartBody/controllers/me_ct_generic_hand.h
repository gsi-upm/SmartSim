/*
 *  me_ct_generic_hand.h - part of Motion Engine and SmartBody-lib
 *  Copyright (C) 2011  University of Southern California
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
 *  Mohammad Adil
 */

#ifndef _ME_CT_GENERIC_HAND_H_
#define _ME_CT_GENERIC_HAND_H_

#include <controllers/me_controller.h>
#include <sb/sbm_character.hpp>
#include <sb/SBController.h>
#include <sb/SBSkeleton.h>
#include <sb/SBJoint.h>
#include <sb/SBMotion.h>
#include <sb/SBHandSynthesis.h>

class MeCtGenericHand : public SmartBody::SBController
{
	public:
		// constructor, destructor and init functions
		MeCtGenericHand( );
		MeCtGenericHand( SmartBody::SBSkeleton* sk, SbmCharacter* c);
		~MeCtGenericHand();
		void init(SmartBody::SBMotion* motion , int num_levels);

		// this function is run per frame to update the data
		virtual bool controller_evaluate( double t, MeFrameData& frame );

		// these are the channels for returning data (i guess)
		virtual SkChannelArray& controller_channels()	{ return( _channels ); }

		// set the duration (-1 for now)
		virtual double controller_duration() { return -1; }

		// set the controller type
		virtual const std::string& controller_type() const	{ return _controller_type; }

		// might wanna do something when the controller starts
		virtual void controller_start();

		// updating the channel buffer
		virtual void updateChannelBuffer(MeFrameData& frame);

		// update channel buffer for only specific joints
		void updateChannelBuffer(MeFrameData& frame, std::vector<std::string>& endJoints, SmartBody::SBMotion* motion);
		
		// notify callback
		void notify(SBSubject* subject);
		
		
	public:
		static std::string _controller_type;
		enum HandState{
			RIGHT_HAND,LEFT_HAND
		};

	private:
		bool						_valid;
		float						_lastTime;
		int							_maxLevels;
		
		SkChannelArray 				_channels;
		
		SmartBody::SBSkeleton*     	_skeletonRef;

		SmartBody::SBSkeleton*     	_sk;
		SbmCharacter*				_character;

		std::string 				_wristJtRtName;
		std::string 				_wristJtLtName;


		float 						_motionTime;

		SmartBody::SBMotion*		_handMotion;
		SmartBody::SBMotion*		_bodyMotion;

		SmartBody::SBMotion* 		_handDatabase;
		SmartBody::SBMotion*		_bodyDatabase;

		
		// variables for left hand segmentation
		std::vector<SmartBody::SBMotion*> _bodyDatabaseSegments;
		std::vector<SmartBody::SBMotion*> _handDatabaseSegments;

		// variables for body motion
		std::vector<SmartBody::SBMotion*> _bodySegments;

		// variable for similar segments
		std::vector<std::vector<std::pair<int,float> > > _similarSegments;
		
		// vector for storing final motion
		std::vector<int> _finalMotionIndices;

		// the final calculated motion
		SmartBody::SBMotion* _finalMotion;

		// number of similar segments
		int _k;

		// cost list for finding top segments
		float currentSegment;

		bool _enableHand;
		bool _enableBody;

		// state variable
		HandState _state;

		// this is the new class
		SmartBody::SBHandSynthesis* _handSynthesis;

		// end joints vector 
		std::vector<std::string> _endJointsRt, _endJointsLt, _endJoints;
};

#endif
