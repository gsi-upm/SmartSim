/*
 *  me_ct_new_locomotion.h - part of Motion Engine and SmartBody-lib
 *  Copyright (C) 2014  University of Southern California
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
 *      Yuyu Xu, Alain Juarez Perez ICT USC
 */

#ifndef _ME_CT_NEW_LOCOMOTION_H_
#define _ME_CT_NEW_LOCOMOTION_H_

#include <controllers/me_controller.h>
#include <sb/sbm_character.hpp>
#include <sb/SBController.h>
#include <sb/SBMotion.h>
#include <sb/SBSkeleton.h>
#include <controllers/me_ct_constraint.hpp>
#define NEW_CONTROL_RUNNING 0

class Fading
{
public:
	enum FadingControlMode	
	{
		FADING_MODE_OFF = 0,
		FADING_MODE_IN,
		FADING_MODE_OUT
	};
	//Fading Control
	Fading();
	float fadeRemainTime, fadeInterval;
	float blendWeight;
	float prev_time, cur_time, dt;
	bool  restart;	
	FadingControlMode fadeMode;
	void setFadeIn(float interval);
	void setFadeOut(float interval);
	bool updateFading(float dt);
	void controlRestart();
	void updateDt(float curTime);
};

class MeCtNewLocomotion : public SmartBody::SBController
{
	public:
		bool useIKLf, useIKRt;
		enum ConstraintType
		{
			CONSTRAINT_POS = 0,
			CONSTRAINT_ROT,
			NUM_OF_CONSTRAINT
		};
		MeCtNewLocomotion();
		~MeCtNewLocomotion();
		void init(SbmCharacter* sbChar);
		bool addEffectorJointPair(const char* effectorName, const char* effectorRootName, const SrVec& pos , const SrQuat& rot , 
								  ConstraintType cType, ConstraintMap& posCons, ConstraintMap& rotCons);
		virtual void controller_map_updated();
		virtual void controller_start();	
		
		virtual bool controller_evaluate(double t, MeFrameData& frame);		
		virtual SkChannelArray& controller_channels()	{return(_channels);}
		virtual double controller_duration()			{return -1;}
		virtual const std::string& controller_type() const		{return(_type_name);}

		virtual void setup();
		
	protected:
		MeCtJacobianIK       ik;
		MeCtIKTreeScenario   ik_scenario;
		float 			_duration;
		Fading LeftFading, RightFading;
		ConstraintMap   posConsRt, rotConsRt;
		ConstraintMap   posConsLf, rotConsLf;
	public:
		static std::string _type_name;
		void  setScootSpd(float v) {scootSpd = v;}
		float getScootSpd() {return scootSpd;}
		void  setMovingSpd(float v) {movingSpd = v;}
		float getMovingSpd() {return movingSpd;}
		void  setTurningSpd(float v) {turningSpd = v;}
		float getTurningSpd() {return turningSpd;}
		void  setValid(bool v) {_valid = v;}
		void  setDesiredHeading(float v) {desiredHeading = v;}
		float getDesiredHeading() {return desiredHeading;}
		void  notify(SmartBody::SBSubject* subject);
		void  play(float t, bool useTemp=false);
		void  reset(bool resetPos=false);
		void  loopMotion(float def, float speed);
	private:
		void  addPawn(SrVec& pos, std::string name);
		float legDistance(bool Leftleg);
		bool  capsule_collision(SrVec  mp1, SrVec mp2, SrVec np1, SrVec np2);
		void  check_collision();
		bool  _valid, _analysis;
		float scootSpd;
		float movingSpd;
		float turningSpd;
		float motionSpd;
		float fadein;
		float fadeout;
		float startTime;
		float errorSum;
		float walkScale;
		float walkSpeedGain;
		float motionSpeed;
		SkChannelArray _channels;
		SbmCharacter* character;
		float _lastTime, _prevTime;
		float desiredHeading, motionTime;
		double ikDamp;
		SmartBody::SBMotion *dataCycle, *smoothCycle;
		SmartBody::SBSkeleton* sk;
		std::string lend, rend, hipjoint;
		vector<string> attributes_names;
		SrBuffer<float> tempBuffer;
		SrBuffer<float>* BufferRef;
		std::vector<int> rplant;
		std::vector<int> lplant;
		float d(SrQuat a, SrQuat b){
			float v = a.w*b.w+a.x*b.x+a.y*b.y+a.z*b.z;
			return 1.0f - v*v;
		}
	protected:	
		void updateChannelBuffer(SrBuffer<float>& buffer, std::vector<SrQuat>& quatList, bool bRead = false);
		void writeToSkeleton(SrBuffer<float>& buffer);
		void updateChannelBuffer(SrBuffer<float>& buffer, float t);
		void updateWorldOffset(SrBuffer<float>& buffer, SrQuat& rot, SrVec& pos);
		void updateConstraints(float t);
};
#endif
