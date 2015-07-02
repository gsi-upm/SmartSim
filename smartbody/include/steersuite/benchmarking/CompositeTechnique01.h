//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_COMPOSITE_TECHNIQUE_01_H__
#define __STEERLIB_COMPOSITE_TECHNIQUE_01_H__

/// @file CompositeTechnique01.h
/// @brief Declares SteerLib::CompositeBenchmarkTechnique01, the first benchmark scoring technique used in the original CAVW 2008 paper.
///
/// @todo
///   - add feature to set options for benchmark techniques in general...

#include "Globals.h"
#include "interfaces/BenchmarkTechniqueInterface.h"
#include "util/GenericException.h"

namespace SteerLib {
	
	class STEERLIB_API CompositeBenchmarkTechnique01 : public SteerLib::BenchmarkTechniqueInterface
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
		float _alpha, _beta, _gamma;
		float _numCollisionsOfAllAgents, _totalTimeOfAllAgents, _totalEnergyOfAllAgents;
		float _numAgents;
		float _totalBenchmarkScore;
	};


} // end namespace SteerLib

#endif
