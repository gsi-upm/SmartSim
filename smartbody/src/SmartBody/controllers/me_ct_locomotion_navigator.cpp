/*
 *  me_ct_locomotion_navigator.hpp - part of SmartBody-lib's Motion Engine
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

#include "controllers/me_ct_locomotion_navigator.hpp"

#include "sbm/sbm_character.hpp"
#include "sbm/gwiz_math.h"
#include "limits.h"
#include "sbm/mcontrol_util.h"


const char* MeCtLocomotionNavigator::TYPE = "MeCtLocomotionNavigator";


/** Constructor */
MeCtLocomotionNavigator::MeCtLocomotionNavigator() 
{
	is_valid = false;
	//displacement.set(0.0f, 0.0f, 0.0f);
	orientation_angle = 0;
	pre_orientation_angle = orientation_angle;
	limb_blending_factor = 0.0f;
	has_destination = false;
	reached_destination = false;
	destination_list.capacity(20);
	speed_list.capacity(20);
	target_height_adjustment = 0.0f;
	prev_height_adjustment = 0.0f;
	curr_height_adjustment = 0.0f;

	worldOffsetWriter = NULL;
}

/** Destructor */
MeCtLocomotionNavigator::~MeCtLocomotionNavigator() 
{
	// Nothing allocated to the heap

}

void MeCtLocomotionNavigator::update_framerate_accelerator(float accelerator, SrArray<MeCtLocomotionLimb*>* limb_list)
{
	framerate_accelerator = accelerator;
	if(accelerator <= 1.0f)
	{
		if(accelerator == 0.0f)
		{
			limb_blending_factor = 0.0f;
			//limb_list.get(dominating_limb)->space_time = 0.0f;
			for(int i = 0; i < limb_list->size(); ++i)
			{
				limb_list->get(i)->set_space_time( 0.0f);
				limb_list->get(i)->direction_planner.reset();
			}
		}
		else
		{
			limb_blending_factor = accelerator;
			framerate_accelerator = 1.0f;
		}
	}
	else limb_blending_factor = 1.0;
}

void MeCtLocomotionNavigator::update(SrBuffer<float>* buffer)
{
	
}

const char* MeCtLocomotionNavigator::controller_type( void )	
{
	return TYPE;
}

void MeCtLocomotionNavigator::CheckNewRoutine(MeFrameData& frame)
{
	SrBuffer<float>& buffer = frame.buffer(); // convenience reference
	float x = buffer[bi_id];
	if(buffer[bi_id] == 0.0f || buffer[bi_id] <= -2.0f) return;
	if(buffer[bi_id] == -1.0f)
	{
		routine_stack.size(0);
	}
	MeCtLocomotionRoutine routine;
	routine.global_rps = buffer[bi_loco_rot_global_y];
	routine.local_rps = buffer[bi_loco_rot_local_y];
	routine.local_angle = buffer[bi_loco_rot_local_angle];
	routine.life_time = buffer[bi_loco_time];
	routine.elapsed_time = 0.0f;
	routine.angle = 0.0f;
	if(routine.global_rps == 0.0f) routine.type = ME_CT_LOCOMOTION_ROUTINE_TYPE_STRAIGHT;
	else routine.type = ME_CT_LOCOMOTION_ROUTINE_TYPE_CIRCULAR;
	routine.direction.set(buffer[bi_loco_vel_x], buffer[bi_loco_vel_y], buffer[bi_loco_vel_z]);
	routine.speed = routine.direction.len();
	routine.direction.normalize();
	if(buffer[bi_id] == -1.0f)
	{
		int max_id = 0;
		for(int i = 0; i < routine_stack.size(); ++i)
		{
			if(routine_stack.get(i).id > max_id)
			{
				max_id = routine_stack.get(i).id;
			}
		}
		routine.id = max_id + 1;
		routine_stack.push() = routine;
	}
	else 
	{
		routine.id = (int)buffer[bi_id];
		int i;
		for(i = 0; i < routine_stack.size(); ++i)
		{
			if(routine_stack.get(i).id == buffer[bi_id])
			{
				routine_stack.set(i, routine);
				break;
			}
		}
		if(i == routine_stack.size())
		{
			routine.id = (int)buffer[bi_id];
			routine_stack.push() = routine;
		}
	}
}

bool MeCtLocomotionNavigator::check_stopped(SrArray<MeCtLocomotionLimb*>* limb_list)
{
	MeCtLocomotionLimb* limb;
	if(target_local_vel.len() != 0.0f || limb_blending_factor != 0.0f) return false;

	for(int i = 0; i < limb_list->size(); ++i)
	{
		limb = limb_list->get(i);
		if(limb->curr_rotation != 0.0f) return false;
	}
	routine_stack.size(0);
	routine_stack.capacity(0);
	return true;
}

bool MeCtLocomotionNavigator::controller_evaluate(double delta_time, MeFrameData& frame ) 
{
	//if(reached_destination) return true;
	SrMat mat;
	SrQuat q;
	SrBuffer<float>& buffer = frame.buffer(); // convenience reference
	if(has_destination && curr_dest_index == -1 && destination_list.size() > 0) next_destination(frame);
	this->delta_time = delta_time;
	CheckNewRoutine(frame);
	if (worldOffsetWriter)
	{
		SrBuffer<float>& data = worldOffsetWriter->get_data();
		world_pos.set( data[0], data[1], data[2] );
		world_rot.set( data[3], data[4], data[5], data[6] );
	}
	//else
	{
		world_pos.set( buffer[ bi_world_x ], buffer[ bi_world_y ], buffer[ bi_world_z ] );
		world_rot.set( buffer[ bi_world_rot ], buffer[ bi_world_rot+1 ], buffer[ bi_world_rot+2 ], buffer[ bi_world_rot+3 ] );
	}
	base_pos.set ( buffer[ bi_base_x ], buffer[ bi_base_y ], buffer[ bi_base_z ] );

	//printf("\n(%f, %f, %f)", world_pos.x, world_pos.y, world_pos.z);
	//world_pos.y = 0.0f;
	SrQuat t_world_rot;
	mat.roty(pre_orientation_angle);
	t_world_rot.set(mat);
	
	if(t_world_rot.w != world_rot.w
		|| t_world_rot.x != world_rot.x
		|| t_world_rot.y != world_rot.y
		|| t_world_rot.z != world_rot.z
		)// if the orientation has been changed manually 
	{
		pre_orientation_angle = world_rot.angle();
		mat.roty(pre_orientation_angle);
		t_world_rot.set(mat);
		if(dot(t_world_rot.axis(), world_rot.axis())< 0.0f) 
			pre_orientation_angle = -pre_orientation_angle;

		orientation_angle = pre_orientation_angle;
	}

	global_vel.set(0,0,0);
	local_rps = 0.0f;
	MeCtLocomotionRoutine routine;

	int i;
	for(i = 0; i < routine_stack.size(); ++i)
	{
		routine = routine_stack.get(i);
		if(routine.local_angle != 0.0f)
		{
			if(abs(routine.local_angle) < abs(routine.angle+routine.local_rps*delta_time))
			{
				routine.local_rps = (routine.local_angle - routine.angle)/(float)delta_time;
				routine.local_angle = 0.0f;
				routine.angle = 0.0f;
				local_rps += routine.local_rps;
				routine.local_rps = 0.0f;
				routine_stack.set(i, routine);
			}
		}
		switch(routine.type)
		{
		case ME_CT_LOCOMOTION_ROUTINE_TYPE_STRAIGHT:
			break;

		case ME_CT_LOCOMOTION_ROUTINE_TYPE_CIRCULAR:
			break;

		default:
			break;
		}
		local_rps += routine.local_rps;
		global_vel += routine.direction * routine.speed;

	}

	for(i = 0; i < routine_stack.size(); ++i)
	{
		routine = routine_stack.get(i);
		if(routine.life_time > 0.0f) 
		{
			routine.elapsed_time += delta_time;
			if(routine.elapsed_time >= routine.life_time)
			{
				routine_stack.remove(i);
				--i;
				continue;
			}
			routine_stack.set(i, routine);
		}
		if(routine.local_angle != 0.0f)
		{
			routine.angle += local_rps*(float)delta_time;
			routine_stack.set(i, routine);
		}

	}

	mat.roty(-pre_orientation_angle);
	local_vel = global_vel*mat;

	calc_target_velocity();
		
	return true;
}

inline void MeCtLocomotionNavigator::calc_target_velocity()
{
	if(has_destination)
	{
		if(destination_list.size() > curr_dest_index && curr_dest_index >= 0)
		{
			SrMat mat;
			mat.roty(-pre_orientation_angle);
			dis_to_dest = destination_list.get(curr_dest_index) - world_pos;
			dis_to_dest.y = 0.0f;
			if(!reached_destination)
			{
				target_global_vel = dis_to_dest;
				target_global_vel.normalize();
				target_local_vel = target_global_vel*mat;
				dis_to_dest_local = dis_to_dest*mat;
				target_local_vel *= global_vel.len();
			}
			else
			{
				target_local_vel.set(0,0,0);
				target_global_vel.set(0,0,0);
			}
		}
	}
	else
	{
		if(reached_destination)
		{
			target_local_vel.set(0,0,0);
			target_global_vel.set(0,0,0);
		}
		else
		{
			target_global_vel = global_vel;
			target_local_vel = local_vel;
		}
	}
}

SrVec MeCtLocomotionNavigator::get_global_velocity()
{
	return global_vel;
}

SrVec MeCtLocomotionNavigator::get_dis_to_dest_local()
{
	return dis_to_dest_local;
}

SrVec MeCtLocomotionNavigator::get_target_local_velocity()
{
	//if(destination_list.size() > curr_dest_index && curr_dest_index >= 0) return local_vel;
	return target_local_vel;
}

SrVec MeCtLocomotionNavigator::get_dis_to_dest()
{
	return dis_to_dest;
}

SrVec MeCtLocomotionNavigator::get_local_velocity()
{
	return local_vel;
}

void MeCtLocomotionNavigator::set_reached_destination(MeFrameData& frame)
{
	SrBuffer<float>& buffer = frame.buffer();
	buffer[ bi_loco_vel_x ] = 0.0f;
	buffer[ bi_loco_vel_y ] = 0.0f;
	buffer[ bi_loco_vel_z ] = 0.0f;
	buffer[ bi_loco_rot_global_y ] = 0.0f;
	buffer[ bi_loco_rot_local_y ] = 0.0f;
	reached_destination = true;
	limb_blending_factor_on_stop_t = limb_blending_factor;
	has_destination = false;
	destination_list.size(0);
}

void MeCtLocomotionNavigator::update_world_offset(SrVec& displacement)
{
	prev_height_adjustment = curr_height_adjustment;
	curr_height_adjustment = target_height_adjustment;
	//curr_height_displacement = target_height_displacement * standing_factor;

	world_pos.x = displacement.x + world_pos.x;
	world_pos.y = displacement.y + world_pos.y + curr_height_adjustment - prev_height_adjustment;
	world_pos.z = displacement.z + world_pos.z;

}

void MeCtLocomotionNavigator::update_world_mat()
{
	world_mat.roty(get_orientation_angle());
	world_mat.set(12, world_pos.x);
	world_mat.set(13, world_pos.y);
	world_mat.set(14, world_pos.z);
}

void MeCtLocomotionNavigator::set_world_mat(SrMat& mat)
{
	world_mat = mat;
}

void MeCtLocomotionNavigator::set_translation_joint_name(const std::string& name)
{
	translation_joint_name = name;
}

void MeCtLocomotionNavigator::update_world_mat_rotation()
{
	world_mat.roty(get_orientation_angle());
}

void MeCtLocomotionNavigator::update_world_mat_offset()
{
	world_mat.set(12, world_pos.x);
	world_mat.set(13, world_pos.y);
	world_mat.set(14, world_pos.z);
}

void MeCtLocomotionNavigator::post_controller_evaluate(MeFrameData& frame, MeCtLocomotionLimb* limb, bool reset) 
{
	//if(reached_destination) return;


	SrBuffer<float>& buffer = frame.buffer();
	if(reset)
	{
		buffer[ bi_world_x ] = 0.0f;
		//buffer[ bi_world_y ] = 0.0f;
		buffer[ bi_world_z ] = 0.0f;
	}

	SrMat mat;
	mat.roty(orientation_angle);
	world_rot.set(mat);

	if(worldOffsetWriter != NULL)
	{
		float world_offset_data[7];
		world_offset_data[0] = world_pos.x;
		world_offset_data[1] = world_pos.y;
		world_offset_data[2] = world_pos.z;
		world_offset_data[3] = world_rot.w;
		world_offset_data[4] = world_rot.x;
		world_offset_data[5] = world_rot.y;
		world_offset_data[6] = world_rot.z;
		worldOffsetWriter->set_data(world_offset_data);
	}
	//else
	{
		buffer[ bi_world_x ] = world_pos.x;
		buffer[ bi_world_y ] = world_pos.y;
		buffer[ bi_world_z ] = world_pos.z;
		buffer[ bi_world_rot+0 ] = world_rot.w;
		buffer[ bi_world_rot+1 ] = world_rot.x;
		buffer[ bi_world_rot+2 ] = world_rot.y;
		buffer[ bi_world_rot+3 ] = world_rot.z;
	}

	MeCtLocomotionRoutine routine;
	SrVec di;
	float delta_angle;
	for(int i = 0; i < routine_stack.size(); ++i)
	{
		routine = routine_stack.get(i);
		//routine.elapsed_time += delta_time;

		switch(routine.type)
		{
		case ME_CT_LOCOMOTION_ROUTINE_TYPE_STRAIGHT:
			target_world_pos += routine.direction * (routine.speed * (float)delta_time);
			break;

		case ME_CT_LOCOMOTION_ROUTINE_TYPE_CIRCULAR:
			delta_angle = orientation_angle - pre_orientation_angle;

			//get the displacement of rotation
			mat.roty(0.5f * delta_angle);
			di = routine.direction * mat;
			di *= 2.0f * sin(0.5f * delta_angle) * routine.speed / routine.global_rps;
			target_world_pos += di; 

			//mat.roty(facing_angle-routine.start_facing_angle);
			//routine.direction = SrVec(0,0,1)* mat;
			mat.roty(delta_angle);

			routine.direction = routine.direction* mat;
			break;

		default:
			break;
		}
		//mat.roty(routine.local_rps * (float)delta_time);
		//target_world_rot = target_world_rot * mat;
		routine_stack.set(i, routine);
	}
	pre_orientation_angle = orientation_angle;
	//LOG("\ntarget world pos: (%f, %f, %f)", target_world_pos.x, target_world_pos.y, target_world_pos.z);
	if(limb_blending_factor == 0.0f)
	{
		local_vel.set(0.0f, 0.0f, 0.0f);
		global_vel.set(0.0f, 0.0f, 0.0f);
		target_local_vel.set(0.0f, 0.0f, 0.0f);
		target_global_vel.set(0.0f, 0.0f, 0.0f);
	}
}

SkChannelArray& MeCtLocomotionNavigator::controller_channels() {
	
	return request_channels;
}

// Implements MeController::context_updated(..).
void MeCtLocomotionNavigator::context_updated(MeControllerContext* _context) 
{
	if( _context == NULL )
		is_valid = false;
}

// Look up the context indices, and check to make sure it isn't -1
#define LOOKUP_BUFFER_INDEX( var_name, index ) \
	var_name = _context->toBufferIndex( _toContextCh->get( index ));  \
	is_valid &= ( var_name != -1 );

bool MeCtLocomotionNavigator::controller_map_updated(MeControllerContext* _context, SrBuffer<int>* _toContextCh) 
{
	int index = 0;
	is_valid = true;
	if( _context != NULL ) 
	{
		// request_channel indices (second param) come from the order of request_channels.add(..) calls in controller_channels()
		LOOKUP_BUFFER_INDEX( bi_world_x,    bi_world_x );
		LOOKUP_BUFFER_INDEX( bi_world_y,    bi_world_y );
		LOOKUP_BUFFER_INDEX( bi_world_z,    bi_world_z );
		LOOKUP_BUFFER_INDEX( bi_world_rot,  bi_world_rot );

		LOOKUP_BUFFER_INDEX( bi_base_x,    bi_base_x );
		LOOKUP_BUFFER_INDEX( bi_base_y,    bi_base_y );
		LOOKUP_BUFFER_INDEX( bi_base_z,    bi_base_z );

		LOOKUP_BUFFER_INDEX( bi_loco_vel_x, bi_loco_vel_x );
		LOOKUP_BUFFER_INDEX( bi_loco_vel_y, bi_loco_vel_y );
		LOOKUP_BUFFER_INDEX( bi_loco_vel_z, bi_loco_vel_z );

		LOOKUP_BUFFER_INDEX( bi_loco_rot_global_y, bi_loco_rot_global_y );
		LOOKUP_BUFFER_INDEX( bi_loco_rot_local_y, bi_loco_rot_local_y );
		LOOKUP_BUFFER_INDEX( bi_loco_rot_local_angle, bi_loco_rot_local_angle );
		LOOKUP_BUFFER_INDEX( bi_loco_time, bi_loco_time );

		LOOKUP_BUFFER_INDEX( bi_id, bi_id );

	} 
	else 
	{
		// This shouldn't get here
		is_valid = false;
	}
	return is_valid;
}

int MeCtLocomotionNavigator::controller_channels(SkChannelArray* request_channels) 
{
	// Initialize Requested Channels                                                  // Indices
	routine_channel_num = 0;

	// Initialize Requested Channels                                                           // Indices

	AddChannel(request_channels, SbmPawn::WORLD_OFFSET_JOINT_NAME , SkChannel::XPos, &bi_world_x);
	AddChannel(request_channels, SbmPawn::WORLD_OFFSET_JOINT_NAME , SkChannel::YPos, &bi_world_y);
	AddChannel(request_channels, SbmPawn::WORLD_OFFSET_JOINT_NAME , SkChannel::ZPos, &bi_world_z);
	AddChannel(request_channels, SbmPawn::WORLD_OFFSET_JOINT_NAME , SkChannel::Quat, &bi_world_rot);

	AddChannel(request_channels, translation_joint_name , SkChannel::XPos, &bi_base_x);
	AddChannel(request_channels, translation_joint_name, SkChannel::YPos, &bi_base_y);
	AddChannel(request_channels, translation_joint_name, SkChannel::ZPos, &bi_base_z);

	AddChannel(request_channels, MeCtLocomotionPawn::LOCOMOTION_VELOCITY, SkChannel::XPos, &bi_loco_vel_x); 
	AddChannel(request_channels, MeCtLocomotionPawn::LOCOMOTION_VELOCITY, SkChannel::YPos, &bi_loco_vel_y);
	AddChannel(request_channels, MeCtLocomotionPawn::LOCOMOTION_VELOCITY, SkChannel::ZPos, &bi_loco_vel_z);

	AddChannel(request_channels, MeCtLocomotionPawn::LOCOMOTION_GLOBAL_ROTATION, SkChannel::YPos, &bi_loco_rot_global_y);
	AddChannel(request_channels, MeCtLocomotionPawn::LOCOMOTION_LOCAL_ROTATION, SkChannel::YPos, &bi_loco_rot_local_y);

	AddChannel(request_channels, MeCtLocomotionPawn::LOCOMOTION_LOCAL_ROTATION_ANGLE, SkChannel::YPos, &bi_loco_rot_local_angle);

	AddChannel(request_channels, MeCtLocomotionPawn::LOCOMOTION_TIME, SkChannel::YPos, &bi_loco_time);

	//AddChannel(request_channels, SkJointName( "base" ), SkChannel::ZPos, &bi_base_z);

	AddChannel(request_channels, MeCtLocomotionPawn::LOCOMOTION_ID, SkChannel::YPos, &bi_id);

	return routine_channel_num;
}

SrVec MeCtLocomotionNavigator::get_base_pos()
{
	return base_pos;
}

void MeCtLocomotionNavigator::AddChannel(SkChannelArray* request_channels, std::string name, SkChannel::Type type, int* index)
{
	request_channels->add( name, type);
	*index = routine_channel_num;
	++routine_channel_num;
}

float MeCtLocomotionNavigator::get_turning_angle()
{
	// Temporary solution: If there is no speed, no rotation.
	if(local_vel.len() == 0.0f) return 0.0f;
	return (float)delta_time * local_rps;
}

void MeCtLocomotionNavigator::update_facing(MeCtLocomotionLimb* limb, bool dominant_limb)
{
	// when coming to stop the limbs should rotate back to original orientation.

	if(local_vel.len() == 0.0f)
	{
		limb->curr_rotation *= 0.95f;
		limb->rotation_record = limb->curr_rotation;
		return;
	}
	
	limb->pre_rotation = limb->curr_rotation;

	SrMat mat;
	float time = 0.0f;
	float ratio = 0.0f;
	
	if(limb_blending_factor_on_stop_t > 0.0f) ratio = limb_blending_factor / limb_blending_factor_on_stop_t;

	if(limb->get_space_time() > 1.0f && limb->get_space_time() <= 1.5f) 
	{
		limb->curr_rotation = limb->rotation_record * (1.5f - limb->get_space_time())*2.0f;

	}

	else if(limb->get_space_time() >= 2.0f || limb->get_space_time() <= 1.0f)
	{
		float delta_angle = get_turning_angle();
		
		limb->curr_rotation += delta_angle;

		if(dominant_limb) 
		{
			if(reached_destination)
				orientation_angle += -delta_angle * ratio;
			else 
				orientation_angle += -delta_angle;
	
			if(orientation_angle > 0.0f) 
				orientation_angle -= (int)(0.5f*orientation_angle/(float)M_PI)*(float)M_PI*2;
			else 
				orientation_angle += ((int)(-0.5f*orientation_angle/(float)M_PI))*(float)M_PI*2;
		}
	}
	else if(limb->get_space_time() > 1.5f && limb->get_space_time() < 2.0f)
	{
		limb->curr_rotation -= get_turning_angle();
		limb->rotation_record = limb->curr_rotation;
	}

	if(reached_destination)
		limb->curr_rotation *= ratio;

	if( limb->get_space_time() >= 2.0f || limb->get_space_time() <= 1.0f )
		limb->rotation_record = limb->curr_rotation;

}

void MeCtLocomotionNavigator::clear_destination_list()
{
	destination_list.size(0);
	speed_list.size(0);
	curr_dest_index = -1;
}

void MeCtLocomotionNavigator::next_destination(MeFrameData& frame)
{
	++curr_dest_index;
	if(destination_list.size() <= curr_dest_index) return;

	SrVec dest = destination_list.get(curr_dest_index);
	SrVec v = dest-world_pos;
	v.y = 0.0f;
	v.normalize();
	v *= speed_list.get(curr_dest_index);
	SrBuffer<float>& buffer = frame.buffer();
	buffer[ bi_loco_vel_x ] = v.x;
	buffer[ bi_loco_vel_y ] = v.y;
	buffer[ bi_loco_vel_z ] = v.z;
}

int MeCtLocomotionNavigator::get_destination_count()
{
	return destination_list.size();
}

int MeCtLocomotionNavigator::get_curr_destination_index()
{
	return curr_dest_index;
}

void MeCtLocomotionNavigator::add_destination(SrVec* destination)
{
	destination_list.push() = *destination;
	reached_destination = false;
}

void MeCtLocomotionNavigator::add_speed(float speed)
{
	int num = destination_list.size()-speed_list.size();
	for(int i = 0; i < num; ++i)
	{
		speed_list.push() = speed;
	}
}

SrVec MeCtLocomotionNavigator::get_world_pos()
{
	return world_pos;
}

SrMat MeCtLocomotionNavigator::get_world_mat()
{
	return world_mat;
}

/*void MeCtLocomotionNavigator::update_displacement(SrVec* displacement)
{
	this->displacement = *displacement;
}*/

float MeCtLocomotionNavigator::get_orientation_angle()
{
	return orientation_angle;
}

float MeCtLocomotionNavigator::get_pre_facing_angle()
{
	return pre_orientation_angle;
}

void MeCtLocomotionNavigator::AddRoutine(MeCtLocomotionRoutine& routine)
{
	routine_stack.push() = routine;
}

void MeCtLocomotionNavigator::DelRoutine(char* name)
{
	for(int i = 0; i < routine_stack.size(); ++i)
	{
		if(strcmp(routine_stack.get(i).name, name) == 0)
		{
			routine_stack.remove(i,1);
			break;
		}
	}
}

void MeCtLocomotionNavigator::print_foot_pos(MeFrameData& frame, MeCtLocomotionLimb* limb)
{
	SrMat gmat;
	SrMat pmat;
	SrMat lmat;
	SrQuat rotation;
	SrVec pos;
	SkSkeleton* skeleton = limb->walking_skeleton;
	SkJoint* tjoint = skeleton->search_joint(limb->get_limb_base_name().c_str());
	gmat = tjoint->parent()->gmat();
	for(int j  = 0;j < limb->limb_joint_info.quat.size()-1;++j)
	{
		pmat = gmat;
		lmat = get_lmat(tjoint, &(limb->limb_joint_info.quat.get(j)));
		gmat.mult ( lmat, pmat );
		if(tjoint->num_children()>0)
		{ 
			tjoint = tjoint->child(0);
		}
		else break;
	}
	pos.set(gmat.get(12), gmat.get(13), gmat.get(14));

	SrBuffer<float>& buffer = frame.buffer(); // convenience reference
	rotation.w = buffer[ bi_world_rot+0 ];
	rotation.x = buffer[ bi_world_rot+1 ];
	rotation.y = buffer[ bi_world_rot+2 ];
	rotation.z = buffer[ bi_world_rot+3 ];
	gmat = rotation.get_mat(gmat);
	pos = pos * gmat;
	pos.x += buffer[ bi_world_x ];
	pos.y += buffer[ bi_world_y ];
	pos.z += buffer[ bi_world_z ];

}
