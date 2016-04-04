/*
 *  GlChartViewCoordinate.hpp - part of SmartBody-lib's Test Suite
 *  Copyright (C) 2009  University of Southern California
 *
 *  SmartBody-lib is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public License
 *  as published by the Free Software Foundation, version 3 of the
 *  license.
 *
 *  SmartBody-lib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with SmartBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Jingqiao Fu, USC
 */

#ifndef _GL_CHART_COORDINATE_H_
#define _GL_CHART_COORDINATE_H_

#include <string>
#include <utility>
#include <sr/sr_vec.h>
#include <sr/sr_vec2.h>
#include <sr/sr_quat.h>
#include <sr/sr_mat.h>
#include <sr/sr_array.h>
#include <sr/sr_camera.h>
#include <sr/sr_buffer.h>
#include <sr/sr_string.h>
#include "math.h"

#ifdef WIN32
#include "glfont2.h"
using namespace glfont;
#endif

class GlChartViewCoordinate
{
public:
	float x_scale;
	float y_scale;

	float y_scale_zoom;

	float prev_x_scale;
	float prev_y_scale;

	float x_margin_ratio;
	float y_margin_ratio;

#ifdef WIN32
	GLFont label;
#endif
	bool font_initialized;

protected:
	float x_size;
	float y_size;
	int x_label_num;
	int y_label_num;

	int default_x_label_num;

public:
	GlChartViewCoordinate();
	~GlChartViewCoordinate();

public:
	void Update(float WindowWidth, float WindowHeight, SrCamera& camera);
	void InitFont();
	float GetXScale();
	float GetYScale();
	
	void Draw();

	void SetXLabelNum(int num);
	void SetYLabelNum(int num);

	void SetXSize(float size);
	void SetYSize(float size);
	float GetYSize();

protected:
	void DrawCoordinateLabels();
	SrVec2 GetStringSize(char* str);
	int GetStringWidth(char* str);
};

#endif //_GL_CHART_COORDINATE_H_
