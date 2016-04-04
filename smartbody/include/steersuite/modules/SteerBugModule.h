//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_STEER_BUG_MODULE__
#define __STEERLIB_STEER_BUG_MODULE__

/// @file SteerBugModule.h
/// @brief Declares the SteerBugModule built-in module.

#include "interfaces/ModuleInterface.h"
#include "interfaces/EngineInterface.h"

namespace SteerLib {

	class SteerBugModule : public SteerLib::ModuleInterface
	{
	public:
		std::string getDependencies() { return "metricsCollector"; }
		std::string getConflicts() { return ""; }
		void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo ) {
			throw Util::GenericException("steerBug module not implemented yet.");
		}
		void finish() { }
	};

} // end namespace SteerLib

#endif
