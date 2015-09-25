//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#include "SteerLib.h"
#include "SimulationPlugin.h"
#include "PPRAIModule.h"
#include "PPRAgent.h"



#define LONG_TERM_PLANNING_INTERVAL    10000
#define MID_TERM_PLANNING_INTERVAL     10000
#define SHORT_TERM_PLANNING_INTERVAL   1
#define PERCEPTIVE_PHASE_INTERVAL      1
#define PREDICTIVE_PHASE_INTERVAL      1
#define REACTIVE_PHASE_INTERVAL        1


using namespace Util;
using namespace SteerLib;



// todo: make these static?
namespace PPRGlobals {
	SteerLib::EngineInterface * gEngineInfo;
	SteerLib::GridDatabase2D * gSpatialDatabase;
	unsigned int gLongTermPlanningPhaseInterval;
	unsigned int gMidTermPlanningPhaseInterval;
	unsigned int gShortTermPlanningPhaseInterval;
	unsigned int gPredictivePhaseInterval;
	unsigned int gReactivePhaseInterval;
	unsigned int gPerceptivePhaseInterval;

	bool gUseDynamicPhaseScheduling;
	bool gShowStats;
	bool gShowAllStats;
	
	PhaseProfilers * gPhaseProfilers;
}

using namespace PPRGlobals;

//
// 
//
void PPRAIModule::init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo )
{
	gSpatialDatabase = engineInfo->getSpatialDatabase();

	gEngineInfo = engineInfo;


	gLongTermPlanningPhaseInterval = LONG_TERM_PLANNING_INTERVAL;
	gMidTermPlanningPhaseInterval = MID_TERM_PLANNING_INTERVAL;
	gShortTermPlanningPhaseInterval = SHORT_TERM_PLANNING_INTERVAL;
	gPerceptivePhaseInterval = PERCEPTIVE_PHASE_INTERVAL;
	gPredictivePhaseInterval = PREDICTIVE_PHASE_INTERVAL;
	gReactivePhaseInterval = REACTIVE_PHASE_INTERVAL;
	gUseDynamicPhaseScheduling = false;
	gShowStats = false;
	gShowAllStats = false;

	SteerLib::OptionDictionary::const_iterator optionIter;
	for (optionIter = options.begin(); optionIter != options.end(); ++optionIter) {
		std::stringstream value((*optionIter).second);
		if ((*optionIter).first == "longplan") {
			value >> gLongTermPlanningPhaseInterval;
		}
		else if ((*optionIter).first == "midplan") {
			value >> gMidTermPlanningPhaseInterval;
		}
		else if ((*optionIter).first == "shortplan") {
			value >> gShortTermPlanningPhaseInterval;
		}
		else if ((*optionIter).first == "perceptive") {
			value >> gPerceptivePhaseInterval;
		}
		else if ((*optionIter).first == "predictive") {
			value >> gPredictivePhaseInterval;
		}
		else if ((*optionIter).first == "reactive") {
			value >> gReactivePhaseInterval;
		}
		else if ((*optionIter).first == "dynamic") {
			gUseDynamicPhaseScheduling = Util::getBoolFromString(value.str());
		}
		else if ((*optionIter).first == "stats") {
			gShowStats = Util::getBoolFromString(value.str());
		}
		else if ((*optionIter).first == "allstats") {
			gShowAllStats = Util::getBoolFromString(value.str());
		}
		else {
			throw Util::GenericException("unrecognized option \"" + Util::toString((*optionIter).first) + "\" given to PPR AI module.");
		}
	}


	std::cout << std::endl;
	if (!gUseDynamicPhaseScheduling) {
		std::cout << " PHASE INTERVALS (in frames):\n";
		std::cout << "   longplan: " << gLongTermPlanningPhaseInterval << "\n";
		std::cout << "    midplan: " << gMidTermPlanningPhaseInterval << "\n";
		std::cout << "  shortplan: " << gShortTermPlanningPhaseInterval << "\n";
		std::cout << " perceptive: " << gPerceptivePhaseInterval << "\n";
		std::cout << " predictive: " << gPredictivePhaseInterval << "\n";
		std::cout << "   reactive: " << gReactivePhaseInterval << "\n";
	}
	else {
		std::cout << " PHASE INTERVALS (in frames):\n";
		std::cout << "   longplan: " << "on demand" << "\n";
		std::cout << "    midplan: " << "on demand" << "\n";
		std::cout << "  shortplan: " << "dynamic" << "\n";
		std::cout << " perceptive: " << "dynamic" << "\n";
		std::cout << " predictive: " << "dynamic" << "\n";
		std::cout << "   reactive: " << "dynamic" << "\n";
	}
	std::cout << std::endl;


	//
	// print a warning if we are using annotations with too many agents.
	//
#ifdef USE_ANNOTATIONS
	if (gEngineInfo->getAgents().size() > 30) {
		std::cerr << "WARNING: using annotations with a large number of agents will use a lot of memory and will be much slower." << std::endl;
	}
#endif

}


void PPRAIModule::initializeSimulation()
{
	//
	// initialize the performance profilers
	//
	gPhaseProfilers = new PhaseProfilers;
	gPhaseProfilers->aiProfiler.reset();
	gPhaseProfilers->longTermPhaseProfiler.reset();
	gPhaseProfilers->midTermPhaseProfiler.reset();
	gPhaseProfilers->shortTermPhaseProfiler.reset();
	gPhaseProfilers->perceptivePhaseProfiler.reset();
	gPhaseProfilers->predictivePhaseProfiler.reset();
	gPhaseProfilers->reactivePhaseProfiler.reset();
	gPhaseProfilers->steeringPhaseProfiler.reset();
	
}


//
// cleanupSimulation()
//
void PPRAIModule::cleanupSimulation()
{
	if (gShowStats || gShowAllStats) {
		if (gShowAllStats) {
			std::cout << "===================================================\n";
			std::cout << "PROFILE RESULTS  " << std::endl;
			std::cout << "===================================================\n";

			std::cout << "--- Long-term planning ---\n";
			gPhaseProfilers->longTermPhaseProfiler.displayStatistics(std::cout);
			std::cout << std::endl;

			std::cout << "--- Mid-term planning ---\n";
			gPhaseProfilers->midTermPhaseProfiler.displayStatistics(std::cout);
			std::cout << std::endl;

			std::cout << "--- Short-term planning ---\n";
			gPhaseProfilers->shortTermPhaseProfiler.displayStatistics(std::cout);
			std::cout << std::endl;

			std::cout << "--- Perceptive phase ---\n";
			gPhaseProfilers->perceptivePhaseProfiler.displayStatistics(std::cout);
			std::cout << std::endl;

			std::cout << "--- Predictive phase ---\n";
			gPhaseProfilers->predictivePhaseProfiler.displayStatistics(std::cout);
			std::cout << std::endl;

			std::cout << "--- Reactive phase ---\n";
			gPhaseProfilers->reactivePhaseProfiler.displayStatistics(std::cout);
			std::cout << std::endl;

			std::cout << "--- Steering phase ---\n";
			gPhaseProfilers->steeringPhaseProfiler.displayStatistics(std::cout);
			std::cout << std::endl;

			std::cout << "--- TOTAL AI ---\n";
			gPhaseProfilers->aiProfiler.displayStatistics(std::cout);
			std::cout << std::endl;


			std::cout << "--- ESTIMATED REFERENCE (excluding long-term and space-time planning) ---\n";
			std::cout << "\n(NOTE: this is not rigorously valid to compare against amortized costs below,\n";
			std::cout << "         because it excludes space-time planning)\n\n";
			float totalAgentTime =
				gPhaseProfilers->midTermPhaseProfiler.getAverageExecutionTime() + 
				gPhaseProfilers->shortTermPhaseProfiler.getAverageExecutionTime() +
				gPhaseProfilers->perceptivePhaseProfiler.getAverageExecutionTime() +
				gPhaseProfilers->predictivePhaseProfiler.getAverageExecutionTime() +
				gPhaseProfilers->reactivePhaseProfiler.getAverageExecutionTime() +
				gPhaseProfilers->steeringPhaseProfiler.getAverageExecutionTime();
			float totalAgentTime_5Hz_amortized =   // 5 Hz skips every 4 frames, so scale by 0.25
				gPhaseProfilers->midTermPhaseProfiler.getAverageExecutionTime() * 0.25f + 
				gPhaseProfilers->shortTermPhaseProfiler.getAverageExecutionTime() * 0.25f +
				gPhaseProfilers->perceptivePhaseProfiler.getAverageExecutionTime() * 0.25f +
				gPhaseProfilers->predictivePhaseProfiler.getAverageExecutionTime() * 0.25f +
				gPhaseProfilers->reactivePhaseProfiler.getAverageExecutionTime() +  // reactive and steering phases still execute 20 Hz.
				gPhaseProfilers->steeringPhaseProfiler.getAverageExecutionTime();
			float totalAgentTime_4Hz_amortized =    // 4 Hz skips every 5 frames, so scale by 0.2
				gPhaseProfilers->midTermPhaseProfiler.getAverageExecutionTime() * 0.2f + 
				gPhaseProfilers->shortTermPhaseProfiler.getAverageExecutionTime() * 0.2f +
				gPhaseProfilers->perceptivePhaseProfiler.getAverageExecutionTime() * 0.2f +
				gPhaseProfilers->predictivePhaseProfiler.getAverageExecutionTime() * 0.2f +
				gPhaseProfilers->reactivePhaseProfiler.getAverageExecutionTime() +  // reactive and steering phases still execute 20 Hz.
				gPhaseProfilers->steeringPhaseProfiler.getAverageExecutionTime();

			std::cout << " percent mid-term:   " << gPhaseProfilers->midTermPhaseProfiler.getAverageExecutionTime()/totalAgentTime * 100.0<< "\n";
			std::cout << " percent short-term: " << gPhaseProfilers->shortTermPhaseProfiler.getAverageExecutionTime()/totalAgentTime * 100.0<< "\n";
			std::cout << " percent perceptive: " << gPhaseProfilers->perceptivePhaseProfiler.getAverageExecutionTime()/totalAgentTime * 100.0 << "\n";
			std::cout << " percent predictive: " << gPhaseProfilers->predictivePhaseProfiler.getAverageExecutionTime()/totalAgentTime * 100.0 << "\n";
			std::cout << " percent reactive:   " << gPhaseProfilers->reactivePhaseProfiler.getAverageExecutionTime()/totalAgentTime * 100.0 << "\n";
			std::cout << " percent steering:   " << gPhaseProfilers->steeringPhaseProfiler.getAverageExecutionTime()/totalAgentTime * 100.0 << "\n";
			std::cout << "\n";
			std::cout << " Average per agent, no amortization: " << totalAgentTime * 1000.0 << " milliseconds\n";
			std::cout << " Average per agent, 5Hz (skip 4 frames): " << totalAgentTime_5Hz_amortized * 1000.0 << " milliseconds\n";
			std::cout << " Average per agent, 4Hz (skip 5 frames): " << totalAgentTime_4Hz_amortized * 1000.0 << " milliseconds\n";
			std::cout << "\n";
		}

		std::cout << "--- PROFILE RESULTS (excluding long-term planning) ---\n\n";
		float totalTimeForAllAgents =
			gPhaseProfilers->midTermPhaseProfiler.getTotalTime()+
			gPhaseProfilers->shortTermPhaseProfiler.getTotalTime() +
			gPhaseProfilers->perceptivePhaseProfiler.getTotalTime() +
			gPhaseProfilers->predictivePhaseProfiler.getTotalTime() +
			gPhaseProfilers->reactivePhaseProfiler.getTotalTime() +
			gPhaseProfilers->steeringPhaseProfiler.getTotalTime();

		// TODO: right now this is hacked, later on need to add an arg or access to the engine to get this value correctly:
		std::cerr << " TODO: 20 frames per second is a hard-coded assumption in the following calculations\n";
		float baseFrequency = 20.0f;
		float totalNumberOfFrames = (float)gPhaseProfilers->steeringPhaseProfiler.getNumTimesExecuted();
		std::cout << " average frequency mid-term:   " << gPhaseProfilers->midTermPhaseProfiler.getNumTimesExecuted()/totalNumberOfFrames * baseFrequency << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->midTermPhaseProfiler.getNumTimesExecuted()) << " frames)\n";
		std::cout << " average frequency short-term: " << gPhaseProfilers->shortTermPhaseProfiler.getNumTimesExecuted()/totalNumberOfFrames * baseFrequency << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->shortTermPhaseProfiler.getNumTimesExecuted()) << " frames)\n";
		std::cout << " average frequency perceptive: " << gPhaseProfilers->perceptivePhaseProfiler.getNumTimesExecuted()/totalNumberOfFrames * baseFrequency << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->perceptivePhaseProfiler.getNumTimesExecuted()) << " frames)\n";
		std::cout << " average frequency predictive: " << gPhaseProfilers->predictivePhaseProfiler.getNumTimesExecuted()/totalNumberOfFrames * baseFrequency << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->predictivePhaseProfiler.getNumTimesExecuted()) << " frames)\n";
		std::cout << " average frequency reactive:   " << gPhaseProfilers->reactivePhaseProfiler.getNumTimesExecuted()/totalNumberOfFrames * baseFrequency << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->reactivePhaseProfiler.getNumTimesExecuted()) << " frames)\n";
		std::cout << " average frequency steering:   " << gPhaseProfilers->steeringPhaseProfiler.getNumTimesExecuted()/totalNumberOfFrames * baseFrequency << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->steeringPhaseProfiler.getNumTimesExecuted()) << " frames)\n";
		std::cout << "\n";

		std::cout << " amortized percent mid-term:   " << gPhaseProfilers->midTermPhaseProfiler.getTotalTime()/totalTimeForAllAgents * 100.0 << "\n";
		std::cout << " amortized percent short-term: " << gPhaseProfilers->shortTermPhaseProfiler.getTotalTime()/totalTimeForAllAgents * 100.0 << "\n";
		std::cout << " amortized percent perceptive: " << gPhaseProfilers->perceptivePhaseProfiler.getTotalTime()/totalTimeForAllAgents * 100.0 << "\n";
		std::cout << " amortized percent predictive: " << gPhaseProfilers->predictivePhaseProfiler.getTotalTime()/totalTimeForAllAgents * 100.0 << "\n";
		std::cout << " amortized percent reactive:   " << gPhaseProfilers->reactivePhaseProfiler.getTotalTime()/totalTimeForAllAgents * 100.0 << "\n";
		std::cout << " amortized percent steering:   " << gPhaseProfilers->steeringPhaseProfiler.getTotalTime()/totalTimeForAllAgents * 100.0 << "\n";
		std::cout << " AVERAGE PER AGENT PER UPDATE: " << totalTimeForAllAgents / ((float)gPhaseProfilers->steeringPhaseProfiler.getNumTimesExecuted()) * 1000.0 << " milliseconds\n";

		std::cout << std::endl;
	}
}

void PPRAIModule::finish()
{
	// nothing to do here
}



PLUGIN_API SteerLib::ModuleInterface * createModule() { return new PPRAIModule; }

PLUGIN_API void destroyModule( SteerLib::ModuleInterface*  module ) { if (module) delete module; module = NULL; }

