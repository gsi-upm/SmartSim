//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_OBSTACLE_INITIAL_CONDITIONS_H__
#define __STEERLIB_OBSTACLE_INITIAL_CONDITIONS_H__

/// @file ObstacleInitialConditions.h
/// @brief Declares the data structures relevant to an obstacles's initial conditions.

#include "Globals.h"

namespace SteerLib {


	/**
	 * @brief The initial conditions of a single obstacle based on the input test case.
	 *
	 * Currently the only type of object supported by test cases is a simple axis-aligned box.
	 */
	struct ObstacleInitialConditions {
		float xmin, xmax, ymin, ymax, zmin, zmax;
	};


} // end namespace SteerLib

#endif
