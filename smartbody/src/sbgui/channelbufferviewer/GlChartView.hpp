/*
 *  GlChartView.hpp - part of SmartBody-lib's Test Suite
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


#ifndef _GL_CHART_VIEW_H_
#define _GL_CHART_VIEW_H_

#include <FL/Fl_Gl_Window.H>
#include <sr/sr_camera.h>
#include <sr/sr_event.h>
#include <sr/sr_light.h>
#include <sr/sr_viewer.h>
#include "GlChartViewCoordinate.hpp"
#include "GlChartViewArchive.hpp"
#include <FL/gl.h>

#ifdef WIN32
#include "glfont2.h"
#endif

class GlChartView : public Fl_Gl_Window, public SrViewer
{
public:
	int th;
	SrCamera camera;
	bool update_coordinate;
	bool automatic_scale;

#ifdef WIN32
	GLFont label;
#endif

	SrEvent e;

	GlChartViewCoordinate coordinate;
	GlChartViewArchive archive;

	GLuint fontTextureName;	
	int quat_shown_type; //0: quaternion; 1: euler angle;

	bool show_x; // if show x value when shown as quaternion or euler angle
	bool show_y; // if show y value when shown as quaternion or euler angle
	bool show_z; // if show z value when shown as quaternion or euler angle
	bool show_w; // if show w value when shown as quaternion

public:
	GlChartView(int x, int y, int w, int h, char* name);
	~GlChartView();

public:
	GlChartViewArchive* get_archive();
	void render();
	void set_max_buffer_size(int max_size);
	void set_quat_show_type(int type);

public:
	virtual int handle ( int event );

	void translate_event ( SrEvent& e, SrEvent::EventType t, int w, int h, GlChartView* viewer );
	int mouse_event ( const SrEvent &e );
	SrVec rotatePoint(SrVec point, SrVec origin, SrVec direction, float angle);
	void init_camera(int type);

protected:
	void initGL(int width, int height);
	void initFont();

	void get_label(char* label, const std::string & str, int type);
	
	void reshape(int width, int height);

	void print_bitmap_string(float x,float y, float z, void *font, char* s);
	void displayText(char* text, int X1, int Y1);

	void draw();
	void draw_coordinate();
	void draw_series();
	void draw_series_value(GlChartViewSeries* series);
	void draw_series_vec2(GlChartViewSeries* series);
	void draw_series_vec3(GlChartViewSeries* series);
	void draw_series_euler(GlChartViewSeries* series);
	void draw_series_axisangle(GlChartViewSeries* series);
	void draw_series_swingtwist(GlChartViewSeries* series);
	void draw_series_3D_euler(GlChartViewSeries* series);
	void draw_series_quat(GlChartViewSeries* series);
	void draw_series_quaternion_velocity(GlChartViewSeries* series);
	void draw_series_euler_velocity(GlChartViewSeries* series);
	void draw_series_axisangle_velocity(GlChartViewSeries* series);

};

#endif //_GL_CHART_VIEW_H_
