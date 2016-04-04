//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#include "SteerLib.h"
#include "PPRAIModule.h"
#include "PPRAgent.h"
#include <math.h>

using namespace Util;
using namespace SteerLib;
using namespace PPRGlobals;

#ifndef _WIN32
// win32 does not define "std::max", instead they define "max" as a macro.
// because of this, on unix we use "using std::max" so that the code only
// needs to use "max()" instead of "std::max()".  This way, the code
// works on both win32 and unix.
using std::max;
using std::min;
#endif

#if !defined(__FLASHPLAYER__)
//
// constructor
//
PPRAgent::PPRAgent()
{
	_enabled = false;
	setUseCollisionFreeGoals(false);
}



//
// destructor
//
PPRAgent::~PPRAgent()
{
	if (_enabled) {
		Util::AxisAlignedBox bounds(_position.x-_radius, _position.x+_radius, 0.0f, 0.0f, _position.z-_radius, _position.z+_radius);
		gSpatialDatabase->removeObject( this, bounds);
	}
}



//
// reset()
//
void PPRAgent::reset(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::EngineInterface * engineInfo)
{

	AxisAlignedBox oldBounds(_position.x-_radius, _position.x+_radius, 0.0f, 0.0f, _position.z-_radius, _position.z+_radius);
	_position = initialConditions.position;
	_forward  = initialConditions.direction;
	_radius   = initialConditions.radius;
	_currentSpeed = initialConditions.speed;
	AxisAlignedBox newBounds(_position.x-_radius, _position.x+_radius, 0.0f, 0.0f, _position.z-_radius, _position.z+_radius);

	_desiredForward = _forward;
	_magnifiedDesiredForward = _desiredForward;
	_isNewGoal = false;
	_startTargetPosition = _position;

	if (!_enabled) {
		gSpatialDatabase->addObject( dynamic_cast<SpatialDatabaseItemPtr>(this), newBounds);
	}
	else {
		gSpatialDatabase->updateObject( dynamic_cast<SpatialDatabaseItemPtr>(this), oldBounds, newBounds);
	}
	_enabled = true;


	clearGoals();
	for (unsigned int i=0; i<initialConditions.goals.size(); i++) {
		if (initialConditions.goals[0].goalType != GOAL_TYPE_SEEK_STATIC_TARGET) {
			throw Util::GenericException("Currently the PPR AI agent does not support goal types other than GOAL_TYPE_SEEK_STATIC_TARGET.");
		}
		addGoal(initialConditions.goals[i]);
	}


	assert(_forward.length()!=0.0f);
	//assert(_landmarkQueue.size() != 0);
	assert(_radius != 0.0f);

	// this assertion does not work with the new AgentGoalInfo struct; probably beacuse there is no == operator
	// assert(_landmarkQueue.front() == _currentGoal);


	//
	// init the vars that don't get initialized from the test case.
	// here they are grouped the same way that they are grouped in the class declaration.
	//

	// LONG-TERM PLANNING PHASE
	_waypoints.clear();
	_currentWaypointIndex = 0;

	// MID-TERM PLANNING PHASE
	_midTermPathSize = 0;

	// SHORT-TERM PLANNING PHASE
	_localTargetLocation = _currentGoal.targetLocation;

	// PERCEPTION PHASE
	_neighbors.clear();
	_numAgentsInVisualField = 0;

	// PREDICTION PHASE
	_timeToWait = 0.0f;  // TODO: is this initialization to 0.0 OK for _timeToWait?
	_minThreatTime = INFINITY;
	_maxThreatTime = -INFINITY;
	_mostImminentThreatIndex = -1;
	_threatList.clear();
	_crowdControlDirection = Vector(1.23456f, 1.23456f, 1.23456f);
	_steeringState = STEERING_STATE_TURN_TOWARDS_TARGET;

	// REACTIVE PHASE
	_finalSteeringCommand.clear();


	// PHASE SCHEDULERS
	_nextFrameToRunLongTermPlanningPhase = 0;
	_nextFrameToRunMidTermPlanningPhase = 0;
	_nextFrameToRunShortTermPlanningPhase = 0;
	_nextFrameToRunPerceptivePhase = 0;
	_nextFrameToRunPredictivePhase = 0;
	_nextFrameToRunReactivePhase = 0;
	_framesToNextLongTermPlanning = 100000; // long-term and mid-term plans are executed on-demand.
	_framesToNextMidTermPlanning = 100000;
	_framesToNextShortTermPlanning = 1;
	_framesToNextPerceptivePhase = 1;
	_framesToNextPredictivePhase = 1;
	_framesToNextReactivePhase = 1;

	// GEOMETRY STATE
	// other geometry state was initialized above using the given initial conditions.
	_rightSide = rightSideInXZPlane(_forward);

	// PHYSICS STATE
	_velocity = normalize(_forward) * _currentSpeed;
	_mass = 1.0f;
	_maxSpeed = PED_MAX_SPEED;
	_maxForce = PED_MAX_FORCE;
	// currentSpeed was initialized above.

	// these will be removed soon.
	_currentTimeStamp = 0.0f;
	_dt = 0.0f;
	_currentFrameNumber = 0;


#ifdef USE_ANNOTATIONS
	// if annotations are enabled, then some additional data is actually stored in the agent itself
	// instead of being locally defined.

	__hitSomething = false;
	__front = false;
	__leftFront = false;
	__rightFront = false;
	__leftSide = false;
	__rightSide = false;
	__objectInPath = NULL;
	__closestPathNode = 0;
#endif


	if (_currentGoal.desiredSpeed > _maxSpeed) {
		std::cerr << "WARNING: initial desired speed (" << _currentGoal.desiredSpeed << " m/s) is larger than the max speed (" << _maxSpeed << " m/s) of our agent." << std::endl;
	}


}

void PPRAgent::setUseCollisionFreeGoals(bool val)
{
	_useCollisionFreeGoals = val;
}

bool PPRAgent::isUseCollisionFreeGoals()
{
	return _useCollisionFreeGoals;
}

//
// addGoal()
//
void PPRAgent::addGoal(const SteerLib::AgentGoalInfo & newGoal) {

	if (newGoal.goalType != SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET) {
		throw Util::GenericException("Currently the PPR agent does not support goal types other than GOAL_TYPE_SEEK_STATIC_TARGET.");
	}

	_landmarkQueue.push(newGoal); 

	if (!isUseCollisionFreeGoals())
	{
		_currentGoal = newGoal;
		_landmarkQueue.front().targetLocation = newGoal.targetLocation;
		_isNewGoal = true;
		return;
	}

	if (_landmarkQueue.size()==1) {
		_currentGoal = newGoal;
		_isNewGoal = true;

		float regionRadius = 0.6f;
		bool succeed = false;
		std::set<SpatialDatabaseItemPtr> allObjects;
		allObjects.clear();
		gSpatialDatabase->getAllItems(allObjects, dynamic_cast<SpatialDatabaseItemPtr>(this));
		
		while (!succeed && regionRadius < 100.0f) {
			// define the searching region
			Util::AxisAlignedBox aab = Util::AxisAlignedBox(_currentGoal.targetLocation.x - regionRadius, _currentGoal.targetLocation.x + regionRadius, 
				0.0f, 0.0f, _currentGoal.targetLocation.z - regionRadius, _currentGoal.targetLocation.z + regionRadius);

			unsigned int numTries = 0;
			while (!succeed) {
				// find a postion that is free of static obstacles and agents
				Util::Point pos = gSpatialDatabase->randomPositionInRegionWithoutCollisions(aab, 0.3f, false, succeed);
				// check if this is also other agents' goal
				if (succeed) {
					for (std::set<SpatialDatabaseItemPtr>::iterator object = allObjects.begin(); object != allObjects.end(); ++object) {
						if ((*object)->isAgent()) {
							PPRAgent* agent = dynamic_cast<PPRAgent*> (*object);
							if((agent->currentGoal().targetLocation - pos).length() < 0.5f) {
								succeed = false;
								break;
							}
						}
					}
				}

				if(succeed) {
					_currentGoal.targetLocation = pos;
					_landmarkQueue.front().targetLocation = pos;
					break;
				}
				if (numTries > 1000) {
					break;
				}
				numTries++;
			}

			regionRadius += 0.3f;
		}
	}
}


//
// updateAI()
//
void PPRAgent::updateAI(float timeStamp, float dt, unsigned int frameNumber)
{

	if ((!_enabled) || (_landmarkQueue.empty())) return;


	AutomaticFunctionProfiler profileThisFunction( &PPRGlobals::gPhaseProfilers->aiProfiler );


	// initialize some vars for this update step
	// todo, this should eventually be removed after addressing the small issue with _currentFrameNumber.
	_currentTimeStamp = timeStamp;

	_currentFrameNumber = frameNumber-1;

	/*if (frameNumber >= 1) {
		_currentFrameNumber = frameNumber-1; // starting at 0 just because we didn't want to remove this while trying to reliably get a new chunk of code in.  TODO, change this later if it seems OK and appropriate...
	}
	else {
		_currentFrameNumber = 0;
	}
	*/
	_dt = dt;



	//
	// run any phases that were scheduled for this frame.
	//

	if (_currentFrameNumber >= _nextFrameToRunLongTermPlanningPhase) {
		runLongTermPlanningPhase();
		_lastFrameLongTermWasCalled = _currentFrameNumber;
	}


	if (_currentFrameNumber >= _nextFrameToRunMidTermPlanningPhase) {
		runMidTermPlanningPhase();
		_lastFrameMidTermWasCalled = _currentFrameNumber;
	}


	if (_currentFrameNumber >= _nextFrameToRunShortTermPlanningPhase) {
		runShortTermPlanningPhase();
		_lastFrameShortTermWasCalled = _currentFrameNumber;
	}


	if (_currentFrameNumber >= _nextFrameToRunPerceptivePhase) {
		runPerceptivePhase();
		_lastFramePerceptiveWasCalled = _currentFrameNumber;
	}


	if (_currentFrameNumber >= _nextFrameToRunPredictivePhase) {
		runPredictivePhase();
		_lastFramePredictiveWasCalled = _currentFrameNumber;
	}


	if (_currentFrameNumber >= _nextFrameToRunReactivePhase) {

		// clearing the decision is not absolutely necessary, but significantly helps debugging,
		// and avoids accidental re-use of the previous command.
		_finalSteeringCommand.clear();

		runReactivePhase();
		_lastFrameReactiveWasCalled = _currentFrameNumber;
	}

		
	if (gUseDynamicPhaseScheduling) {
		_nextFrameToRunLongTermPlanningPhase = _lastFrameLongTermWasCalled + _framesToNextLongTermPlanning;
		_nextFrameToRunMidTermPlanningPhase = _lastFrameMidTermWasCalled + _framesToNextMidTermPlanning;
		_nextFrameToRunShortTermPlanningPhase = _lastFrameShortTermWasCalled + _framesToNextShortTermPlanning;
		_nextFrameToRunPerceptivePhase = _lastFramePerceptiveWasCalled + _framesToNextPerceptivePhase;
		_nextFrameToRunPredictivePhase = _lastFramePredictiveWasCalled + _framesToNextPredictivePhase;
		_nextFrameToRunReactivePhase = _lastFrameReactiveWasCalled + _framesToNextReactivePhase;
	}
	else {
		_nextFrameToRunLongTermPlanningPhase = _lastFrameLongTermWasCalled + gLongTermPlanningPhaseInterval;
		_nextFrameToRunMidTermPlanningPhase = _lastFrameMidTermWasCalled + gMidTermPlanningPhaseInterval;
		_nextFrameToRunShortTermPlanningPhase = _lastFrameShortTermWasCalled + gShortTermPlanningPhaseInterval;
		_nextFrameToRunPerceptivePhase = _lastFramePerceptiveWasCalled + gPerceptivePhaseInterval;
		_nextFrameToRunPredictivePhase = _lastFramePredictiveWasCalled + gPredictivePhaseInterval;
		_nextFrameToRunReactivePhase = _lastFrameReactiveWasCalled + gReactivePhaseInterval;
	}


	// always do locomotion
	//doSteering();

}


//
// runCognitivePhase()
//
void PPRAgent::runCognitivePhase()
{
	// pop off the previous goal
	_landmarkQueue.pop();

	if (_landmarkQueue.empty()) {
		// nowhere left to steer, do nothing.
		//disable();
		return;
	}
	else {
		_currentGoal = _landmarkQueue.front();
		_isNewGoal = true;
	}

	// if the goal is something other than "seek static target", PPR does not support it at the moment.
	if (_currentGoal.goalType != GOAL_TYPE_SEEK_STATIC_TARGET) {
		throw GenericException("Currently the PPR AI agent only supports GOAL_TYPE_SEEK_STATIC_TARGET goal types");
	}

	// if the goal asks for a random target, then randomly assign the target location
	if (_currentGoal.targetIsRandom) {
		AxisAlignedBox aab = AxisAlignedBox(-100.0f, 100.0f, 0.0f, 0.0f, -100.0f, 100.0f);
		_currentGoal.targetLocation = gSpatialDatabase->randomPositionInRegionWithoutCollisions(aab, 1.0f, true);
	}
}


//
// runLongTermPlanningPhase()
//
void PPRAgent::runLongTermPlanningPhase()
{
#ifndef USE_ANNOTATIONS
	// if not using annotations, then declare things local here.
	std::stack<unsigned int> longTermPath;
#endif

#ifdef IGNORE_PLANNING
	return;
#endif

	if ((!_enabled) || (_landmarkQueue.empty())) return;


	// if we're at our destination, then chose a new landmark target.
	if (reachedCurrentGoal()) {
		runCognitivePhase();
		if ((!_enabled) || (_landmarkQueue.empty())) return;
	}

	AutomaticFunctionProfiler profileThisFunction( &PPRGlobals::gPhaseProfilers->longTermPhaseProfiler );

	//==========================================================================

	int myIndexPosition = gSpatialDatabase->getCellIndexFromLocation(_position);
	int goalIndex = gSpatialDatabase->getCellIndexFromLocation(_currentGoal.targetLocation);

	if (myIndexPosition != -1) {

		// run the main a-star search here
		gSpatialDatabase->planPath(myIndexPosition, goalIndex, longTermPath);


		// set up the waypoints along this path.
		// if there was no path, then just make one waypoint that is the landmark target.
		_waypoints.clear();
		if (longTermPath.size() > 2) {

			// repeatedly pop the path stack, adding waypoints every so often, until the stack is empty.
			while ( ! longTermPath.empty()) {
				unsigned int mostRecentNode = 0;
				for (unsigned int i=0; i < PED_NEXT_WAYPOINT_DISTANCE; i++) {
					if ( ! longTermPath.empty()) {
						mostRecentNode = longTermPath.top();
						longTermPath.pop();
					}
					else {
						// this is the common case... we reach the end of the path while popping
						// so we need to add the very last target location as the last waypoint.
						_waypoints.push_back(_currentGoal.targetLocation);
					}
				}

				// every time we successfully popped that many nodes in the path, we can add the next one as a waypoint.
				Point waypoint;
				gSpatialDatabase->getLocationFromIndex(mostRecentNode,waypoint);
				_waypoints.push_back(waypoint);
			}

			/*
			 
			 TODO, delete this after debugging the new version.

			// note the >2 condition: if the astar path is not at least this large, then there will be a behavior bug in the AI
			// when it tries to create waypoints.  in this case, the right thing to do is create only one waypoint that is at the landmark target.
			// remember the astar lib produces "backwards" paths that start at [pathLengh-1] and end at [0].
			int nextWaypointIndex = ((int)longTermAStar.getPath().size())-1 - PED_NEXT_WAYPOINT_DISTANCE;
			while (nextWaypointIndex > 0) {
				Point waypoint;
				gSpatialDatabase->getLocationFromIndex(longTermAStar.getPath()[nextWaypointIndex],waypoint);
				_waypoints.push_back(waypoint);
				nextWaypointIndex -= PED_NEXT_WAYPOINT_DISTANCE;
			}
			_waypoints.push_back(_currentGoal.targetLocation);
			
			*/
		}
		else {
			_waypoints.push_back(_currentGoal.targetLocation);
		}


		// since we just computed a new long-term path, set the character to steer towards the first waypoint.
		_currentWaypointIndex = 0; 
	}
	else {
		// can't do A-star if we are outside the database.
		// this happens rarely, if ever, but still needs to be robustly handled... this seems like a reasonable decision to make in the extreme case.
		_waypoints.push_back(_currentGoal.targetLocation);
	}

}


//
// runMidTermPlanningPhase()
//
void PPRAgent::runMidTermPlanningPhase()
{

#ifndef USE_ANNOTATIONS
	// if not using annotations, then declare things local here.
	std::stack<unsigned int> midTermPathStack;
#endif

#ifdef IGNORE_PLANNING
	return;
#endif

	if ((!_enabled) || (_landmarkQueue.empty())) return;

	// if we reached the final goal, then schedule long-term planning to run
	// long-term planning will call cognitive
	if (reachedCurrentGoal()) {
		runLongTermPlanningPhase();
		if ((!_enabled) || (_landmarkQueue.empty())) return;
	}

	AutomaticFunctionProfiler profileThisFunction( &PPRGlobals::gPhaseProfilers->midTermPhaseProfiler );

	// if we reached the current waypoint, then increment to the next waypoint
	if (reachedCurrentWaypoint()) {
		_currentWaypointIndex++;
		if (_currentWaypointIndex == (int)_waypoints.size()) {
			_waypoints.push_back(_currentGoal.targetLocation);
		}
	}

	// compute a local a-star from your current location to the waypoint.
	int myIndexPosition = gSpatialDatabase->getCellIndexFromLocation(_position.x, _position.z);
	int waypointIndexPosition = gSpatialDatabase->getCellIndexFromLocation(_waypoints[_currentWaypointIndex].x, _waypoints[_currentWaypointIndex].z);

	gSpatialDatabase->planPath(myIndexPosition, waypointIndexPosition,midTermPathStack);

	// copy the local AStar path to your array
	_midTermPathSize = (int)midTermPathStack.size();

	// sanity checks
	//
	// TODO, something is not quite right with this sanity check, it needs to be debugged.
	//
	if (_midTermPathSize > PED_NEXT_WAYPOINT_DISTANCE + 3) {
		// the plus 1 is because a-star counts the agent's immediate location, but we do not.
		std::cerr << "ERROR!!!  _midTermPathSize is larger than expected: should be less than or equal to " << PED_NEXT_WAYPOINT_DISTANCE+1 << ", but it actually is " << _midTermPathSize << "\n";
		assert(false);
	}

	// TODO, this "pop" is awfully wasteful and unnecessary
	// we could just keep the stack itself, and call it midTermPath,
	// and remove the fixed array.
	// The only reason not to do this is that it makes 
	// drawing annotations more costly (i.e. cannot peer into a stack).
	for (unsigned int i=0; i<_midTermPathSize; i++) {
		//_midTermPath[i] = (midTermAStar.getPath())[i];
		_midTermPath[i] = midTermPathStack.top();
		midTermPathStack.pop();
	}

	assert(midTermPathStack.empty());

	// TODO: should we reset the localTarget here??
}


//
// runShortTermPlanningPhase()
//
void PPRAgent::runShortTermPlanningPhase()
{
	unsigned int closestPathNode;
	if ((!_enabled) || (_landmarkQueue.empty())) return;

#ifdef IGNORE_PLANNING
	// if we want to ignore planning, then just decide to steer towards the final target.
	// possibly update the landmark target if we arrived at it.
	if (reachedCurrentGoal()) {
		runCognitivePhase();
	}
	_localTargetLocation = _currentGoal.targetLocation;
	return;
#endif


	// 0. if you're at your current waypoint
	if (reachedCurrentWaypoint()) {
		// then schedule midTermPlanning phase
		runMidTermPlanningPhase();
		if ((!_enabled) || (_landmarkQueue.empty())) return;
	}


	AutomaticFunctionProfiler profileThisFunction( &PPRGlobals::gPhaseProfilers->shortTermPhaseProfiler );

	int myIndexPosition = gSpatialDatabase->getCellIndexFromLocation(_position.x, _position.z);


	closestPathNode = 0;

	if ((myIndexPosition != -1) && (closestPathNode != _midTermPathSize-1)) {

		// 1. find the node that you're nearest to in your current path
		// NOTE that we MUST search ALL nodes of the path here.
		// reason: the path may unintuitively snake around so that some nodes are closer than others even if they are actually on some other portion of the path far away from you.
		float minDistSquared = INFINITY;
		for (unsigned int i=0; i<_midTermPathSize; i++) {
			Point tempTargetLocation;
			gSpatialDatabase->getLocationFromIndex( _midTermPath[i], tempTargetLocation);
			Vector temp = tempTargetLocation-_position;
			float distSquared = temp.lengthSquared();
			if (distSquared < minDistSquared) {
				minDistSquared = distSquared;
				closestPathNode = i;
			}
		}

		// at this point in code, closestPathNode is the node that is closest to your ped's position.

		// 2. iterate over nodes tracing rays to the find the local target	
		//printf("closest path node = %d, midtermPathSize = %d",closestPathNode, _midTermPathSize);
		if (closestPathNode < _midTermPathSize-3 && _midTermPathSize > 2)
		{
			float dummyt;
			SpatialDatabaseItemPtr dummyObject;
			int localTargetIndex = (int)closestPathNode;
			unsigned int furthestTargetIndex = min(_midTermPathSize-1, closestPathNode + PED_FURTHEST_LOCAL_TARGET_DISTANCE);
			unsigned int localTargetCellID = _midTermPath[localTargetIndex];
			gSpatialDatabase->getLocationFromIndex( localTargetCellID, _localTargetLocation );
			Ray lineOfSightTest1, lineOfSightTest2;
			lineOfSightTest1.initWithUnitInterval(_position + _radius*_rightSide, _localTargetLocation - (_position + _radius*_rightSide));
			lineOfSightTest2.initWithUnitInterval(_position - _radius*_rightSide, _localTargetLocation - (_position - _radius*_rightSide));
			while ( (localTargetIndex <= (int)furthestTargetIndex)
				&& (!gSpatialDatabase->trace(lineOfSightTest1,dummyt, dummyObject, dynamic_cast<SpatialDatabaseItemPtr>(this),true))
				&& (!gSpatialDatabase->trace(lineOfSightTest2,dummyt, dummyObject, dynamic_cast<SpatialDatabaseItemPtr>(this),true)))
			{
				localTargetIndex++;
				localTargetCellID = _midTermPath[localTargetIndex];
				gSpatialDatabase->getLocationFromIndex( localTargetCellID, _localTargetLocation );
				lineOfSightTest1.initWithUnitInterval(_position + _radius*_rightSide, _localTargetLocation - (_position + _radius*_rightSide));
				lineOfSightTest2.initWithUnitInterval(_position - _radius*_rightSide, _localTargetLocation - (_position - _radius*_rightSide));
			}
			localTargetIndex--; // the last node we found was actually NOT visible, so backtrack by one.
			if (localTargetIndex >= (int)closestPathNode) {
				// if localTargetIndex is valid
				localTargetCellID = _midTermPath[localTargetIndex];
				gSpatialDatabase->getLocationFromIndex( localTargetCellID, _localTargetLocation );
				float localToWayPointDist = (_localTargetLocation - _waypoints[_currentWaypointIndex]).length();
				//printf("localToWayPointDist = %f\n",localToWayPointDist);
				if ( localToWayPointDist < 2.0f * PED_REACHED_TARGET_DISTANCE_THRESHOLD ) {
					_localTargetLocation = _waypoints[_currentWaypointIndex];
				}
			}
			else {
				// if localTargetIndex is pointing backwards, then just aim for 2 nodes ahead of the current closestPathNode.
				localTargetCellID = _midTermPath[closestPathNode+2];
				gSpatialDatabase->getLocationFromIndex( localTargetCellID, _localTargetLocation );
			}
		}
		else {		
			_localTargetLocation = _waypoints[_currentWaypointIndex];
		}
	}
	else {
		if (myIndexPosition==-1) {
			// this case is reached when you're outside the database
			// in this case, just point towards your waypoint and hope for the best.
			_localTargetLocation = _waypoints[_currentWaypointIndex];
		}
		else if (closestPathNode == _midTermPathSize-1) {
			// this case is reached when you're very close to your goal, and the planned path is very short.
			// in this case, just point towards the closest node.
			gSpatialDatabase->getLocationFromIndex( closestPathNode, _localTargetLocation);
		}
		else {
			// this case should never be reached
			std::cerr << "ERROR: unexpected case! closestPathNode==" << closestPathNode << ",  myIndexPosition==" << myIndexPosition << ".\nexiting ungracefully.\n";
			exit(1);
		}
	}



	if (gUseDynamicPhaseScheduling) {
		// decimating short-term planning
		float distanceHeuristic = (_position - _localTargetLocation).length() - 5.0f;
		if (distanceHeuristic <= 0.0f) {
			_framesToNextShortTermPlanning = 75;
		}
		else {
			// starting at 75 frames
			_framesToNextShortTermPlanning = 75 + (unsigned int)(2*distanceHeuristic*distanceHeuristic);
			if (_framesToNextShortTermPlanning > 170) _framesToNextShortTermPlanning = 150;
		}
	}

#ifdef USE_ANNOTATIONS
	__closestPathNode = closestPathNode;
#endif
}


//
// runPerceptivePhase()
//
void PPRAgent::runPerceptivePhase()
{
	if ((!_enabled) || (_landmarkQueue.empty())) return;

	AutomaticFunctionProfiler profileThisFunction( &PPRGlobals::gPhaseProfilers->perceptivePhaseProfiler );
	collectObjectsInVisualField();

	if (gUseDynamicPhaseScheduling) {
		if (_currentSpeed <= 0.4f) {
			_framesToNextPerceptivePhase = 65;
		}
		else if (_currentSpeed <= PED_TYPICAL_SPEED - 0.2f) {
			_framesToNextPerceptivePhase = 65;
		}
		else {
			_framesToNextPerceptivePhase = 30;
		}
	}

}

//
// runPredictivePhase()
//
void PPRAgent::runPredictivePhase()
{
	if ((!_enabled) || (_landmarkQueue.empty())) return;

	AutomaticFunctionProfiler profileThisFunction( &PPRGlobals::gPhaseProfilers->predictivePhaseProfiler );

	bool threatListChanged = false;
	bool alreadyExists = false;

	// threat_min_t and threat_max_t are not really used except for annotation.
	float threat_min_t, threat_max_t;
	//bool threatListIsLocked;
	//bool threatIsStillImminent = false; // until proven true
	threat_min_t = INFINITY;
	threat_max_t = -INFINITY;
	Vector directionToLocalTarget = normalize(_localTargetLocation - _position);

	_crowdControlDirection = _forward; // just a decent default
	_numAgentsInVisualField = 0; // will be counted as we loop through _neighbors.

	//cerr << "================================================\n_currentTimeStamp is " << _currentTimeStamp << "\n================================================\n";
	//========================================================
	// clean through the threat-list, removing any items that are out-dated.
	//========================================================
	for (unsigned int i=0; i<_threatList.size();) {
		if (_currentTimeStamp > _threatList[i].maxTime) {
			// swap the last item into this slot, and truncate the list.
			// this works even on the very last item, where we swap with itself.
			//cerr << "REMOVING threat " << i << ", " << _threatList[i].maxTime << " < " << _currentTimeStamp << " (original time " << _threatList[i].originalMaxTime << ")" << endl;
			std::swap(_threatList[i], _threatList.back());
			_threatList.pop_back();
		}
		else {
			i++;
		}
	}

	//========================================================
	// determine which _neighbors might be threats and 
	// update the _threatList
	//========================================================
	if (_steeringState != STEERING_STATE_TURN_TOWARDS_TARGET) {	// ignore threats in the STEERING_STATE_TURN_TOWARDS_TARGET state.

		for (std::set<SteerLib::SpatialDatabaseItemPtr>::iterator neighbor = _neighbors.begin(); neighbor != _neighbors.end(); ++neighbor) {
		//for (unsigned int i=0; i<_neighbors.size(); i++) {

			// ignore items that are not AI agents.
			if (!(*neighbor)->isAgent())
				continue;

			PPRAgent * otherGuy = dynamic_cast<PPRAgent*>(*neighbor);

			//Vector aff = _position - otherGuy->position();
			//if (aff.lengthlength) _numAgentsInVisualField++;
			_numAgentsInVisualField++;


			// ignore disabled pedestrians.
			if (!otherGuy->enabled())
				continue;

			// ignore pedestrians that are currently changing their direction significantly.
			if (otherGuy->steeringState() == STEERING_STATE_TURN_TOWARDS_TARGET)
				continue;


			// TODO?: add: if the other guy has you in his threatlist, in the space-time planning state, that means you realize he sees you,
			//       then you can safely ignore him?
			// if he didnt see you and then gets put on the threatlist, that means he will get interrupted anyway and deal with you as a threat.

			unsigned int threatIndex=0;
			alreadyExists = threatListContainsAgent(otherGuy,threatIndex);

			Vector dV = _velocity - otherGuy->velocity();
			Vector dO = _position - otherGuy->position();
			float distanceThreshold = _radius + otherGuy->radius() + PED_DYNAMIC_COLLISION_PADDING;
			float A = dot(dV,dV);
			float B = 2.0f*dot(dV,dO);
			float C = dot(dO,dO) - (distanceThreshold*distanceThreshold);
			float discriminant = (B*B) - (4.0f*A*C);

			if (!alreadyExists) {
				if (discriminant > 0) { // then these two agents are predicted to collide
					float minTimeOfThreat, maxTimeOfThreat;
					float sqrtDiscrim = sqrtf(discriminant);
					float inv2A = 0.5f / A;
					minTimeOfThreat = (-B - sqrtDiscrim)*inv2A;
					maxTimeOfThreat = (-B + sqrtDiscrim)*inv2A;

					if ((minTimeOfThreat < 0) && (maxTimeOfThreat > 0)) {
						// this would imply that we already ARE in a collision!!
						// TODO: what todo in this situation?
						// note, we do not necessarily reach this code for ALL agent-agent collisions, because of scheduling phases.
					}
					else if ((minTimeOfThreat > PED_THREAT_MIN_TIME_THRESHOLD) && (maxTimeOfThreat < PED_THREAT_MAX_TIME_THRESHOLD)) {
						//cerr << "NEW THREAT!!!\n";
						PredictedThreat newThreat;
						newThreat.maxTime = _currentTimeStamp + maxTimeOfThreat;
						newThreat.originalMaxTime = _currentTimeStamp + maxTimeOfThreat;
						newThreat.minTime = _currentTimeStamp + minTimeOfThreat;
						newThreat.threatGuy = otherGuy;
						newThreat.threatType = PredictedThreat::THREAT_TYPE_UNKNOWN; // just in case, might help debugging;
						newThreat.imminent = true;
						newThreat.oncomingToRightSide = false;

						float cosTheta = dot(_forward,otherGuy->forward());
						if (cosTheta > PED_SIMILAR_DIRECTION_DOT_PRODUCT_THRESHOLD) {
							// otherGuy is facing a similar direction as you
							// in the current implementation, this is not considered a 
							// threat, and reactive steering handles it.
						} 
						else if (cosTheta < PED_ONCOMING_PREDICTION_THRESHOLD) {
							// otherGuy is oncoming.
							float whichSideOfTarget = directionToLocalTarget.x * (otherGuy->position().x-_localTargetLocation.x) + directionToLocalTarget.z * (otherGuy->position().z-_localTargetLocation.z);
							float whichSideOfLocation = directionToLocalTarget.x * (otherGuy->position().x-position().x) + directionToLocalTarget.z * (otherGuy->position().z-position().z);
							newThreat.threatType = PredictedThreat::THREAT_TYPE_ONCOMING;
							if ((whichSideOfTarget<0.0f)&&(whichSideOfLocation>0.0f)) { // this checks if the agent is actually in-between you and your local target.
								threatListChanged = true;
								Vector dirToOtherGuy = otherGuy->position() - _position;
								if ((dot(dirToOtherGuy, _rightSide) > 0.0f) && (dot(-dirToOtherGuy,otherGuy->rightSide()) > 0.0f)) {
									newThreat.oncomingToRightSide = true;
								}
								_threatList.push_back(newThreat);
							}
						}
						else {
							float my_t = 0.0f, his_t = 0.0f;
							Ray myRay, hisRay, rayToOtherGuy;
							myRay.initWithLengthInterval(_position, _forward);
							hisRay.initWithLengthInterval(otherGuy->position(),otherGuy->forward());
							rayToOtherGuy.initWithLengthInterval( _position, otherGuy->position()-position());
							intersectTwoRays2D( myRay.pos, myRay.dir, my_t, hisRay.pos, hisRay.dir, his_t);

							if (my_t < rayToOtherGuy.maxt) {  // if expected threat is actually further away than the agent, its not actually a threat.
								float tempt1=0.0f, tempt2=0.0f;
								// NOTE CAREFULLY: localTargetLocation-position() is correct here - it should not be normalized.
								intersectTwoRays2D(_position, _localTargetLocation - _position, tempt1, otherGuy->position(), otherGuy->localTargetLocation() - otherGuy->position(), tempt2); 
								if ( (tempt1>0.0f) && (tempt1<1.0f) && (tempt2>0.0f) && (tempt2<1.0f) ) { // if paths actually cross - i.e. if its not a fake-out where the agent's goal is before the threat.
									if (my_t < his_t) {
										newThreat.threatType = PredictedThreat::THREAT_TYPE_CROSSING_SOON;
										threatListChanged = true;
										_threatList.push_back(newThreat);
										threat_min_t = min(minTimeOfThreat*_currentSpeed, threat_min_t);
										threat_max_t = max(maxTimeOfThreat*_currentSpeed, threat_max_t);
									}
									else {
										newThreat.threatType = PredictedThreat::THREAT_TYPE_CROSSING_LATE;
										threatListChanged = true;
										_threatList.push_back(newThreat);
										threat_min_t = min(minTimeOfThreat*_currentSpeed, threat_min_t);
										threat_max_t = max(maxTimeOfThreat*_currentSpeed, threat_max_t);
									}
								}
							}
						}
					} else {
						// either threat is in the past, or its too far into the future.
						// here, the threat it doesnt already exist, and here
						// we the predicted intersection is outside of the time interval
						// we care about, so don't worry about it this agent.
					}
				}
				else {
					// discriminant indicates no soln, which means no intersection predicted.
				}
			}
			else {
				// threat already existed, update it
				if (discriminant > 0) { // then these two agents are predicted to collide
					float minTimeOfThreat, maxTimeOfThreat;
					float sqrtDiscrim = sqrtf(discriminant);
					float inv2A = 0.5f / A;
					minTimeOfThreat = (-B - sqrtDiscrim)*inv2A;
					maxTimeOfThreat = (-B + sqrtDiscrim)*inv2A;
					if ((minTimeOfThreat < 0) && (maxTimeOfThreat > 0)) {
						// collided with a threat that we already predicted
						// doh!
					}
					else if ((minTimeOfThreat > PED_THREAT_MIN_TIME_THRESHOLD) && (maxTimeOfThreat < PED_THREAT_MAX_TIME_THRESHOLD)) {
						// still imminent, update the threat where it exists in the _threatList.
						_threatList[threatIndex].maxTime = _currentTimeStamp + maxTimeOfThreat;
						_threatList[threatIndex].minTime = _currentTimeStamp + minTimeOfThreat;
						//cerr << "COLLISION IS STILL IMMINENT\n";
						_threatList[threatIndex].imminent = true;
					}
					else {
						// outside of the time interval we care about, so no longer imminent.
						_threatList[threatIndex].imminent = false;
					}
				}
				else {
					// no intersection predicted, so no longer imminent.
					_threatList[threatIndex].imminent = false;
				}
			}
		}
	}

	//========================================================
	// reset _maxThreatTime and _minThreatTime with the 
	// updated _threatList.
	//
	// at the same time, compute some counters that affect
	// the state we're in... such as how many imminent threats 
	// there are.
	//========================================================
	_maxThreatTime = -INFINITY;
	_minThreatTime = INFINITY;
	_mostImminentThreatIndex = -1; // no imminent threat until we find one
	unsigned int numImminentThreats = 0;
	for (unsigned int i=0; i<_threatList.size(); i++) {
		//if (i==0) cerr << "CURRENT THREAT LIST:\n";
		//cerr << (_threatList[i].imminent ? "imminent" : "avoided") << " threat, times out at " << _threatList[i].maxTime << "/" << _threatList[i].originalMaxTime << endl;
		//cerr << "        occurs as early as: " << _threatList[i].minTime << "." << endl;

		_maxThreatTime = max(_maxThreatTime, _threatList[i].originalMaxTime);
		if ((_threatList[i].imminent) && (_threatList[i].minTime < _minThreatTime)) {
			_minThreatTime = _threatList[i].minTime;
			_mostImminentThreatIndex = i;
		} else {
			// we want to know that they were considered by prediction and deemed as non-threats
			// so that when the reactive feelers hit them, we know we can ignore it.
			// therefore we should keep non-imminent threats until they really time-out.
			// if you want to remove non-imminent threats: the following code should do it more efficiently (just test if it works, first...)
			//swap(_threatList[i],_threatList.back());
			//_threatList.pop_back();
			//i--;
		}
		if (_threatList[i].imminent) numImminentThreats++;
	}



	//
	// 2. simple state machine for threat predictions.
	// depending on the threatlist, assume various states
	// steering actions in reactive phase are determined by the chosen state.
	//
//	if (_threatList.size() >= PED_FEELING_CROWDED_THRESHOLD) {
//		// if there are too many threats, then we feel crowded and set the state accordingly.
//		// reactive steering behaviors will be more crowd-oriented in this state.
//		_steeringState = STEERING_STATE_COOPERATE_WITH_CROWD;
//	}
//	else 
	if ( (dot(directionToLocalTarget,forward()) < 0.3f) || ((dot(directionToLocalTarget,forward()) < 0.5f) && (_steeringState == STEERING_STATE_TURN_TOWARDS_TARGET))) {
		// if we are not following a space-time path, then we should be facing our local target.
		// in this case, we are not... so change the state to turn towards the target.
		_steeringState = STEERING_STATE_TURN_TOWARDS_TARGET;
	}
	else if ((_threatList.size() > 0) && (numImminentThreats > 0)) {
		// prediction/state:  a new imminent threat was detected, so react to it
		_steeringState = STEERING_STATE_PROACTIVELY_AVOID;
		//_timeToWait = _currentTimeStamp + _threatTime + 0.5f; // don't remember the expected threat for more than a small few seconds
		_timeToWait = _threatList[_mostImminentThreatIndex].originalMaxTime + 0.5f; // don't remember the expected threat for more than a small few seconds
	}
	else if ((_steeringState == STEERING_STATE_PROACTIVELY_AVOID) || ((_steeringState == STEERING_STATE_WAIT_UNTIL_CLEAR) && (!threatListChanged) && (_timeToWait > _currentTimeStamp))) {
		// TODO: changed the parentheses because of a g++ warning, but now I'm not sure if this was the correct desired semantics...
		//       double-check this!!
		// prediction/state:  wait in this state for as long as you predicted there would be a threat.  (might be overridden by the previous case when detecting a new threat, of course this is desirable)
		_steeringState = STEERING_STATE_WAIT_UNTIL_CLEAR;
	}
	else {
		// prediction/state:  no threats or crowds, steer normally towards local target location.
		_steeringState = STEERING_STATE_NO_THREAT;
		_threatList.clear();
	}


#ifdef USE_ANNOTATIONS
	__threat_min_t = threat_min_t;
	__threat_max_t = threat_max_t;
#endif

}


//
// runReactivePhase()
//
void PPRAgent::runReactivePhase()
{

	if ((!_enabled) || (_landmarkQueue.empty())) return;

	if (reachedLocalTarget()) {
		// then schedule shortTermPlanning phase
		runShortTermPlanningPhase();
		if ((!_enabled) || (_landmarkQueue.empty())) return;
	}

	AutomaticFunctionProfiler profileThisFunction( &PPRGlobals::gPhaseProfilers->reactivePhaseProfiler );

	FeelerInfo feelers;

	bool comfortZoneViolated = false;
	bool hitSomething = updateReactiveFeelers(feelers);

	// note steeringMode probably should NOT be initialized here ?
	_finalSteeringCommand.scoot = 0.0f;
	_finalSteeringCommand.aimForTargetDirection = true; // assume its true unless the reactions below indicate otherwise.
	_finalSteeringCommand.targetDirection = normalize(_localTargetLocation - _position);
	_finalSteeringCommand.aimForTargetSpeed = true;  // assume its true unless reactions indicate otherwise.
	_finalSteeringCommand.turningAmount = PED_ADJUSTMENT_TURN_RATE;
	_finalSteeringCommand.acceleration = 1.0f;
	_finalSteeringCommand.targetSpeed = PED_TYPICAL_SPEED_FACTOR * _currentGoal.desiredSpeed;
	//_finalSteeringCommand.dynamicsSteeringForce
	//_finalSteeringCommand.steeringMode

	if(_isNewGoal) {
		_startTargetPosition = _localTargetLocation;
		_isNewGoal = false;
	}

	//
	// apply scooting.
	// scoot value:  exert a side force proportional to how close an object is to you.
	// TODO: it is OK that this happens regardless of state?
	// TODO: should you comment out the 2nd and 3rd conditions for violating comfort zone in the conditional above??
	//
	if (feelers.t_right < PED_COMFORT_ZONE) {
		if ((!feelers.object_left) && (!feelers.object_lside)) {
			_finalSteeringCommand.scoot = max(-1.0f / feelers.t_right, -_maxForce);
		} else {
			comfortZoneViolated=true;
		}
	}
	if (feelers.t_left < PED_COMFORT_ZONE) {
		if ((!feelers.object_right) && (!feelers.object_rside)) {
			_finalSteeringCommand.scoot = min(1.0f / feelers.t_left, _maxForce);
		} else {
			comfortZoneViolated=true;
		}
	}
/*	if (feelers.object_rside && !feelers.object_lside) {
		_finalSteeringCommand.scoot = max(-1.0f / feelers.t_rside, -_maxForce);
	} else if (feelers.object_lside && !feelers.object_rside) {
		_finalSteeringCommand.scoot = min(1.0f / feelers.t_lside, _maxForce);
	}
*/


	//========================
	if (hitSomething) {

		//
		// if we hit something, of course we have to react to it.
		//
		// the first several chunks of code are used first to recognize 
		// what situation we're in.
		//
		// then there is a big sequence of nested if-statements to decide what 
		// steering behavior to actually produce based on the situation.
		//

		// check if we hit any static objects with our feelers
		bool hitStaticObject = (((feelers.object_front) && (!feelers.object_front->isAgent()))  
								|| ((feelers.object_left) && (!feelers.object_left->isAgent()))
								|| ((feelers.object_right) && (!feelers.object_right->isAgent())) );

		// check if we hit agents that were not already in our _threatList
		bool foundNewThreat = false;
		if (   ((feelers.object_front) && (feelers.object_front->isAgent()) && (!threatListContainsAgent(dynamic_cast<PPRAgent*>(feelers.object_front))))
			|| ((feelers.object_left) && (feelers.object_left->isAgent()) && (!threatListContainsAgent(dynamic_cast<PPRAgent*>(feelers.object_left))))
			|| ((feelers.object_right) && (feelers.object_right->isAgent()) && (!threatListContainsAgent(dynamic_cast<PPRAgent*>(feelers.object_right)))))
		{
			foundNewThreat = true;
		}

		// check if we hit any agents that were in our _threatList, but considered non-imminent.
		// TODO: its not clear whether we should react to existing non-imminent threats or not ???
		bool existingThreatRaisedAgain = false;
		unsigned int tempIndex;
		if (   ((feelers.object_front) && (feelers.object_front->isAgent()) && (threatListContainsAgent(dynamic_cast<PPRAgent*>(feelers.object_front), tempIndex)) && (!_threatList[tempIndex].imminent) )
			|| ((feelers.object_left) && (feelers.object_left->isAgent()) && (threatListContainsAgent(dynamic_cast<PPRAgent*>(feelers.object_left), tempIndex)) && (!_threatList[tempIndex].imminent) )
			|| ((feelers.object_right) && (feelers.object_right->isAgent()) && (threatListContainsAgent(dynamic_cast<PPRAgent*>(feelers.object_right), tempIndex)) && (!_threatList[tempIndex].imminent) ))
		{
			existingThreatRaisedAgain = true;
		}

		// check if our comfortzone is violated
		if ((feelers.t_front < PED_COMFORT_ZONE) || (feelers.t_right < PED_COMFORT_ZONE) || (feelers.t_left < PED_COMFORT_ZONE)) {
			comfortZoneViolated=true;
		}

		// how many agents touched our feelers
		unsigned int numAgentsHit = 0;
		// how many agents we THINK were predictively avoided (but maybe the situation changed by the time our feelers touch them)
		unsigned int numAgentsNotPosingThreat = 0;
		if ((feelers.object_front) && (feelers.object_front->isAgent())) {
			numAgentsHit++;
			PPRAgent * p = dynamic_cast<PPRAgent*>(feelers.object_front);
			Vector dV = _velocity - p->velocity();
			Vector dO = _position - p->position();
			float distanceThreshold = _radius + p->radius() + PED_DYNAMIC_COLLISION_PADDING;
			float A = dot(dV,dV);
			float B = 2.0f*dot(dV,dO);
			float C = dot(dO,dO) - (distanceThreshold*distanceThreshold);
			float discriminant = (B*B) - (4.0f*A*C);
			if (discriminant <= 0) {
				// note: this is not a full check if we will collide with them at a FUTURE point
				// however, given that we are hitting them with our feelers, this is a sufficient condition.
				numAgentsNotPosingThreat++;
			}
		}
		if ((feelers.object_left) && (feelers.object_left!=feelers.object_front) && (feelers.object_left->isAgent())) {
			numAgentsHit++;
			PPRAgent * p = dynamic_cast<PPRAgent*>(feelers.object_left);
			Vector dV = _velocity - p->velocity();
			Vector dO = _position - p->position();
			float distanceThreshold = _radius + p->radius() + PED_DYNAMIC_COLLISION_PADDING;
			float A = dot(dV,dV);
			float B = 2.0f*dot(dV,dO);
			float C = dot(dO,dO) - (distanceThreshold*distanceThreshold);
			float discriminant = (B*B) - (4.0f*A*C);
			if (discriminant <= 0) {
				// note: this is not a full check if we will collide with them at a FUTURE point
				// however, given that we are hitting them with our feelers, this is a sufficient condition.
				numAgentsNotPosingThreat++;
			}
		}
		if ((feelers.object_right) && (feelers.object_right!=feelers.object_front) && (feelers.object_right!=feelers.object_left) && (feelers.object_right->isAgent())) {
			numAgentsHit++;
			PPRAgent * p = dynamic_cast<PPRAgent*>(feelers.object_right);
			Vector dV = _velocity - p->velocity();
			Vector dO = _position - p->position();
			float distanceThreshold = _radius + p->radius() + PED_DYNAMIC_COLLISION_PADDING;
			float A = dot(dV,dV);
			float B = 2.0f*dot(dV,dO);
			float C = dot(dO,dO) - (distanceThreshold*distanceThreshold);
			float discriminant = (B*B) - (4.0f*A*C);
			if (discriminant <= 0) {
				// note: this is not a full check if we will collide with them at a FUTURE point
				// however, given that we are hitting them with our feelers, this is a sufficient condition.
				numAgentsNotPosingThreat++;
			}
		}

		//
		// determine the reactive situation.
		// there is a reactive situation to deal with if at least one agent was not predictively avoided.
		// however, once that criterion is met (i.e.  (numAgentsPredicted < XXX)), then the 
		// rest of the reactive situation is based on the agents hit regardless of whether they were predicted or not.
		//
		ReactiveSituationEnum reactiveSituation;
		if ((!hitStaticObject) && (numAgentsHit == numAgentsNotPosingThreat)) {
			// in this case, we can safely ignore what we intersected, because everything we perceived with the feelers
			// was already predicted.
			reactiveSituation = REACTIVE_SITUATION_NO_THREATS;
		}
		else if ((!hitStaticObject) && (numAgentsHit == 1)) {
			// in this case, we perceived only one agent.
			reactiveSituation = REACTIVE_SITUATION_ONE_AGENT;
		}
		else if ((!hitStaticObject) && (numAgentsHit == 2)) {
			reactiveSituation = REACTIVE_SITUATION_TWO_AGENTS;
		}
		else if ((!hitStaticObject) && (numAgentsHit == 3)) {
			reactiveSituation = REACTIVE_SITUATION_THREE_AGENTS;
		}
		else if ((hitStaticObject) && (numAgentsHit == 0)) {
			reactiveSituation = REACTIVE_SITUATION_STATIC_OBJECTS_ZERO_AGENTS;
		}
		else if ((hitStaticObject) && (numAgentsHit == 1)) {
			reactiveSituation = REACTIVE_SITUATION_STATIC_OBJECTS_ONE_AGENT;
		}
		else if ((hitStaticObject) && (numAgentsHit == 2)) {
			reactiveSituation = REACTIVE_SITUATION_STATIC_OBJECTS_TWO_AGENTS;
		}
		else {
			std::cerr << "ERROR: DID NOT EXPECT TO GET HERE IN THE CODE (IN REACTIVE PHASE) (unknown reactive situation)!\n";
			reactiveSituation = REACTIVE_SITUATION_UNKNOWN;
		}




		//
		// then, based on the "steering state" and the "reactive situation", steer accordingly.
		//

		if (_steeringState == STEERING_STATE_TURN_TOWARDS_TARGET) {
			if ((dot(_finalSteeringCommand.targetDirection,forward()) > PED_SAME_DIRECTION_DOT_PRODUCT_THRESHOLD)) {
				_steeringState = STEERING_STATE_NO_THREAT;
			}
			_finalSteeringCommand.aimForTargetDirection = true;
			_finalSteeringCommand.turningAmount = PED_CORNERING_TURN_RATE;
			_finalSteeringCommand.aimForTargetSpeed = true;
			_finalSteeringCommand.targetSpeed = PED_TYPICAL_SPEED_FACTOR * _currentGoal.desiredSpeed; //0.0f;	
			if ((feelers.t_left < PED_COMFORT_ZONE) && (!(feelers.t_right < PED_COMFORT_ZONE))) {
				_finalSteeringCommand.scoot = 0.5f * _maxForce;
			}
			else if (feelers.t_right < PED_COMFORT_ZONE) {
				_finalSteeringCommand.scoot = -0.5f * _maxForce;
			}
		}
		else if ((_steeringState == STEERING_STATE_PROACTIVELY_AVOID) && (foundNewThreat) && (reactiveSituation == REACTIVE_SITUATION_ONE_AGENT)) {
			// in this case: match the speed of whoever you're steering into, and avoid it if its oncoming; 
			// otherwise keep steering to avoid the prediction.
			
			// match speed:
			if ((feelers.object_left)&&(feelers.object_left->isAgent())) {
				float tempVelocity = dot(forward(),(dynamic_cast<PPRAgent*>(feelers.object_left))->velocity());
				//if (tempVelocity > -1.0f)
					_finalSteeringCommand.targetSpeed = min(_finalSteeringCommand.targetSpeed,tempVelocity);
			}
			if ((feelers.object_right)&&(feelers.object_right->isAgent())) {
				float tempVelocity = dot(forward(),(dynamic_cast<PPRAgent*>(feelers.object_right))->velocity());
				//if (tempVelocity > -1.0f)
					_finalSteeringCommand.targetSpeed = min(_finalSteeringCommand.targetSpeed,tempVelocity);
			}
			if ((feelers.object_front)&&(feelers.object_front->isAgent())) {
				float tempVelocity = dot(forward(),(dynamic_cast<PPRAgent*>(feelers.object_front))->velocity());
				//if (tempVelocity > -1.0f)
					_finalSteeringCommand.targetSpeed = min(_finalSteeringCommand.targetSpeed,tempVelocity);
			}
			//if (comfortZoneViolated) {
				_finalSteeringCommand.targetSpeed = 0.7f * _finalSteeringCommand.targetSpeed;
			//}
			if (_finalSteeringCommand.targetSpeed < 0.0f) _finalSteeringCommand.targetSpeed = 0.0f;


			SpatialDatabaseItemPtr obj = (feelers.object_front) ? feelers.object_front : (feelers.object_right) ? feelers.object_right : feelers.object_left;
			assert(obj!=NULL);
			PPRAgent * p = dynamic_cast<PPRAgent*>(obj);


			float cosTheta = dot(_forward, p->forward());
			if ( cosTheta < 0.0f ) {
				if ((feelers.object_front || feelers.object_left) && (!feelers.object_right)) {
					//if (isSelected()) cerr << "REACTION: one oncoming agent, while I was trying to avoid another threat. I'll go to the right.\n";
					_finalSteeringCommand.aimForTargetDirection = false;
					_finalSteeringCommand.turningAmount = PED_TYPICAL_AVOIDANCE_TURN_RATE;
				}
				else {
					//if (isSelected()) cerr << "REACTION: one oncoming agent, while I was trying to avoid another threat. I'll go to the left.\n";
					_finalSteeringCommand.aimForTargetDirection = false;
					_finalSteeringCommand.turningAmount = -PED_TYPICAL_AVOIDANCE_TURN_RATE;
				}

			}
			else {
				// steer to avoid prediction anyway, the prediction will probably change drastically anyway as you change your speed and direction...
				if (_threatList[_mostImminentThreatIndex].threatType == PredictedThreat::THREAT_TYPE_ONCOMING) {
					if (_threatList[_mostImminentThreatIndex].oncomingToRightSide) {
						//if (isSelected()) cerr << "REACTION: one oncoming agent,while I was trying to avoid another threatI'll go to the left.\n";
						_finalSteeringCommand.aimForTargetDirection = false;
						_finalSteeringCommand.turningAmount = -PED_ADJUSTMENT_TURN_RATE;
					}
					else {
						//if (isSelected()) cerr << "REACTION: one oncoming agent, while I was trying to avoid another threat. I'll go to the right.\n";
						_finalSteeringCommand.aimForTargetDirection = false;
						_finalSteeringCommand.turningAmount = PED_ADJUSTMENT_TURN_RATE;
					}
				}
				else if (_threatList[_mostImminentThreatIndex].threatType == PredictedThreat::THREAT_TYPE_CROSSING_SOON) {
					// action: steer away and speed up
					// do a dot product between the side vector and dirToOtherGuy to determine if its on your right side or left side
					PPRAgent * threatGuy = _threatList[_mostImminentThreatIndex].threatGuy;
					if ( dot((_rightSide),threatGuy->forward()) < 0.0f) {
						//if (isSelected()) cerr << "REACTION: one crossing agent, while I was trying to avoid another threat. (crossing_soon) steering left.\n";
						//_finalSteeringCommand.aimForTargetSpeed = true;
						//_finalSteeringCommand.targetSpeed = PED_SLIGHTLY_FASTER_SPEED_FACTOR*_currentGoal.desiredSpeed;
						_finalSteeringCommand.aimForTargetDirection = false;
						_finalSteeringCommand.turningAmount = -PED_TYPICAL_AVOIDANCE_TURN_RATE;
					}
					else {
						//if (isSelected()) cerr << "REACTION: one crossing agent, while I was trying to avoid another threat. (crossing_soon) steering right.\n";
						//_finalSteeringCommand.aimForTargetSpeed = true;
						//_finalSteeringCommand.targetSpeed = PED_SLIGHTLY_FASTER_SPEED_FACTOR*_currentGoal.desiredSpeed;
						_finalSteeringCommand.aimForTargetDirection = false;
						_finalSteeringCommand.turningAmount = PED_TYPICAL_AVOIDANCE_TURN_RATE;
					}
				}
				else if (_threatList[_mostImminentThreatIndex].threatType == PredictedThreat::THREAT_TYPE_CROSSING_LATE) {
					// action: steer towards the other agent and slow down
					// do a dot product between the side vector and dirToOtherGuy to determine if its on your right side or left side
					PPRAgent * threatGuy = _threatList[_mostImminentThreatIndex].threatGuy;
					if ( dot((_rightSide),threatGuy->forward()) < 0.0f) {
						//if (isSelected()) cerr << "REACTION: one crossing agent, while I was trying to avoid another threat. (crossing_late) steering right.\n";
						//_finalSteeringCommand.aimForTargetSpeed = true;
						//_finalSteeringCommand.targetSpeed = PED_SLIGHTLY_SLOWER_SPEED_FACTOR*_currentGoal.desiredSpeed;
						_finalSteeringCommand.aimForTargetDirection = false;
						_finalSteeringCommand.turningAmount = PED_TYPICAL_AVOIDANCE_TURN_RATE;
					}
					else {
						//if (isSelected()) cerr << "REACTION: one crossing agent, while I was trying to avoid another threat. (crossing_late) steering left.\n";
						//_finalSteeringCommand.aimForTargetSpeed = true;
						//_finalSteeringCommand.targetSpeed = PED_SLIGHTLY_SLOWER_SPEED_FACTOR*_currentGoal.desiredSpeed;
						_finalSteeringCommand.aimForTargetDirection = false;
						_finalSteeringCommand.turningAmount = -PED_TYPICAL_AVOIDANCE_TURN_RATE;
					}
				}
				else {
					std::cerr << "ERROR: invalid threatType specified: " << ((int)_threatList[_mostImminentThreatIndex].threatType) << "\n";
				}
			}
		}
		else if (reactiveSituation != REACTIVE_SITUATION_NO_THREATS) {
		 
			if (reactiveSituation == REACTIVE_SITUATION_ONE_AGENT) {
				// five possibilities:
				//  - oncoming, should steer right
				//  - oncoming, should steer left
				//  - similar direction
				//  - other guy is crossing your path, other guy is perceived to be "in front"
				//  - you are crossing the other guy's path, you are perceived to be "in front"
				SpatialDatabaseItemPtr obj = (feelers.object_front) ? feelers.object_front : (feelers.object_right) ? feelers.object_right : feelers.object_left;
				assert(obj!=NULL);
				PPRAgent * p = dynamic_cast<PPRAgent*>(obj);
				float cosTheta = dot(_forward, p->forward());
				if ( cosTheta < PED_ONCOMING_REACTION_THRESHOLD ) {
					if ((feelers.object_front || feelers.object_left) && (!feelers.object_right)) {
						//if (isSelected()) cerr << "REACTION: one oncoming agent, I'll go to the right.\n";
						_finalSteeringCommand.aimForTargetDirection = false;
						_finalSteeringCommand.turningAmount = (comfortZoneViolated) ? PED_FASTER_AVOIDANCE_TURN_RATE : PED_TYPICAL_AVOIDANCE_TURN_RATE;
						_finalSteeringCommand.targetSpeed = (comfortZoneViolated) ? PED_SLOWER_SPEED_FACTOR * _currentGoal.desiredSpeed : PED_TYPICAL_SPEED_FACTOR * _currentGoal.desiredSpeed;
					}
					else {
						//if (isSelected()) cerr << "REACTION: one oncoming agent, I'll go to the left.\n";
						_finalSteeringCommand.aimForTargetDirection = false;
						_finalSteeringCommand.turningAmount = (comfortZoneViolated) ? -PED_FASTER_AVOIDANCE_TURN_RATE : -PED_TYPICAL_AVOIDANCE_TURN_RATE;
						_finalSteeringCommand.targetSpeed = (comfortZoneViolated) ? PED_SLOWER_SPEED_FACTOR * _currentGoal.desiredSpeed : PED_TYPICAL_SPEED_FACTOR * _currentGoal.desiredSpeed;
					}

				}
				else {
					//Vector dirToOtherGuy = normalize(p->position() - _position);
					float my_time = INFINITY, his_time = INFINITY;
					intersectTwoRays2D(position(), forward(), my_time, p->position(), p->forward(), his_time);
					if (his_time < my_time && p->velocity().length() != 0) {
						//if (isSelected()) cerr << "REACTION: one agent, he'll go in front of me, so I'll wait\n";
						float tempVelocity = dot(forward(),p->velocity());
						_finalSteeringCommand.targetSpeed = min(_finalSteeringCommand.targetSpeed, PED_SLOWER_SPEED_FACTOR * tempVelocity);
					}
					else {
						//if (isSelected()) cerr << "REACTION: one agent, I'm in front of him, so I'll go.\n";
						if (!(feelers.object_right)) {
							_finalSteeringCommand.aimForTargetDirection = false;
							_finalSteeringCommand.turningAmount = PED_TYPICAL_AVOIDANCE_TURN_RATE;
						}
						else if (!(feelers.object_left)) {
							_finalSteeringCommand.aimForTargetDirection = false;
							_finalSteeringCommand.turningAmount = -PED_TYPICAL_AVOIDANCE_TURN_RATE;
						}
						else if (!(feelers.object_front)) {
							_finalSteeringCommand.aimForTargetDirection = false;
							// just keep straight
						}
						else { // there are all obstacles in three directions, just turn right largely 
							_finalSteeringCommand.aimForTargetDirection = false;
							_finalSteeringCommand.turningAmount = PED_FASTER_AVOIDANCE_TURN_RATE;
						}
						_finalSteeringCommand.targetSpeed = (comfortZoneViolated) ? PED_SLOWER_SPEED_FACTOR * _currentGoal.desiredSpeed : PED_TYPICAL_SPEED_FACTOR * _currentGoal.desiredSpeed;
					}
				}
			}
			else if (reactiveSituation == REACTIVE_SITUATION_TWO_AGENTS) {
				SpatialDatabaseItemPtr objLeft = (feelers.object_left) ? feelers.object_left : feelers.object_front;
				SpatialDatabaseItemPtr objRight = (feelers.object_right) ? feelers.object_right : feelers.object_front;
				assert(objLeft!=objRight);
				PPRAgent * pLeft = dynamic_cast<PPRAgent*>(objLeft);
				PPRAgent * pRight = dynamic_cast<PPRAgent*>(objRight);
				float cosThetaLeft = dot(_forward, pLeft->forward());
				float cosThetaRight = dot(_forward, pRight->forward());
				if (pLeft->velocity().length() == 0 && pRight->velocity().length() == 0) {
					_finalSteeringCommand.aimForTargetDirection = false;
					if (!feelers.object_left)
						_finalSteeringCommand.turningAmount = -PED_TYPICAL_AVOIDANCE_TURN_RATE;
					else if (!feelers.object_front)
						_finalSteeringCommand.turningAmount = 0.0f;
					else
						_finalSteeringCommand.turningAmount = PED_TYPICAL_AVOIDANCE_TURN_RATE;
					_finalSteeringCommand.targetSpeed = (comfortZoneViolated) ? PED_SLOWER_SPEED_FACTOR * _currentGoal.desiredSpeed : PED_TYPICAL_SPEED_FACTOR * _currentGoal.desiredSpeed;
				}
				else {
					if ((cosThetaLeft < PED_ONCOMING_REACTION_THRESHOLD) && (cosThetaRight < PED_ONCOMING_REACTION_THRESHOLD)) {
						//if (isSelected()) cerr << "REACTION: two agents oncoming to me... I'll just stop...\n";
						_finalSteeringCommand.aimForTargetDirection = true;
						_finalSteeringCommand.turningAmount = PED_TYPICAL_AVOIDANCE_TURN_RATE;
						_finalSteeringCommand.targetSpeed = 0.0;
					}
					else if ((cosThetaLeft > PED_SAME_DIRECTION_DOT_PRODUCT_THRESHOLD) && (cosThetaRight < PED_ONCOMING_REACTION_THRESHOLD)) {
						//if (isSelected()) cerr << "REACTION: two agents - I'll follow the one on the left.\n";
						_finalSteeringCommand.aimForTargetDirection = false;
						_finalSteeringCommand.turningAmount = (objRight == feelers.object_front) ? -PED_TYPICAL_AVOIDANCE_TURN_RATE : -PED_ADJUSTMENT_TURN_RATE;
						float tempVelocity = dot(forward(),pLeft->velocity());
						_finalSteeringCommand.targetSpeed = min(_finalSteeringCommand.targetSpeed, tempVelocity);
						if (comfortZoneViolated) _finalSteeringCommand.targetSpeed = 0.7f * _finalSteeringCommand.targetSpeed;
						if (_finalSteeringCommand.targetSpeed < 0.0f) _finalSteeringCommand.targetSpeed = 0.0f;
					}
					else if ((cosThetaLeft < PED_ONCOMING_REACTION_THRESHOLD) && (cosThetaRight > PED_SAME_DIRECTION_DOT_PRODUCT_THRESHOLD)) {
						//if (isSelected()) cerr << "REACTION: two agents - I'll follow the one on the right.\n";
						_finalSteeringCommand.aimForTargetDirection = false;
						_finalSteeringCommand.turningAmount = (objLeft == feelers.object_front) ? PED_TYPICAL_AVOIDANCE_TURN_RATE : PED_ADJUSTMENT_TURN_RATE;
						float tempVelocity = dot(forward(),pRight->velocity());
						_finalSteeringCommand.targetSpeed = min(_finalSteeringCommand.targetSpeed, tempVelocity);
						if (comfortZoneViolated) _finalSteeringCommand.targetSpeed = 0.7f * _finalSteeringCommand.targetSpeed;
						if (_finalSteeringCommand.targetSpeed < 0.0f) _finalSteeringCommand.targetSpeed = 0.0f;
					}
					else {
						//if (isSelected()) cerr << "REACTION: two agents - I'll just match the speed they are going.\n";
						float tempVelocity = dot(forward(),pLeft->velocity());
						tempVelocity = min(tempVelocity, dot(forward(),pRight->velocity()));
						_finalSteeringCommand.targetSpeed = min(_finalSteeringCommand.targetSpeed, tempVelocity);
						if (comfortZoneViolated) _finalSteeringCommand.targetSpeed = 0.7f * _finalSteeringCommand.targetSpeed;
						if (_finalSteeringCommand.targetSpeed < 0.0f) _finalSteeringCommand.targetSpeed = 0.0f;
					}
				}
			}
			else if (reactiveSituation == REACTIVE_SITUATION_STATIC_OBJECTS_ZERO_AGENTS) {
				if ((feelers.t_left >= feelers.t_front) && (feelers.t_front > feelers.t_right)) {
					//if (isSelected()) cerr << "REACTION: just static obstacles... I should steer left.\n";
					_finalSteeringCommand.aimForTargetDirection = false;
					_finalSteeringCommand.turningAmount = (comfortZoneViolated) ? -PED_FASTER_AVOIDANCE_TURN_RATE : -PED_TYPICAL_AVOIDANCE_TURN_RATE;
					_finalSteeringCommand.targetSpeed = PED_VERY_SLOWER_SPEED_FACTOR * _currentGoal.desiredSpeed;
				}
				else if ((feelers.t_right >= feelers.t_front) && (feelers.t_front > feelers.t_left)) {
					//if (isSelected()) cerr << "REACTION: just static obstacles... I should steer right.\n";
					_finalSteeringCommand.aimForTargetDirection = false;
					_finalSteeringCommand.turningAmount = (comfortZoneViolated) ? PED_FASTER_AVOIDANCE_TURN_RATE : PED_TYPICAL_AVOIDANCE_TURN_RATE;
					_finalSteeringCommand.targetSpeed = PED_VERY_SLOWER_SPEED_FACTOR * _currentGoal.desiredSpeed;
				}
				else {
					// in this case, t_front was not correctly in-between t_right and t_left
					// so either you're in a corner, or a doorway.
					if ((feelers.t_front < feelers.t_left) && (feelers.t_front < feelers.t_right)) {
						//if (isSelected()) cerr << "REACTION: just static obstacles... I'm reaching a convex corner... I'll just quickly steer towards my targetDirection\n";
						_finalSteeringCommand.aimForTargetDirection = true;
						_finalSteeringCommand.turningAmount = PED_FASTER_AVOIDANCE_TURN_RATE;
						_finalSteeringCommand.targetSpeed = (comfortZoneViolated) ? PED_SLOWER_SPEED_FACTOR * _currentGoal.desiredSpeed : PED_TYPICAL_SPEED_FACTOR * _currentGoal.desiredSpeed;
					}
					else if (feelers.t_front == INFINITY) {
						//if (isSelected()) cerr << "REACTION: just static obstacles... I should steer through.\n";
						_finalSteeringCommand.aimForTargetDirection = true;
						_finalSteeringCommand.turningAmount = PED_TYPICAL_AVOIDANCE_TURN_RATE;
						_finalSteeringCommand.targetSpeed = PED_TYPICAL_SPEED_FACTOR * _currentGoal.desiredSpeed;
						_finalSteeringCommand.scoot = (feelers.t_left < feelers.t_right) ? 0.5f * _maxForce : -0.5f * _maxForce;
					}
					else {
						//if (isSelected()) cerr << "REACTION: just static obstacles... I think I'm steering into a concave corner... I'll just quickly steer towards my targetDirection\n";
						// TODO: is there something more intelligent to do?
						_finalSteeringCommand.aimForTargetDirection = true;
						_finalSteeringCommand.turningAmount = PED_FASTER_AVOIDANCE_TURN_RATE;
						_finalSteeringCommand.targetSpeed = PED_VERY_SLOWER_SPEED_FACTOR * _currentGoal.desiredSpeed;
					}
				}
			}
			else if (reactiveSituation == REACTIVE_SITUATION_STATIC_OBJECTS_ONE_AGENT) {
				//if (isSelected()) cerr << "REACTION: static obstacle and one agent...\n";

				SpatialDatabaseItemPtr objAgent = (feelers.object_front && feelers.object_front->isAgent()) ? feelers.object_front : (feelers.object_right && feelers.object_right->isAgent()) ? feelers.object_right : feelers.object_left;
				SpatialDatabaseItemPtr obstacle = (feelers.object_front && !feelers.object_front->isAgent()) ? feelers.object_front : (feelers.object_right && !feelers.object_right->isAgent()) ? feelers.object_right : feelers.object_left;

				PPRAgent * p = dynamic_cast<PPRAgent*>(objAgent);

				if ( dot(p->forward(), _forward) < PED_ONCOMING_REACTION_THRESHOLD ) {
					if (obstacle == feelers.object_right) {
						//if (isSelected()) cerr << "REACTION: a static obstacle and an oncoming agent... I'll just wait for him to go around me.\n";
						if(p->velocity().length() == 0) {
							if (!feelers.object_front) {
								// if there is no obstacle in front, just keep forward
							}
							if (obstacle == feelers.object_right) {
								_finalSteeringCommand.aimForTargetDirection = false;
								_finalSteeringCommand.turningAmount = -PED_TYPICAL_AVOIDANCE_TURN_RATE;
							}
							else {
								_finalSteeringCommand.aimForTargetDirection = false;
								_finalSteeringCommand.turningAmount = PED_TYPICAL_AVOIDANCE_TURN_RATE;
							}
						}
						else
							_finalSteeringCommand.targetSpeed = 0.0f;
					}
					else if (obstacle == feelers.object_left) {
						//if (isSelected()) cerr << "REACTION: a static obstacle and an oncoming agent... I'll go around them on the right\n";
						_finalSteeringCommand.aimForTargetDirection = false;
						_finalSteeringCommand.turningAmount = PED_TYPICAL_AVOIDANCE_TURN_RATE;
						_finalSteeringCommand.targetSpeed = PED_TYPICAL_SPEED_FACTOR * _currentGoal.desiredSpeed;
					}
					else {
						// weird, only hit the static object in the middle, its either a small obstacle, or a corner.
						//if (isSelected()) cerr << "REACTION: a static obstacle and an oncoming agent... a corner? what do I do?\n";
						// TODO: is this the right thing to do here?
						_finalSteeringCommand.aimForTargetDirection = true;
						_finalSteeringCommand.turningAmount = PED_CORNERING_TURN_RATE;
						_finalSteeringCommand.targetSpeed = 0.0f;
					}
				}
				else {
					float my_time = INFINITY, his_time = INFINITY;
					intersectTwoRays2D(position(), forward(), my_time, p->position(), p->forward(), his_time);

					// choose target speed based on agent
					if (my_time < 0.0f) {
						// if we unfortunately enter the collision already, just walk towards the target
					}
					else if (p->velocity().length() == 0) {
						if (!feelers.object_front) {
							// if there is no obstacle in front, just keep forward
						}
						if (obstacle == feelers.object_right) {
							_finalSteeringCommand.aimForTargetDirection = false;
							_finalSteeringCommand.turningAmount = -PED_TYPICAL_AVOIDANCE_TURN_RATE;
						}
						else {
							_finalSteeringCommand.aimForTargetDirection = false;
							_finalSteeringCommand.turningAmount = PED_TYPICAL_AVOIDANCE_TURN_RATE;
						}
					}
					else if (my_time < his_time) {
						//if (isSelected()) cerr << "REACTION: a static obstacle and an non-oncoming agent, I'm in front of him, so I'll go.\n";
						_finalSteeringCommand.targetSpeed = PED_SLIGHTLY_FASTER_SPEED_FACTOR * _currentGoal.desiredSpeed;
					}
					else {
						//if (isSelected()) cerr << "REACTION: a static obstacle and an non-oncoming agent, he'll go in front of me, so I'll wait\n";
						float tempVelocity = dot(forward(),p->velocity());
						_finalSteeringCommand.targetSpeed = min(_finalSteeringCommand.targetSpeed, PED_SLOWER_SPEED_FACTOR * tempVelocity);
					}
					/*
					// choose target direction to avoid the static obstacle.
					if (obstacle == feelers.object_right) {
						_finalSteeringCommand.aimForTargetDirection = false;
						_finalSteeringCommand.turningAmount = -PED_ADJUSTMENT_TURN_RATE;
					}
					else if (obstacle == feelers.object_left) {
						_finalSteeringCommand.aimForTargetDirection = false;
						_finalSteeringCommand.turningAmount = PED_ADJUSTMENT_TURN_RATE;
					}
					else {
						// TODO: is this the right thing to do here?
						_finalSteeringCommand.aimForTargetDirection = true;
						_finalSteeringCommand.turningAmount = PED_CORNERING_TURN_RATE;
						_finalSteeringCommand.targetSpeed = 0.0f;
					}*/

				}
			}
			else if (reactiveSituation == REACTIVE_SITUATION_THREE_AGENTS) {
				// unlikely case...
				PPRAgent * pLeft = dynamic_cast<PPRAgent*>(feelers.object_left);
				PPRAgent * pMiddle = dynamic_cast<PPRAgent*>(feelers.object_front);
				PPRAgent * pRight = dynamic_cast<PPRAgent*>(feelers.object_right);
				if (pLeft->velocity().length() == 0 && pMiddle->velocity().length() == 0 && pRight->velocity().length() == 0) {
					_finalSteeringCommand.aimForTargetDirection = false;
					_finalSteeringCommand.turningAmount = PED_TYPICAL_AVOIDANCE_TURN_RATE;
					_finalSteeringCommand.targetSpeed = (comfortZoneViolated) ? PED_SLOWER_SPEED_FACTOR * _currentGoal.desiredSpeed : PED_TYPICAL_SPEED_FACTOR * _currentGoal.desiredSpeed;
				}
				else {
					//if (isSelected()) cerr << "REACTION: three agents - I'll just match their speed and hope it doesnt get clogged?\n";
					if ((feelers.object_left)&&(feelers.object_left->isAgent())) {
						float tempVelocity = dot(forward(),(dynamic_cast<PPRAgent*>(feelers.object_left))->velocity());
						_finalSteeringCommand.targetSpeed = min(_finalSteeringCommand.targetSpeed,tempVelocity);
					}
					if ((feelers.object_right)&&(feelers.object_right->isAgent())) {
						float tempVelocity = dot(forward(),(dynamic_cast<PPRAgent*>(feelers.object_right))->velocity());
						_finalSteeringCommand.targetSpeed = min(_finalSteeringCommand.targetSpeed,tempVelocity);
					}
					if ((feelers.object_front)&&(feelers.object_front->isAgent())) {
						float tempVelocity = dot(forward(),(dynamic_cast<PPRAgent*>(feelers.object_front))->velocity());
						_finalSteeringCommand.targetSpeed = min(_finalSteeringCommand.targetSpeed,tempVelocity);
					}
					if (comfortZoneViolated) {
						_finalSteeringCommand.targetSpeed = 0.7f * _finalSteeringCommand.targetSpeed;
					}

					if (_finalSteeringCommand.targetSpeed < 0.0f) _finalSteeringCommand.targetSpeed = 0.0f;
				}
			}
			else if (reactiveSituation == REACTIVE_SITUATION_STATIC_OBJECTS_TWO_AGENTS) {
				// unlikely case...
				//if (isSelected()) cerr << "REACTION: static obstacle and two agents...I'll just match the agent's speeds and avoid the obstacle.\n";
				SpatialDatabaseItemPtr objLeft = (feelers.object_left) ? feelers.object_left : feelers.object_front;
				SpatialDatabaseItemPtr objRight = (feelers.object_right) ? feelers.object_right : feelers.object_front;
				assert(objLeft!=objRight);
				PPRAgent * pLeft = dynamic_cast<PPRAgent*>(objLeft);
				PPRAgent * pRight = dynamic_cast<PPRAgent*>(objRight);
				if (pLeft->velocity().length() == 0 && pRight->velocity().length() == 0) {
					_finalSteeringCommand.aimForTargetDirection = false;
					_finalSteeringCommand.turningAmount = PED_TYPICAL_AVOIDANCE_TURN_RATE;
					_finalSteeringCommand.targetSpeed = (comfortZoneViolated) ? PED_SLOWER_SPEED_FACTOR * _currentGoal.desiredSpeed : PED_TYPICAL_SPEED_FACTOR * _currentGoal.desiredSpeed;
				}
				else {
					// choose speed
					SpatialDatabaseItemPtr obstacle = (feelers.object_front && !feelers.object_front->isAgent()) ? feelers.object_front : (feelers.object_right && !feelers.object_right->isAgent()) ? feelers.object_right : feelers.object_left;

					if ((feelers.object_left)&&(feelers.object_left->isAgent())) {
						float tempVelocity = dot(forward(),(dynamic_cast<PPRAgent*>(feelers.object_left))->velocity());
						_finalSteeringCommand.targetSpeed = min(_finalSteeringCommand.targetSpeed,tempVelocity);
					}
					if ((feelers.object_right)&&(feelers.object_right->isAgent())) {
						float tempVelocity = dot(forward(),(dynamic_cast<PPRAgent*>(feelers.object_right))->velocity());
						_finalSteeringCommand.targetSpeed = min(_finalSteeringCommand.targetSpeed,tempVelocity);
					}
					if ((feelers.object_front)&&(feelers.object_front->isAgent())) {
						float tempVelocity = dot(forward(),(dynamic_cast<PPRAgent*>(feelers.object_front))->velocity());
						_finalSteeringCommand.targetSpeed = min(_finalSteeringCommand.targetSpeed,tempVelocity);
					}
					if (comfortZoneViolated) {
						_finalSteeringCommand.targetSpeed = 0.7f * _finalSteeringCommand.targetSpeed;
					}

					if (_finalSteeringCommand.targetSpeed < 0.0f) _finalSteeringCommand.targetSpeed = 0.0f;

					// choose turning
					if (obstacle == feelers.object_right) {
						_finalSteeringCommand.aimForTargetDirection = false;
						_finalSteeringCommand.turningAmount = (comfortZoneViolated) ? -PED_FASTER_AVOIDANCE_TURN_RATE : -PED_TYPICAL_AVOIDANCE_TURN_RATE;
					}
					else if (obstacle == feelers.object_left) {
						_finalSteeringCommand.aimForTargetDirection = false;
						_finalSteeringCommand.turningAmount = (comfortZoneViolated) ? PED_FASTER_AVOIDANCE_TURN_RATE : PED_TYPICAL_AVOIDANCE_TURN_RATE;
					}
					else {
						// weird, only hit the static object in the middle, its either a small obstacle, or a corner.
						// TODO: is this the right thing to do here?
						_finalSteeringCommand.aimForTargetDirection = true;
						_finalSteeringCommand.turningAmount = PED_CORNERING_TURN_RATE;
						_finalSteeringCommand.targetSpeed = 0.0f;
					}
				}
			}
			else if (reactiveSituation == REACTIVE_SITUATION_UNKNOWN) {
				// should not reach here in code ???
				//if (isSelected()) cerr << "REACTION: unknown situation...\n";
				std::cerr << "TODO: case REACTIVE_SITUATION_UNKNOWN not implemented yet\n";
			}
			else {
				std::cerr << "INTERNAL ERROR: should not have reached this place in code, in reactive phase\n";
				assert(false);
			}

			if (foundNewThreat || existingThreatRaisedAgain) _timeToWait = _currentTimeStamp + 0.1f; // effectively "reset" any predicted threats, because a more imminent threat suddenly appeared.

		}
	}
	//========================
	else if (_steeringState == STEERING_STATE_TURN_TOWARDS_TARGET) {
		if ((dot(_finalSteeringCommand.targetDirection,forward()) > PED_SAME_DIRECTION_DOT_PRODUCT_THRESHOLD)) {
			_steeringState = STEERING_STATE_NO_THREAT;
		}
		_finalSteeringCommand.aimForTargetDirection = true;
		_finalSteeringCommand.turningAmount = PED_CORNERING_TURN_RATE;
		_finalSteeringCommand.aimForTargetSpeed = true;
		_finalSteeringCommand.targetSpeed = _currentGoal.desiredSpeed;
	}
	//========================
	else if (_steeringState == STEERING_STATE_COOPERATE_WITH_CROWD) {
		// do the following:
		// aim for the "average aggregate" crowd direction
		// if that direction is the same direction as your goal, then great, no problem
		// if its opposite from your goal (e.g. goal steering would be to the left, crowd direction is to the right)
		// then slow down while turning gradually towards goal.


		// note, crowdControldDirection is not normalized (at least when this comment was written)
		// it does not need to be, if we ae only checking for positive/negative.
		float posneg = (dot(_crowdControlDirection,_rightSide)) * (dot(_finalSteeringCommand.targetDirection,_rightSide));
		if (posneg < 0.0f) {
			// opposite sides of your direction
			_finalSteeringCommand.aimForTargetDirection = true;
			// targetDirection already initialized to point towards the local target location
			_finalSteeringCommand.turningAmount = PED_ADJUSTMENT_TURN_RATE;
			_finalSteeringCommand.aimForTargetSpeed = true;
			_finalSteeringCommand.targetSpeed = PED_TYPICAL_SPEED_FACTOR*_currentGoal.desiredSpeed;
		}
		else {
			// same side, so steer with the crowd.
			_finalSteeringCommand.aimForTargetDirection = true;
			_finalSteeringCommand.targetDirection = normalize(_crowdControlDirection);
			_finalSteeringCommand.turningAmount = PED_ADJUSTMENT_TURN_RATE;
			_finalSteeringCommand.aimForTargetSpeed = true;
			_finalSteeringCommand.targetSpeed = PED_TYPICAL_SPEED_FACTOR*_currentGoal.desiredSpeed;
		}


	}
	//========================
	else if (_steeringState == STEERING_STATE_PROACTIVELY_AVOID) {
		if (_threatList[_mostImminentThreatIndex].threatType == PredictedThreat::THREAT_TYPE_ONCOMING) {
			if (_threatList[_mostImminentThreatIndex].oncomingToRightSide) {
				_finalSteeringCommand.aimForTargetDirection = false;
				_finalSteeringCommand.turningAmount = -PED_ADJUSTMENT_TURN_RATE;
			}
			else {
				_finalSteeringCommand.aimForTargetDirection = false;
				_finalSteeringCommand.turningAmount = PED_ADJUSTMENT_TURN_RATE;
			}
		}
		else if (_threatList[_mostImminentThreatIndex].threatType == PredictedThreat::THREAT_TYPE_CROSSING_SOON) {
			// action: steer away and speed up
			// do a dot product between the side vector and dirToOtherGuy to determine if its on your right side or left side
			//					if ( (dot(_rightSide),rayToOtherGuy.dir) > 0.0f) {
			PPRAgent * threatGuy = _threatList[_mostImminentThreatIndex].threatGuy;//((Pedestrian*)(imminentThreat->object));
			if ( dot((_rightSide),threatGuy->forward()) < 0.0f) {
				_finalSteeringCommand.aimForTargetSpeed = true;
				_finalSteeringCommand.targetSpeed = PED_SLIGHTLY_FASTER_SPEED_FACTOR*_currentGoal.desiredSpeed;
				_finalSteeringCommand.aimForTargetDirection = false;
				_finalSteeringCommand.turningAmount = -PED_TYPICAL_AVOIDANCE_TURN_RATE;
			}
			else {
				_finalSteeringCommand.aimForTargetSpeed = true;
				_finalSteeringCommand.targetSpeed = PED_SLIGHTLY_FASTER_SPEED_FACTOR*_currentGoal.desiredSpeed;
				_finalSteeringCommand.aimForTargetDirection = false;
				_finalSteeringCommand.turningAmount = PED_TYPICAL_AVOIDANCE_TURN_RATE;
			}
		}
		else if (_threatList[_mostImminentThreatIndex].threatType == PredictedThreat::THREAT_TYPE_CROSSING_LATE) {
			// action: steer towards the other agent and slow down
			//					if ( dot((_rightSide),rayToOtherGuy.dir) > 0.0f) {
			PPRAgent * threatGuy = _threatList[_mostImminentThreatIndex].threatGuy;//((Pedestrian*)(imminentThreat->object));
			if ( dot((_rightSide),threatGuy->forward()) < 0.0f) {
				_finalSteeringCommand.aimForTargetSpeed = true;
				_finalSteeringCommand.targetSpeed = PED_SLIGHTLY_SLOWER_SPEED_FACTOR*_currentGoal.desiredSpeed;
				_finalSteeringCommand.aimForTargetDirection = false;
				_finalSteeringCommand.turningAmount = PED_TYPICAL_AVOIDANCE_TURN_RATE;
			}
			else {
				_finalSteeringCommand.aimForTargetSpeed = true;
				_finalSteeringCommand.targetSpeed = PED_SLIGHTLY_SLOWER_SPEED_FACTOR*_currentGoal.desiredSpeed;
				_finalSteeringCommand.aimForTargetDirection = false;
				_finalSteeringCommand.turningAmount = -PED_TYPICAL_AVOIDANCE_TURN_RATE;
			}
		}
		else {
			std::cerr << "ERROR: invalid threatType specified: " << ((int)_threatList[_mostImminentThreatIndex].threatType) << "\n";
		}
	}
	//========================
	else if (_steeringState == STEERING_STATE_WAIT_UNTIL_CLEAR) {
		_finalSteeringCommand.aimForTargetSpeed = false;
		_finalSteeringCommand.acceleration = 0.0f;
		_finalSteeringCommand.aimForTargetDirection = false;
		_finalSteeringCommand.turningAmount = 0.0f;
	}
	//========================
	else if (_steeringState == STEERING_STATE_NO_THREAT) {
		// steer towards the local target node
		float td = dot(_finalSteeringCommand.targetDirection,forward());
		if ((td > PED_WRONG_DIRECTION_DOT_PRODUCT_THRESHOLD) && (td < PED_SIMILAR_DIRECTION_DOT_PRODUCT_THRESHOLD)) {
			_finalSteeringCommand.turningAmount = PED_FASTER_AVOIDANCE_TURN_RATE;
		}
		else {
			_finalSteeringCommand.turningAmount = PED_ADJUSTMENT_TURN_RATE;
		}
		_finalSteeringCommand.aimForTargetDirection = true;
		_finalSteeringCommand.aimForTargetSpeed = true;
		_finalSteeringCommand.targetSpeed = PED_TYPICAL_SPEED_FACTOR*_currentGoal.desiredSpeed;
	}


	_finalSteeringCommand.steeringMode = SteeringCommand::LOCOMOTION_MODE_COMMAND;

	if (gUseDynamicPhaseScheduling) {
	
		// potentially give a break to perception
		if (hitSomething) {
			_framesToNextPerceptivePhase = 100;
		}


		// currently we do not dynamically decimate predictions dynamically, just keep it at 6 frames;
		_framesToNextPredictivePhase = 6;


		// decimating reactions
		if (hitSomething) {
			_framesToNextReactivePhase = 1;
		}
		else if (_neighbors.size() == 0) {
			_framesToNextReactivePhase = 4;
		}
		else if (_threatList.size() == 0) {
			_framesToNextReactivePhase = 2;
		} else {
			_framesToNextReactivePhase = 1;
		}
	}

	return;
}


//
// doSteering()
//
void PPRAgent::doSteering()
{
	if ((!_enabled) || (_landmarkQueue.empty())) return;

	AutomaticFunctionProfiler profileThisFunction( &PPRGlobals::gPhaseProfilers->steeringPhaseProfiler );

	switch ( _finalSteeringCommand.steeringMode) {
		case SteeringCommand::LOCOMOTION_MODE_COMMAND:
			doCommandBasedSteering();
			break;
		case SteeringCommand::LOCOMOTION_MODE_DYNAMICS:
			doDynamicsSteering();
			break;
		case SteeringCommand::LOCOMOTION_MODE_SPACETIMEPATH:
			throw GenericException("Support for following an exact space-time path is not provided yet.");
			break;
		default:
			std::cerr << "ERROR: invalid steeringMode specified!\n";
			assert(false);
			break;
	}
}


//
// doEulerStepWithForce() - updates _position, _velocity, and _currentSpeed, given force and the _dt time step.
//
void PPRAgent::doEulerStepWithForce(const Vector & force)
{
	// compute acceleration and velocity by a simple Euler step
	const Vector clippedForce = clamp(force, _maxForce);
	Vector acceleration = (clippedForce / _mass);
	_velocity = _velocity + (_dt*acceleration);
	_velocity = clamp(_velocity, _maxSpeed);  // clamp _velocity to the max speed
	_currentSpeed = _velocity.length();
	const Point newPosition = _position + (_dt*_velocity);


	// update the database with the new agent's setup
	AxisAlignedBox oldBounds = AxisAlignedBox(_position.x - _radius, _position.x + _radius, 0.0f, 0.0f, _position.z - _radius, _position.z + _radius);
	AxisAlignedBox newBounds = AxisAlignedBox(newPosition.x - _radius, newPosition.x + _radius, 0.0f, 0.0f, newPosition.z - _radius, newPosition.z + _radius);
	gSpatialDatabase->updateObject( dynamic_cast<SpatialDatabaseItemPtr>(this), oldBounds, newBounds);

	_position = newPosition;
}



//
// doDynamicsSteering()
//
void PPRAgent::doDynamicsSteering()
{
	doEulerStepWithForce(_finalSteeringCommand.dynamicsSteeringForce);
#ifdef USE_ANNOTATIONS
	__plannedSteeringForce = _finalSteeringCommand.dynamicsSteeringForce;
#endif
}


//
// doCommandBasedSteering()
//
void PPRAgent::doCommandBasedSteering()
{

	// TODO: define/initialize these vars properly:
	Vector totalSteeringForce;
	Vector newForward;

	//
	// choose the new orientation of the agent
	//
	if (!_finalSteeringCommand.aimForTargetDirection) {
		// simple turning case "turn left" or "turn right"
		newForward = _forward + PED_MAX_TURNING_RATE * _finalSteeringCommand.turningAmount * _rightSide;
	}
	else {
		// turn to face "targetDirection" - magnitude of targetDirection doesn't matter
		float initialDot = dot(_finalSteeringCommand.targetDirection,_rightSide);
		float turningRate = (initialDot > 0.0f) ? PED_MAX_TURNING_RATE : -PED_MAX_TURNING_RATE;  // positive rate is right-turn
		newForward = _forward + turningRate * fabsf(_finalSteeringCommand.turningAmount) * _rightSide;
		float newDot = dot(_finalSteeringCommand.targetDirection, rightSideInXZPlane(newForward)); // dot with the new side vector
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
			newForward = _finalSteeringCommand.targetDirection;
		}
	}

	//
	// set the orientation
	//
	newForward = normalize(newForward);
	_forward = newForward;
	_rightSide = rightSideInXZPlane(newForward);

	// This next line is specific to command-based steering, but is not physically based.
	// everything else in command-based steering, however, is physcially based.
	_velocity = newForward * _currentSpeed;


	//
	// choose the force of the agent.  In command-based mode, the force is always aligned 
	// with the agent's forward facing direction, so we can use scalars until we add 
	// side-to-side scoot at the end.
	//
	assert(fabsf(_finalSteeringCommand.acceleration) <= 1.0f); // -1.0f <= acceleration <= 1.0f;
	if (!_finalSteeringCommand.aimForTargetSpeed) {
		// simple "speed up" or "slow down"
		totalSteeringForce = _maxForce * _finalSteeringCommand.acceleration * _forward;
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
		float maxBackwardsForce = (-PED_BRAKING_RATE * fabsf(_currentSpeed) * _mass / _dt);
		float scalarForce = (_finalSteeringCommand.targetSpeed - _currentSpeed) * 8.0f; // crudely trying to make accelerations quicker...
		if (scalarForce > _maxForce) scalarForce = _maxForce;
		if (scalarForce < maxBackwardsForce) scalarForce = maxBackwardsForce;
		totalSteeringForce = scalarForce * _forward; // forward is a unit vector, normalized during turning just above.
	}

	// TODO: should we clamp scoot?
	// add the side-to-side motion to the planned steering force.
	totalSteeringForce = totalSteeringForce + PED_SCOOT_RATE * _finalSteeringCommand.scoot * _rightSide;

	doEulerStepWithForce(totalSteeringForce);

#ifdef USE_ANNOTATIONS
	__plannedSteeringForce = totalSteeringForce;
#endif

}


//
// collectObjectsInVisualField()
//
void PPRAgent::collectObjectsInVisualField()
{
	_neighbors.clear();
	gSpatialDatabase->getItemsInVisualField(_neighbors, _position.x-PED_QUERY_RADIUS, _position.x+PED_QUERY_RADIUS, 
		_position.z-PED_QUERY_RADIUS, _position.z+PED_QUERY_RADIUS, dynamic_cast<SpatialDatabaseItemPtr>(this), 
		_position, _forward, (float)(PED_QUERY_RADIUS*PED_QUERY_RADIUS));
}


//
// reachedCurrentGoal()
//
bool PPRAgent::reachedCurrentGoal()
{
	return ( (_currentGoal.targetLocation-_position).lengthSquared() < (PED_REACHED_TARGET_DISTANCE_THRESHOLD * PED_REACHED_TARGET_DISTANCE_THRESHOLD) );
}


//
// reachedCurrentWaypoint()
//
bool PPRAgent::reachedCurrentWaypoint()
{
	return ( (_waypoints[_currentWaypointIndex]-_position).lengthSquared() < (PED_REACHED_TARGET_DISTANCE_THRESHOLD * PED_REACHED_TARGET_DISTANCE_THRESHOLD) );
}



//
// reachedLocalTarget()
//
bool PPRAgent::reachedLocalTarget()
{
	return ( (_localTargetLocation-_position).lengthSquared() < (PED_REACHED_TARGET_DISTANCE_THRESHOLD * PED_REACHED_TARGET_DISTANCE_THRESHOLD) );
}


//
// threatListContainsAgent()
//
bool PPRAgent::threatListContainsAgent(PPRAgent * agent, unsigned int & index)
{
	for (unsigned int i=0; i < _threatList.size(); i++) {
		if (_threatList[i].threatGuy == agent) {
			index = i;
			return true;
		}
	}
	return false;
}


//
// 	updateReactiveFeelers()
//
bool PPRAgent::updateReactiveFeelers( FeelerInfo & feelers )
{
	// TODO: would it be faster to use one ray and just keep re-initializing it?

	Ray myRay, myLeftRay, myRightRay, myLSideRay, myRSideRay;

	feelers.clear();

	myRay.initWithLengthInterval(_position, _forward * (PED_TYPICAL_SPEED*PED_REACTIVE_ANTICIPATION_FACTOR));
	myRightRay.initWithLengthInterval( _position + _radius * _rightSide,  (_forward + 0.1f*_rightSide)* (PED_TYPICAL_SPEED*PED_REACTIVE_ANTICIPATION_FACTOR));
	myLeftRay.initWithLengthInterval( _position - _radius * _rightSide,  (_forward - 0.1f*_rightSide)* (PED_TYPICAL_SPEED*PED_REACTIVE_ANTICIPATION_FACTOR));
	myRSideRay.initWithLengthInterval( _position + _radius * _rightSide,  (0.05f * _forward + 0.1f * _rightSide)* (PED_TYPICAL_SPEED*PED_REACTIVE_ANTICIPATION_FACTOR));
	myLSideRay.initWithLengthInterval( _position - _radius * _rightSide,  (0.05f * _forward - 0.1f * _rightSide)* (PED_TYPICAL_SPEED*PED_REACTIVE_ANTICIPATION_FACTOR));

	SpatialDatabaseItemPtr me = dynamic_cast<SpatialDatabaseItemPtr>(this);
	gSpatialDatabase->trace(myRay,      feelers.t_front, feelers.object_front, me, false);
	gSpatialDatabase->trace(myRightRay, feelers.t_right, feelers.object_right, me, false);
	gSpatialDatabase->trace(myLeftRay,  feelers.t_left,  feelers.object_left,  me, false);
	gSpatialDatabase->trace(myRSideRay, feelers.t_rside, feelers.object_rside, me, false);
	gSpatialDatabase->trace(myLSideRay, feelers.t_lside, feelers.object_lside, me, false);

#ifdef USE_ANNOTATIONS
	__myRay = myRay;
	__myRightRay = myRightRay;
	__myLeftRay = myLeftRay;
	__myRSideRay = myRSideRay;
	__myLSideRay = myLSideRay;
	__front = (feelers.object_front != NULL);
	__rightFront = (feelers.object_right != NULL);
	__leftFront = (feelers.object_left != NULL);
	__hitSomething = (__front || __rightFront || __leftFront);
	if (__hitSomething) {
		__hitPosFront = myRay.pos + feelers.t_front * myRay.dir;
		__hitPosRight = myRightRay.pos + feelers.t_right * myRightRay.dir;
		__hitPosLeft = myLeftRay.pos + feelers.t_left * myLeftRay.dir;
	}
#endif

	if (feelers.object_front || feelers.object_right || feelers.object_left) {
		return true;
	}

	return false;
}


//
// disable()
//
void PPRAgent::disable()
{
	// if we tried to disable a second time, most likely we accidentally ignored that it was disabled, and should catch that error.
	assert(_enabled==true);  

	//  1. remove from database
	AxisAlignedBox b = AxisAlignedBox(_position.x - _radius, _position.x + _radius, 0.0f, 0.0f, _position.z - _radius, _position.z + _radius);
	gSpatialDatabase->removeObject(dynamic_cast<SpatialDatabaseItemPtr>(this), b);

	//  2. set enabled = false
	_enabled = false;
}


void PPRAgent::updateAgentState(const Util::Point & newPosition,  const Util::Vector & newOrientation, float newSpeed)
{
	_forward = normalize(newOrientation);
	_rightSide = rightSideInXZPlane(_forward);
	_currentSpeed = newSpeed;
	_velocity = _forward * newSpeed;

	// update the database with the new agent's setup
	Util::AxisAlignedBox oldBounds = Util::AxisAlignedBox(_position.x - _radius, _position.x + _radius, 0.0f, 0.0f, _position.z - _radius, _position.z + _radius);
	Util::AxisAlignedBox newBounds = Util::AxisAlignedBox(newPosition.x - _radius, newPosition.x + _radius, 0.0f, 0.0f, newPosition.z - _radius, newPosition.z + _radius);
	gSpatialDatabase->updateObject( dynamic_cast<SpatialDatabaseItemPtr>(this), oldBounds, newBounds);

	_position = newPosition;

}

//
// drawPlannedPath()
//
void PPRAgent::drawPlannedPath()
{
#ifdef ENABLE_GUI
#ifdef USE_ANNOTATIONS
#ifndef IGNORE_PLANNING

	
	// draw long-term path line
	if (longTermPath.size() > 0) {
		for (unsigned int i=0; i < longTermPath.size() - 1; i++) {
			Vector xOffset,zOffset;
			Point center,nextCenter;
			xOffset.x = 0.5f * gSpatialDatabase->getCellSizeX();
			zOffset.z = 0.5f * gSpatialDatabase->getCellSizeZ();
			gSpatialDatabase->getLocationFromIndex(longTermPath._Get_container()[i], center);
			gSpatialDatabase->getLocationFromIndex(longTermPath._Get_container()[i+1], nextCenter);
			center.y = 0.01f;
			nextCenter.y = 0.01f;
			DrawLib::glColor(gDarkBlue);
			DrawLib::drawLine(center, nextCenter);
		}
	}
	
	
	// draw markers on each waypoint
	for(unsigned int i=0; i<_waypoints.size(); i++) {
		DrawLib::drawStar(_waypoints[i] + Vector(0.0f, 0.005f, 0.0f), Vector(1.0f, 0.0f, 0.0f), 0.15f, gGreen);
	}

	// draw mid-term path line
	if (_midTermPathSize > 0) {
		for (unsigned int i=0; i < _midTermPathSize - 1; i++) {
			Vector xOffset,zOffset;
			Point center,nextCenter;
			xOffset.x = 0.5f * gSpatialDatabase->getCellSizeX();
			zOffset.z = 0.5f * gSpatialDatabase->getCellSizeZ();
			gSpatialDatabase->getLocationFromIndex(_midTermPath[i], center);
			gSpatialDatabase->getLocationFromIndex(_midTermPath[i+1], nextCenter);
			center.y = 0.02f;
			nextCenter.y = 0.02f;
			DrawLib::glColor(gBlue);
			DrawLib::drawLine( center, nextCenter);
		}
	}

	// draw a marker on the closest node you are to the mid-term path (computed from short-term planning)
	Point closestNodeOnPath;
	gSpatialDatabase->getLocationFromIndex(_midTermPath[__closestPathNode],closestNodeOnPath);
	//DrawLib::drawHighlight(closestNodeOnPath, Vector(1.0f, 0.0f, 0.0f), 0.5f, gBlue);
	//drawXZCircle(0.30f, closestNodeOnPath, gBlue, 10);

#endif  // #ifndef IGNORE_PLANNING

	DrawLib::glColor(gWhite);
	DrawLib::drawLine(_position, _localTargetLocation);
	DrawLib::drawStar(_localTargetLocation, Vector(1.0f, 0.0f, 0.0f), 0.22f, gGray80);

#endif // ifdef USE_ANNOTATIONS
#endif // ifdef ENABLE_GUI
}


//
// draw()
//
void PPRAgent::draw()
{
#ifdef ENABLE_GUI
	if (!_enabled) return;
	AutomaticFunctionProfiler profileThisFunction( &PPRGlobals::gPhaseProfilers->drawProfiler );


#ifndef USE_ANNOTATIONS
	DrawLib::drawAgentDisc(_position, _forward, _radius); 
	if (_currentGoal.goalType == SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET) {
		DrawLib::drawFlag(_currentGoal.targetLocation);
	}
#else
	if (_steeringState == STEERING_STATE_NO_THREAT) {
		DrawLib::drawAgentDisc(_position, _forward, _radius, gGray50); 
	}
	else if (_steeringState == STEERING_STATE_PROACTIVELY_AVOID) {
		DrawLib::drawAgentDisc(_position, _forward, _radius, gYellow); 
	}
	else if (_steeringState == STEERING_STATE_WAIT_UNTIL_CLEAR) {
		DrawLib::drawAgentDisc(_position, _forward, _radius, gDarkYellow); 
	}
	else if (_steeringState == STEERING_STATE_TURN_TOWARDS_TARGET) {
		//DrawLib::drawAgentDisc(_position, _forward, _radius, gBlue); 
		DrawLib::drawStar(_position, _forward, _radius, gBlue);
	}
	else if (_steeringState == STEERING_STATE_COOPERATE_WITH_CROWD) {
		DrawLib::drawAgentDisc(_position, _forward, _radius, gGreen); 
	}
	else if (_steeringState == STEERING_STATE_STATIONARY_OBJECT) {
		DrawLib::drawAgentDisc(_position, _forward, _radius, gMagenta); 
	}
	else if (_steeringState == STEERING_STATE_FOLLOW_SPACETIME_PATH) {
		DrawLib::drawAgentDisc(_position, _forward, _radius, gMagenta); 
	}

	if (_currentGoal.goalType == SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET) {
		DrawLib::drawFlag(_currentGoal.targetLocation);
	}

	DrawLib::drawStar(_startTargetPosition, Vector(1.0f, 0.0f, 0.0f), 0.20f, gYellow);

	Vector verticalOffset = Vector(0.0f, 0.03f, 0.0f);

	float speedRatio = _currentSpeed / PED_TYPICAL_SPEED;
	__myPredictorRay.initWithUnitInterval(__myRay.pos + verticalOffset, __myRay.dir*__myRay.maxt*speedRatio);
	__myPredictorRightRay.initWithUnitInterval(__myRightRay.pos + verticalOffset, __myRightRay.dir*__myRightRay.maxt*speedRatio);
	__myPredictorLeftRay.initWithUnitInterval(__myLeftRay.pos + verticalOffset, __myLeftRay.dir*__myLeftRay.maxt*speedRatio);

	DrawLib::glColor(gGray50);
	DrawLib::drawLine(__myRay.pos + verticalOffset, __myRay.pos + verticalOffset + (__myRay.dir * __myRay.maxt));
	DrawLib::drawLine(__myRightRay.pos + verticalOffset, __myRightRay.pos + verticalOffset + (__myRightRay.dir * __myRightRay.maxt));
	DrawLib::drawLine(__myLeftRay.pos + verticalOffset, __myLeftRay.pos + verticalOffset + (__myLeftRay.dir * __myLeftRay.maxt));

	DrawLib::glColor(gGray90);
	DrawLib::drawLine(__myPredictorRay.pos + verticalOffset, __myPredictorRay.pos + verticalOffset + (__myPredictorRay.dir * __myPredictorRay.maxt));
	DrawLib::drawLine(__myPredictorRightRay.pos + verticalOffset, __myPredictorRightRay.pos + verticalOffset + (__myPredictorRightRay.dir * __myPredictorRightRay.maxt));
	DrawLib::drawLine(__myPredictorLeftRay.pos + verticalOffset, __myPredictorLeftRay.pos + verticalOffset + (__myPredictorLeftRay.dir * __myPredictorLeftRay.maxt));

	DrawLib::drawLine(__myRSideRay.pos + verticalOffset, __myRSideRay.pos + verticalOffset + (__myRSideRay.dir * __myRSideRay.maxt));
	DrawLib::drawLine(__myLSideRay.pos + verticalOffset, __myLSideRay.pos + verticalOffset + (__myLSideRay.dir * __myLSideRay.maxt));

	//DrawLib::drawLine(_position, _position+(__plannedSteeringForce), gGreen);
	DrawLib::glColor(gGreen);
	DrawLib::drawLine(_position, _position + _desiredForward * 3.0f);
	//DrawLib::glColor(gYellow);
	//DrawLib::drawLine(_position, _position + _magnifiedDesiredForward * 3.0f);


	if (__hitSomething) {
		if (__front) DrawLib::drawStar(__hitPosFront + 2*verticalOffset, Vector(1.0f, 0.0f, 0.0f), 0.10f, gRed);
		if (__rightFront) DrawLib::drawStar(__hitPosRight + 2*verticalOffset, Vector(1.0f, 0.0f, 0.0f), 0.10f, gRed);
		if (__leftFront)  DrawLib::drawStar(__hitPosLeft + 2*verticalOffset, Vector(1.0f, 0.0f, 0.0f), 0.10f, gRed);
	}

	if (isSelected()) {
		DrawLib::glColor(gRed);
		for (std::set<SteerLib::SpatialDatabaseItemPtr>::iterator neighbor = _neighbors.begin(); neighbor != _neighbors.end(); ++neighbor) {
		//for (unsigned int i=0; i<_neighbors.size(); i++) {
			if ((*neighbor)->isAgent()) DrawLib::drawLine(_position + verticalOffset, AGENT_PTR((*neighbor))->position() + verticalOffset);
		}
		drawPlannedPath();

		// draw lines from sides to local goal:
		//Vector temp = normalize(_localTargetLocation - _position);
		//Vec3 perpendicular = rightSideInXZPlane(temp);
		//drawLine(_position+_radius*perpendicular, _position+_radius*perpendicular + _localTargetLocation - _position, gWhite);
		//drawLine(_position-_radius*perpendicular, _position-_radius*perpendicular + _localTargetLocation - _position, gWhite);

		// highlight everyone on your threatlist.
		for (unsigned int i=0; i<_threatList.size(); i++) {
			DrawLib::drawStar((_threatList[i].threatGuy)->position() + verticalOffset,Vector(1.0f, 0.0f, 0.0f), 1.15f, gGreen);
			if (_mostImminentThreatIndex != -1) DrawLib::drawFlag(_threatList[_mostImminentThreatIndex].threatGuy->position());
		}
	}
#endif  // ifndef USE_ANNOTATIONS
#endif  // ifdef ENABLE_GUI
}

#else
//
// constructor
//
PPRAgent::PPRAgent()
{
}



//
// destructor
//
PPRAgent::~PPRAgent()
{
}



//
// reset()
//
void PPRAgent::reset(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::EngineInterface * engineInfo)
{
}

void PPRAgent::setUseCollisionFreeGoals(bool val)
{
}

bool PPRAgent::isUseCollisionFreeGoals()
{
	return true;
}

//
// addGoal()
//
void PPRAgent::addGoal(const SteerLib::AgentGoalInfo & newGoal) 
{
}


//
// updateAI()
//
void PPRAgent::updateAI(float timeStamp, float dt, unsigned int frameNumber)
{
}


//
// runCognitivePhase()
//
void PPRAgent::runCognitivePhase()
{
}


//
// runLongTermPlanningPhase()
//
void PPRAgent::runLongTermPlanningPhase()
{
}


//
// runMidTermPlanningPhase()
//
void PPRAgent::runMidTermPlanningPhase()
{
}


//
// runShortTermPlanningPhase()
//
void PPRAgent::runShortTermPlanningPhase()
{
}


//
// runPerceptivePhase()
//
void PPRAgent::runPerceptivePhase()
{
}

//
// runPredictivePhase()
//
void PPRAgent::runPredictivePhase()
{
}


//
// runReactivePhase()
//
void PPRAgent::runReactivePhase()
{
}


//
// doSteering()
//
void PPRAgent::doSteering()
{
}


//
// doEulerStepWithForce() - updates _position, _velocity, and _currentSpeed, given force and the _dt time step.
//
void PPRAgent::doEulerStepWithForce(const Vector & force)
{
}



//
// doDynamicsSteering()
//
void PPRAgent::doDynamicsSteering()
{
}


//
// doCommandBasedSteering()
//
void PPRAgent::doCommandBasedSteering()
{
}


//
// collectObjectsInVisualField()
//
void PPRAgent::collectObjectsInVisualField()
{
}


//
// reachedCurrentGoal()
//
bool PPRAgent::reachedCurrentGoal()
{
	return true;
}


//
// reachedCurrentWaypoint()
//
bool PPRAgent::reachedCurrentWaypoint()
{
	return true;
}



//
// reachedLocalTarget()
//
bool PPRAgent::reachedLocalTarget()
{
	return true;
}


//
// threatListContainsAgent()
//
bool PPRAgent::threatListContainsAgent(PPRAgent * agent, unsigned int & index)
{
	return false;
}


//
// 	updateReactiveFeelers()
//
bool PPRAgent::updateReactiveFeelers( FeelerInfo & feelers )
{
	return false;
}


//
// disable()
//
void PPRAgent::disable()
{
}


void PPRAgent::updateAgentState(const Util::Point & newPosition,  const Util::Vector & newOrientation, float newSpeed)
{
}

//
// drawPlannedPath()
//
void PPRAgent::drawPlannedPath()
{
}


//
// draw()
//
void PPRAgent::draw()
{
}
#endif
