//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __PPR_AI_MODULE_H__
#define __PPR_AI_MODULE_H__

/// @file PPRAIModule.h
/// @brief Declares the PPRAIModule class.


#include "SteerLib.h"
#include "PPRAgent.h"

// forward declaration
class GridEnvironment;
class PerformanceProfiler;

namespace PPRGlobals {

	struct PhaseProfilers {
		Util::PerformanceProfiler aiProfiler;
		Util::PerformanceProfiler drawProfiler;
		Util::PerformanceProfiler longTermPhaseProfiler;
		Util::PerformanceProfiler midTermPhaseProfiler;
		Util::PerformanceProfiler shortTermPhaseProfiler;
		Util::PerformanceProfiler perceptivePhaseProfiler;
		Util::PerformanceProfiler predictivePhaseProfiler;
		Util::PerformanceProfiler reactivePhaseProfiler;
		Util::PerformanceProfiler steeringPhaseProfiler;
	};


	extern SteerLib::EngineInterface * gEngineInfo;
	extern SteerLib::GridDatabase2D * gSpatialDatabase;
	extern unsigned int gLongTermPlanningPhaseInterval;
	extern unsigned int gMidTermPlanningPhaseInterval;
	extern unsigned int gShortTermPlanningPhaseInterval;
	extern unsigned int gPredictivePhaseInterval;
	extern unsigned int gReactivePhaseInterval;
	extern unsigned int gPerceptivePhaseInterval;
	extern bool gUseDynamicPhaseScheduling;
	extern bool gShowStats;
	extern bool gShowAllStats;


	extern PhaseProfilers * gPhaseProfilers;
}

class PPRAIModule : public SteerLib::ModuleInterface
{
public:
	std::string getDependencies() { return "testCasePlayer"; }
	std::string getConflicts() { return ""; }
	void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo );
	void finish();
	SteerLib::AgentInterface * createAgent() { return new PPRAgent; }
	void destroyAgent( SteerLib::AgentInterface * agent ) { if (agent) delete agent;  agent = NULL; }

	void initializeSimulation();
	void cleanupSimulation();
};


#endif
