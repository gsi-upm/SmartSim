//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file BoxObstacle.cpp
/// @brief Implements the BoxObstacle class.

#include "obstacles/BoxObstacle.h"
#include "util/DrawLib.h"

using namespace SteerLib;
using namespace Util;

void BoxObstacle::draw() {
#ifdef ENABLE_GUI
	DrawLib::glColor(Util::gBlack);
	DrawLib::drawBoxWireFrame(_bounds.xmin,_bounds.xmax,_bounds.ymin,_bounds.ymax,_bounds.zmin,_bounds.zmax);
	DrawLib::glColor(Util::gDarkCyan);
	DrawLib::drawBox(_bounds.xmin,_bounds.xmax,_bounds.ymin,_bounds.ymax,_bounds.zmin,_bounds.zmax);
#endif // ifdef ENABLE_GUI
}
