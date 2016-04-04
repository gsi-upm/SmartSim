/*
 *  locomotion_cmds.cpp - part of SmartBody-lib
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
#include <iostream>
#include <string>

#include "sbm/gwiz_math.h"
#include "locomotion_cmds.hpp"
#include "controllers/me_ct_locomotion.hpp"

#include <sk/sk_channel_array.h>
//#include <controllers/me_ct_raw_writer.hpp>
#include <controllers/me_ct_navigation_circle.hpp>

using namespace std;

int locomotion_cmd_func( srArgBuffer& args, mcuCBHandle *mcu_p  )	
{
	// should this function be removed?

/*	SkMotion* walking1_p = NULL;
	SkMotion* walking2_p = NULL;
	SkMotion* standing_p = NULL;
	
	std::map<std::string, SkMotion*>::iterator walkIter = mcu_p->motion_map.find("Step_WalkForward");
	if (walkIter != mcu_p->motion_map.end())
		walking1_p = (*walkIter).second;

	std::map<std::string, SkMotion*>::iterator strafeIter = mcu_p->motion_map.find("Step_StrafeRight");
	if (strafeIter != mcu_p->motion_map.end())
		walking2_p = (*strafeIter).second;

	std::map<std::string, SkMotion*>::iterator standIter = mcu_p->motion_map.find("LHandOnHip_Motex");
	if (standIter != mcu_p->motion_map.end())
		standing_p = (*standIter).second;

	if(walking1_p && walking2_p && standing_p)
	{
		MeCtLocomotion ct_locomotion;
		//MeCtLocomotionAnalysis analysis;
		//analysis.set_ct_pawn((MeCtLocomotionPawn*)&ct_locomotion);
		init_locomotion_skeleton(const char* skel_file, mcuCBHandle *mcu_p)
		ct_locomotion.init_skeleton();
		analysis.init(standing_p, mcu_p->me_paths);
		analysis.add_locomotion(walking1_p, 1, 0, 1.0f);
		analysis.add_locomotion(walking2_p, 2, 0, 1.0f);
		return CMD_SUCCESS;
	}
	else
	{
		return CMD_FAILURE;
	}*/
	return CMD_SUCCESS;
}


int test_locomotion_cmd_func( srArgBuffer& args, mcuCBHandle *mcu_p  )	{
	string arg = args.read_token();
	if( arg.empty() || arg=="help" ) {
		cout << "Syntax:" << endl
		     << "\ttest locomotion [char <agent-id>]"<<endl
			 << "\t                [dx <dx>] [dy <dy>] [dz <dz>]" <<endl
			 << "\t                [deg_per_sec <angle> | rad_per_sec <angle>]" <<endl
			 << "\t                [global_deg_per_sec <angle> | global_rad_per_sec <angle>]" <<endl
			 << "\t                [local_deg_per_sec <angle> | local_rad_per_sec <angle>]" <<endl
			 << "\t                [id <int>]" <<endl
			 << "\t                [del <int>]" <<endl
			 << "\t                [enable | disable]" <<endl
		     << "\ttest locomotion [char <agent-id>] zero"<<endl
			 << "Translational velocity is in global coordinates" <<endl;
		return CMD_SUCCESS;
	}
	
	if(arg=="initialize")
	{
		LOG("Command: 'locomotion initialize' has been deprecated.");
		return CMD_FAILURE;
	}

	SbmCharacter* actor = NULL;
	if(arg == "status")
	{
		for (std::map<std::string, SbmCharacter*>::iterator iter = mcu_p->getCharacterMap().begin();
			iter != mcu_p->getCharacterMap().end();
			iter++)
		{
 			(*iter).second->get_locomotion_ct()->print_info(actor->getName());
		}
		return CMD_SUCCESS;
	}
	
	if( arg=="character" || arg=="char" ) {
		string name = args.read_token();
		actor = mcu_p->getCharacter( name );
		if( actor == NULL ) {
			LOG("ERROR: Could not find character \"%s\".", name.c_str());
			return CMD_FAILURE;
		}

		arg = args.read_token();
	} else {
		if( mcu_p->test_character_default.empty() ) {
			LOG("ERROR: No character specified, and no default set.");
			return CMD_FAILURE;
		}
		actor = mcu_p->getCharacter( mcu_p->test_character_default );
		if( actor == NULL ) {
			LOG("ERROR: Could not find default character \"%s\".", mcu_p->test_character_default.c_str());
			return CMD_FAILURE;
		}
	}

	if (!actor->get_locomotion_ct())
	{
		LOG("Character %s does not have a semi-procedural locomotion controller. Command ignored.", actor->getName().c_str());
		return CMD_FAILURE;
	}

	if( arg == "ground_height" )
	{
		actor->get_locomotion_ct()->get_terrain()->set_ground_height(args.read_float());
		return CMD_SUCCESS;
	}

	if(arg == "anim")
	{
		std::string type = args.read_token();
		SkMotion* anim_p = NULL;
		bool motionsNotLoaded = false;
		arg = args.read_token();
		std::map<std::string, SkMotion*>::iterator animIter = mcu_p->motion_map.find(arg);
		if (animIter != mcu_p->motion_map.end())
		anim_p = (*animIter).second;
		if (!anim_p)
		{
			LOG("No animation: %s", arg.c_str());
			motionsNotLoaded = true;
			if (actor->get_locomotion_ct())
				actor->get_locomotion_ct()->motions_loaded = false;
			return CMD_FAILURE;
		}
		if(type == "standing")
		{
			actor->get_locomotion_ct()->get_analyzer()->init(anim_p, mcu_p->me_paths);
			actor->get_locomotion_ct()->get_analyzer()->init_blended_anim();
		}
		else if(type == "walking")
		{
			if(!actor->get_locomotion_ct()->get_analyzer()->get_standing_initialized())
			{
				actor->get_locomotion_ct()->get_analyzer()->init(NULL, mcu_p->me_paths);
				actor->get_locomotion_ct()->get_analyzer()->init_blended_anim();
			}

			int remainingTokens = args.calc_num_tokens();

			float l_land_time = 0.0f;
			float l_stance_time = 0.0f;
			float l_lift_time = 0.0f;

			float r_land_time = 0.0f;
			float r_stance_time = 0.0f;
			float r_lift_time = 0.0f;

			if(remainingTokens >= 6)
			{
				l_land_time = args.read_float();
				l_stance_time = args.read_float();
				l_lift_time = args.read_float();

				r_land_time = args.read_float();
				r_stance_time = args.read_float();
				r_lift_time = args.read_float();
			}

			float translation_scale = 1.0f;
			float height_offset = 0.0f;

			while(!arg.empty())
			{
				arg = args.read_token();
				if(arg == "scale")
				{
					translation_scale = args.read_float();
				}
				else if(arg == "height_offset")
				{
					height_offset = args.read_float();
				}
			}

			actor->get_locomotion_ct()->add_locomotion_anim(anim_p);

			if(remainingTokens >= 6)
				actor->get_locomotion_ct()->get_analyzer()->add_locomotion(anim_p, l_land_time, l_stance_time, l_lift_time, r_land_time, r_stance_time, r_lift_time, translation_scale);
			else 
				actor->get_locomotion_ct()->get_analyzer()->add_locomotion(anim_p, 0, 0, translation_scale, height_offset);
		}
		return CMD_SUCCESS;
	}

	if(!actor->is_locomotion_controller_initialized()) 
	{
		LOG("ERROR: Locomotion controller not initialized.");
		return CMD_FAILURE;
	}

	if(arg=="enable")
	{
		if(!actor->get_locomotion_ct()->get_analyzer()->get_standing_initialized())
		{
			actor->get_locomotion_ct()->get_analyzer()->init(NULL, mcu_p->me_paths);
			if(!actor->get_locomotion_ct()->is_valid()) 
			{
				LOG("Locomotion not valid, can not be enabled.");
				return CMD_FAILURE;
			}
			actor->get_locomotion_ct()->get_analyzer()->init_blended_anim();
		}
		actor->get_locomotion_ct()->set_enabled(true);
		LOG("Locomotion engine has been enabled.");

		return CMD_SUCCESS;
	}
	else if(arg=="disable")
	{
		actor->get_locomotion_ct()->set_enabled(false);
		LOG("Locomotion engine has been disabled.");

		return CMD_SUCCESS;
	}

	if(!actor->is_locomotion_controller_enabled()) 
	{
		LOG("ERROR: Locomotion controller not enabled.");
		return CMD_FAILURE;
	}

	if(!actor->get_locomotion_ct()->motions_loaded)
	{
		LOG("locomotion animations not loaded correctly.");
		return CMD_FAILURE;
	}

	if(actor->get_locomotion_ct()->get_limb_list()->get(0)->walking_list.size() < 3)
	{
		LOG("locomotion animations not loaded correctly.");
		return CMD_FAILURE;
	}

	if(actor->get_locomotion_ct()->get_anim_global_info()->size() == 0) return CMD_FAILURE;
	bool local = false;
	SrVec global_direction;
	MeCtNavigationCircle* nav_circle = actor->get_locomotion_ct()->get_navigation_circle();
	//MeCtNavigationCircle* nav_circle = new MeCtNavigationCircle();
	nav_circle->set( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1);
//	actor->posture_sched_p->create_track( NULL, NULL, nav_circle);
	
	if( arg=="stop" )
	{
		/*SkChannelArray channels;
		channels.add( SkJointName( SbmCharacter::LOCOMOTION_VELOCITY ), SkChannel::XPos );
		channels.add( SkJointName( SbmCharacter::LOCOMOTION_VELOCITY ), SkChannel::YPos );
		channels.add( SkJointName( SbmCharacter::LOCOMOTION_VELOCITY ), SkChannel::ZPos );
		channels.add( SkJointName( SbmCharacter::LOCOMOTION_GLOBAL_ROTATION ), SkChannel::YPos );
		channels.add( SkJointName( SbmCharacter::LOCOMOTION_LOCAL_ROTATION ), SkChannel::YPos );
		channels.add( SkJointName( SbmCharacter::LOCOMOTION_LOCAL_ROTATION_ANGLE ), SkChannel::YPos );
		channels.add( SkJointName( SbmCharacter::LOCOMOTION_TIME ), SkChannel::YPos );
		channels.add( SkJointName( SbmCharacter::LOCOMOTION_ID ), SkChannel::YPos );*/

		float data[] = { 0, 0, 0, 0, 0, 0 };

		float dx = 0.0f, dy = 0.0f, dz = 0.0f, g_angular = 0.0f, l_angular = 0.0f, l_angle = 0.0f;
		int id = -1;

		nav_circle->set( dx, dy, dz, g_angular, l_angular, 0, id, 0, 0, 0, -1 );

		return CMD_SUCCESS;
	}

	if( arg == "freeze" )
	{
		arg = args.read_token();
		actor->get_locomotion_ct()->set_freeze(true);
		/*if(!arg.empty())
		{
			if(arg == "spacetime")
			{
				float dominant_space_time = args.read_float();
				actor->get_locomotion_ct()->get_limb_list()->get(actor->get_locomotion_ct()->get_dominant_limb_index())->space_time = dominant_space_time;
				
			}
		}*/
		return CMD_SUCCESS;
	}

	if( arg == "unfreeze" )
	{
		arg = args.read_token();
		actor->get_locomotion_ct()->set_freeze(false);
		return CMD_SUCCESS;
	}

	if( arg == "nextframe" )
	{
		if(actor->get_locomotion_ct()->is_freezed())
		{
			arg = args.read_token();
			actor->get_locomotion_ct()->freeze_delta_time = 0.0016666667f;
		}
		return CMD_SUCCESS;
	}

	if( arg == "height_offset" )
	{
		actor->get_locomotion_ct()->translation_joint_height = args.read_float();
		return CMD_SUCCESS;
	}

	if( arg == "balance")
	{
		actor->get_locomotion_ct()->set_balance_factor(args.read_float());
		return CMD_SUCCESS;
	}

	if( arg == "local")
	{
		global_direction = actor->get_locomotion_ct()->get_facing_vector();
		local = true;
	}

	if( arg == "max_acc_start" )
	{
		actor->get_locomotion_ct()->get_speed_accelerator()->set_max_acceleration(abs(args.read_float()));
		return CMD_SUCCESS;
	}

	if( arg == "max_acc_stop" )
	{
		actor->get_locomotion_ct()->get_speed_accelerator()->set_max_acceleration(-abs(args.read_float()));
		return CMD_SUCCESS;
	}

	if( arg == "forward" || arg == "backward" || arg == "leftward" || arg == "rightward")
	{
		float spd = 0.0f;
		float rps = 0.0f;
		float lrps = 0.0f;
		float time = 0.0f;
		float angle = 0.0f;
		SrMat mat;
		SrVec direction = actor->get_locomotion_ct()->get_facing_vector();

		while( !arg.empty() ) 
		{
			if(arg == "forward")
			{
				direction = direction;
				if(actor->get_locomotion_ct()->get_anim_global_info()->size() < 2) return CMD_FAILURE;
				spd = actor->get_locomotion_ct()->get_anim_global_info()->get(1)->speed;
			}
			else if(arg == "backward")
			{
				direction = -direction;
				if(actor->get_locomotion_ct()->get_anim_global_info()->size() < 2) return CMD_FAILURE;
				spd = actor->get_locomotion_ct()->get_anim_global_info()->get(1)->speed;
			}

			else if(arg == "leftward")
			{
				//SrVec v(1.0f, 0.0f, 0.0f);
				mat.roty(3.14159265f*0.5f);
				direction = direction*mat;
				if(actor->get_locomotion_ct()->get_anim_global_info()->size() < 3) return CMD_FAILURE;
				spd = actor->get_locomotion_ct()->get_anim_global_info()->get(2)->speed;
			}

			else if(arg == "rightward")
			{
				//SrVec v(1.0f, 0.0f, 0.0f);
				mat.roty(3.14159265f*0.5f);
				direction = direction*mat;
				direction = - direction;
				if(actor->get_locomotion_ct()->get_anim_global_info()->size() < 3) return CMD_FAILURE;
				spd = actor->get_locomotion_ct()->get_anim_global_info()->get(2)->speed;
			}

			else if(arg == "spd")
			{
				float t_spd = args.read_float();
				if(t_spd != 0.0f) spd = t_spd;
				direction *= spd;
			}
			else if(arg == "rps")
			{
				rps = args.read_float();
				lrps = rps;
			}
			else if(arg == "lrps")
			{
				lrps = args.read_float();
			}
			else if(arg == "angle")
			{
				angle = args.read_float();
			}
			else if(arg == "time")
			{
				time = args.read_float();
				//actor->get_locomotion_ct()->set_motion_time(time);
			}
			else 
			{
				LOG("Unknown token");
				//cerr << "Please specify the speed. (example:'spd 50')" <<endl;
				return CMD_FAILURE;
			}
			arg = args.read_token();
		}

		nav_circle->set( direction.x, direction.y, direction.z, rps, lrps, angle, -1, 0, 0, 0, time );

		return CMD_SUCCESS;
	}

	else if( arg == "ik" )
	{
		int enable = args.read_int();
		if(enable == 1) 
		{
			actor->locomotion_ik_enable(true);
		}
		else if(enable == 0)
		{
			actor->locomotion_ik_enable(false);
		}

		return CMD_SUCCESS;
	}

	else if( arg=="reset" ) {

		actor->locomotion_reset();//temp command process
		
		return CMD_SUCCESS;
	} 	
	else if( arg=="turnspd" ) {

		actor->locomotion_set_turning_speed(args.read_float());//temp command process
		
		return CMD_SUCCESS;
	} 

	else if( arg=="turnmode" ) {

		actor->locomotion_set_turning_mode(args.read_int());//temp command process
		
		return CMD_SUCCESS;
	} 
	
	else if (arg == "tx" || arg == "tz" || arg == "spd")
	{
		SkChannelArray channels;
		float tx = 0.0f, ty = 0.0f, tz = 0.0f, lrps = 0.0f, rps = 0.0f, spd = 0.0f, angle = 0.0f, time = -1.0f;
		int flagx = 0, flagz = 0;
		int tx_index=-1, ty_index=-1, tz_index=-1, spd_index = -1;
		actor->get_locomotion_ct()->get_navigator()->clear_destination_list();
		while( !arg.empty() ) 
		{
			if(arg == "tx" || arg == "tz")
			{
				while( !arg.empty() ) 
				{
					if( arg == "tx" ) 
					{
						if( tx_index == -1 ) 
						{
							tx_index = channels.size();
						}
						tx = args.read_float();
						flagx = 1;
					}

					else if( arg == "tz" ) 
					{
						if( tz_index == -1 ) 
						{
							tz_index = channels.size();
						}
						tz = args.read_float();
						flagz = 1;
					} 

					else break;

					arg = args.read_token();

					if(flagx == 1 && flagz == 1) break;

				}
				if(flagx == 1 && flagz == 1)
				{
					SrVec dest(tx, 0.0f, tz);
					actor->get_locomotion_ct()->get_navigator()->add_destination(&dest);
					actor->get_locomotion_ct()->get_navigator()->has_destination = true;
					flagx = 0;
					flagz = 0;
				}
			}

			else if(arg == "lrps")
			{
				lrps = args.read_float();
				arg = args.read_token();
			}

			else if(arg == "angle")
			{
				angle = args.read_float();
				arg = args.read_token();
			}

			else if( arg == "spd" ) 
			{
				if( spd_index == -1 ) {
					spd_index = channels.size();
					//channels.add( SkJointName( SbmCharacter::LOCOMOTION_VELOCITY ), SkChannel::ZPos );
				}
				spd = args.read_float();
				actor->get_locomotion_ct()->get_navigator()->add_speed(spd);
				arg = args.read_token();
			} 
			else {
				LOG("ERROR: Unexpected token \"%s\"", arg.c_str() );
				return CMD_FAILURE;
			}
			
		}

		if(angle == 0.0f)
		{
			SrVec orientation(0,0,1);
			SrMat mat;
			mat.roty(actor->get_locomotion_ct()->get_navigator()->get_orientation_angle());
			orientation = orientation * mat;

			SrVec v;
			SrVec world_pos = actor->get_locomotion_ct()->get_navigator()->get_world_pos();
			
			v.set(tx-world_pos.x, 0.0f, tz-world_pos.z);
			v.normalize();
			orientation.normalize();

			if(cross(orientation, v).y > 0.0f)
			{
				lrps = -fabs(lrps);
				angle = -acos(dot(orientation, v));
			}
			else 
			{
				lrps = fabs(lrps);
				angle = acos(dot(orientation, v));
			}
		}

		nav_circle->set( 0.0f, 0.0f, 0.0f, rps, lrps, angle, -1, 0, 0, 0, time );
		return CMD_SUCCESS;
	}
	
	else if( arg=="dx" || arg=="dy" || arg == "dz" || arg=="deg_per_sec" || arg=="dps" || arg=="rad_per_sec" || arg=="rps" || arg=="grps" || arg=="lrps" || arg == "time") {
		SkChannelArray channels;
		actor->get_locomotion_ct()->get_navigator()->clear_destination_list();
		actor->get_locomotion_ct()->get_navigator()->has_destination = false;
		int dx_index=-1, dy_index=-1, dz_index=-1, g_angular_index=-1, l_angular_index=-1, id_index = -1;
		//float data[6] = {0,0,0,0,0,0};
		float dx = 0.0f, dy = 0.0f, dz = 0.0f, g_angular = 0.0f, l_angular = 0.0f, l_angle = 0.0f, time = -1.0f;
		int id = -1;

		while( !arg.empty() ) {
			if( arg == "dx" ) {
				if( dx_index == -1 ) {
					dx_index = channels.size();
					//channels.add( SkJointName( SbmCharacter::LOCOMOTION_VELOCITY ), SkChannel::XPos );
				}
				dx = args.read_float();
				//data[ dx_index ] = dx;
			} else if( arg == "dy" ) {
				if( dy_index == -1 ) {
					dy_index = channels.size();
					//channels.add( SkJointName( SbmCharacter::LOCOMOTION_VELOCITY ), SkChannel::YPos );
				}
				dy = args.read_float();
				//data[ dy_index ] = dy;
			} else if( arg == "dz" ) {
				if( dz_index == -1 ) {
					dz_index = channels.size();
					//channels.add( SkJointName( SbmCharacter::LOCOMOTION_VELOCITY ), SkChannel::ZPos );
				}
				dz = args.read_float();
				//data[ dz_index ] = dz;
			} else if( arg == "deg_per_sec" || arg=="dps" ) {
				if( g_angular_index == -1 ) {
					g_angular_index = channels.size();
					//channels.add( SkJointName( SbmCharacter::LOCOMOTION_GLOBAL_ROTATION ), SkChannel::YPos );
				}
				g_angular = (float)RAD(args.read_float());
				//data[ g_angular_index ] = g_angular;
				if( l_angular_index == -1 ) {
					l_angular_index = channels.size();
					//channels.add( SkJointName( SbmCharacter::LOCOMOTION_LOCAL_ROTATION ), SkChannel::YPos );
				}
				l_angular = g_angular;

			} else if( arg == "rad_per_sec" || arg=="rps" ) {
				if( g_angular_index == -1 ) {
					g_angular_index = channels.size();
					//channels.add( SkJointName( SbmCharacter::LOCOMOTION_GLOBAL_ROTATION ), SkChannel::YPos );
				}
				g_angular = args.read_float();

				if( l_angular_index == -1 ) {
					l_angular_index = channels.size();
					//channels.add( SkJointName( SbmCharacter::LOCOMOTION_LOCAL_ROTATION ), SkChannel::YPos );
				}
				l_angular = g_angular;

			} else if(arg=="grps" ) {
				if( g_angular_index == -1 ) {
					g_angular_index = channels.size();
					//channels.add( SkJointName( SbmCharacter::LOCOMOTION_GLOBAL_ROTATION ), SkChannel::YPos );
				}
				g_angular = args.read_float();
	
			} else if(arg=="lrps" ) {
				if( l_angular_index == -1 ) {
					l_angular_index = channels.size();
					//channels.add( SkJointName( SbmCharacter::LOCOMOTION_LOCAL_ROTATION ), SkChannel::YPos );
				}
				l_angular = args.read_float();

			} else if(arg=="angle" ) {
				l_angle = args.read_float();

			} else if(arg=="time" ) {
				time = args.read_float();

			} else if(arg=="id" ) {
				if( id_index == -1 ) {
					id_index = channels.size();
					//channels.add( SkJointName( SbmCharacter::LOCOMOTION_ID ), SkChannel::YPos );
				}
				id = args.read_int();
				//data[ id_index ] = (float)id;
			} else {
				LOG("ERROR: Unexpected token \"%d\".", arg.c_str());
				return CMD_FAILURE;
			}
			if(id_index == -1)
			{
				id_index = channels.size();
				//channels.add( SkJointName( SbmCharacter::LOCOMOTION_ID ), SkChannel::YPos );
				//data[ id_index ] = -1.0f;
				id = -1;
			}
			arg = args.read_token();
		}
		//actor->get_locomotion_ct()->set_motion_time(time);
		nav_circle->set( dx, dy, dz, g_angular, l_angular, l_angle, id, 0, 0, 0, time);

		return CMD_SUCCESS;
	}


	else if(arg == "del")
	{
		int id = args.read_int();
		nav_circle->set( 0, 0, 0, 0, 0, 0, id, 0, 0, 0, -1);
		return CMD_SUCCESS;
	}

	else if(arg=="cx" || arg=="cy" || arg == "cz")
	{
		return CMD_SUCCESS;
	}

	
	/*else if( arg=="circle" ) 
	{
		arg = args.read_token();
		if( arg.empty() || arg=="help" ) {
			cout << "Syntax: test locomotion circle <velocity> <radius>" << endl;
			return CMD_SUCCESS;
		}
		float forward_velocity, radius;

		if( !( istringstream( arg ) >> forward_velocity ) ) {
			cerr << "ERROR: Invalid velocity \"" << arg << "\"." << endl;
			return CMD_FAILURE;
		}
		arg = args.read_token();
		if( !( istringstream( arg ) >> radius ) ) {
			cerr << "ERROR: Invalid radius \"" << arg << "\"." << endl;
			return CMD_FAILURE;
		}

		//MeCtNavigationCircle* nav_circle = new MeCtNavigationCircle();
		//nav_circle->ref();

		//nav_circle->initByRadius( forward_velocity, radius );
		//actor->posture_sched_p->create_track( NULL, NULL, nav_circle );

		//actor->init_locomotion_analyzer("common.sk", mcu_p);

		//nav_circle->unref();

		return CMD_SUCCESS;
	} */


	else {
		return CMD_NOT_FOUND;
	}
}
