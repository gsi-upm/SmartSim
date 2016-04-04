/*
 *  SteeringAgent.h - part of Motion Engine and SmartBody-lib
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
 *      Yuyu Xu, USC
 */


#ifndef _PPRAISteeringAgent_H_
#define _PPRAISteeringAgent_H_

#include <sb/SBTypes.h>
#include <sb/SBCharacter.h>
#include <sb/SBSteerAgent.h>
#include <sbm/SteerPath.h>
#include <sbm/SteerSuiteEngineDriver.h>

class SbmCharacter;

class PPRAISteeringAgent : public SmartBody::SBSteerAgent
{
	public:
		enum SteeringStateConfig { MINIMAL = 0, STANDARD};
		
		PPRAISteeringAgent(SmartBody::SBCharacter* c);
		~PPRAISteeringAgent();

		void setCharacter(SmartBody::SBCharacter* c);
		void evaluate(double dt);

		SBAPI void setAgent(SteerLib::AgentInterface* a);
		SteerLib::AgentInterface* getAgent();
		
		
		SBAPI void setTargetAgent(SmartBody::SBPawn* tChar);
		SmartBody::SBPawn* getTargetAgent();
		void startParameterTesting();
		void updateSteerStateName();

		void setSteerParamsDirty(bool val);
		bool isSteerParamsDirty();
		void initSteerParams();
		void addSteeringAttributes();
		void sendLocomotionEvent(const std::string& status);
		void locomotionHalt();
		void locomotionReset();

		bool isInLocomotion();		// include starting moving and ending
		bool isHittingOnTarget();	// is sending out reaching target signal (one time)

		SteeringStateConfig steeringConfig;

	private:
		void normalizeAngle(float& angle);
		inline float cmToM(float v)		{return (v / 100.0f);}
		inline float mToCm(float v)		{return (v * 100.0f);}
		inline float degToRad(float v)	{return (v * 3.14159f / 180.0f);}
		inline float radToDeg(float v)	{return (v * 180.0f / 3.14159f);}

		float evaluateBasicLoco(float dt, float x, float y, float z, float yaw);
		float evaluateNewLoco(float dt, float x, float y, float z, float yaw);
		float evaluateBasicLocoDirect(float dt, float x, float y, float z, float yaw);
		float evaluateExampleLoco(float dt, float x, float y, float z, float yaw);


		float evaluateSteppingLoco(float dt, float x, float y, float z, float yaw);
		void  evaluatePathFollowing(float dt, float x, float y, float z, float yaw);

		void parameterTesting();
		void cacheParameter(std::list<float>& sampleData, float data, int size);
		float getFilteredParameter(std::list<float>& sampleData);
		void startIdleToWalkState(float angleDiff);

		void startLocomotionState();
		void adjustFacingAngle(float angleDiff);
		SrVec getCollisionFreeGoal(SrVec target, SrVec curPos);

		void adjustLocomotionBlend(SmartBody::SBCharacter* character, const std::string& blendName, int blendDimension, double x, double y, double z, bool directPlay, bool queued);

	private:
		
		int	_numSteeringGoal;
		
		SteerLib::AgentInterface* agent;
		SmartBody::SBCharacter* character;
		SmartBody::SBPawn* target;

	public:
		enum SteeringParamType
		{
			ForwardSpeed, TurningSpeed, TravelDirection, TerrainAngle
		};

		// basic param
		float basicLocoAngleGain;
		float basicLocoScootGain;
		//  new param
		float newLocoAngleGain;
		float newLocoScootGain;
		// procedural param
		float locoSpdGain;
		float locoScootGain;
		// example param
		float paLocoAngleGain;	
		float paLocoScootGain;
		// global param
		float scootThreshold;
		float speedThreshold;
		float angleSpeedThreshold;
		float distThreshold;	
		float distDownThreshold;
		float desiredSpeed;
		float currentTargetSpeed;
		float brakingGain;
		float facingAngle;
		float facingAngleThreshold;
		float acceleration;
		float scootAcceleration;
		float angleAcceleration;
		float turningEpsilon;		
		SrVec targetLoc;
		bool stepAdjust;
		float pedMaxTurningRateMultiplier;
		float tiltGain;
		bool terrainMode;
		float collisionTime;

		SrVec curSteerPos, curSteerDir, nextSteerDir, nextSteerPos, nextPtOnPath;


		float newSpeed;
		//----------------------------
		// WJ added start
		// basic param
		Util::Vector forward;
		Util::Vector rightSide;
		float currentSpeed;
		Util::Vector velocity;
		// WJ added end
		//----------------------------

		// stepping param
		float stepTargetX;
		float stepTargetZ;
		bool steppingMode;

		// parameter testing
		float paramTestDur;
		float paramTestStartTime;
		bool paramTestFlag;
		float paramTestAngle;
		float paramTestDistance;
		float prevX;
		float prevZ;
		float prevYaw;

		// goal lists
		std::list<float> goalList;
		SteerPath        steerPath;

		// low pass filter
		int speedWindowSize;
		int angleWindowSize;
		int scootWindowSize;
		std::list<float> speedCache;
		std::list<float> angleCache;
		std::list<float> scootCache;
		float steerCurSpeed, steerCurAngle, steerCurScoot;

		// heading over control to parameterized animation engine
		bool inControl;

		std::string stepStateName;
		std::string locomotionName;
		std::string startingLName;
		std::string startingRName;
		std::string idleTurnName;
		std::string jumpName;

		bool fastInitial;	// whether to use the transition animation
		bool smoothing;
		bool pathFollowing;	
		bool facingDirectBlend;
		bool facingAdjust;

		protected:
		std::string lastMessage;
		int numMessageRepeats;
		bool _dirty;
		int _curFrame;
};

#endif
