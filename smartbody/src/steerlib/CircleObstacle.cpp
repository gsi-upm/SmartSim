//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file CircleObstacle.cpp
/// @brief Implements the CircleObstacle class.

#include "obstacles/CircleObstacle.h"
#include "util/DrawLib.h"

using namespace SteerLib;
using namespace Util;

CircleObstacle::CircleObstacle ( Point centerPosition, float radius, float ymin, float ymax )
{
	_radius = radius;
	_centerPosition = centerPosition;


	// calculating bounds
	_bounds.ymin = ymin;
	_bounds.ymax = ymax;
	_bounds.xmin = _centerPosition.x - _radius;
	_bounds.xmax = _centerPosition.x + _radius;
	_bounds.zmin = _centerPosition.z - _radius;
	_bounds.zmax = _centerPosition.z + _radius;

	// calculating _blocksLineOfSight
	_blocksLineOfSight = (ymax > 0.7) ? true : false;

}

void CircleObstacle::draw() {
#ifdef ENABLE_GUI

	DrawLib::glColor(gDarkCyan);
	DrawLib::drawCylinder(_centerPosition,_radius,_bounds.ymin,_bounds.ymax);
	DrawLib::drawBoxWireFrame(_bounds.xmin,_bounds.xmax,_bounds.ymin,_bounds.ymax,_bounds.zmin,_bounds.zmax);
#endif // ifdef ENABLE_GUI
}
