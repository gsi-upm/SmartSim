//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file SteerSuiteEngineDriver.cpp
/// @brief Implements the SteerSuiteEngineDriver functionality.
///
/// @todo
///   - update documentation in this file
///
#include "SteerSuiteEngineDriver.h"

#include <iostream>

using namespace std;
using namespace SteerLib;
using namespace Util;

//
// constructor
//
SteerSuiteEngineDriver::SteerSuiteEngineDriver() : SBObject()
{
	_alreadyInitialized = false;
	_engine = NULL;
	_done = false;
	_options = NULL;
	_startTime = 0;


}

SteerSuiteEngineDriver::~SteerSuiteEngineDriver()
{
	if (_engine)
		delete _engine;
	if (_options)
		delete _options;
}

bool SteerSuiteEngineDriver::isInitialized()
{
	return _alreadyInitialized;
}

bool SteerSuiteEngineDriver::isDone()
{
	return _done;
}
void SteerSuiteEngineDriver::setDone(bool val)
{
	_done = val;
}

void SteerSuiteEngineDriver::setStartTime(double time)
{
	_startTime = time;
}
double SteerSuiteEngineDriver::getStartTime()
{
	return _startTime;
}

void SteerSuiteEngineDriver::setLastUpdateTime(double time)
{
	_lastUpdateTime = time;
}
double SteerSuiteEngineDriver::getLastUpdateTime()
{
	return _lastUpdateTime;
}


//
// init()
//
void SteerSuiteEngineDriver::init(SteerLib::SimulationOptions * options)
{
	if (_alreadyInitialized) {
		throw GenericException("SteerSuiteEngineDriver::init() - should not call this function twice.\n");
	}

	_options = options;
	_alreadyInitialized = true;
	_done = false;

	_engine = new SimulationEngine();
	_engine->init(options, this);
}


//
// run() - never returns, will end the program properly when appropriate
//
void SteerSuiteEngineDriver::run()
{
	/*
	bool verbose = true;  // TODO: make this a user option...
	bool done = false;

	if (verbose) std::cout << "\rInitializing...\n";
	_engine->initializeSimulation();

	if (verbose) std::cout << "\rPreprocessing...\n";
	_engine->preprocessSimulation();

	// loop until the engine tells us its done
	while (!done) {
		if (verbose) std::cout << "\rFrame Number:   " << _engine->getClock().getCurrentFrameNumber();
		done = !_engine->update(false);
	}

	if (verbose) std::cout << "\rFrame Number:   " << _engine->getClock().getCurrentFrameNumber() << std::endl;

	if (verbose) std::cout << "\rPostprocessing...\n";
	_engine->postprocessSimulation();

	if (verbose) std::cout << "\rCleaning up...\n";
	_engine->cleanupSimulation();

	if (verbose) std::cout << "\rDone.\n";
	*/
}

//
// finish() - cleans up.
//
void SteerSuiteEngineDriver::finish()
{
	_engine->finish();
	delete _engine;
	_engine = NULL;
	_alreadyInitialized = false;
}


void SteerSuiteEngineDriver::loadSimulation()
{
	_engine->initializeSimulation();
	std::cout << "Simulation loaded.\n";
}

void SteerSuiteEngineDriver::startSimulation()
{
	std::cout << "Simulation started.\n";
	_engine->preprocessSimulation();
}

void SteerSuiteEngineDriver::stopSimulation()
{
	_engine->postprocessSimulation();
	std::cout << "Simulation stopped.\n";
}

void SteerSuiteEngineDriver::unloadSimulation()
{
	_engine->cleanupSimulation();
	std::cout << "Simulation unloaded.\n";
}

float SteerSuiteEngineDriver::collisionPenetration( SrVec pos, float radius, SteerLib::AgentInterface* agent )
{
	std::set<SpatialDatabaseItemPtr> neighborList;
	std::set<SpatialDatabaseItemPtr>::iterator neighbor;
	// exclude the current agent
	_engine->getSpatialDatabase()->getItemsInRange(neighborList, pos.x-radius,pos.x+radius,pos.z-radius,pos.z+radius, agent);
	Util::Point steerPt = Util::Point(pos.x , 0.0f, pos.z);
	float maxPenetration = 0.f;
	for (neighbor = neighborList.begin(); neighbor != neighborList.end(); ++neighbor) {

		// this way, collisionKey will be unique across all objects in the spatial database.
		//unsigned int collisionKey = reinterpret_cast<unsigned int>((*neighbor));
		SpatialDatabaseItemPtr item = (*neighbor);
		intptr_t collisionKey = reinterpret_cast<intptr_t>(item);
		float penetration = 0.0f;
		penetration = (*neighbor)->computePenetration(steerPt, radius);
		if (penetration > maxPenetration)
		{
			maxPenetration = penetration;						
		}
	}
	return maxPenetration;
}


