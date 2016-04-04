/*
 *  GlChartView.cpp - part of SmartBody-lib's Test Suite
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

#include "vhcl.h"

#include "GlChartView.hpp"

#include <FL/Fl.H>
#include <FL/gl.h>

#include <sr/sr_gl.h>
#include <sbm/gwiz_math.h>
#include <sb/SBScene.h>


GlChartView::GlChartView(int x, int y, int w, int h, char* name) : Fl_Gl_Window( x, y, w, h, name ), SrViewer(x, y, w, h, name)
{
	//initGL(w, h);
	//initFont();
	init_camera(0);
	th = 0;
	//max_buffer_size = 800;
	quat_shown_type = 0;
	update_coordinate = true;
	show_x = true;
	show_y = true;
	show_z = true;
	show_w = true;
	automatic_scale = true;
}

GlChartView::~GlChartView()
{
	LOG("GlChartView::destructor");	
	//make_current();
	//glDeleteTextures(1, &fontTextureName);
}

void GlChartView::set_max_buffer_size(int max_size)
{
	//max_buffer_size = max_size;
	coordinate.SetXSize((float)max_size);
}

void GlChartView::initGL(int width, int height)
{
	make_current();
	//float pos1[4] = {1500.0, 1500.0, 1500.0, 1.0};
	float pos0[4] = {-15000.0f, -12000.0f, 15000.0f, 1.0f};
	float ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
	float diffuse[4] = {0.6f, 0.6f, 0.6f, 1.0f};
	float specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	//float lmodel_ambient[4] = {0.4, 0.4, 0.4, 1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, pos0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);       
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glViewport ( 0, 0, width, height );

	glCullFace ( GL_BACK );
	glDepthFunc ( GL_LEQUAL );
	glFrontFace ( GL_CCW );

	glEnable ( GL_POLYGON_SMOOTH );

	glEnable ( GL_POINT_SMOOTH );
	glPointSize ( 1.0 );

	glShadeModel ( GL_SMOOTH );
}

void GlChartView::initFont()
{
	make_current();
	GLint src;
	glGetIntegerv(GL_BLEND_SRC, &src);
	GLint dest;
	glGetIntegerv(GL_BLEND_DST, &dest);

	glPushAttrib(GL_ENABLE_BIT);	
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glGenTextures(1, &fontTextureName);

#ifdef WIN32
	std::string mediaPath = SmartBody::SBScene::getScene()->getMediaPath();
	std::string fontPath = mediaPath + "/" +  "fonts/font.glf";
	if (!label.Create(fontPath.c_str(), fontTextureName))
	{
		if(!label.Create(".font.glf", fontTextureName))
			LOG("GlChartViewCoordinate::InitFont(): Error: Cannot load font file\n");
	}
#endif
	glPopAttrib();
	glBlendFunc(src, dest);
}

void GlChartView::init_camera(int type)
{
	camera.init();
	camera.setAspectRatio((float)w()/(float)h());
	coordinate.Update((float)w(), (float)h(), camera);
	camera.setEye(coordinate.GetXScale()/2, 0.0f, 2000.0f);
	camera.setCenter(coordinate.GetXScale()/2, 0.0f, 0.0f);
	automatic_scale = true;
	if(type == 0) 
	{
		coordinate.y_scale_zoom = 1.0f;
		coordinate.SetYSize(1.0f);
	}
	else if(type == 1)
	{
		coordinate.y_scale_zoom = 1.0f/180.0f;
		coordinate.SetYSize(180.0f);
	}
	else if (type == 3)
	{
		coordinate.y_scale_zoom = 1.0f/180.0f;
		coordinate.SetYSize(180.0f);
	}
}

void GlChartView::reshape(int width, int height)
{
	make_current();
	glViewport(0, 0, width, height);
	// transform
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45.0f, (GLdouble)width/(GLdouble)height, 100.0f, 1000000.0f);
}

void GlChartView::render()
{
	if(update_coordinate) coordinate.Update((float)w(), (float)h(), camera);
	redraw();
}

void GlChartView::draw()
{
	
	make_current();
	if (!valid())
	{
		initGL(w(),h());
	}
	if (!context_valid())
	{
		initFont();
	}
	//LOG("data viewer GL context = %d",wglGetCurrentContext());
	glViewport ( 0, 0, w(), h() );
	SrLight light1;
	SrLight light2;
	SrMat mat;

	light1.directional = true;
	light1.diffuse = SrColor( 1.0f, 0.95f, 0.8f );
	light1.position = SrVec( 100.0, 250.0, 400.0 );
	light1.constant_attenuation = 1.0f;

	light2 = light1;
	light2.directional = false;
	light2.diffuse = SrColor( 0.8f, 0.85f, 1.0f );
	light2.position = SrVec( 100.0, 500.0, -200.0 );

	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	camera.setAspectRatio((float)w()/(float)h());

	glMatrixMode ( GL_PROJECTION );
	glLoadMatrix ( camera.get_perspective_mat(mat) );

	glMatrixMode ( GL_MODELVIEW );
	glLoadMatrix ( camera.get_view_mat(mat) );

	glScalef ( camera.getScale(), camera.getScale(), camera.getScale() );

	glPushAttrib(GL_ENABLE_BIT);
	glEnable ( GL_LIGHTING );
	glLight ( 0, light1 );
	glLight ( 1, light2 );
	

	
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	static GLfloat mat_emissin[] = { 0.0,  0.0,    0.0,    1.0 };
	static GLfloat mat_ambient[] = { 0.0,  0.0,    0.0,    1.0 };
	static GLfloat mat_diffuse[] = { 1.0,  1.0,    1.0,    1.0 };
	static GLfloat mat_speclar[] = { 0.0,  0.0,    0.0,    1.0 };
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, mat_emissin );
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_speclar );
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0 );
	glColorMaterial( GL_FRONT_AND_BACK, GL_DIFFUSE );
	glEnable( GL_COLOR_MATERIAL );
	glEnable( GL_NORMALIZE );

	draw_coordinate();
	draw_series();
	
	glDisable(GL_TEXTURE_2D);
	glPopAttrib();		
}

void GlChartView::draw_coordinate()
{
	coordinate.Draw();
}

void GlChartView::draw_series()
{
	GlChartViewSeries* series = NULL;
	for(int i = 0; i < archive.GetSeriesCount(); ++i)
	{
		series = archive.GetSeries(i);
		if(series->data_type == CHART_DATA_TYPE_VALUE)
		{
			draw_series_value(series);
		}
		else if(series->data_type == CHART_DATA_TYPE_VEC2)
		{
			draw_series_vec2(series);
		}
		else if(series->data_type == CHART_DATA_TYPE_VEC)
		{
			draw_series_vec3(series);
		}

		else if(series->data_type == CHART_DATA_TYPE_QUAT)
		{
			if(quat_shown_type == 0) draw_series_quat(series);
			else if(quat_shown_type == 1) draw_series_euler(series);
			//else if(quat_shown_type == 2) draw_series_3D_euler(series);			
			//else if (quat_shown_type == 3) draw_series_swingtwist(series);	
			else if (quat_shown_type == 3) draw_series_axisangle(series);
			else if (quat_shown_type == 4) draw_series_quaternion_velocity(series);
			else if (quat_shown_type == 5) draw_series_euler_velocity(series);
			else if (quat_shown_type == 6) draw_series_axisangle_velocity(series);
			
		}
	}
}

void GlChartView::set_quat_show_type(int type)
{
	quat_shown_type = type;
	if(type == 0 && coordinate.GetYSize() == 180.0f) 
	{
		coordinate.SetYSize(1.0f);
		coordinate.y_scale_zoom = 1.0f;
	}
	else if(type == 1 && coordinate.GetYSize() == 1.0f) 
	{
		coordinate.SetYSize(180.0f);
		coordinate.y_scale_zoom = 1.0f/180.0f;
	}
	else if (type == 3 && coordinate.GetYSize() == 1.0f)
	{
		coordinate.SetYSize(180.0f);
		coordinate.y_scale_zoom = 1.0f/180.0f;
	}
	else if (type == 4 && coordinate.GetYSize() == 1.0f)
	{
		coordinate.SetYSize(180.0f);
		coordinate.y_scale_zoom = 1.0f/180.0f;
	}
	else if (type == 5 && coordinate.GetYSize() == 1.0f)
	{
		coordinate.SetYSize(180.0f);
		coordinate.y_scale_zoom = 1.0f/180.0f;
	}
}

void GlChartView::draw_series_value(GlChartViewSeries* series)
{
	float value = 0.0f;
	char t_label[50];
	SrVec color;
	float step = coordinate.GetXScale()/(series->max_size-1);
	float y_scale = coordinate.GetYScale();
	float y_size = coordinate.GetYSize();
	if(series->bold)
	{
		glLineWidth(3.0f);
	}
	color = series->GetColor(1);
	glColor4f(color.x, color.y, color.z, 0.5f);

	glBegin(GL_LINE_STRIP);
		for(int i = 0; i < series->size; ++i)
		{
			value = series->GetValue(i);
			if(abs(value) > this->coordinate.GetYSize()) 
			{
				if(automatic_scale) coordinate.y_scale_zoom = 1.0f/abs(value);
				this->coordinate.SetYSize(abs(value));
			}
			glVertex3f(i*step, value*y_scale, 0.0f);
		}
	glEnd();
	glEnable(GL_TEXTURE_2D);
#ifdef WIN32
	label.Begin();
	get_label(t_label, series->title, -1);
	label.DrawString(t_label, 2.0f, (series->size-1)*step, value*y_scale);
#endif
	glDisable(GL_TEXTURE_2D);

	glLineWidth(1.0f);
}

void GlChartView::draw_series_vec2(GlChartViewSeries* series)
{
	
}

void GlChartView::draw_series_vec3(GlChartViewSeries* series)
{
	
}

// not used for now
void GlChartView::draw_series_3D_euler(GlChartViewSeries* series)
{
	SrVec euler;
	SrVec prev_euler;
	SrVec color;
	color = series->GetColor(1);
	glColor4f(color.x, color.y, color.z, 0.5f);
	glBegin(GL_LINES);
		for(int i = 0; i < series->size; ++i)
		{
			euler = series->GetEuler(i);
			euler *= 500.0f;
			if(i > 0)
			{
				glVertex3f((i-1)*5.0f+prev_euler.x, prev_euler.y, prev_euler.z);
				glVertex3f(i*5.0f+euler.x, euler.y, euler.z);
			}
			glVertex3f(i*5.0f, 0.0f, 0.0f);
			glVertex3f(i*5.0f+euler.x, euler.y, euler.z);
			prev_euler = euler;
		}
	glEnd();

}

void GlChartView::get_label(char* label, const std::string & str, int type)
{
	label[0] = '\0';
	switch(type)
	{
		case 0:
			sprintf(label, "X.");
			break;
		case 1:
			sprintf(label, "Y.");
			break;
		case 2:
			sprintf(label, "Z.");
			break;
		case 3:
			sprintf(label, "W.");
			break;
		default:
			break;
	}
	strcat(label, &(str[0]));
	label[strlen(label)-3] = '\0';

}

void GlChartView::draw_series_swingtwist( GlChartViewSeries* series )
{
	SrVec euler;
	SrVec color;
	char t_label[50];
	float step = coordinate.GetXScale()/(series->max_size-1);
	float y_scale = coordinate.GetYScale();
	if(series->bold)
	{
		glLineWidth(3.0f);
	}

#ifdef WIN32
	label.Begin();
#endif
	if(show_x)
	{
		color = series->GetColor(1);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
		for(int i = 0; i < series->size; ++i)
		{
			euler = series->GetSwingTwist(i);
			glVertex3f(i*step, euler.x*y_scale, 0.0f);
		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 0);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, euler.x*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}
	if(show_y)
	{
		color = series->GetColor(2);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
		for(int i = 0; i < series->size; ++i)
		{
			euler = series->GetSwingTwist(i);
			glVertex3f(i*step, euler.y*y_scale, 0.0f);
		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 1);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, euler.y*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}
	if(show_z)
	{
		color = series->GetColor(3);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
		for(int i = 0; i < series->size; ++i)
		{
			euler = series->GetSwingTwist(i);
			glVertex3f(i*step, euler.z*y_scale, 0.0f);
		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 2);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, euler.z*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}
	glLineWidth(1.0f);
}


void GlChartView::draw_series_axisangle( GlChartViewSeries* series )
{
	SrVec axisAngle;
	SrVec prevAxisAngle;
	SrVec color;
	char t_label[50];
	float step = coordinate.GetXScale()/(series->max_size-1);
	float y_scale = coordinate.GetYScale();

	float dt = series->dt;

	if(series->bold)
	{
		glLineWidth(3.0f);
	}
#ifdef WIN32
	label.Begin();
#endif
	if(show_x)
	{
		color = series->GetColor(1);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
		for(int i = 1; i < series->size; ++i)
		{
			axisAngle = series->GetQuat(i).axisAngle()*180.f/(float)M_PI;			
			glVertex3f(i*step, axisAngle.x*y_scale, 0.0f);
		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 0);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, axisAngle.x*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}
	if(show_y)
	{
		color = series->GetColor(2);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
		for(int i = 1; i < series->size; ++i)
		{
			axisAngle = series->GetQuat(i).axisAngle()*180.f/(float)M_PI;				
			glVertex3f(i*step, axisAngle.y*y_scale, 0.0f);
		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 1);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, axisAngle.y*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}
	if(show_z)
	{
		color = series->GetColor(3);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
		for(int i = 1; i < series->size; ++i)
		{
			axisAngle = series->GetQuat(i).axisAngle()*180.f/(float)M_PI;	
			prevAxisAngle = series->GetQuat(i-1).axisAngle()*180.f/(float)M_PI;
			glVertex3f(i*step, axisAngle.z*y_scale, 0.0f);
		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 2);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, axisAngle.z*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}
	glLineWidth(1.0f);
}

void GlChartView::draw_series_euler(GlChartViewSeries* series)
{
	SrVec euler;
	SrVec color;
	char t_label[50];
	float step = coordinate.GetXScale()/(series->max_size-1);
	float y_scale = coordinate.GetYScale();
	if(series->bold)
	{
		glLineWidth(3.0f);
	}

#ifdef WIN32
	label.Begin();
#endif
	if(show_x)
	{
		color = series->GetColor(1);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
			for(int i = 0; i < series->size; ++i)
			{
				euler = series->GetEuler(i);
				glVertex3f(i*step, euler.x*y_scale, 0.0f);
			}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 0);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, euler.x*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}
	if(show_y)
	{
		color = series->GetColor(2);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
			for(int i = 0; i < series->size; ++i)
			{
				euler = series->GetEuler(i);
				glVertex3f(i*step, euler.y*y_scale, 0.0f);
			}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 1);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, euler.y*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}
	if(show_z)
	{
		color = series->GetColor(3);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
			for(int i = 0; i < series->size; ++i)
			{
				euler = series->GetEuler(i);
				glVertex3f(i*step, euler.z*y_scale, 0.0f);
			}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 2);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, euler.z*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}
	glLineWidth(1.0f);
}


void GlChartView::draw_series_axisangle_velocity( GlChartViewSeries* series )
{
	SrVec axisAngle;
	SrVec prevAxisAngle;
	SrVec color;
	char t_label[50];
	float step = coordinate.GetXScale()/(series->max_size-1);
	float y_scale = coordinate.GetYScale();

	float dt = series->dt;

	if(series->bold)
	{
		glLineWidth(3.0f);
	}
#ifdef WIN32
	label.Begin();
#endif
	if(show_x)
	{
		color = series->GetColor(1);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
		for(int i = 1; i < series->size; ++i)
		{
			axisAngle = series->GetQuat(i).axisAngle()*180.f/(float)M_PI;	
			prevAxisAngle = series->GetQuat(i-1).axisAngle()*180.f/(float)M_PI;
			glVertex3f(i*step, (axisAngle.x-prevAxisAngle.x)/dt*y_scale, 0.0f);
		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 0);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, axisAngle.x*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}
	if(show_y)
	{
		color = series->GetColor(2);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
		for(int i = 1; i < series->size; ++i)
		{
			axisAngle = series->GetQuat(i).axisAngle()*180.f/(float)M_PI;	
			prevAxisAngle = series->GetQuat(i-1).axisAngle()*180.f/(float)M_PI;
			glVertex3f(i*step, (axisAngle.y-prevAxisAngle.y)/dt*y_scale, 0.0f);
		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 1);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, axisAngle.y*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}
	if(show_z)
	{
		color = series->GetColor(3);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
		for(int i = 1; i < series->size; ++i)
		{
			axisAngle = series->GetQuat(i).axisAngle()*180.f/(float)M_PI;	
			prevAxisAngle = series->GetQuat(i-1).axisAngle()*180.f/(float)M_PI;
			glVertex3f(i*step, (axisAngle.z-prevAxisAngle.z)/dt*y_scale, 0.0f);
		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 2);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, axisAngle.z*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}
	glLineWidth(1.0f);

}


void GlChartView::draw_series_euler_velocity( GlChartViewSeries* series )
{
	SrVec euler;
	SrVec prevEuler;
	SrVec color;
	char t_label[50];
	float step = coordinate.GetXScale()/(series->max_size-1);
	float y_scale = coordinate.GetYScale();

	float dt = series->dt;

	if(series->bold)
	{
		glLineWidth(3.0f);
	}
#ifdef WIN32
	label.Begin();
#endif
	if(show_x)
	{
		color = series->GetColor(1);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
		for(int i = 1; i < series->size; ++i)
		{
			euler = series->GetEuler(i);	
			prevEuler = series->GetEuler(i-1);
			glVertex3f(i*step, (euler.x-prevEuler.x)/dt*y_scale, 0.0f);
		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 0);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, euler.x*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}
	if(show_y)
	{
		color = series->GetColor(2);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
		for(int i = 1; i < series->size; ++i)
		{
			euler = series->GetEuler(i);
			prevEuler = series->GetEuler(i-1);
			glVertex3f(i*step, (euler.y-prevEuler.y)/dt*y_scale, 0.0f);
		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 1);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, euler.y*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}
	if(show_z)
	{
		color = series->GetColor(3);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
		for(int i = 1; i < series->size; ++i)
		{
			euler = series->GetEuler(i);
			prevEuler = series->GetEuler(i-1);
			glVertex3f(i*step, (euler.z-prevEuler.z)/dt*y_scale, 0.0f);
		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 2);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, euler.z*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}
	glLineWidth(1.0f);
}

void GlChartView::draw_series_quaternion_velocity( GlChartViewSeries* series )
{
	SrQuat quat;
	SrVec color;
	char t_label[50];

	float step = coordinate.GetXScale()/(series->max_size-1);
	float y_scale = coordinate.GetYScale();
	if(series->bold)
	{
		glLineWidth(3.0f);
	}

#ifdef WIN32
	label.Begin();
#endif

	float dt = series->dt;
	SrQuat q1 = series->GetQuat(0);
	SrQuat q2 = series->GetQuat(series->size/2-1);
	
	SrQuat diff = q2*q1.inverse();
	SrVec mainAxis = diff.axis();

	if(show_x)
	{
		color = series->GetColor(1);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
		for(int i = 1; i < series->size; ++i)
		{
			quat = series->GetQuat(i);
			SrQuat prevQ = series->GetQuat(i-1);
			SrQuat diff = quat*prevQ.inverse();
			diff.normalize();
			float angle = quat.angle()*180.f/(float)M_PI;
			float prevAngle = prevQ.angle()*180.f/(float)M_PI;
			float diffAngle = diff.angle()*180.f/(float)M_PI;
			//if (dot(diff.axis(),mainAxis)<0.f)
			//	diffAngle = -diffAngle;
			//glVertex3f(i*step, angle*y_scale, 0.0f);
			glVertex3f(i*step, (diffAngle)/dt*y_scale, 0.0f);
			//glVertex3f(i*step, (angle-prevAngle)/dt*y_scale, 0.0f);
		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 0);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, quat.x*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}
}

void GlChartView::draw_series_quat(GlChartViewSeries* series)
{
	SrQuat quat;
	SrVec color;
	char t_label[50];

	float step = coordinate.GetXScale()/(series->max_size-1);
	float y_scale = coordinate.GetYScale();
	if(series->bold)
	{
		glLineWidth(3.0f);
	}

#ifdef WIN32
	label.Begin();
#endif

	if(show_x)
	{
		color = series->GetColor(1);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
			for(int i = 0; i < series->size; ++i)
			{
				quat = series->GetQuat(i);
				//float angle = quat.angle();
				//glVertex3f(i*step, angle*y_scale, 0.0f);
				glVertex3f(i*step, quat.x*y_scale, 0.0f);
			}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 0);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, quat.x*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}

	if(show_y)
	{
		color = series->GetColor(2);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
			for(int i = 0; i < series->size; ++i)
			{
				quat = series->GetQuat(i);
				glVertex3f(i*step, quat.y*y_scale, 0.0f);
			}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 1);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, quat.y*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}

	if(show_z)
	{
		color = series->GetColor(3);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
			for(int i = 0; i < series->size; ++i)
			{
				quat = series->GetQuat(i);
				glVertex3f(i*step, quat.z*y_scale, 0.0f);
			}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 2);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, quat.z*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}

	if(show_w)
	{
		color = series->GetColor(4);
		glColor4f(color.x, color.y, color.z, 0.5f);
		glBegin(GL_LINE_STRIP);
			for(int i = 0; i < series->size; ++i)
			{
				quat = series->GetQuat(i);
				glVertex3f(i*step, quat.w*y_scale, 0.0f);
			}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		get_label(t_label, series->title, 3);
#ifdef WIN32
		label.DrawString(t_label, 2.0f, (series->size-1)*step, quat.w*y_scale);
#endif
		glDisable(GL_TEXTURE_2D);
	}

	glLineWidth(1.0f);
}

GlChartViewArchive* GlChartView::get_archive()
{
	return &archive;
}

int GlChartView::handle ( int event )
{
	//printf("\n%d", event);
	int y = 0;
	switch ( event )
	{ 
	case FL_PUSH:
		translate_event ( e, SrEvent::EventPush, w(), h(), this );
		break;

	case FL_RELEASE:
		translate_event ( e, SrEvent::EventRelease, w(), h(), this);
		break;
	
// 	case FL_MOVE:
// 		//SR_TRACE2 ( "Move buts: "<<(Fl::event_state(FL_BUTTON1)?1:0)<<" "<<(Fl::event_state(FL_BUTTON2)?1:0) );
// 		if ( !Fl::event_state(FL_BUTTON1) && !Fl::event_state(FL_BUTTON2) ) break;
// 		// otherwise, this is a drag: enter in the drag case.
// 		// not sure if this is a hack or a feature.
	case FL_DRAG:
		update_coordinate = false;
		translate_event ( e, SrEvent::EventDrag, w(), h(), this );	
		break;

	case FL_WHEN_RELEASE:
		//translate_event ( e, SrEvent::Release, w(), h(), this);
		break;

	case FL_KEYBOARD:
        break;

	case FL_HIDE: // Called when the window is iconized
		break;

	case FL_SHOW: // Called when the window is de-iconized or when show() is called
        show ();
        break;

	  default:
		  break;
    }

	mouse_event(e);

	if (event == FL_PUSH)
		return 1;

	return Fl_Gl_Window::handle(event);
}

void GlChartView::translate_event ( SrEvent& e, SrEvent::EventType t, int w, int h, GlChartView* viewer )
 {
   e.init_lmouse ();
   
   // put coordinates inside [-1,1] with (0,0) in the middle :
   e.mouse.x  = ((float)Fl::event_x())*2.0f / ((float)w) - 1.0f;
   e.mouse.y  = ((float)Fl::event_y())*2.0f / ((float)h) - 1.0f;
   e.mouse.y *= -1.0f;
   e.width = w;
   e.height = h;
   e.mouseCoord.x = (float)Fl::event_x();
   e.mouseCoord.y = (float)Fl::event_y();

   if ( Fl::event_state(FL_BUTTON1) ) 
	   e.button1 = 1;

   if ( Fl::event_state(FL_BUTTON2) ) 
	   e.button2 = 1;

   if ( Fl::event_state(FL_BUTTON3) ) 
	   e.button3 = 1;


   if(e.button1 == 0 && e.button2 == 0 && e.button3 == 0) 
   {
	   t = SrEvent::EventRelease;
   }

   e.type = t;

   if ( t==SrEvent::EventPush)
   {
	   e.button = Fl::event_button();
	   e.origUp = viewer->camera.getUpVector();
	   e.origEye = viewer->camera.getEye();
	   e.origCenter = viewer->camera.getCenter();
	   e.origMouse.x = e.mouseCoord.x;
	   e.origMouse.y = e.mouseCoord.y;
   }
   else if (t==SrEvent::EventRelease )
   {
	   e.button = Fl::event_button();
	   e.origMouse.x = -1;
	   e.origMouse.y = -1;
   }


   if ( Fl::event_state(FL_ALT)   ) e.alt = 1;
   else e.alt = 0;
   if ( Fl::event_state(FL_CTRL)  ) e.ctrl = 1;
   else e.ctrl = 0;
   if ( Fl::event_state(FL_SHIFT) ) e.shift = 1;
   else e.shift = 0;    
   e.key = Fl::event_key();
 }

SrVec GlChartView::rotatePoint(SrVec point, SrVec origin, SrVec direction, float angle)
{
	float originalLength = point.len();

	SrVec v = direction;
	SrVec o = origin;
	SrVec p = point;
	float c = cos(angle);
	float s = sin(angle);
	float C = 1.0f - c;

	SrMat mat;
	mat.e11() = v[0] * v[0] * C + c;
	mat.e12() = v[0] * v[1] * C - v[2] * s;
	mat.e13() = v[0] * v[2] * C + v[1] * s;
	mat.e21() = v[1] * v[0] * C + v[2] * s;
	mat.e22() = v[1] * v[1] * C + c;
	mat.e23() = v[1] * v[2] * C - v[0] * s;
	mat.e31() = v[2] * v[0] * C - v[1] * s;
	mat.e32() = v[2] * v[1] * C + v[0] * s;
	mat.e33() = v[2] * v[2] * C + c;

	mat.transpose();

	SrVec result = origin + mat * (point - origin);

	return result;
}

int GlChartView::mouse_event ( const SrEvent &e )
{
	int res=0;

	if ( e.mouse_event())
	{ 
		if ( e.type == SrEvent::EventDrag )
		{ 
			float dx = e.mousedx() * camera.getAspectRatio();
			float dy = e.mousedy() / camera.getAspectRatio();

			if ( e.alt && e.button3 )
			{
				automatic_scale = false;
				if(coordinate.y_scale_zoom < 1.0f) 
				{
					if(e.lmouse.y > e.mouse.y) 
						coordinate.y_scale_zoom = 0.93f*coordinate.y_scale_zoom;
					else 
						coordinate.y_scale_zoom = coordinate.y_scale_zoom/0.93f;
					if(coordinate.y_scale_zoom < 0.0001f) coordinate.y_scale_zoom = 0.0001f;
					coordinate.SetYSize(1.0f/coordinate.y_scale_zoom);
				}
				else 
				{
					float s = e.mouse.y - e.lmouse.y;
					coordinate.y_scale_zoom += s*coordinate.y_scale_zoom;
					coordinate.SetYSize(1.0f);
				}
			}
			else if(e.button1 && e.alt)
			{
				SrVec center = camera.getCenter();
				center.y +=  (e.lmouse.y - e.mouse.y)*coordinate.y_scale;
				camera.setCenter(center.x, center.y, center.z);
				SrVec eye = camera.getEye();
				eye.y += (e.lmouse.y - e.mouse.y)*coordinate.y_scale;
				camera.setEye(eye.x, eye.y, eye.z);
			}
			/*else if ( e.alt && e.button3 )
			{ 
				float amount = dx;
				SrVec cameraPos(camera.eye);
				SrVec targetPos(camera.center);
				SrVec diff = targetPos - cameraPos;
				float distance = diff.len();
				diff.normalize();

				if (amount >= distance)
					amount = distance - .000001f;

				SrVec diffVector = diff;
				SrVec adjustment = diffVector * distance * amount;
				cameraPos += adjustment;
				camera.eye = cameraPos;			
			}*/
			else if ( e.alt && e.button2 )
			{ 
				camera.apply_translation_from_mouse_motion ( e.lmouse.x, e.lmouse.y, e.mouse.x, e.mouse.y );
			}
			//rotation with mouse doesn't seem useful in this case?
			/*else if (?????) 
			{ 
 				float deltaX = -(e.mouseCoord.x - e.origMouse.x) / e.width;
				float deltaY = -(e.mouseCoord.y -  e.origMouse.y) / e.height;
				if (deltaX == 0.0 && deltaY == 0.0)
					return 1;

				SrVec origUp = e.origUp;
				SrVec origCenter = e.origCenter;
				SrVec origCamera = e.origEye;

				SrVec dirX = origUp;
				SrVec  dirY;
				dirY.cross(origUp, (origCenter - origCamera));
				dirY /= dirY.len();

				SrVec camera_p = rotatePoint(origCamera, origCenter, dirX, -deltaX * float(M_PI));
				camera_p = rotatePoint(camera_p, origCenter, dirY, deltaY * float(M_PI));

				camera.eye = camera_p;
			}*/
		}
		else if ( e.type==SrEvent::EventRelease )
		{ 

		}
	}

	return res;
 }

