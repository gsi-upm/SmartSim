//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_COMPOSITE_TECHNIQUE_02_H__
#define __STEERLIB_COMPOSITE_TECHNIQUE_02_H__

/// @file CompositeTechnique02.h
/// @brief Declares SteerLib::CompositeBenchmarkTechnique02, an extension of SteerLib::CompositeTechnique01 that tries to capture "second-order" effort efficiency.

#include "Globals.h"
#include "interfaces/BenchmarkTechniqueInterface.h"
#include "util/GenericException.h"

namespace SteerLib {
	
	class STEERLIB_API CompositeBenchmarkTechnique02 : public SteerLib::BenchmarkTechniqueInterface
	{
	public:
		void init();
		void update(SimulationMetricsCollector * simulationMetrics, float timeStamp, float dt) { }
		float getTotalBenchmarkScore(SimulationMetricsCollector * simulationMetrics);
		float getAgentBenchmarkScore(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics);
		void printTotalScoreDetails(SimulationMetricsCollector * simulationMetrics, std::ostream & out);
		void printAgentScoreDetails(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics, std::ostream & out);

	protected:
		float _computeTotalBenchmarkScore(SimulationMetricsCollector * simulationMetrics);

		bool _benchmarkScoreComputed;
		float _alpha, _beta, _gamma, _delta;
		float _numCollisionsOfAllAgents, _totalTimeOfAllAgents, _totalEnergyOfAllAgents, _totalInstantaneousAcceleration;
		float _numAgents;
		float _totalBenchmarkScore;
	};


} // end namespace SteerLib

#endif
