/*
 *  GlChartViewArchive.cpp - part of SmartBody-lib's Test Suite
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

#include "GlChartViewArchive.hpp"
#include <string.h>
#include <sr/sr_euler.h>
#include <vhcl_math.h>
#include <stdlib.h>

GlChartViewSeries::GlChartViewSeries()
{
	data_type = CHART_DATA_TYPE_UNKNOWN;
	max_size = 0;
	current_ind = -1;
	size = 0;
	bold = false;
	dt = 0.01f;
};

GlChartViewSeries::~GlChartViewSeries()
{
	Clear();
};

void GlChartViewSeries::Reset()
{
	current_ind = -1;
	size = 0;
}

void GlChartViewSeries::SetColor(int index, SrVec& color)
{
	if(index == 1) this->color_x = color;
	else if(index == 2) this->color_y = color;
	else if(index == 3) this->color_z = color;
	else if(index == 4) this->color_w = color;
}

SrVec GlChartViewSeries::GetEulerFromQuaternion(SrQuat& quat)
{
	const int rotType = 132;
	SrVec euler;
	//euler.x = atan2(2.0f*(quat.w*quat.x+quat.y*quat.z), 1.0f-2.0f*(quat.x*quat.x+quat.y*quat.y))*180.0f/3.14159265f;
	//euler.y = asin(2.0f*(quat.w*quat.y - quat.z*quat.x))*180.0f/3.14159265f;
	//euler.z = atan2(2.0f*(quat.w*quat.z + quat.x*quat.y), 1.0f-2.0f*(quat.y*quat.y+quat.z*quat.z))*180.0f/3.14159265f;
	SrMat mat;
	quat.get_mat(mat);		
	sr_euler_angles(rotType, mat, euler.x, euler.y, euler.z);	
	euler = euler*vhcl::RAD_TO_DEG();
	return euler;
}

SrVec GlChartViewSeries::GetSwingTwistFromQuaternion( SrQuat& quat )
{
	// feng : add swing twist conversion
	const float EPSILON6 = 0.0000001f;	
	
	SrQuat q;
	if( quat.w < 0.0 )	{
		q = SrQuat(-quat.w,-quat.x,-quat.y,-quat.z);
	}
	else	{
		q = quat;
	}

	float gamma = atan2( q.z, q.w );
	float beta = atan2( sqrt( q.x*q.x + q.y*q.y ), sqrt( q.z*q.z + q.w*q.w ) );
	float sinc = 1.0f;
	if( beta > EPSILON6 )	{
		sinc = sin( beta )/beta;
	}
	float s = sin( gamma );
	float c = cos( gamma );
	float sinc2 = 2.0f / sinc;
	float swing_x = sinc2 * ( c * q.x - s * q.y );
	float swing_y = sinc2 * ( s * q.x + c * q.y);
	float twist = 2.0f * gamma;
	
	// to degree instead of radian
	return( SrVec( ( swing_x )* 57.295779513082323f, ( swing_y )* 57.295779513082323f, ( twist )* 57.295779513082323f ) );

}

SrVec GlChartViewSeries::GetColor(int index)
{
	if(index == 1) return color_x;
	if(index == 2) return color_y;
	if(index == 3) return color_z;
	if(index == 4) return color_w;

	else return SrVec(0,0,0);
}

void GlChartViewSeries::Clear()
{
	x.clear();
	y.clear();
	z.clear();
	w.clear();
	Reset();
}

int GlChartViewSeries::GetBufferIndex()
{
	return buffer_index;
}

void GlChartViewSeries::SetRGBColor()
{
	color_x.x = 1.0f;
	color_x.y = 0.0f;
	color_x.z = 0.0f;

	color_y.x = 0.0f;
	color_y.y = 1.0f;
	color_y.z = 0.0f;

	color_z.x = 0.0f;
	color_z.y = 0.0f;
	color_z.z = 1.0f;

	color_w.x = 1.0f;
	color_w.y = 1.0f;
	color_w.z = 1.0f;
}

void GlChartViewSeries::SetColorOnBufferIndex()
{
	SetColorOnBufferIndex(buffer_index);
}

void GlChartViewSeries::SetColorOnBufferIndex(int index)
{
	srand(index);
	while(true)
	{
		color_x.x = (float)rand()/(float)RAND_MAX;
		color_x.y = (float)rand()/(float)RAND_MAX;
		color_x.z = (float)rand()/(float)RAND_MAX;
		if(color_x.x < 0.4f && color_x.y < 0.4f && color_x.z < 0.4f) continue;
		break;
	}
	while(true)
	{
		color_y.x = (float)rand()/(float)RAND_MAX;
		color_y.y = (float)rand()/(float)RAND_MAX;
		color_y.z = (float)rand()/(float)RAND_MAX;
		if(color_y.x < 0.4f && color_y.y < 0.4f && color_y.z < 0.4f) continue;
		SrVec t = color_x-color_y;
		if(t.len()<0.3f) continue;
		break;
	}
	while(true)
	{
		color_z.x = (float)rand()/(float)RAND_MAX;
		color_z.y = (float)rand()/(float)RAND_MAX;
		color_z.z = (float)rand()/(float)RAND_MAX;
		if(color_z.x < 0.4f && color_z.y < 0.4f && color_z.z < 0.4f) continue;
		SrVec t = color_z-color_x;
		if(t.len()<0.3f) continue;
		break;
	}
	while(true)
	{
		color_w.x = (float)rand()/(float)RAND_MAX;
		color_w.y = (float)rand()/(float)RAND_MAX;
		color_w.z = (float)rand()/(float)RAND_MAX;
		if(color_w.x < 0.4f && color_w.y < 0.4f && color_w.z < 0.4f) continue;
		SrVec t = color_y-color_w;
		if(t.len()<0.3f) continue;
		break;
	}
	
}

void GlChartViewSeries::SetBufferIndex(int index)
{
	buffer_index = index;
}

#ifdef WIN32
__forceinline int GlChartViewSeries::CheckIndex(int index)
#else
int GlChartViewSeries::CheckIndex(int index)
#endif
{
	if(max_size > 0) 
	{
		index = current_ind-index;
		if(index < 0) index += max_size;
	}
	return index;
}

float GlChartViewSeries::GetValue(int index)
{
	index = CheckIndex(index);
	return x[index];
};

SrVec2 GlChartViewSeries::GetVec2(int index)
{
	index = CheckIndex(index);
	return SrVec2(x[index], y[index]);
}

SrVec GlChartViewSeries::GetVec3(int index)
{
	index = CheckIndex(index);
	return SrVec(x[index], y[index], z[index]);
}

SrVec GlChartViewSeries::GetEuler(int index)
{
	index = CheckIndex(index);
	SrQuat quat(x[index], y[index], z[index], w[index]);
	return GetEulerFromQuaternion(quat);
}


SrVec GlChartViewSeries::GetSwingTwist( int index )
{
	index = CheckIndex(index);
	SrQuat quat(x[index], y[index], z[index], w[index]);
	return GetSwingTwistFromQuaternion(quat);
}

SrQuat GlChartViewSeries::GetQuat(int index)
{
	index = CheckIndex(index);
	return SrQuat(x[index], y[index], z[index], w[index]);
}

void GlChartViewSeries::Push(float x)
{
	if(current_ind == max_size-1) 
	{
		current_ind = 0;
	}
	else ++current_ind;
	if(size < max_size) ++size;
	this->x[current_ind] = x;
}

void GlChartViewSeries::Push(float x, float y)
{
	if(current_ind == max_size-1) 
	{
		current_ind = 0;
	}
	else ++current_ind;
	if(size < max_size) ++size;
	this->x[current_ind] = x;
	this->y[current_ind] =  y;
}

void GlChartViewSeries::Push(float x, float y, float z)
{
	if(current_ind == max_size-1) 
	{
		current_ind = 0;
	}
	else ++current_ind;
	if(size < max_size) ++size;
	this->x[current_ind] = x;
	this->y[current_ind] = y;
	this->z[current_ind] = z;
}

void GlChartViewSeries::Push(float x, float y, float z, float w)
{
	if(current_ind == max_size-1) 
	{
		current_ind = 0;
	}
	else ++current_ind;
	if(size < max_size) ++size;
	this->x[current_ind] = x;
	this->y[current_ind] = y;
	this->z[current_ind] = z;
	this->w[current_ind] = w;
}

void GlChartViewSeries::SetMaxSize(int max)
{
	if(max_size == max || max <= 0) return;
	int r;
	int t_old;
	int t_new;

	if(size == 0)
	{
		this->x.resize(max);
		this->y.resize(max);
		this->z.resize(max);
		this->w.resize(max);
		this->max_size = max;
		return;
	}

	r = max - current_ind;

	if(r > 0)
	{
		if(max > max_size)
		{
			this->x.resize(max);
			this->y.resize(max);
			this->z.resize(max);
			this->w.resize(max);
		}
	
		int start = this->max_size-1;
		int end =  max > this->max_size? current_ind+1: current_ind + 1 + this->max_size - max;
		for(int i = start, j = max-1; i >= end; --i, --j)
		{
			t_old = i;
			t_new = j;
			if(t_old<0 || t_old>=this->max_size || t_new<0 || t_new>=max)
			{
				int y = 0;
			}
			x[t_new] = x[t_old];
			y[t_new] = y[t_old];
			z[t_new] = z[t_old];
			w[t_new] = w[t_old];
		}
		this->max_size = max;
		if(this->size > max) this->size = max;
		if(max_size > max)
		{
			this->x.resize(max);
			this->y.resize(max);
			this->z.resize(max);
			this->w.resize(max);
		}
	}

	else
	{
		for(size_t i = 0; i < (size_t) max; ++i)
		{
			t_old = -r+i;
			t_new = i;
			if(t_old<0 || t_old>=this->max_size || t_new<0 || t_new>=this->max_size)
			{
				int y = 0;
			}
			x[t_new] = x[t_old];
			y[t_new] = y[t_old];
			z[t_new] = z[t_old];
			w[t_new] = w[t_old];
		}
		current_ind = max-1;
		this->size = max;
		this->max_size = max;
		this->x.resize(max);
		this->y.resize(max);
		this->z.resize(max);
		this->w.resize(max);
	}

}

void GlChartViewSeries::Push(SrVec& quat)
{
	Push(quat.x, quat.y, quat.z);
}


void GlChartViewSeries::Push(SrVec2& quat)
{
	Push(quat.x, quat.y);
}

void GlChartViewSeries::Push(SrQuat& quat)
{
	Push(quat.x, quat.y, quat.z, quat.w);
}

void GlChartViewSeries::SetLast(float x)
{
	this->x[current_ind] = x;
}

void GlChartViewSeries::SetLast(float x, float y, float z)
{
	this->x[current_ind] = x;
	this->y[current_ind] = y;
	this->z[current_ind] = z;
}

void GlChartViewSeries::SetLast(float x, float y, float z, float w)
{
	this->x[current_ind] = x;
	this->y[current_ind] = y;
	this->z[current_ind] = z;
	this->w[current_ind] = w;
}

void GlChartViewSeries::SetLast(SrVec& quat)
{
	this->x[current_ind] = quat.x;
	this->y[current_ind] = quat.y;
	this->z[current_ind] = quat.z;
}

void GlChartViewSeries::SetLast(SrQuat& quat)
{
	this->x[current_ind] =quat.x;
	this->y[current_ind] = quat.y;
	this->z[current_ind] = quat.z;
	this->w[current_ind] = quat.w;
}



GlChartViewArchive::GlChartViewArchive()
{

}

GlChartViewArchive::~GlChartViewArchive()
{
	GlChartViewSeries* series = NULL;
	int num = series_list.size();
	for(int i = 0; i < num; ++i)
	{
		series = series_list[i];
		series->Clear();
		delete series;
	}
	series_list.clear();

}

void GlChartViewArchive::NewSeries(const char* title, int type, int buffer_index)
{
	GlChartViewSeries* series = new GlChartViewSeries();
	series->title = title;
	series->data_type = type;
	series->SetBufferIndex(buffer_index);
	if(series_list.size() == 0) series->SetRGBColor();
	else series->SetColorOnBufferIndex();
	series->SetMaxSize(0);
	series_list.push_back(series);
}

void GlChartViewArchive::AddSeries(GlChartViewSeries* series)
{
	series_list.push_back(series);
}

void GlChartViewArchive::DeleteSeries(const char* title)
{
	GlChartViewSeries* series = NULL;
	for (std::vector<GlChartViewSeries*>::iterator iter = series_list.begin();
		 iter != series_list.end();
		 iter++)
	{
		series = (*iter);
		if(strcmp(series->title.c_str(), title) == 0)
		{
			series->Clear();
			delete series;
			series_list.erase(iter);
			break;
		}
	}
}

void GlChartViewArchive::DeleteSeries(int index)
{
	GlChartViewSeries* series = NULL;
	int count = 0;
	for (std::vector<GlChartViewSeries*>::iterator iter = series_list.begin();
		 iter != series_list.end();
		 iter++)
	{
		if (count == index)
		{
			series = (*iter);
			series->Clear();
			delete series;
			series_list.erase(iter);
			break;
		}
	}
	
}

void GlChartViewArchive::ClearSeries()
{
	GlChartViewSeries* series = NULL;
	int num = series_list.size();
	for(int i = 0; i < num; ++i)
	{
		series = series_list[i];
		series->Clear();
		delete series;
	}
	series_list.clear();
}

GlChartViewSeries* GlChartViewArchive::GetLastSeries()
{
	return series_list[series_list.size()-1];
}

int GlChartViewArchive::GetSeriesCount()
{
	return series_list.size();
}

GlChartViewSeries* GlChartViewArchive::GetSeries(int index)
{
	return series_list[index];
}

GlChartViewSeries* GlChartViewArchive::GetSeries(const char* title)
{
	for(size_t i = 0; i < series_list.size(); ++i)
	{
		if(strcmp(series_list[i]->title.c_str(), title) == 0) 
			return series_list[i];
	}
	return NULL;
}

void GlChartViewArchive::Update(SrBuffer<float>& buffer)
{
	GlChartViewSeries* series = NULL;
	float x, y, z, w;
	int buffer_index = 0;
	for(size_t i = 0; i < series_list.size(); ++i)
	{
		series = series_list[i];
		buffer_index = series->GetBufferIndex();

		if(series->data_type >= CHART_DATA_TYPE_VALUE) x = buffer[buffer_index];
		if(series->data_type >= CHART_DATA_TYPE_VEC2) y = buffer[buffer_index+1];
		if(series->data_type >= CHART_DATA_TYPE_VEC) z = buffer[buffer_index+2];
		if(series->data_type >= CHART_DATA_TYPE_QUAT) w = buffer[buffer_index+3];
		 
		if(series->data_type == CHART_DATA_TYPE_VALUE) 
		{
			series->Push(x);
		}
		if(series->data_type == CHART_DATA_TYPE_VEC2) series->Push(x, y);
		if(series->data_type == CHART_DATA_TYPE_VEC) series->Push(x, y, z);
		if(series->data_type == CHART_DATA_TYPE_QUAT) series->Push(x, y, z, w);
	}
}
