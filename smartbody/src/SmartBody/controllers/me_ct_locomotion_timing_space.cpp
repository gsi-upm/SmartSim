/*
 *  me_ct_locomotion_timing_space.cpp - part of SmartBody-lib's Motion Engine
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

#include "controllers/me_ct_locomotion_timing_space.hpp"

#include "sbm/sbm_character.hpp"
#include "sbm/gwiz_math.h"
#include "limits.h"
#include <vhcl_log.h>

const char* MeCtLocomotionTimingSpace::TYPE = "MeCtLocomotionTimingSpace";

bool check_timing_space(MeCtLocomotionTimingSpace* space1, MeCtLocomotionTimingSpace* space2);
MeCtLocomotionTimingSpace* get_blended_timing_space(MeCtLocomotionTimingSpace* space1, float weight1, MeCtLocomotionTimingSpace space2, float weight2);

/** Constructor */
MeCtLocomotionTimingSpace::MeCtLocomotionTimingSpace() {

	frame_num = 0;
	mode = 0;
}

/** Destructor */
MeCtLocomotionTimingSpace::~MeCtLocomotionTimingSpace() {

}

int MeCtLocomotionTimingSpace::set_mode(int mode)
{
	if(mode != 1 && mode != -1) 
	{
		LOG("Error: timing space mode must be either 1 or -1");
	}
	else this->mode = mode;
	return this->mode;
}

int MeCtLocomotionTimingSpace::get_mode()
{
	return mode;
}

int MeCtLocomotionTimingSpace::add_ref_time_name(char* ref_name)
{
	_ref_time_name.push() = ref_name;
	_ref_time.push() = -1.0f;
	return _ref_time_name.size();
}

void MeCtLocomotionTimingSpace::set_ref_time(int index, float frame)
{
	if(index > _ref_time.size())
	{
		LOG("Error: MeCtLocomotionTimingSpace::set_ref_time(), index out of range");
	}
	_ref_time.set(index, frame);
}

float MeCtLocomotionTimingSpace::get_ref_time(char* ref_name)
{
	for(int i = 0; i < _ref_time.size(); ++i)
	{
		if(strcmp(ref_name, _ref_time_name.get(i)) == 0) return _ref_time.get(i);
	}
	LOG("Error: failed to get ref time.");
	return -1.0f;
}

float MeCtLocomotionTimingSpace::get_normalized_frame(float frame)
{
	if(frame < 0.0f)
	{
		int iter = (int)(-frame/(frame_num));
		if(iter != (-frame/(frame_num))) ++iter;
		frame += iter*(frame_num);
	}
	else
	{
		int iter = (int)(frame/(frame_num));
		frame -= iter*(frame_num);
	}
	return frame;
}

int MeCtLocomotionTimingSpace::get_next_ref_time_index(int index)
{
	int next_ref = -1;
	if(mode == 1)
	{
		if(index == _ref_time.size()-1) next_ref = 0;
		else next_ref = index+1;
	}
	else
	{
		if(index == 0) next_ref = _ref_time.size()-1;
		else next_ref = index -1;
	}
	return next_ref;
}

float MeCtLocomotionTimingSpace::get_section_length(int ref1, int ref2)
{
	float f1 = _ref_time.get(ref1);
	float f2 = _ref_time.get(ref2);
	float len = 0.0f;
	if(mode == 1)
	{
		if(f1 > f2)
			len = frame_num - (f1-f2);
		else len = f2-f1;
	}

	else
	{
		if(f1 < f2)
			len = frame_num - (f2-f1);
		else len = f1-f2;
	}
	return len;
}

float MeCtLocomotionTimingSpace::get_section_length(float space_value1, float space_value2)
{
	float f1 = get_virtual_frame(space_value1);
	float f2 = get_virtual_frame(space_value2);
	float len = 0.0f;
	if(mode == 1)
	{
		if(f1 > f2)
			len = frame_num - (f1-f2);
		else len = f2-f1;
	}

	else
	{
		if(f1 < f2)
			len = frame_num - (f2-f1);
		else len = f1-f2;
	}
	return len;
}

float MeCtLocomotionTimingSpace::get_section_length_by_frame(float frame1, float frame2)
{
	frame1 = get_normalized_frame(frame1);
	frame2 = get_normalized_frame(frame2);
	if(frame2 < frame1) frame2 = frame_num+frame2;
	return (frame2-frame1);
}

float MeCtLocomotionTimingSpace::get_ref_time(int index)
{
	if(index < 0 || index > _ref_time.size()-1) 
	{
		LOG("Error: wrong reference time index: %d", index);
		return 0;
	}
	if(mode == -1) return frame_num - _ref_time.get(index);
	return _ref_time.get(index);
}

char* MeCtLocomotionTimingSpace::get_ref_time_name(int index)
{
	return _ref_time_name.get(index);
}

float MeCtLocomotionTimingSpace::get_virtual_frame(float space_value)
{
	if(space_value < 0.0f || space_value >= _ref_time.size())
	{
		LOG("Error: invalid space_value: %f", space_value);
		return -1.0f;
	}
	int space_index = 0;
	for(int i = 0; i < int(space_value); ++i)
	{
		space_index = get_next_ref_time_index(space_index);
	}

	//start & end of space
	float start = get_ref_time(space_index);
	float end = get_ref_time(get_next_ref_time_index(space_index));

	float framecount = 0;
	float frame = -1.0f;
	if(end < start)
	{
		framecount += frame_num - start + end;
	}
	else framecount += end - start;
	frame = framecount*(space_value - (int)space_value)+start;

	//if(frame > frame_num-1 && frame < frame_num)
	//	int y = 0;
	frame = get_normalized_frame(frame);
	/*while(true)
	{
		if(frame < 0)
			frame += frame_num;
		else if(frame > frame_num)
			frame -= frame_num;
		else break;
	}*/
	if(mode == -1) frame = frame_num - frame;
	return frame;
}

MeCtLocomotionTimingSpace* MeCtLocomotionLimbAnim::get_timing_space()
{
	return &timing_space;
}

float MeCtLocomotionTimingSpace::get_space_value(float frame)
{
	if(frame < 0.0f || frame > frame_num)
	{
		LOG("Error: invalid input frame: %f", frame);
	}

	float space_value = 0.0f;
	float start = -1;
	float end = -1;
	float frame_count = 0;
	float frame_inspace = 0.0f;
	int i = 0;
	for(; i < _ref_time.size(); ++i)
	{
		start = get_ref_time(i);
		end = (i >= _ref_time.size()-1)? get_ref_time(0):get_ref_time(i+1);
		if(end < start)
		{
			if(frame >= start || frame >= 0.0f && frame < end) 
			{
				frame_count = frame_num - start + end;
				frame_inspace = frame >= start? (frame - start):(frame_num - start + frame);
				break;
			}
		}
		else
		{
			if(frame >= start && frame < end) 
			{
				frame_count = end - start;
				frame_inspace = frame - start;
				break;
			}
		}
	}

	space_value = i + frame_inspace/frame_count;

	if(space_value >= _ref_time.size()) 
		space_value -= _ref_time.size();

	return space_value;
}

SrArray<float>* MeCtLocomotionTimingSpace::get_ref_time_p()
{
	return &_ref_time;
}

float MeCtLocomotionTimingSpace::get_frame_num()
{
	return frame_num;
}

void MeCtLocomotionTimingSpace::set_frame_num(float num)
{
	frame_num = num;
}

int MeCtLocomotionTimingSpace::get_ref_time_num()
{
	return _ref_time_name.size();
}

bool MeCtLocomotionTimingSpace::check_with_timing_space(MeCtLocomotionTimingSpace* space)
{
	if(space->get_ref_time_num() != get_ref_time_num()) return false;
	for(int i = 0; i < get_ref_time_num(); ++i)
	{
		if(strcmp(get_ref_time_name(i), space->get_ref_time_name(i)) != 0) return false;
	}
	return true;
}

void MeCtLocomotionTimingSpace::set_lower_bound(float lower_bound)
{
	this->lower_bound = lower_bound;
}

float MeCtLocomotionTimingSpace::get_lower_bound()
{
	if(mode == -1) return frame_num - lower_bound;
	return lower_bound;
}

void MeCtLocomotionTimingSpace::clean_ref_time()
{
	this->set_frame_num(0);
	this->set_lower_bound(0);
	this->set_mode(1);
}

void MeCtLocomotionTimingSpace::copy_to(MeCtLocomotionTimingSpace* space)
{
	if(space == NULL) 
	{
		printf("Error, timing space ptr is NULL.Copy can't be finished");
		return;
	}
	// continue.......
}

void MeCtLocomotionTimingSpace::copy_from(MeCtLocomotionTimingSpace* space)
{
	_ref_time.capacity(0);
	_ref_time_name.capacity(0);
	set_frame_num(space->get_frame_num());
	for(int i = 0; i < space->get_ref_time_num(); ++i)
	{
		add_ref_time_name(space->get_ref_time_name(i));
		set_ref_time(i, space->get_ref_time(i));
	}
	set_lower_bound(space->get_lower_bound());
	set_mode(space->get_mode());
}

MeCtLocomotionTimingSpace* get_blended_timing_space(MeCtLocomotionTimingSpace* space, MeCtLocomotionTimingSpace* space1, MeCtLocomotionTimingSpace* space2, float weight)
{
	space->clean_ref_time();

	if(space1 && !space2)
	{
		space->copy_from(space1);
		return space;
	}
	else if(!space1 && space2)
	{
		space->copy_from(space2);
		return space;
	}
	else if(!space1 && !space2)
	{

	}

	float weight1 = weight;
	float weight2 = (1-weight);
	float s1, s2, t = 0.0f;
	int pres1 = 0;
	int pres2 = 0;
	int next1 = 0;
	int next2 = 0;
	space->set_frame_num(space1->get_frame_num()*weight1 + space2->get_frame_num()*weight2);
	space->set_lower_bound(space1->get_lower_bound()*weight1 + space2->get_lower_bound()*weight2);

	float t1 = space1->get_ref_time(0);
	float t2 = space2->get_ref_time(0);
	if(t1 < space1->get_lower_bound()) t1 += space1->get_frame_num();
	if(t2 < space2->get_lower_bound()) t2 += space2->get_frame_num();
	t = weight1*t1 + weight2*t2;
	if(t >= space->get_frame_num()) t -= space->get_frame_num();
	space->set_ref_time(0, t);
	pres1 = 0;
	pres2 = 0;
	for(int i = 0; i < space1->get_ref_time_num()-1; ++i)
	{
		next1 = space1->get_next_ref_time_index(pres1);
		next2 = space2->get_next_ref_time_index(pres2);
		s1 = space1->get_section_length(pres1, next1);
		s2 = space2->get_section_length(pres2, next2);
		t += weight1*s1+weight2*s2;
		if(t >= space->get_frame_num()) t -= space->get_frame_num();
		space->set_ref_time(i+1, t);
		pres1 = next1;
		pres2 = next2;
	}
	return space;
}

bool check_timing_space(MeCtLocomotionTimingSpace* space1, MeCtLocomotionTimingSpace* space2)
{
	return space1->check_with_timing_space(space2);
}


void MeCtLocomotionTimingSpace::print_info()
{
	LOG("\nnew MeCtLocomotionTimingSpace");
	LOG("\nframe num: %f", frame_num);
	for(int i = 0; i < _ref_time.size(); ++i)
	{
		LOG("\n[%d]: frame:%f", i, _ref_time.get(i));
	}
}