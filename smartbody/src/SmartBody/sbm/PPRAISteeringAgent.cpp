/*
 *  SteeringAgent.cpp - part of Motion Engine and SmartBody-lib
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
 */


#include "PPRAISteeringAgent.h"

#include <sb/SBScene.h>
#include <sb/sbm_character.hpp>
#ifndef __native_client__
#include <sb/SBPythonClass.h>
#endif

#include <controllers/me_ct_param_animation.h>
#include <controllers/me_ct_basic_locomotion.h>
#include <controllers/me_ct_new_locomotion.h>
#include <sb/SBSteerManager.h>
#include <sb/SBAnimationStateManager.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBCommandManager.h>

#include <sbm/SteerPath.h>

#include <sb/SBAnimationState.h>

#include <sb/SBEvent.h>
#include <sb/SBRetargetManager.h>
#include <sb/SBRetarget.h>
#include <sbm/Heightfield.h>

#define DebugInfo 0
#define FastStart 1
#define SOLVE_HEADING_ONLY 1

PPRAISteeringAgent::PPRAISteeringAgent(SmartBody::SBCharacter* c) : SmartBody::SBSteerAgent(c)
{
	character = c;
	
	agent = NULL;
	target = NULL;

	forward = Util::Vector(0.0f, 0.0f, 1.0f);
	rightSide = rightSideInXZPlane(forward);
	currentSpeed = 1.0f;
	velocity = forward * currentSpeed;

	steerCurSpeed = steerCurAngle = steerCurScoot = 0.f;

	stepTargetX = 0.0f;
	stepTargetZ = 0.0f;
	steppingMode = false;
	
	paramTestDur = 2.0f;
	paramTestStartTime = 0.0f;
	paramTestFlag = false;
	paramTestAngle = 0.0f;
	paramTestDistance = 0.0f;
	prevX = 0.0f;
	prevZ = 0.0f;
	prevYaw = 0.0f;
	collisionTime = 0.f;

	turningEpsilon = 1.f;
	inControl = true;

	fastInitial = false;
	smoothing = false;
	facingAdjust = false;

	lastMessage = "";
	numMessageRepeats = 0;

	updateSteerStateName();

	// add the steering attributes to the character
	addSteeringAttributes();

	setSteerParamsDirty(true);
	initSteerParams();
	_curFrame = 0;
	currentTargetSpeed = 1.5f;
	
	_numSteeringGoal = 0;
}

PPRAISteeringAgent::~PPRAISteeringAgent()
{
}

void PPRAISteeringAgent::addSteeringAttributes()
{
	if (!character)
		return;
	
	if (!character->hasAttribute("steering.useDirectControl"))
		character->createDoubleAttribute("steering.useDirectControl", false, true, "steering", 200, false, false, false, ""); 

	if (!character->hasAttribute("steering.basicLocoAngleGain"))
		character->createDoubleAttribute("steering.basicLocoAngleGain", 2.0f, true, "steering", 205, false, false, false, ""); 
	
	if (!character->hasAttribute("steering.basicLocoScootGain"))
		character->createDoubleAttribute("steering.basicLocoScootGain", 10.0f, true, "steering", 210, false, false, false, ""); 

	if (!character->hasAttribute("steering.newLocoAngleGain"))
		character->createDoubleAttribute("steering.newLocoAngleGain", 2.0f, true, "steering", 205, false, false, false, ""); 
	
	if (!character->hasAttribute("steering.newLocoScootGain"))
		character->createDoubleAttribute("steering.newLocoScootGain", 10.0f, true, "steering", 210, false, false, false, ""); 

	if (!character->hasAttribute("steering.locoSpdGain"))
		character->createDoubleAttribute("steering.locoSpdGain", 70.0f, true, "steering", 220, false, false, false, ""); 

	if (!character->hasAttribute("steering.locoScootGain"))
		character->createDoubleAttribute("steering.locoScootGain", 2.0f, true, "steering", 230, false, false, false, ""); 

	if (!character->hasAttribute("steering.paLocoAngleGain"))
		character->createDoubleAttribute("steering.paLocoAngleGain", 2.0f, true, "steering", 240, false, false, false, ""); 

	if (!character->hasAttribute("steering.paLocoScootGain"))
		character->createDoubleAttribute("steering.paLocoScootGain", 9.0f, true, "steering", 250, false, false, false, ""); 
	
	if (!character->hasAttribute("steering.scootThreshold"))
		character->createDoubleAttribute("steering.scootThreshold", .1f, true, "steering", 260, false, false, false, ""); 
	
	if (!character->hasAttribute("steering.speedThreshold"))
		character->createDoubleAttribute("steering.speedThreshold", .1f, true, "steering", 270, false, false, false, ""); 
	
	if (!character->hasAttribute("steering.angleSpeedThreshold"))
		character->createDoubleAttribute("steering.angleSpeedThreshold", 10.f, true, "steering", 280, false, false, false, ""); 

	if (!character->hasAttribute("steering.distThreshold"))
		character->createDoubleAttribute("steering.distThreshold", 1.80f, true, "steering", 290, false, false, false, ""); 

	if (!character->hasAttribute("steering.distDownThreshold"))
		character->createDoubleAttribute("steering.distDownThreshold", .3f, true, "steering", 300, false, false, false, ""); 

	if (!character->hasAttribute("steering.turningEpsilon"))
		character->createDoubleAttribute("steering.turningEpsilon", 2.f, true, "steering", 305, false, false, false, ""); 

	if (!character->hasAttribute("steering.brakingGain"))
		character->createDoubleAttribute("steering.brakingGain", 1.2f, true, "steering", 310, false, false, false, ""); 
	
	if (!character->hasAttribute("steering.desiredSpeed"))
		character->createDoubleAttribute("steering.desiredSpeed", 1.0f, true, "steering", 310, false, false, false, ""); 	

	if (!character->hasAttribute("steering.pathAngleAcc"))
		character->createDoubleAttribute("steering.pathAngleAcc", 250.f, true, "steering", 310, false, false, false, "");

	if (!character->hasAttribute("steering.pathMaxSpeed"))
		character->createDoubleAttribute("steering.pathMaxSpeed", 1.5f, true, "steering", 310, false, false, false, "");	

	if (!character->hasAttribute("steering.pathMinSpeed"))
		character->createDoubleAttribute("steering.pathMinSpeed", 0.6f, true, "steering", 310, false, false, false, "");
	
	if (!character->hasAttribute("steering.pathStartStep"))
		character->createBoolAttribute("steering.pathStartStep", true, true, "steering", 330, false, false, false, ""); 
	
	if (!character->hasAttribute("steering.facingAngleThreshold"))
		character->createDoubleAttribute("steering.facingAngleThreshold", 10.f, true, "steering", 340, false, false, false, ""); 

	if (!character->hasAttribute("steering.acceleration"))
		character->createDoubleAttribute("steering.acceleration", 2.f, true, "steering", 350, false, false, false, ""); 

	if (!character->hasAttribute("steering.scootAcceleration"))
		character->createDoubleAttribute("steering.scootAcceleration", 200.f, true, "steering", 360, false, false, false, ""); 
	
	if (!character->hasAttribute("steering.angleAcceleration"))
		character->createDoubleAttribute("steering.angleAcceleration", 400.f, true, "steering", 370, false, false, false, ""); 

	if (!character->hasAttribute("steering.stepAdjust"))
		character->createBoolAttribute("steering.stepAdjust", false, true, "steering", 380, false, false, false, ""); 

	if (!character->hasAttribute("steering.smoothing"))
		character->createBoolAttribute("steering.smoothing", true, true, "steering", 390, false, false, false, ""); 

	if (!character->hasAttribute("steering.pathFollowingMode"))
		character->createBoolAttribute("steering.pathFollowingMode", false, true, "steering", 390, false, false, false, "");

	if (!character->hasAttribute("steering.facingDirectBlend"))
		character->createBoolAttribute("steering.facingDirectBlend", true, true, "steering", 390, false, false, false, "");

	if (!character->hasAttribute("steering.speedWindowSize"))
		character->createIntAttribute("steering.speedWindowSize", 10, true, "steering", 400, false, false, false, ""); 

	if (!character->hasAttribute("steering.angleWindowSize"))
		character->createIntAttribute("steering.angleWindowSize", 3, true, "steering", 410, false, false, false, ""); 

	if (!character->hasAttribute("steering.scootWindowSize"))
		character->createIntAttribute("steering.scootWindowSize", 3, true, "steering", 420, false, false, false, ""); 
	
	if (!character->hasAttribute("steering.pedMaxTurningRateMultiplier"))
		character->createDoubleAttribute("steering.pedMaxTurningRateMultiplier", 20.f, true, "steering", 430, false, false, false, ""); 

	if (!character->hasAttribute("steering.tiltGain"))
		character->createDoubleAttribute("steering.tiltGain", 2.f, true, "steering", 440, false, false, false, ""); 

	if (!character->hasAttribute("steering.terrainMode"))
		character->createBoolAttribute("steering.terrainMode", false, true, "steering", 450, false, false, false, ""); 

	if (!character->hasAttribute("steering.useCollisionFreeGoal"))
		character->createBoolAttribute("steering.useCollisionFreeGoal", true, true, "steering", 450, false, false, false, ""); 

	setSteerParamsDirty(false);
}

void PPRAISteeringAgent::initSteerParams()
{
	if (character && character->hasAttribute("steering.basicLocoAngleGain"))
		basicLocoAngleGain = (float) character->getDoubleAttribute("steering.basicLocoAngleGain");
	else
		basicLocoAngleGain = 2.0f;

	if (character && character->hasAttribute("steering.basicLocoScootGain"))
		basicLocoScootGain = (float) character->getDoubleAttribute("steering.basicLocoScootGain");
	else
		basicLocoScootGain = 10.0f;

	if (character && character->hasAttribute("steering.newLocoAngleGain"))
		newLocoAngleGain = (float) character->getDoubleAttribute("steering.newLocoAngleGain");
	else
		newLocoAngleGain = 2.0f;

	if (character && character->hasAttribute("steering.newLocoScootGain"))
		newLocoScootGain = (float) character->getDoubleAttribute("steering.newLocoScootGain");
	else
		newLocoScootGain = 10.0f;
		
	if (character && character->hasAttribute("steering.locoSpdGain"))
		locoSpdGain = (float) character->getDoubleAttribute("steering.locoSpdGain");
	else
		locoSpdGain = 70.0f;

	if (character && character->hasAttribute("steering.locoScootGain"))
		locoScootGain = (float) character->getDoubleAttribute("steering.locoScootGain");
	else
		locoScootGain =  2.0f;
	
	if (character && character->hasAttribute("steering.paLocoAngleGain"))
		paLocoAngleGain = (float) character->getDoubleAttribute("steering.paLocoAngleGain");
	else
		paLocoAngleGain =  2.0f;
	
	if (character && character->hasAttribute("steering.paLocoScootGain"))
		paLocoScootGain = (float) character->getDoubleAttribute("steering.paLocoScootGain");
	else
		paLocoScootGain =  9.0f;

	if (character && character->hasAttribute("steering.scootThreshold"))
		scootThreshold = (float) character->getDoubleAttribute("steering.scootThreshold");
	else
		scootThreshold = 0.1f;

	if (character && character->hasAttribute("steering.speedThreshold"))
		speedThreshold = (float) character->getDoubleAttribute("steering.speedThreshold");
	else
		speedThreshold = 0.1f;
	
	if (character && character->hasAttribute("steering.angleSpeedThreshold"))
		angleSpeedThreshold = (float) character->getDoubleAttribute("steering.angleSpeedThreshold");
	else
		angleSpeedThreshold = 10.0f;

	if (character && character->hasAttribute("steering.distThreshold"))
		distThreshold = (float) character->getDoubleAttribute("steering.distThreshold");
	else
		distThreshold = 1.80f;	

	if (character && character->hasAttribute("steering.distDownThreshold"))
		distDownThreshold = (float) character->getDoubleAttribute("steering.distDownThreshold");
	else
		distDownThreshold = 0.3f;

	if (character && character->hasAttribute("steering.turingEpsilon"))
		turningEpsilon = (float) character->getDoubleAttribute("steering.turingEpsilon");
	else
		turningEpsilon = 2.f;

	if (character && character->hasAttribute("steering.brakingGain"))
		brakingGain = (float) character->getDoubleAttribute("steering.brakingGain");
	else
		brakingGain = 1.2f;
	
	if (character && character->hasAttribute("steering.desiredSpeed"))
		desiredSpeed = (float) character->getDoubleAttribute("steering.desiredSpeed");
	else
		desiredSpeed = 1.0f;	

	if (character && character->hasAttribute("steering.facingAngle"))
		facingAngle = (float) character->getDoubleAttribute("steering.facingAngle");
	else
		facingAngle = -200.0f;

	if (character && character->hasAttribute("steering.facingAngleThreshold"))
		facingAngleThreshold = (float) character->getDoubleAttribute("steering.facingAngleThreshold");
	else
		facingAngleThreshold = 10;
	
	if (character && character->hasAttribute("steering.acceleration"))
		acceleration = (float) character->getDoubleAttribute("steering.acceleration");
	else
		acceleration = 2.0f;	
	
	if (character && character->hasAttribute("steering.scootAcceleration"))
		scootAcceleration = (float) character->getDoubleAttribute("steering.scootAcceleration");
	else
		scootAcceleration = 200.f;	

	if (character && character->hasAttribute("steering.angleAcceleration"))
		angleAcceleration = (float) character->getDoubleAttribute("steering.angleAcceleration");
	else
		angleAcceleration = 400.f;

	if (character && character->hasAttribute("steering.stepAdjust"))
		stepAdjust = character->getBoolAttribute("steering.stepAdjust");
	else
		stepAdjust = false;

	if (character && character->hasAttribute("steering.smoothing"))
		smoothing = character->getBoolAttribute("steering.smoothing");
	else
		smoothing = true;

	if (character && character->hasAttribute("steering.pathFollowingMode"))
		pathFollowing = character->getBoolAttribute("steering.pathFollowingMode");
	else
		pathFollowing = false;

	if (character && character->hasAttribute("steering.facingDirectBlend"))
		facingDirectBlend = character->getBoolAttribute("steering.facingDirectBlend");
	else
		facingDirectBlend = true;
	
	if (character && character->hasAttribute("steering.speedWindowSize"))
		speedWindowSize = character->getIntAttribute("steering.speedWindowSize");
	else
		speedWindowSize = 10;
	
	if (character && character->hasAttribute("steering.angleWindowSize"))
		angleWindowSize = character->getIntAttribute("steering.angleWindowSize");
	else
		angleWindowSize = 3;

	if (character && character->hasAttribute("steering.scootWindowSize"))
		scootWindowSize = character->getIntAttribute("steering.scootWindowSize");
	else
		scootWindowSize = 3;

	if (character && character->hasAttribute("steering.pedMaxTurningRateMultiplier"))
		pedMaxTurningRateMultiplier = (float) character->getDoubleAttribute("steering.pedMaxTurningRateMultiplier");
	else
		pedMaxTurningRateMultiplier = 20.f;

	if (character && character->hasAttribute("steering.tiltGain"))
		tiltGain = (float) character->getDoubleAttribute("steering.tiltGain");
	else
		tiltGain = 2.0f;

	if (character && character->hasAttribute("steering.terrainMode"))
		terrainMode = character->getBoolAttribute("steering.terrainMode");
	else
		terrainMode = false;

	setSteerParamsDirty(false);

}

void PPRAISteeringAgent::updateSteerStateName()
{
	if (!character)
		return;

	std::string prefix = character->getName();
	if (character->statePrefix != "")
		prefix = character->statePrefix;
	stepStateName = prefix + "Step";
	locomotionName = prefix + "Locomotion";
	startingLName = prefix + "StartingLeft";
	startingRName = prefix + "StartingRight";
	idleTurnName = prefix + "IdleTurn";
	jumpName = prefix + "Jump";
}



SrVec PPRAISteeringAgent::getCollisionFreeGoal( SrVec targetPos, SrVec curPos )
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBSteerManager* manager = scene->getSteerManager();

	SmartBody::SBCharacter* character = this->getCharacter();
	if (character->hasAttribute("steering.useCollisionFreeGoal"))
	{
		if (!character->getBoolAttribute("steering.useCollisionFreeGoal"))
		{
			// if steering.useCollisionFreeGoal is set to False, use the original target
			return targetPos;
		}
	}
	
	SrVec scaleCurPos = curPos;
	SrVec scaleTarget = targetPos;	
	SrVec newGoal = scaleTarget;
	scaleCurPos.y = 0.f;
	scaleTarget.y = 0.f;
	
	try 
	{
		SteerSuiteEngineDriver* driver = manager->getEngineDriver();	
		float penetrationDeth = driver->collisionPenetration(newGoal, agent->radius(), agent);
		float dist = (scaleTarget-scaleCurPos).len();
		SrVec dir = (scaleCurPos-scaleTarget);
		dir.normalize();
		// not move at all if the agent is close to target and there are obstacles on the way.
		if (penetrationDeth > 0.f && dist <= agent->radius()) 
			return scaleCurPos;

		// if there are obstacles in target position, setting the new target closer to agent and test again
		while (penetrationDeth > 0.f && dist > agent->radius())
		{
			float offset = min(penetrationDeth+0.2f,agent->radius());
			newGoal = newGoal + dir*offset;
			penetrationDeth = driver->collisionPenetration(newGoal, agent->radius(), agent);
			dist -= offset;
		}
	}
	catch  (std::exception &e) 
	{
		LOG("Exception '%s' occurs at PPRAISteeringAgent::getCollisionFreeGoal",e.what());
	}

	return newGoal;
}

void PPRAISteeringAgent::evaluate(double dtime)
{
	//LOG("evaluate PPRAISteeringAgent, time = %f",dtime);
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBSteerManager* manager = scene->getSteerManager();
	
	float dt = (float) dtime;

	if (!character)
		return;

	if (isSteerParamsDirty())
		initSteerParams();

	//---get current world offset position
	float x, y, z;
	float yaw, pitch, roll;
	character->get_world_offset(x, y, z, yaw, pitch, roll);
	curSteerDir = SrVec(sin(degToRad(yaw)), 0, cos(degToRad(yaw)));
	curSteerPos = SrVec(x,y,z);
	nextSteerPos = curSteerPos;

	//LOG("Character world offset : x = %f, y = %f, z = %f",x,y,z);

	// parameter testing
	if (paramTestFlag)
		parameterTesting();

	if (!agent)
		return;

	
	PPRAgent* pprAgent = dynamic_cast<PPRAgent*>(agent);
	//mcu.mark("Steering",0,"Evaluate");
	int numGoals;
	try 
	{
		const std::queue<SteerLib::AgentGoalInfo>& goalQueue = pprAgent->getLandmarkQueue();
		numGoals = goalQueue.size();

		// make sure the character is within the grid
		if (!pathFollowing && manager->getEngineDriver()->_engine->getSpatialDatabase()->getCellIndexFromLocation(x * scene->getScale(), z * scene->getScale()) == -1)
		{
			if (numGoals > 0)
			{
				LOG("Character %s is out of range of grid (%f, %f). All goals will be cancelled.", character->getName().c_str(), x * scene->getScale(), z * scene->getScale());
				agent->clearGoals();
				sendLocomotionEvent("failure");
			}
		}

		if (numGoals == 0) {
			newSpeed = 0.0f;
		}

		if (goalList.size() != 0 && numGoals == 0)
		{
			float goalx = goalList.front();
			goalList.pop_front();
			float goaly = goalList.front();
			goalList.pop_front();
			float goalz = goalList.front();
			goalList.pop_front();
			agent->clearGoals();
			SrVec newGoal = getCollisionFreeGoal(SrVec(goalx,goaly,goalz)*scene->getScale(),curSteerPos*scene->getScale());		

			//LOG("original goal = %f %f %f, new goal = %f %f %f",goalx,goaly,goalz, newGoal.x*100.f, newGoal.y*100.f, newGoal.z*100.f);
			SteerLib::AgentGoalInfo goal;		
			goal.desiredSpeed = desiredSpeed;
			goal.goalType = SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET;
			goal.targetIsRandom = false;
			// goal.targetLocation = Util::Point(goalx * scene->getScale(), 0.0f, goalz * scene->getScale());
			// compute a collision free goal toward the target
			goal.targetLocation = Util::Point(newGoal.x , 0.0f, newGoal.z);
			// make sure that the desired goal is within the bounds of the steering grid
			if (!pathFollowing && manager->getEngineDriver()->_engine->getSpatialDatabase()->getCellIndexFromLocation(goal.targetLocation.x, goal.targetLocation.z) == -1)
			{
				LOG("Goal (%f, %f) for character %s is out of range of grid.", goal.targetLocation.x, goal.targetLocation.z, character->getName().c_str());
			}
			else
			{
				agent->addGoal(goal);
			}

			if (pprAgent)
			{
				pprAgent->_nextFrameToRunLongTermPlanningPhase = 0;
				pprAgent->_nextFrameToRunMidTermPlanningPhase = 0;
			}
		}
		numGoals = goalQueue.size();
	}
	catch  (std::exception &e) 
	{
		LOG("Exception '%s' occurs at PPRAISteeringAgent::evaluate",e.what());
	}
	

	

	// Update Steering Engine (position, orientation, scalar speed)
	Util::Point newPosition(x * scene->getScale(), 0.0f, z * scene->getScale());
	Util::Vector newOrientation = Util::rotateInXZPlane(Util::Vector(0.0f, 0.0f, 1.0f), yaw * float(M_PI) / 180.0f);
	static float accumTime = 0.f;
	try {
			
		agent->updateAgentState(newPosition, newOrientation, newSpeed);				
		//accumTime += dt;
		//agent->updateAI((float) SmartBody::SBScene::getScene()->getSimulationManager()->getTime(), dt, _curFrame++);
	} catch (Util::GenericException& ge) {
		std::string message = ge.what();
		if (lastMessage == message)
		{
			numMessageRepeats++;
			if (numMessageRepeats % 100 == 0)
			{
				LOG("Message repeated %d times", numMessageRepeats);
			}
			else
			{
				numMessageRepeats = 0;
				LOG("Problem updating agent state: %s", ge.what());
			}
		}
	}


	try 
	{
		const std::queue<SteerLib::AgentGoalInfo>& goalQueue = pprAgent->getLandmarkQueue();

		int remainingGoals = goalQueue.size();
		if (remainingGoals < numGoals)
		{
			// AI satisfied the goals, send message
			sendLocomotionEvent("success");
		}
	}
	catch  (std::exception &e) 
	{
		LOG("Exception '%s' occurs at PPRAISteeringAgent::getCollisionFreeGoal",e.what());
	}
	

	
	// Prepare Data
	//---if there is a target, update the goal
	if (target)
	{
		float x1, y1, z1;
		float yaw1, pitch1, roll1;
		target->get_world_offset(x1, y1, z1, yaw1, pitch1, roll1);
		float dist = sqrt((x - x1) * (x - x1) + (z - z1) * (z - z1));
		if (dist > distThreshold)
		{
			agent->clearGoals();
			SteerLib::AgentGoalInfo goal;
			goal.desiredSpeed = desiredSpeed;
			goal.goalType = SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET;
			goal.targetIsRandom = false;
			if (dynamic_cast<SmartBody::SBCharacter*>(target)) // if target is a character
				goal.targetLocation = Util::Point(x1 * scene->getScale(), 0.0f, (z1 * scene->getScale() - 1.0f * scene->getScale()));
			else 
				goal.targetLocation = Util::Point(x1 * scene->getScale(), 0.0f, z1 * scene->getScale());
			
			agent->addGoal(goal);
		}
	}
	character->_lastReachStatus = character->_reachTarget;
	character->_reachTarget = false;

	// Evaluate
	//float newSpeed = desiredSpeed;
	// Meat Hook Locomotion Evaluation
	if (character->locomotion_type == character->Basic)
	{
		if (!character->basic_locomotion_ct)
			return;
		newSpeed = evaluateBasicLoco(dt, x, y, z, yaw);
	}
	
	if (character->locomotion_type == character->New)
	{
		if (!character->new_locomotion_ct)
			return;
		newSpeed = evaluateNewLoco(dt, x, y, z, yaw);
	}

	// Example-Based Locomotion Evaluation
	if (character->locomotion_type == character->Example)
	{
		if (!character->param_animation_ct)
			return;

		if (!pathFollowing)
		//if (1)
		{
			newSpeed = evaluateExampleLoco(dt, x, y, z, yaw);
		}
		else
		{
			evaluatePathFollowing(dt, x, y, z, yaw);
		}
	}

	try 
	{
		const std::queue<SteerLib::AgentGoalInfo>& goalQueue = pprAgent->getLandmarkQueue();
		_numSteeringGoal = goalQueue.size();
	}
	catch  (std::exception &e) 
	{
		LOG("Exception '%s' occurs at PPRAISteeringAgent::getCollisionFreeGoal",e.what());
	}
	

	//mcu.mark("Steering");
}

void PPRAISteeringAgent::sendLocomotionEvent(const std::string& status)
{
	std::string eventType = "locomotion";
	SmartBody::SBMotionEvent motionEvent;
	motionEvent.setType(eventType);			
	std::stringstream strstr;
	strstr << character->getName() << " " << status;
	motionEvent.setParameters(strstr.str());
	SmartBody::SBEventManager* manager = SmartBody::SBScene::getScene()->getEventManager();		
	manager->handleEvent(&motionEvent, SmartBody::SBScene::getScene()->getSimulationManager()->getTime());
}

void PPRAISteeringAgent::evaluatePathFollowing(float dt, float x, float y, float z, float yaw)
{
	PABlendData* curStateData = character->param_animation_ct->getCurrentPABlendData();	
	std::string curStateName = "";
	if (curStateData)
		curStateName = curStateData->state->stateName;

	bool inTransition = character->param_animation_ct->isInTransition();
	
	bool locomotionEnd = false;
	static int counter = 0;		
	float sceneScale = 1.f/SmartBody::SBScene::getScene()->getScale();	
	//float distThreshold = 0.05f*sceneScale;
	//float speedThreshold = 0.05f*sceneScale;	
	float pathDistThreshold = distThreshold*sceneScale;
	float pathSpeedThreshold = speedThreshold*sceneScale;	
	float smallDistThreshold = 0.05f*sceneScale;

	SmartBody::SBAnimationBlend* blend = NULL;
	if (curStateData)
		blend = dynamic_cast<SmartBody::SBAnimationBlend*>(curStateData->state);

	float parameterScale = 1.f;
	if (character && blend)
	{
		SmartBody::SBRetarget* retarget = SmartBody::SBScene::getScene()->getRetargetManager()->getRetarget(blend->getBlendSkeleton(),character->getSkeleton()->getName());				
		if (retarget)
			parameterScale = 1.f/retarget->getHeightRatio();
	}

	//if (steerPath.pathLength() == 0) // do nothing if there is no steer path
	//	return; 
	if (character->param_animation_ct->isIdle() && steerPath.pathLength() > 0)    // need to define when you want to start the locomotion
	{
		PABlend* locoState = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(locomotionName.c_str());
		SrVec pathDir;
		float pathDist;		
		curSteerPos = SrVec(x,0,z);
		
		//float distToTarget = (curSteerPos - steerPath.pathPoint(steerPath.pathLength()-0.01f)).len();
		float distToTarget = (curSteerPos - steerPath.pathGoalPoint()).len();
		float distToPathEnd = (curSteerPos - steerPath.pathPoint(steerPath.pathLength()-0.01f)).len();
		SrVec targetPos = steerPath.closestPointOnNextGoal(SrVec(x,0,z),pathDir,pathDist);
		//SrVec targetPos = steerPath.closestPointOnPath(SrVec(x,0,z),pathDir,pathDist);
		float distOnPath = steerPath.pathDistance(targetPos);
		float sceneScale = 1.f/SmartBody::SBScene::getScene()->getScale();
		//LOG("pathfollowing:targetSpeed = %f",this->currentTargetSpeed);
		float maxSpeed = this->currentTargetSpeed*sceneScale;//(float)character->getDoubleAttribute("steering.pathMaxSpeed")*sceneScale;
		nextPtOnPath = steerPath.pathPoint(distOnPath+maxSpeed);
		
		float targetAngle = radToDeg(atan2(nextPtOnPath.x - x, nextPtOnPath.z - z));
		normalizeAngle(targetAngle);
		//LOG("nextPtOnPath = %f %f, curePt = %f %f",nextPtOnPath.x,nextPtOnPath.z, x,z);
		//LOG("path following:normalize target angle = %f",targetAngle);
		normalizeAngle(yaw);
		float diff = targetAngle - yaw;
		normalizeAngle(diff);
		
		// using the transition motion with different direction
		if (distToTarget > smallDistThreshold*3.f || distToPathEnd > smallDistThreshold*3.f)
		{
			if (character->getBoolAttribute("steering.pathStartStep"))// && distToTarget > distThreshold*10.f)
				//if (0)
			{
				//LOG("Path following, start locomotion, diff = %f",diff);
				startIdleToWalkState(diff);
			}
			else
			{
				startLocomotionState();
			}	
		}
		else
		{
			// end locomotion directly
			character->trajectoryGoalList.clear();
			agent->clearGoals();
			goalList.clear();
			steerPath.clearPath();					
		}
		counter = 0;	
	}

	// always update current steering information
	float curSpeed;
	float curTurningAngle;
	float curScoot;

	//LOG("dt = %f, curSteerPos = %f %f, dir = %f",dt, x,z, yaw);

	if (curStateName == locomotionName && steerPath.pathLength() != 0)
	{
		//locomotionEnd = true;
		

		curStateData->state->getParametersFromWeights(curSpeed, curTurningAngle, curScoot, curStateData->weights);		
		curSpeed /= parameterScale;


		//curSpeed = steerCurSpeed;
		//curTurningAngle = steerCurAngle;
		//curScoot = steerCurScoot;

		curSteerPos = SrVec(x,0,z);
		curSteerDir = SrVec(sin(degToRad(yaw)), 0, cos(degToRad(yaw)));

		SrMat rotMat; rotMat.roty(curTurningAngle*dt);
		nextSteerDir = curSteerDir*rotMat;
		float radius = fabs(curSpeed/curTurningAngle);
		nextSteerPos = curSteerPos;// + curSteerDir*curSpeed*dt;//curSteerPos - curSteerDir*radius + nextSteerDir*radius; //
		// predict next position
		//LOG("curSpeed = %f, curTurningAngle = %f, curScoot = %f",curSpeed,curTurningAngle,curScoot);
#if 1
		//SrVec nextPos = curPos + (curDir+nextDir)*0.5f*curSpeed*dt;
		SrVec pathDir;
		float pathDist;
		//nextPtOnPath = steerPath.closestPointOnPath(nextSteerPos,pathDir,pathDist);		
		nextPtOnPath = steerPath.closestPointOnNextGoal(nextSteerPos, pathDir, pathDist);
		float distOnPath = steerPath.pathDistance(nextPtOnPath);
		nextPtOnPath = steerPath.pathPoint(distOnPath+curSpeed);
		//SrVec ptDir = nextPtOnPath - curSteerPos; ptDir.normalize();
		SrVec ptDir = steerPath.pathGoalPoint() - curSteerPos; ptDir.normalize();		
		steerPath.advanceToNextGoal(distOnPath+curSpeed);		
		
		float maxSpeed = this->currentTargetSpeed*sceneScale;//(float)character->getDoubleAttribute("steering.pathMaxSpeed")*sceneScale;
		float minSpeed = (float)character->getDoubleAttribute("steering.pathMinSpeed")*sceneScale;
		float angAcc = (float)character->getDoubleAttribute("steering.pathAngleAcc");
		float detectSeg = maxSpeed*1.5f;
		const float maxAcc = (maxSpeed)/5.f;//-minSpeed);///2.f;
		const float maxDcc = -(maxSpeed)/5.f;

		float pathCurvature = steerPath.pathCurvature(distOnPath-detectSeg*0.2f,distOnPath+detectSeg*0.8f)*2.0f;
		if (pathCurvature > M_PI) pathCurvature = (float)M_PI;
		static int counter = 0;		
		float curvSpeed = (1.f - pathCurvature/(float)M_PI)*(maxSpeed-minSpeed) + minSpeed;
		float acc = (curSpeed > curvSpeed) ? maxDcc : maxAcc;
		// do whatever you need to calculate
 		float newSpeed = curSpeed;
		if (!inTransition)
			newSpeed = curSpeed + acc*dt; 

		float distToTarget = (curSteerPos - steerPath.pathPoint(steerPath.pathLength()-0.01f)).len();		
		if (distToTarget < curSpeed * brakingGain && steerPath.atLastGoal()) // if close to target, slow down directly
			newSpeed = distToTarget / brakingGain;
		
		
		if (distToTarget < pathDistThreshold && newSpeed < pathSpeedThreshold &&  steerPath.atLastGoal())
		{
			locomotionEnd = true;			
		}

		float newTurningAngle = radToDeg(asin(cross(curSteerDir,ptDir).y));
		float nextTurningAngle = curTurningAngle;
		if (!inTransition)
		{
			normalizeAngle(newTurningAngle);
			newTurningAngle = newTurningAngle * (float)character->getDoubleAttribute("steering.paLocoAngleGain");			
			if (newTurningAngle > curTurningAngle)
			{
				nextTurningAngle = curTurningAngle + angAcc*dt;
				if (nextTurningAngle > newTurningAngle) nextTurningAngle = newTurningAngle;
			}
			else
			{
				nextTurningAngle = curTurningAngle - angAcc*dt;
				if (nextTurningAngle < newTurningAngle) nextTurningAngle = newTurningAngle;
			}

		}
		
				
		//float newTurningAngle = normalizeAngle();//dt;
		//float newTurningAngle = (asin(cross(curSteerDir,ptDir).y))/dt;
		float newScoot =0.f;

#if 0 // output debug info
		if (counter > 100)
		{
			counter = 0;
			LOG("path curvature = %f, curveSpeed = %f, newSpeed = %f, distToTarget = %f, newTurningAngle = %f",pathCurvature,curvSpeed,newSpeed,distToTarget, newTurningAngle);			
		}
		counter++;
#endif
		// update locomotion state
		float tnormal[3];
		float terrainHeight = 0.0f;
		Heightfield* heightField = SmartBody::SBScene::getScene()->getHeightfield();
		if (heightField)
		{
			terrainHeight = SmartBody::SBScene::getScene()->queryTerrain(x, z, tnormal);
		}
		float difference = y - terrainHeight;
		float ang = - difference * tiltGain;
		//LOG("current y %f, terrain height %f, character height %f, difference %f, angle %f", y, terrainHeight, character->getHeight(), difference, ang);

		std::vector<double> weights;
		weights.resize(curStateData->state->getNumMotions());
		//if (terrainMode)
		//	curStateData->state->getWeightsFromParameters(newSpeed*parameterScale, nextTurningAngle, ang, weights);
		//else		
		curStateData->state->getWeightsFromParameters(newSpeed*parameterScale, nextTurningAngle, newScoot, weights);
		//LOG("dt = %f, newSpeed = %f, newTurningAngle = %f, newScoot = %f",dt, newSpeed,nextTurningAngle,newScoot);
		//curStateData->state->getWeightsFromParameters(newSpeed*parameterScale, 0.f, 0.f, weights);
		character->param_animation_ct->updateWeights(weights);	
#endif
		//steerCurSpeed = newSpeed;
		//steerCurAngle = nextTurningAngle;
		//steerCurScoot = newScoot;		
	}
	if (locomotionEnd)      // need to define when you want to end the locomotion
	{
		locomotionHalt();
		facingAdjust = true;		
		//character->param_animation_ct->schedule(NULL, weights);		
		//LOG("path following end");			
	}
	// adjust facing angle 			
	if (fabs(facingAngle) <= 180 && character->param_animation_ct->isIdle() && facingAdjust )
	{		
		float diff = facingAngle - yaw;
		//LOG("Idle state, facing angle = %f, diff = %f", facingAngle, diff);
		normalizeAngle(diff);
		adjustFacingAngle(diff);			
	}
}


void PPRAISteeringAgent::setAgent(SteerLib::AgentInterface* a)
{
	agent = a;
}

SteerLib::AgentInterface* PPRAISteeringAgent::getAgent()
{
	return agent;
}

void PPRAISteeringAgent::setCharacter(SmartBody::SBCharacter* c)
{
	character = c;
	addSteeringAttributes();
}


void PPRAISteeringAgent::setTargetAgent(SmartBody::SBPawn* tChar)
{
	target = tChar;
}

SmartBody::SBPawn* PPRAISteeringAgent::getTargetAgent()
{
	return target;
}

void PPRAISteeringAgent::normalizeAngle(float& angle)
{
	while (angle > 180.0f)
		angle -= 360.0f;
	while (angle < -180.0f)
		angle += 360.0f;	
}

/*
	Notes:
	- The proximity is decided by Steering Suite
	- Facing not supported
*/
float PPRAISteeringAgent::evaluateBasicLoco(float dt, float x, float y, float z, float yaw)
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	
 	PPRAgent* pprAgent = dynamic_cast<PPRAgent*>(agent);
	const std::queue<SteerLib::AgentGoalInfo>& goalQueue = pprAgent->getLandmarkQueue();
	const SteerLib::SteeringCommand & steeringCommand = pprAgent->getSteeringCommand();


	//--------------------------------------------------------
	// WJ added start
	// TODO: define/initialize these vars properly:
	Util::Vector totalSteeringForce;
	Util::Vector newForward;

	//
	// choose the new orientation of the agent
	//
	if (!steeringCommand.aimForTargetDirection) {
		// simple turning case "turn left" or "turn right"
		newForward = forward + PED_MAX_TURNING_RATE * steeringCommand.turningAmount * rightSide;
	}
	else {
		// turn to face "targetDirection" - magnitude of targetDirection doesn't matter
		float initialDot = dot(steeringCommand.targetDirection, rightSide);
		float turningRate = (initialDot > 0.0f) ? PED_MAX_TURNING_RATE : -PED_MAX_TURNING_RATE;  // positive rate is right-turn
		newForward = forward + turningRate * fabsf(steeringCommand.turningAmount) * rightSide;
		float newDot = dot(steeringCommand.targetDirection, rightSideInXZPlane(newForward)); // dot with the new side vector
		if (initialDot*newDot <= 0.0f) {
			// if the two dot products are different signs, that means we turned too much, so just set the new forward to the goal vector.
			// NOTE that above condition is less than **OR EQUALS TO** - that is because initialDot will be zero when the agent is 
			// pointing already in the exact correct direction.  If we used strictly less than, the pedestrian oscillates between a 
			// small offset direction and the actual target direction.

			//
			// TODO: known bug here: if agent is facing exactly opposite its goal, it completely flips around because of this condition.
			//   ironically, because of the equals sign above...
			//   proper solution is to add extra conditions that verify the original direction of forward was not opposite of targetDirection.
			//
			// WJ: need to change here
			newForward = Util::Vector(steeringCommand.targetDirection.x, 0.0f, steeringCommand.targetDirection.z);

		}
	}

	//
	// set the orientation
	//
	newForward = normalize(newForward);
	forward = newForward;
	rightSide = rightSideInXZPlane(newForward);

	// This next line is specific to command-based steering, but is not physically based.
	// everything else in command-based steering, however, is physcially based.
	velocity = newForward * currentSpeed;

	//
	// choose the force of the agent.  In command-based mode, the force is always aligned 
	// with the agent's forward facing direction, so we can use scalars until we add 
	// side-to-side scoot at the end.
	//
	assert(fabsf(steeringCommand.acceleration) <= 1.0f); // -1.0f <= acceleration <= 1.0f;
	if (!steeringCommand.aimForTargetSpeed) {
		// simple "speed up" or "slow down"
		totalSteeringForce = PED_MAX_FORCE * steeringCommand.acceleration * forward;
	}
	else {
		// accelerate towards a target speed
		// do it the naive greedy way;
		//
		// the most force you can apply without making velocity direction flip:
		// (force / mass) * time-step = delta-speed
		// if delta-speed == -speed
		// force * mass * time-step = -speed
		//
		float maxBackwardsForce = (-PED_BRAKING_RATE * fabsf(currentSpeed) * 60.0f/* * _mass / _dt*/);
		float scalarForce = (steeringCommand.targetSpeed - currentSpeed) * 8.0f; // crudely trying to make accelerations quicker...
		if (scalarForce > PED_MAX_FORCE) scalarForce = PED_MAX_FORCE;
		if (scalarForce < maxBackwardsForce) scalarForce = maxBackwardsForce;
		totalSteeringForce = scalarForce * forward; // forward is a unit vector, normalized during turning just above.
	}

	// TODO: should we clamp scoot?
	// add the side-to-side motion to the planned steering force.
	totalSteeringForce = totalSteeringForce + PED_SCOOT_RATE * steeringCommand.scoot * rightSide;

	// WJ added end
	//---------------------------------------------------------------------------


	float angleGlobal = radToDeg(atan2(steeringCommand.targetDirection.x, steeringCommand.targetDirection.z));
	normalizeAngle(angleGlobal);
	normalizeAngle(yaw);
	float angleDiff = angleGlobal - yaw;
	//LOG("turning Rate= %f\n",angleDiff/dt);
	normalizeAngle(angleDiff);

	float newSpeed = desiredSpeed;

	int numGoals = goalQueue.size();
	if (numGoals == 0)
	{
		character->_reachTarget = true;
		character->basic_locomotion_ct->setValid(false);
		character->basic_locomotion_ct->setMovingSpd(0.0f);
		character->basic_locomotion_ct->setTurningSpd(0.0f);
		character->basic_locomotion_ct->setScootSpd(0.0f);
		newSpeed = 0.0f;
	}
	else
	{
		character->basic_locomotion_ct->setValid(true);
		float curSpeed = character->basic_locomotion_ct->getMovingSpd() * scene->getScale();
		if (steeringCommand.aimForTargetSpeed)
		{
			if (curSpeed < steeringCommand.targetSpeed)
			{
				curSpeed += acceleration * dt;
				if (curSpeed > steeringCommand.targetSpeed)
					curSpeed = steeringCommand.targetSpeed;
			}
			else
			{
				curSpeed -= acceleration * dt;
				if (curSpeed < steeringCommand.targetSpeed)
					curSpeed = steeringCommand.targetSpeed;
			}
		}
		else
			curSpeed += acceleration * dt;
		newSpeed = curSpeed;
		curSpeed = curSpeed / scene->getScale();
		character->basic_locomotion_ct->setMovingSpd(curSpeed);	


		//-------------------------------
		// WJ added start

		// do euler step with force
		// compute acceleration and velocity by a simple Euler step
		const Util::Vector clippedForce = clamp(totalSteeringForce, PED_MAX_FORCE);
		Util::Vector acceleration = (clippedForce/* / _mass*/);
		velocity = velocity + (dt*acceleration);
		velocity = clamp(velocity, PED_MAX_SPEED);  // clamp _velocity to the max speed
		currentSpeed = velocity.length();
		forward = Util::normalize(velocity);
		angleGlobal = radToDeg(atan2(forward.x, forward.z));
		normalizeAngle(angleGlobal);

		curSpeed = currentSpeed / scene->getScale();
		newSpeed = currentSpeed;
		character->basic_locomotion_ct->setMovingSpd(curSpeed);
		character->basic_locomotion_ct->setDesiredHeading(angleGlobal); // affective setting

		// WJ added end
		//------------------------------------


		character->basic_locomotion_ct->setTurningSpd(angleDiff * basicLocoAngleGain);
		float curScoot = character->basic_locomotion_ct->getScootSpd() / basicLocoScootGain;
		if (steeringCommand.scoot != 0.0)
		{
			if (curScoot < steeringCommand.scoot)
				curScoot += scootAcceleration * dt;
			else
				curScoot -= scootAcceleration * dt;	
		}
		else
		{
			if (fabs(curScoot) < scootThreshold)
				curScoot = 0.0f;
			else
			{
				if (curScoot > 0.0f)
				{
					curScoot -= scootAcceleration * dt;
					if (curScoot < 0.0)	curScoot = 0.0;
				}
				else
				{
					curScoot += scootAcceleration * dt;
					if (curScoot > 0.0)	curScoot = 0.0;
				}
			}
		}
		character->basic_locomotion_ct->setScootSpd(curScoot * basicLocoScootGain);
	}
	return newSpeed;
}

float PPRAISteeringAgent::evaluateNewLoco(float dt, float x, float y, float z, float yaw)
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
 	PPRAgent* pprAgent = dynamic_cast<PPRAgent*>(agent);
	const std::queue<SteerLib::AgentGoalInfo>& goalQueue = pprAgent->getLandmarkQueue();
	const SteerLib::SteeringCommand & steeringCommand = pprAgent->getSteeringCommand();
	Util::Vector totalSteeringForce;
	float angleGlobal;
	float curSpeed;
	//--------------------------------------------------------
	// WJ added start
	// TODO: define/initialize these vars properly:
	Util::Vector newForward;

	//
	// choose the new orientation of the agent
	//
	if (!steeringCommand.aimForTargetDirection) {
		// simple turning case "turn left" or "turn right"
		newForward = forward + PED_MAX_TURNING_RATE * steeringCommand.turningAmount * rightSide;
	}
	else {
		// turn to face "targetDirection" - magnitude of targetDirection doesn't matter
		float initialDot = dot(steeringCommand.targetDirection, rightSide);
		float turningRate = (initialDot > 0.0f) ? PED_MAX_TURNING_RATE : -PED_MAX_TURNING_RATE;  // positive rate is right-turn
		turningRate *= (float)character->getDoubleAttribute("TurningRate") *dt;//Slows the turning rate
		newForward = forward + turningRate * fabsf(steeringCommand.turningAmount) * rightSide;
		float newDot = dot(steeringCommand.targetDirection, rightSideInXZPlane(newForward)); // dot with the new side vector
		if (initialDot*newDot <= 0.0f) {
			// if the two dot products are different signs, that means we turned too much, so just set the new forward to the goal vector.
			// NOTE that above condition is less than **OR EQUALS TO** - that is because initialDot will be zero when the agent is 
			// pointing already in the exact correct direction.  If we used strictly less than, the pedestrian oscillates between a 
			// small offset direction and the actual target direction.

			//
			// TODO: known bug here: if agent is facing exactly opposite its goal, it completely flips around because of this condition.
			//   ironically, because of the equals sign above...
			//   proper solution is to add extra conditions that verify the original direction of forward was not opposite of targetDirection.
			//
			// WJ: need to change here

			newForward = Util::Vector(steeringCommand.targetDirection.x, 0.0f, steeringCommand.targetDirection.z);

		}
	}

	// set the orientation
	newForward = normalize(newForward);
	forward = newForward;
	rightSide = rightSideInXZPlane(newForward);

	// This next line is specific to command-based steering, but is not physically based.
	// everything else in command-based steering, however, is physcially based.
	velocity = newForward * currentSpeed;

	// choose the force of the agent.  In command-based mode, the force is always aligned 
	// with the agent's forward facing direction, so we can use scalars until we add 
	// side-to-side scoot at the end.

	assert(fabsf(steeringCommand.acceleration) <= 1.0f); // -1.0f <= acceleration <= 1.0f;
	if (!steeringCommand.aimForTargetSpeed) {
		// simple "speed up" or "slow down"
		totalSteeringForce = PED_MAX_FORCE * steeringCommand.acceleration * forward;
	}
	else {
		// accelerate towards a target speed
		// do it the naive greedy way;
		//
		// the most force you can apply without making velocity direction flip:
		// (force / mass) * time-step = delta-speed
		// if delta-speed == -speed
		// force * mass * time-step = -speed
		//
		float maxBackwardsForce = (-PED_BRAKING_RATE * fabsf(currentSpeed) * 60.0f/* * _mass / _dt*/);
		float scalarForce = (steeringCommand.targetSpeed - currentSpeed) * 8.0f; // crudely trying to make accelerations quicker...
		if (scalarForce > PED_MAX_FORCE) scalarForce = PED_MAX_FORCE;
		if (scalarForce < maxBackwardsForce) scalarForce = maxBackwardsForce;
		totalSteeringForce = scalarForce * forward; // forward is a unit vector, normalized during turning just above.
	}

	// TODO: should we clamp scoot?
	// add the side-to-side motion to the planned steering force.
	totalSteeringForce = totalSteeringForce + PED_SCOOT_RATE * steeringCommand.scoot * rightSide;

	// WJ added end
	//---------------------------------------------------------------------------

#if !SOLVE_HEADING_ONLY
	angleGlobal = radToDeg(atan2(steeringCommand.targetDirection.x, steeringCommand.targetDirection.z));
	normalizeAngle(angleGlobal);
	normalizeAngle(yaw);
	float angleDiff = angleGlobal - yaw;
	normalizeAngle(angleDiff);

	float newSpeed = desiredSpeed;

	int numGoals = goalQueue.size();
	if (numGoals == 0)
	{
		character->_reachTarget = true;
		character->new_locomotion_ct->setValid(false);
		character->new_locomotion_ct->setMovingSpd(0.0f);
		character->new_locomotion_ct->setTurningSpd(0.0f);
		character->new_locomotion_ct->setScootSpd(0.0f);
		newSpeed = 0.0f;
	}
	else
	{
		character->new_locomotion_ct->setValid(true);
		curSpeed = character->new_locomotion_ct->getMovingSpd() * scene->getScale();
		if (steeringCommand.aimForTargetSpeed)
		{
			if (curSpeed < steeringCommand.targetSpeed)
			{
				curSpeed += acceleration * dt;
				if (curSpeed > steeringCommand.targetSpeed)
					curSpeed = steeringCommand.targetSpeed;
			}
			else
			{
				curSpeed -= acceleration * dt;
				if (curSpeed < steeringCommand.targetSpeed)
					curSpeed = steeringCommand.targetSpeed;
			}
		}
		else
			curSpeed += acceleration * dt;
		newSpeed = curSpeed;
		curSpeed = curSpeed / scene->getScale();
		character->new_locomotion_ct->setMovingSpd(curSpeed);	
#endif
	static bool flag=true;
		//-------------------------------
		// WJ added start
	int numGoals = goalQueue.size();
	if (numGoals == 0)
	{
		if(flag)
		{
			flag= false;
			character->_reachTarget = true;
			sendLocomotionEvent("success");
			character->new_locomotion_ct->reset();
			//forward = Util::Vector(0.0f, 0.0f, 1.0f);
			//rightSide = rightSideInXZPlane(forward);
			newSpeed = 0.0f;
		}
	}
	else
	{
		flag=true;
		// do euler step with force
		// compute acceleration and velocity by a simple Euler step
		const Util::Vector clippedForce = clamp(totalSteeringForce, PED_MAX_FORCE);
		Util::Vector acceleration = (clippedForce/* / _mass*/);
		velocity = velocity + (dt*acceleration);
		velocity = clamp(velocity, PED_MAX_SPEED);  // clamp _velocity to the max speed
		currentSpeed = velocity.length();
		forward = Util::normalize(velocity);
		angleGlobal = radToDeg(atan2(forward.x, forward.z));
		normalizeAngle(angleGlobal);
		curSpeed = currentSpeed / scene->getScale();
		newSpeed = currentSpeed;
		character->new_locomotion_ct->setValid(true);
		character->new_locomotion_ct->setMovingSpd(curSpeed);
		character->new_locomotion_ct->setDesiredHeading(angleGlobal); // affective setting
		// WJ added end
		//------------------------------------
	}

#if !SOLVE_HEADING_ONLY
		character->new_locomotion_ct->setTurningSpd(angleDiff * newLocoAngleGain);
		float curScoot = character->new_locomotion_ct->getScootSpd() / newLocoScootGain;
		if (steeringCommand.scoot != 0.0)
		{
			if (curScoot < steeringCommand.scoot)
				curScoot += scootAcceleration * dt;
			else
				curScoot -= scootAcceleration * dt;	
		}
		else
		{
			if (fabs(curScoot) < scootThreshold)
				curScoot = 0.0f;
			else
			{
				if (curScoot > 0.0f)
				{
					curScoot -= scootAcceleration * dt;
					if (curScoot < 0.0)	curScoot = 0.0;
				}
				else
				{
					curScoot += scootAcceleration * dt;
					if (curScoot > 0.0)	curScoot = 0.0;
				}
			}
		}
		character->new_locomotion_ct->setScootSpd(curScoot * newLocoScootGain);
	}
#endif
	return newSpeed;
}

/*
	Notes:
	- Proximity controller by user
*/
float PPRAISteeringAgent::evaluateExampleLoco(float dt, float x, float y, float z, float yaw)
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	
	PPRAgent* pprAgent = dynamic_cast<PPRAgent*>(agent);
	const std::queue<SteerLib::AgentGoalInfo>& goalQueue = pprAgent->getLandmarkQueue();
	const SteerLib::SteeringCommand & steeringCommand = pprAgent->getSteeringCommand();
	//LOG("%f %f %f %f %f %f", steeringCommand.acceleration, steeringCommand.targetDirection.x, steeringCommand.targetDirection.y, steeringCommand.targetDirection.z, steeringCommand.targetSpeed, steeringCommand.turningAmount);
	

	//*** IMPORTANT: use the example-based animation to update the steering agent

	forward = agent->forward();
	rightSide = rightSideInXZPlane(forward);

	//--------------------------------------------------------
	// WJ added start
	// TODO: define/initialize these vars properly:
	float ped_max_turning_rate = PED_MAX_TURNING_RATE * pedMaxTurningRateMultiplier;

	//Util::Vector totalSteeringForce;
	Util::Vector newForward;

	//
	// choose the new orientation of the agent
	//
	if (!steeringCommand.aimForTargetDirection) {
		// simple turning case "turn left" or "turn right"
		newForward = forward + ped_max_turning_rate * steeringCommand.turningAmount * rightSide;
	}
	else {
		// turn to face "targetDirection" - magnitude of targetDirection doesn't matter
		float initialDot = dot(steeringCommand.targetDirection, rightSide);
		float turningRate = (initialDot > 0.0f) ? ped_max_turning_rate : -ped_max_turning_rate;  // positive rate is right-turn
		newForward = forward + turningRate * fabsf(steeringCommand.turningAmount) * rightSide;
		float newDot = dot(steeringCommand.targetDirection, rightSideInXZPlane(newForward)); // dot with the new side vector
		if (initialDot*newDot <= 0.0f) {
			// if the two dot products are different signs, that means we turned too much, so just set the new forward to the goal vector.
			// NOTE that above condition is less than **OR EQUALS TO** - that is because initialDot will be zero when the agent is 
			// pointing already in the exact correct direction.  If we used strictly less than, the pedestrian oscillates between a 
			// small offset direction and the actual target direction.

			//
			// TODO: known bug here: if agent is facing exactly opposite its goal, it completely flips around because of this condition.
			//   ironically, because of the equals sign above...
			//   proper solution is to add extra conditions that verify the original direction of forward was not opposite of targetDirection.
			//
			// WJ: need to change here
			newForward = Util::Vector(steeringCommand.targetDirection.x, 0.0f, steeringCommand.targetDirection.z);

		}
		//*** remove the gradually change of turning
		//*** let the animation system try its best to achieve this turning angle
		//newForward = Util::Vector(steeringCommand.targetDirection.x, 0.0f, steeringCommand.targetDirection.z);
	}

	//
	// set the orientation
	//
	newForward = normalize(newForward);
	forward = newForward;
	nextSteerDir = SrVec(newForward.x,newForward.y,newForward.z);
	

	rightSide = rightSideInXZPlane(newForward);

	bool sentLocomotionEvent = false;
	bool reachTarget = false;
	float agentToTargetDist = 0.0f;
	SrVec agentToTargetVec;
	float distToTarget = -1;
	
	try 
	{
		pprAgent->updateDesiredForward(forward);

		// WJ added end
		//---------------------------------------------------------------------------		
		if (goalQueue.size() > 0)
		{
			targetLoc.x = goalQueue.front().targetLocation.x;
			targetLoc.y = goalQueue.front().targetLocation.y;
			targetLoc.z = goalQueue.front().targetLocation.z;
			distToTarget = sqrt((x * scene->getScale() - targetLoc.x) * (x * scene->getScale() - targetLoc.x) + 
				(z * scene->getScale() - targetLoc.z) * (z * scene->getScale() - targetLoc.z));

			if (distToTarget < agent->radius()*10.f)
			{
				SmartBody::SBSteerManager* manager = scene->getSteerManager();
				SteerSuiteEngineDriver* driver = manager->getEngineDriver();	
				float penetration = driver->collisionPenetration(targetLoc,agent->radius(),agent);		
				if (penetration > 0) 
					collisionTime += dt;
				else
					collisionTime = 0;

				if (penetration > 0.f && collisionTime > 1.f) // if the current target become blocked
				{
					SrVec newTarget = getCollisionFreeGoal(targetLoc, curSteerPos*scene->getScale());
					agent->clearGoals();
					SteerLib::AgentGoalInfo goal;
					goal.desiredSpeed = desiredSpeed;
					goal.goalType = SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET;
					goal.targetIsRandom = false;
					//goal.targetLocation = Util::Point(goalx * scene->getScale(), 0.0f, goalz * scene->getScale());
					goal.targetLocation = Util::Point(newTarget.x, 0.0f, newTarget.z);
					//LOG("agent %s, newTarget = %f %f %f",character->getName().c_str(),newTarget.x, newTarget.y,newTarget.z);
					agent->addGoal(goal);
					collisionTime = 0;
					pprAgent->_nextFrameToRunLongTermPlanningPhase = 0;
					pprAgent->_nextFrameToRunMidTermPlanningPhase = 0;
				}		
			}


			if (distToTarget < distDownThreshold)
			{
				getAgent()->clearGoals();
				sendLocomotionEvent("success");
				sentLocomotionEvent = true;
				//LOG("dist close to target. stop locomotion");
			}
		}
	}
	catch  (std::exception &e) 
	{
		LOG("Exception '%s' occurs at PPRAISteeringAgent::getCollisionFreeGoal",e.what());
	}
	

	
	
	int numGoals = goalQueue.size();
	if (numGoals == 0)
	{
		reachTarget = true;		
		character->_reachTarget = reachTarget;
	}
	if (_numSteeringGoal == 0 && numGoals != 0 && distToTarget < distThreshold)
	{
		stepAdjust = true;
	}
	if (distToTarget > distThreshold)
		stepAdjust = false;

	// slow down mechanism when close to the target
	float targetSpeed = steeringCommand.targetSpeed;	
	if (distToTarget < targetSpeed * brakingGain && goalList.size() == 0)
		targetSpeed = distToTarget / brakingGain;

	if (stepAdjust)
		if (!character->param_animation_ct->hasPABlend(stepStateName.c_str()))
		{
			agentToTargetDist = distToTarget / scene->getScale();
			agentToTargetVec.x = targetLoc.x - x * scene->getScale();
			agentToTargetVec.y = targetLoc.y - y * scene->getScale();
			agentToTargetVec.z = targetLoc.z - z * scene->getScale();
			agentToTargetVec /= scene->getScale();
		}

	PABlendData* curStateData =  character->param_animation_ct->getCurrentPABlendData();
	const std::string& curStateName = character->param_animation_ct->getCurrentStateName();
	const std::string& nextStateName = character->param_animation_ct->getNextStateName();

	//---If you are close enough to the target when starting locomotion, use step adjust
	if (character->param_animation_ct->isIdle() && (agentToTargetDist > distDownThreshold))
	{
		SrVec heading = SrVec(sin(degToRad(yaw)), 0, cos(degToRad(yaw)));
		float y = dot(agentToTargetVec, heading);
		SrVec verticalHeading = SrVec(sin(degToRad(yaw - 90)), 0, cos(degToRad(yaw - 90)));
		float x = dot(agentToTargetVec, verticalHeading);
		if (!character->param_animation_ct->hasPABlend(stepStateName.c_str()))
		{
			adjustLocomotionBlend(character, stepStateName, 2, x, y, 0, false, false);
			return 0;
		}		
		
	}

	//---start locomotion
	if (character->param_animation_ct->isIdle() && numGoals != 0 && nextStateName == "" && distToTarget > distDownThreshold)
	{
		// check to see if there's anything obstacles around it		
		//float targetAngle = radToDeg(atan2(pprAgent->getStartTargetPosition().x - x * scene->getScale(), pprAgent->getStartTargetPosition().z - z * scene->getScale()));
		float targetAngle = radToDeg(atan2(targetLoc.x - x * scene->getScale(), targetLoc.z - z * scene->getScale()));
		normalizeAngle(targetAngle);
		//LOG("steering:normalize target angle = %f",targetAngle);
		normalizeAngle(yaw);
		float diff = targetAngle - yaw;
		normalizeAngle(diff);

		// Improve on the starting angle by examining whether there's obstacles around
		

		std::vector<float> neigbors;
		const std::vector<std::string>& pawns = SmartBody::SBScene::getScene()->getPawnNames();
		for (std::vector<std::string>::const_iterator pawnIter = pawns.begin();
			pawnIter != pawns.end();
			pawnIter++)
		{
		
			SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(*pawnIter);
			if (pawn->getName() != character->getName())
			{
				float cX, cY, cZ, cYaw, cRoll, cPitch;
				pawn->get_world_offset(cX, cY, cZ, cYaw, cPitch, cRoll);
				float cDist = sqrt((x - cX) * (x - cX) + (z - cZ) * (z - cZ));
				if (cDist < (1.5f / scene->getScale()))
				{
					float cAngle = radToDeg(atan2(pprAgent->getStartTargetPosition().x - x, pprAgent->getStartTargetPosition().z - z));
					normalizeAngle(cAngle);
					float cDiff = cAngle - yaw;
					normalizeAngle(cDiff);
					if (diff * cDiff > 0)
						neigbors.push_back(cAngle);
				}
			}
		}
		if (neigbors.size() > 0)
			fastInitial = true;
		else
			fastInitial = false;
		if (!fastInitial)
		{
			//LOG("start locomotion %f",diff);
			startIdleToWalkState(diff);	
		}
		else
		{
	//		if (steeringConfig == character->MINIMAL)
			{

				adjustLocomotionBlend(character, locomotionName, 3, 0, 0, 0, false, true);
			}
		}
		return 0;
	}	

	//---end locomotion
	if (_numSteeringGoal != 0 && numGoals == 0)
	{
		if (goalList.size() == 0)
		{
			std::vector<double> weights;
			character->param_animation_ct->schedule(NULL, weights);
			if (!sentLocomotionEvent)
				sendLocomotionEvent("success");	
			facingAdjust = true;	
			//LOG("no next goal now. stop locomotion");
		}
		else
		{
			float goalx = goalList.front();
			goalList.pop_front();
			float goaly = goalList.front();
			goalList.pop_front();
			float goalz = goalList.front();
			goalList.pop_front();
			agent->clearGoals();

			SrVec newGoal = getCollisionFreeGoal(SrVec(goalx,goaly,goalz)*scene->getScale(),curSteerPos*scene->getScale());		
			//LOG("evaluateExampleLoco : original goal = %f %f %f, new goal = %f %f %f",goalx,goaly,goalz, newGoal.x*100.f, newGoal.y*100.f, newGoal.z*100.f);
			SteerLib::AgentGoalInfo goal;
			goal.desiredSpeed = desiredSpeed;
			goal.goalType = SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET;
			goal.targetIsRandom = false;
			//goal.targetLocation = Util::Point(goalx * scene->getScale(), 0.0f, goalz * scene->getScale());
			goal.targetLocation = Util::Point(newGoal.x, 0.0f, newGoal.z);
			agent->addGoal(goal);

			pprAgent->_nextFrameToRunLongTermPlanningPhase = 0;
			pprAgent->_nextFrameToRunMidTermPlanningPhase = 0;
		}
		
		return 0;
	}

	//---If the facing angle is not correct, use idle turning

	if (character->param_animation_ct->isIdle() && fabs(facingAngle) <= 180 && 
		steeringConfig == STANDARD && facingAdjust)
	{
		float diff = facingAngle - yaw;
		normalizeAngle(diff);
		adjustFacingAngle(diff);
		
		return 0;
	}


	//---Need a better way to handle the control between steering and Parameterized Animation Controller
	if (character->param_animation_ct->hasPABlend(jumpName))
		inControl = false;
	else
		inControl = true;

	
	SmartBody::SBAnimationBlend* blend = NULL;
	if (curStateData)
		blend = dynamic_cast<SmartBody::SBAnimationBlend*>(curStateData->state);

	float parameterScale = 1.f;
	if (character && blend)
	{
		SmartBody::SBRetarget* retarget = SmartBody::SBScene::getScene()->getRetargetManager()->getRetarget(blend->getBlendSkeleton(),character->getSkeleton()->getName());				
		if (retarget)
			parameterScale = 1.f/retarget->getHeightRatio();
	}
	//---update locomotion
	float curSpeed = 0.0f;
	float curTurningAngle = 0.0f;
	float curScoot = 0.0f;
	float targetAngleDiff = 0.f;
	if (curStateName == locomotionName && numGoals != 0)
	{
		

		float tnormal[3];
		Heightfield* heightField = SmartBody::SBScene::getScene()->getHeightfield();
		if (heightField)
		{
			SmartBody::SBScene::getScene()->queryTerrain(x, z, tnormal);
		}
		//LOG("current normal %f %f %f", tnormal[0], tnormal[1], tnormal[2]);

		curStateData->state->getParametersFromWeights(curSpeed, curTurningAngle, curScoot, curStateData->weights);	
		curSpeed /= parameterScale;
		if (smoothing)
		{
			float addOnScoot = steeringCommand.scoot * paLocoScootGain;
			if (steeringCommand.scoot != 0.0)
			{
				if (curScoot < addOnScoot)
				{
					curScoot += scootAcceleration * dt;
					if (curScoot > addOnScoot)
						curScoot = addOnScoot;
				}
				else
				{
					curScoot -= scootAcceleration * dt;
					if (curScoot < addOnScoot)
						curScoot = addOnScoot;
				}
			}
			else
			{
				if (fabs(curScoot) < scootThreshold)
					curScoot = 0.0f;
				else
				{
					if (curScoot > 0.0f)
					{
						curScoot -= scootAcceleration * dt;
						if (curScoot < 0.0)	curScoot = 0.0;
					}
					else
					{
						curScoot += scootAcceleration * dt;
						if (curScoot > 0.0)	curScoot = 0.0;
					}
				}
			}
			curSpeed = curSpeed * scene->getScale();

			if (steeringCommand.aimForTargetSpeed)
			{
				if (fabs(curSpeed - targetSpeed) > speedThreshold)
				{
					if (curSpeed < targetSpeed)
					{
						curSpeed += acceleration * dt;
						if (curSpeed > targetSpeed)
							curSpeed = targetSpeed;
					}
					else
					{
						curSpeed -= acceleration * dt;
						if (curSpeed < targetSpeed)
							curSpeed = targetSpeed;
					}
				}
			}
			else
				curSpeed += acceleration * dt;

			float angleGlobal = radToDeg(atan2(forward.x, forward.z));
			normalizeAngle(angleGlobal);
			normalizeAngle(yaw);
			float angleDiff = angleGlobal - yaw;
			normalizeAngle(angleDiff);
			
			if (fabs(angleDiff) > turningEpsilon)
			{
				targetAngleDiff = angleDiff;
				float addOnTurning = angleDiff * paLocoAngleGain;
				if (fabs(curTurningAngle - addOnTurning) > angleSpeedThreshold)
				{
					if (curTurningAngle < addOnTurning)
					{
						curTurningAngle += angleAcceleration * dt;
						if (curTurningAngle > addOnTurning)
							curTurningAngle = addOnTurning;
					}
					else if (curTurningAngle > addOnTurning)
					{					
						curTurningAngle -= angleAcceleration * dt;
						if (curTurningAngle < addOnTurning)
							curTurningAngle = addOnTurning;
					}
				}
			}			
			// update locomotion state
			newSpeed = curSpeed;
			curSpeed = curSpeed / scene->getScale();
		}
		else	// direct gaining
		{
			float angleGlobal = radToDeg(atan2(forward.x, forward.z));
			normalizeAngle(angleGlobal);
			normalizeAngle(yaw);
			float angleDiff = angleGlobal - yaw;
			normalizeAngle(angleDiff);

			curSpeed = targetSpeed / scene->getScale();
			curTurningAngle = angleDiff * paLocoAngleGain ;
			curScoot = steeringCommand.scoot * paLocoScootGain;			
			newSpeed = targetSpeed;
		}

		if (inControl)
		{
			std::vector<double> weights;
			weights.resize(curStateData->state->getNumMotions());
			//LOG("Character name = %s",character->getName().c_str());
			//LOG("target Speed = %f, curSpeed = %f, angleDiff = %f, curTurningAngle = %f, curScoot = %f",targetSpeed, curSpeed, targetAngleDiff, curTurningAngle, curScoot);			
			curStateData->state->getWeightsFromParameters(curSpeed*parameterScale, curTurningAngle, curScoot, weights);
			character->param_animation_ct->updateWeights(weights);
		}
	}
	return newSpeed;
}


void PPRAISteeringAgent::startIdleToWalkState( float angleDiff )
{
	//		if (character->steeringConfig == character->STANDARD)
	{
		/*			
		float angleGlobal = radToDeg(atan2(forward.x, forward.z));
		normalizeAngle(angleGlobal);
		normalizeAngle(yaw);
		float diff = angleGlobal - yaw;
		normalizeAngle(diff);
		*/
		
		std::stringstream command;
		//double w;
		float maxRotAngle = 180;
		
		//LOG("angleDiff = %f",angleDiff);
		if (angleDiff > 0)
		{
			adjustLocomotionBlend(character, startingLName, 1,-angleDiff, 0, 0, false, false);
			
		}		
		else		
		{
			adjustLocomotionBlend(character, startingRName, 1, -angleDiff, 0, 0, false, false);
		}
		//LOG("start turn command = %s",command.str().c_str());
		
		startLocomotionState();
		return;
	}
}

void PPRAISteeringAgent::adjustFacingAngle( float angleDiff )
{
		
	std::string playNow;
	if (fabs(angleDiff) > facingAngleThreshold && !character->param_animation_ct->hasPABlend(idleTurnName.c_str()))
	{
		if (facingDirectBlend)
			adjustLocomotionBlend(character, idleTurnName, 1,-angleDiff, 0, 0, true, false);
		else
			adjustLocomotionBlend(character, idleTurnName, 1,-angleDiff, 0, 0, false, false);
	}
	else
	{
		SmartBody::SBMotionEvent facingEvent;
		facingEvent.setType("adjustFacing");
		std::string cmd = "bml chr " + character->getName() + " success";
		//cmd = cmd + " facing: " + boost::lexical_cast<std::string>(facing)
		facingEvent.setParameters(cmd);
		SmartBody::SBEventManager* manager = SmartBody::SBScene::getScene()->getEventManager();		
		manager->handleEvent(&facingEvent, 0.0f);
		facingAdjust = false; // stop facing adjustment
	}
	/*
	else
	{
		facingAngle = -200;
	}
	*/
}

float PPRAISteeringAgent::evaluateSteppingLoco(float dt, float x, float y, float z, float yaw)
{
	if (!character->param_animation_ct)
		return .0f;
	
	float dist = sqrt((x - stepTargetX) * (x - stepTargetX) + (z - stepTargetZ) * (z - stepTargetZ));	
	SrVec agentToTargetVec;
	agentToTargetVec.x = stepTargetX - x;
	agentToTargetVec.y = 0.0f;
	agentToTargetVec.z = stepTargetZ - z;
	if (character->param_animation_ct->isIdle() && (dist > 10.f))
	{
		SrVec heading = SrVec(sin(degToRad(yaw)), 0, cos(degToRad(yaw)));
		float offsety = dot(agentToTargetVec, heading);
		SrVec verticalHeading = SrVec(sin(degToRad(yaw - 90)), 0, cos(degToRad(yaw - 90)));
		float offsetx = dot(agentToTargetVec, verticalHeading);
		if (!character->param_animation_ct->hasPABlend(stepStateName.c_str()))
		{
			adjustLocomotionBlend(character, stepStateName, 2, x, y, 0, false, false);
		}	
	}
	if (dist < 10.0f)
		steppingMode = false;

	return 0.0f;
}

void PPRAISteeringAgent::startParameterTesting()
{
	LOG("Parameter Testing Start...");
	
	paramTestStartTime = (float) SmartBody::SBScene::getScene()->getSimulationManager()->getTime();
	paramTestFlag = true;
	paramTestAngle = 0.0f;
	paramTestDistance = 0.0f;
	float y, pitch, roll;
	character->get_world_offset(prevX, y, prevZ, prevYaw, pitch, roll);
	normalizeAngle(prevYaw);
}

void PPRAISteeringAgent::parameterTesting()
{
	
	if ((SmartBody::SBScene::getScene()->getSimulationManager()->getTime() - paramTestStartTime) > paramTestDur)
	{
		paramTestFlag = false;
		paramTestAngle *= (float(M_PI) / 1.8f);
		float paramTestVelocity = paramTestDistance / paramTestDur;
		float paramTestAngleVelocity = paramTestAngle / paramTestDur;
		LOG("Parameter Testing Result");
		LOG("Duration: %f", paramTestDur);
		LOG("Velocity: %f", paramTestVelocity);
		LOG("Angle Velocity: %f", paramTestAngleVelocity);
		return;
	}
	// current location
	float x, y, z;
	float yaw, pitch, roll;
	character->get_world_offset(x, y, z, yaw, pitch, roll);
	normalizeAngle(yaw);

	paramTestDistance += sqrt((x - prevX) * (x - prevX) + (z - prevZ) * (z - prevZ));
	if (fabs(yaw - prevYaw) < 300) 
		paramTestAngle += (yaw - prevYaw);

	prevX = x;
	prevZ = z;
	prevYaw = yaw;
	normalizeAngle(prevYaw);
}

void PPRAISteeringAgent::cacheParameter(std::list<float>& sampleData, float data, int size)
{
	sampleData.push_back(data);
	while (sampleData.size() > (size_t)size)
		sampleData.pop_front();
}

float PPRAISteeringAgent::getFilteredParameter(std::list<float>& sampleData)
{
	float ret = 0.0f;
	std::list<float>::iterator iter = sampleData.begin();
	for (; iter != sampleData.end(); iter++)
		ret += *iter;
	ret /= float(sampleData.size());
	return ret;
}

void PPRAISteeringAgent::setSteerParamsDirty(bool val)
{
	_dirty = val;
}

bool PPRAISteeringAgent::isSteerParamsDirty()
{
	return _dirty;
}



void PPRAISteeringAgent::locomotionReset()
{
	//LOG("reset locomotion");
	locomotionHalt();
	if (character->param_animation_ct)
	{			
		character->param_animation_ct->reset();
	}
}

void PPRAISteeringAgent::locomotionHalt()
{
	//LOG("halt locomotion");
	std::vector<double> weights;
	ScheduleType sc;
	if (character->param_animation_ct)
	{		
		sc.schedule = PABlendData::Now;
		character->param_animation_ct->schedule(NULL, weights,sc);		
	}
	sendLocomotionEvent("success");
	character->trajectoryGoalList.clear();
	agent->clearGoals();
	goalList.clear();
	steerPath.clearPath();
	facingAdjust = false; // stop facing adjustment as well
	

	steerCurSpeed = 0.f;
	steerCurAngle = 0.f;
	steerCurScoot = 0.f;
}

bool PPRAISteeringAgent::isInLocomotion()
{
	if (!character->param_animation_ct)
		return false;
	if (character->param_animation_ct->isIdle())
		return false;
	return true;
}

bool PPRAISteeringAgent::isHittingOnTarget()
{
	return (character->_reachTarget && !character->_lastReachStatus);
}

void PPRAISteeringAgent::startLocomotionState()
{
	PPRAgent* pprAgent = dynamic_cast<PPRAgent*>(agent);
	const SteerLib::SteeringCommand & steeringCommand = pprAgent->getSteeringCommand();
	float targetSpeed = this->desiredSpeed;//steeringCommand.targetSpeed;
	targetSpeed *= 1.0f / SmartBody::SBScene::getScene()->getScale();

	SmartBody::SBAnimationBlendManager* stateManager = SmartBody::SBScene::getScene()->getBlendManager();
	SmartBody::SBAnimationBlend* state = stateManager->getBlend(locomotionName);
	if (!state)
	{
		LOG("No state named %s found for character %s. Cannot start locomotion.", locomotionName.c_str(), character->getName().c_str());
		return;
	}
	
	float parameterScale = 1.f;
	if (character && state)
	{
		SmartBody::SBRetarget* retarget = SmartBody::SBScene::getScene()->getRetargetManager()->getRetarget(state->getBlendSkeleton(),character->getSkeleton()->getName());
		if (retarget)
			parameterScale = 1.f/retarget->getHeightRatio();
	}
	//LOG("desire speed = %f, parameter scele = %f",targetSpeed, parameterScale);
	adjustLocomotionBlend(character, locomotionName, 3, targetSpeed*parameterScale, 0, 0, false, true);
}

void PPRAISteeringAgent::adjustLocomotionBlend(SmartBody::SBCharacter* character, const std::string& blendName, int blendDimension, double x, double y, double z, bool directPlay, bool loop)
{
	if (!character->param_animation_ct)
	{
		LOG("Parameterized animation Not Enabled, cannot update locomotion to (%d, %d, %d)", x, y, z);
		return;
	}

	SmartBody::SBAnimationBlend* blend = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(blendName);
	if (!blend)
	{
		LOG("Blend %s does not exist, cannot update locomotion to (%d, %d, %d)", blendName.c_str(), x, y, z);
		return;
	}

	const std::string& currentStateName = character->param_animation_ct->getCurrentStateName();

	std::vector<double> weights;
	weights.resize(blend->getNumMotions());
	if (blendDimension == 0)
	{
	}
	else if (blendDimension == 1)
	{
		blend->PABlend::getWeightsFromParameters(x, weights);
	}
	else if (blendDimension == 2)
	{
		blend->PABlend::getWeightsFromParameters(x, y, weights);
	}
	else if (blendDimension == 3)
	{
		blend->PABlend::getWeightsFromParameters(x, y, z, weights);
	}

	if (currentStateName == blendName)
	{
		character->param_animation_ct->updateWeights(weights);
	}
	else
	{
		PABlendData::WrapMode wrap = PABlendData::Loop;
		if (!loop)
			wrap = PABlendData::Once;
		PABlendData::ScheduleMode schedule = PABlendData::Now;
		PABlendData::BlendMode blendMode = PABlendData::Overwrite;
		//LOG("blendName = %s, parameter = %f %f %f",blendName.c_str(), x,y,z);
		character->param_animation_ct->schedule(blend, weights, wrap, schedule, blendMode, "null", 0.0, 0.0, 0.0, -1.0, directPlay);
	}
}
