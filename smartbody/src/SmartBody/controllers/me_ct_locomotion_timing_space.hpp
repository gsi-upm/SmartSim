/*
 *  me_locomotion_timing_space.hpp - part of SmartBody-lib's Motion Engine
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

#ifndef ME_CT_LOCOMOTION_TIMING_SPACE_HPP
#define ME_CT_LOCOMOTION_TIMING_SPACE_HPP

#include "sr/sr_array.h"

#pragma once

class MeCtLocomotionTimingSpace{
public:
	// Public Constants
	static const char* TYPE;

protected:
	// Data
	SrArray<float> _ref_time;
	SrArray<char*> _ref_time_name;
	float frame_num;
	float lower_bound;
	int mode;

public:
	MeCtLocomotionTimingSpace();
	~MeCtLocomotionTimingSpace();

	int set_mode(int mode);
	int get_mode();

	void clean_ref_time();

	void set_lower_bound(float lower_bound);
	float get_lower_bound();

	int add_ref_time_name(char* ref_name);
	void set_ref_time(int index, float frame);

	float get_ref_time(char* ref_name);
	float get_ref_time(int index);

	char* get_ref_time_name(int index);

	float get_virtual_frame(float space_value); // return the virtual frame number
	float get_space_value(float frame);

	int get_ref_time_num();
	bool check_with_timing_space(MeCtLocomotionTimingSpace* space);

	int get_next_ref_time_index(int index);
	float get_section_length(int ref1, int ref2);
	float get_section_length(float space_value1, float space_value2);
	float get_section_length_by_frame(float frame1, float frame2);

	float get_normalized_frame(float frame);

	void copy_to(MeCtLocomotionTimingSpace* space);
	void copy_from(MeCtLocomotionTimingSpace* space);

	//temp function for test, to be deleted......
	void print_info();

public:
	SrArray<float>* get_ref_time_p();
	void set_frame_num(float num);
	float get_frame_num();

};

MeCtLocomotionTimingSpace* get_blended_timing_space(MeCtLocomotionTimingSpace* space, MeCtLocomotionTimingSpace* space1, MeCtLocomotionTimingSpace* space2, float weight);

#endif // ME_CT_LOCOMOTION_TIMING_SPACE_HPP
