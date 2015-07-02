//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_STEER_BENCH_MODULE__
#define __STEERLIB_STEER_BENCH_MODULE__

/// @file SteerBenchModule.h
/// @brief Declares the SteerBenchModule built-in module.

#include "interfaces/ModuleInterface.h"
#include "interfaces/EngineInterface.h"
#include "benchmarking/BenchmarkEngine.h"
#include "interfaces/BenchmarkTechniqueInterface.h"
#include "modules/MetricsCollectorModule.h"

namespace SteerLib {

	class SteerBenchModule : public SteerLib::ModuleInterface
	{
	public:
		std::string getDependencies() { return "metricsCollector"; }

		std::string getConflicts() { return ""; }

		void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo ) {

			_engine = engineInfo;
			_metricsCollectorModule = NULL;
			_techniqueName = "composite02";

			// iterate over all the options
			SteerLib::OptionDictionary::const_iterator optionIter;
			for (optionIter = options.begin(); optionIter != options.end(); ++optionIter) {
				if ((*optionIter).first == "technique") {
					_techniqueName = (*optionIter).second;
				}
				else {
					throw Util::GenericException("unrecognized option \"" + Util::toString((*optionIter).first) + "\" given to steerBench module.");
				}
			}

			_benchmarkTechnique = SteerLib::createBenchmarkTechnique(_techniqueName);
			_benchmarkTechnique->init();
		}

		void finish() { }

		void preprocessSimulation() {
			_metricsCollectorModule = dynamic_cast<MetricsCollectorModule*>( _engine->getModule("metricsCollector"));
		}

		void postprocessFrame(float timeStamp, float dt, unsigned int frameNumber) {
			assert(_metricsCollectorModule != NULL);
			_benchmarkTechnique->update(_metricsCollectorModule->getSimulationMetrics(), timeStamp, dt);
		}

		void postprocessSimulation() {
			std::cout << "Benchmark score using the \"" << _techniqueName << "\" benchmark technique:  ";
			std::cout << _benchmarkTechnique->getTotalBenchmarkScore(_metricsCollectorModule->getSimulationMetrics()) << "\n";
			_benchmarkTechnique->printTotalScoreDetails(_metricsCollectorModule->getSimulationMetrics(), std::cout);
			_metricsCollectorModule = NULL;
		}

	protected:
		SteerLib::EngineInterface * _engine;
		MetricsCollectorModule * _metricsCollectorModule;

		std::string _techniqueName;
		SteerLib::BenchmarkTechniqueInterface * _benchmarkTechnique;
	};

} // end namespace SteerLib

#endif
