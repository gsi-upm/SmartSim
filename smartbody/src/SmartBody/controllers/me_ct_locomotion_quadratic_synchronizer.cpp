/*
 *  me_ct_locomotion_quadratic_synchronizer.hpp - part of SmartBody-lib's Motion Engine
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

#include "controllers/me_ct_locomotion_quadratic_synchronizer.hpp"
#include <stdio.h>

MeCtLocomotionQuadraticSynchronizer::MeCtLocomotionQuadraticSynchronizer()
{
	time_flag = false;
	target_time = 0.0f;
	time = 0.0f;

	distance_flag = false;
	target_distance = 0.0f;
	distance = 0.0f;

	acceleration_flag = false;
	target_acceleration = 0.0f;
	acceleration = 0.0f;

	speed_flag = false;
	target_speed = 0.0f;
	speed = 0.0f;

	delta_distance = 0.0f;
}

MeCtLocomotionQuadraticSynchronizer::~MeCtLocomotionQuadraticSynchronizer()
{
	
}

void MeCtLocomotionQuadraticSynchronizer::set_target_flag(int ind1)
{
	set_target_flag(ind1, QUAD_SYNC_NONE);
}

void MeCtLocomotionQuadraticSynchronizer::set_target_flag(int ind1, int ind2)
{
	time_flag = false;
	distance_flag = false;
	acceleration_flag = false;
	speed_flag = false;

	if(ind1 == QUAD_SYNC_DISTANCE || ind2 == QUAD_SYNC_DISTANCE) distance_flag = true;
	if(ind1 == QUAD_SYNC_TIME || ind2 == QUAD_SYNC_TIME) time_flag = true;
	if(ind1 == QUAD_SYNC_ACCELERATION || ind2 == QUAD_SYNC_ACCELERATION) acceleration_flag = true;
	if(ind1 == QUAD_SYNC_SPEED || ind2 == QUAD_SYNC_SPEED) speed_flag = true;
	if(ind1 != QUAD_SYNC_NONE) primary_ind = ind1;
	else if(ind2 != QUAD_SYNC_NONE) primary_ind = ind2;
	else printf("MeCtLocomotionQuadraticSynchronizer::set_target_flag(): Error: no valid flag set");
}

void MeCtLocomotionQuadraticSynchronizer::set_target(int ind, float target_value)
{
	if(ind == QUAD_SYNC_DISTANCE)
	{
		target_distance = target_value;
	}
	else if(ind == QUAD_SYNC_TIME)
	{
		target_time = target_value;
	}
	if(ind == QUAD_SYNC_SPEED)
	{
		target_speed = target_value;
	}
	if(ind == QUAD_SYNC_ACCELERATION)
	{
		target_acceleration = target_value;
	}
}

void MeCtLocomotionQuadraticSynchronizer::update_target(int ind, float target_value)
{
	if(ind == QUAD_SYNC_DISTANCE)
	{
		if(distance_flag == false) printf("MeCtLocomotionQuadraticSynchronizer::update_target(): Error, target not exist");
		else target_distance = target_value;
	}
	else if(ind == QUAD_SYNC_TIME)
	{
		if(time_flag == false) printf("MeCtLocomotionQuadraticSynchronizer::update_target(): Error, target not exist");
		else target_time = target_value;
	}
	else if(ind == QUAD_SYNC_SPEED)
	{
		if(speed_flag == false) printf("MeCtLocomotionQuadraticSynchronizer::update_target(): Error, target not exist");
		else target_speed = target_value;
	}
	else if(ind == QUAD_SYNC_ACCELERATION)
	{
		if(acceleration_flag == false) printf("MeCtLocomotionQuadraticSynchronizer::update_target(): Error, target not exist");
		else target_acceleration = target_value;
	}
}

void MeCtLocomotionQuadraticSynchronizer::update(float delta_time)
{
	if(time_flag && speed_flag) update_for_time_speed(delta_time);
	else if(time_flag && distance_flag) update_for_time_distance(delta_time);
	else if(speed_flag && distance_flag) update_for_speed_distance(delta_time);
	else if(distance_flag) update_on_distance(delta_time);
}

void MeCtLocomotionQuadraticSynchronizer::update_on_distance(float delta_time)
{
	acceleration = 0.0f;
	if(target_distance > 0.0f) 
	{
		//acceleration = max_acceleration;
	}
	else if(target_distance < 0.0f) 
		acceleration = -max_acceleration;
	delta_distance = speed * delta_time + 0.5f * acceleration * delta_time * delta_time;
	speed += acceleration * delta_time;
	if(target_distance < 0.0f && delta_distance < target_distance
	|| target_distance > 0.0f && delta_distance > target_distance)
	{
		delta_distance = target_distance;
	}
}

void MeCtLocomotionQuadraticSynchronizer::update_for_time_speed(float delta_time)
{
	acceleration = (target_speed - speed)/(target_time - delta_time);
	//time += delta_time;
	speed += delta_time * acceleration;
	if(primary_ind == QUAD_SYNC_SPEED)
	{
		if(acceleration > 0.0f && speed > target_speed) 
			speed = target_speed;
		else if(acceleration < 0.0f && speed < target_speed)
			speed = target_speed;
	}
	else if(primary_ind == QUAD_SYNC_TIME && time> target_time) 
		speed = target_speed;
}

void MeCtLocomotionQuadraticSynchronizer::update_for_time_distance(float delta_time)
{
	float t_time = target_time - delta_time;
	acceleration = 2*(target_distance - distance - speed * t_time) / t_time * t_time;
	if(acceleration > 0.0f && acceleration > max_acceleration) acceleration = max_acceleration;
	if(acceleration < 0.0f && acceleration < -max_acceleration) acceleration = -max_acceleration;
	delta_distance = speed * delta_time + 0.5f * acceleration * delta_time * delta_time;
	distance += delta_distance;
	speed += acceleration * delta_time;
	//time += delta_time;
	
	//if(primary_ind == QUAD_SYNC_TIME && time > target_time) time = target_time;
	if(primary_ind == QUAD_SYNC_DISTANCE)
	{
		if(time > 0.0f && distance > target_distance) distance = target_distance;
		if(time < 0.0f && distance < target_distance) distance = target_distance;
	}
}

void MeCtLocomotionQuadraticSynchronizer::set_max_acceleration(float value)
{
	max_acceleration = value;
}

void MeCtLocomotionQuadraticSynchronizer::update_for_speed_distance(float delta_time)
{
	float a = 0.0f;
	if(target_distance != 0.0f && speed == 0.0f && acceleration == 0.0f)
	{
		if(target_distance > 0.0f) a = max_acceleration;
		else if(target_distance < 0.0f) a = -max_acceleration;
		if(delta_time >= 2*sqrt(target_distance/a))
		{
			delta_distance = target_distance;
			speed = 0.0f;
			acceleration = 0.0f;
			return;
		}
	}
	if(target_distance < 0.0f && speed > 0.0f || target_distance > 0.0f && speed < 0.0f)
	{
		if(target_distance > 0.0f) a = max_acceleration;
		else if(target_distance < 0.0f) a = -max_acceleration;
		float v = speed + a * delta_time;
		if(speed > 0.0f && v < 0.0f || speed < 0.0f && v > 0.0f)
		{
			delta_time = -speed / a;
			if(delta_time<0.0f) printf("time");
			delta_distance = speed * delta_time + 0.5f * a * delta_time * delta_time;
			speed = 0.0f;
			acceleration = a;
		}
		else 
		{
			delta_distance = speed * delta_time + 0.5f * a * delta_time * delta_time;
			speed += a * delta_time;
			acceleration = a;
		}
		if(target_distance < 0.0f && delta_distance < target_distance
		|| target_distance > 0.0f && delta_distance > target_distance)
		{
			delta_distance = target_distance;
		}

	}
	else if(target_distance == 0.0f)
	{
		delta_distance = speed * delta_time;
		acceleration = 0.0f;
	}
	else
	{
		a = -0.5f * speed * speed / target_distance;
		if(a < -max_acceleration || a > max_acceleration)
		{
			if(a < -max_acceleration) a = -max_acceleration;
			else a = max_acceleration;
			delta_distance = speed * delta_time + 0.5f * a * delta_time * delta_time;
			speed += a * delta_time;
			acceleration = a;
			if(target_distance < 0.0f && delta_distance < target_distance
			|| target_distance > 0.0f && delta_distance > target_distance)
			{
				speed = 0.0f;
				delta_distance = target_distance;
				acceleration = 0.0f;
			}
		}
		else
		{
			if(target_distance>0.0f) a = max_acceleration;
			else if(target_distance<0.0f) a = -max_acceleration;
			delta_distance = speed * delta_time + 0.5f * a * delta_time * delta_time;
			speed += a * delta_time;
			acceleration = a;
			if(target_distance < 0.0f && delta_distance < target_distance / 2
			|| target_distance > 0.0f && delta_distance > target_distance / 2)
			{
				delta_distance = target_distance / 2.0f;
			}
		}

	}
	/*printf("\ndelta_distance: %f", delta_distance);
	printf(" time:%f, a:%f [", time, a);
	for(int i = 0; i < 4; ++i)
	{
		printf("%f, ", value[i]);
	}
	printf("]");*/
}
/*void MeCtLocomotionQuadraticSynchronizer::update_for_speed_distance(float time)
{
	if(target[QUAD_SYNC_DISTANCE] == value[QUAD_SYNC_DISTANCE]) return;
	float a = 0.5f*(target[QUAD_SYNC_SPEED]*target[QUAD_SYNC_SPEED]-value[QUAD_SYNC_SPEED]*value[QUAD_SYNC_SPEED])/-target[QUAD_SYNC_DISTANCE];
	if(a == 0.0f)
	{
		if(target[QUAD_SYNC_DISTANCE]>0.0f) a = abs(max[QUAD_SYNC_ACCELERATION]);
		else a = -abs(max[QUAD_SYNC_ACCELERATION]);
	}

	if(a>max[QUAD_SYNC_ACCELERATION]) a = max[QUAD_SYNC_ACCELERATION];
	else if(-a < -max[QUAD_SYNC_ACCELERATION]) a = -max[QUAD_SYNC_ACCELERATION];

	value[QUAD_SYNC_ACCELERATION] = a;
	

	delta_distance = value[QUAD_SYNC_SPEED]*time+0.5f*value[QUAD_SYNC_ACCELERATION]*time*time;
	value[QUAD_SYNC_DISTANCE] += delta_distance;
	float t_speed = value[QUAD_SYNC_SPEED];
	value[QUAD_SYNC_SPEED] += value[QUAD_SYNC_ACCELERATION]*time;
	if(primary_ind == QUAD_SYNC_SPEED)
	{
		if(t_speed < target[QUAD_SYNC_SPEED] && value[QUAD_SYNC_SPEED] > target[QUAD_SYNC_SPEED]) 
			value[QUAD_SYNC_SPEED] = target[QUAD_SYNC_SPEED];
		else if(t_speed > target[QUAD_SYNC_SPEED] && value[QUAD_SYNC_SPEED] < target[QUAD_SYNC_SPEED])
			value[QUAD_SYNC_SPEED] = target[QUAD_SYNC_SPEED];
	}
	else if(primary_ind == QUAD_SYNC_DISTANCE)
	{
		if(value[QUAD_SYNC_ACCELERATION] > 0.0f && value[QUAD_SYNC_DISTANCE] > target[QUAD_SYNC_DISTANCE]) value[QUAD_SYNC_DISTANCE] = target[QUAD_SYNC_DISTANCE];
		if(value[QUAD_SYNC_ACCELERATION] < 0.0f && value[QUAD_SYNC_DISTANCE] < target[QUAD_SYNC_DISTANCE]) value[QUAD_SYNC_DISTANCE] = target[QUAD_SYNC_DISTANCE];
	}
}*/

float MeCtLocomotionQuadraticSynchronizer::get_target_speed()
{
	return target_speed;
}

float MeCtLocomotionQuadraticSynchronizer::get_target_distance()
{
	return target_distance;
}

float MeCtLocomotionQuadraticSynchronizer::get_target_time()
{
	return target_time;
}

float MeCtLocomotionQuadraticSynchronizer::get_delta_distance()
{
	return delta_distance;
}

float MeCtLocomotionQuadraticSynchronizer::get_speed()
{
	return speed;
}

float MeCtLocomotionQuadraticSynchronizer::get_distance()
{
	return distance;
}

float MeCtLocomotionQuadraticSynchronizer::get_time()
{
	return time;
}

float MeCtLocomotionQuadraticSynchronizer::get_acceleration()
{
	return acceleration;
}
