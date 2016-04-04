/*
 *  channelbufferWindow.cpp - part of SmartBody-lib's Test Suite
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
#include "channelbufferWindow.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>
#include <sb/SBMotion.h>
#include <bml/bml.hpp>
#include <controllers/me_controller_tree_root.hpp>

ChannelBufferWindow::ChannelBufferWindow(int x, int y, int w, int h, char* name) : Fl_Double_Window(w, h, name), GenericViewer(x, y, w, h), SBWindowListener()
{
	set_default_values();
	char value[10];
	this->begin();

	int yOffset = 20;
	int xOffset = 10;
	Fl_Group* firstGroup = new Fl_Group(xOffset, yOffset, w - 20, h/4 - 20, "");
	firstGroup->begin();
		character = new Fl_Choice(60 + xOffset, 20 + yOffset, w/8, 20, "Character");
		loadCharacters(this);
		character->callback(refreshChannels, this);

		refresh = new Fl_Button(60+w/8+5 + xOffset, 20 + yOffset, w/16, 20, "Refresh");
		refresh->callback(refreshCharacters, this);

		controller = new Fl_Choice(60 + xOffset, 50 + yOffset, w/4, 20, "Controller");
		loadControllers(controller, character);
		controller->callback(refreshControllerChannels, this);

		check_hide_other_channels = new Fl_Check_Button(60 + xOffset, 70 + yOffset, 18, 18, "Hide other channels");
		check_hide_other_channels->callback(refreshHideOtherChannels, this);
		check_hide_other_channels->deactivate();

		motion = new Fl_Choice(60 + xOffset, 100 + yOffset, w/4, 20, "Motion");
		loadMotions(this);
		motion->callback(refreshMotionChannels, this);

		channel_filter = new Fl_Input(50+w/4+w/16+50 + xOffset, 0 + yOffset, w/4-50, 18, "Channels:");
		channel_filter->when(FL_WHEN_CHANGED);
		channel_filter->callback(FilterChannelItem, this);

		channel_list = new Fl_Multi_Browser(50+w/4+w/16 + xOffset, 20 + yOffset, w/4, 120, "");
		loadChannels(this);
		refreshChannelsWidget(this);
		
		channel_monitored_filter = new Fl_Input(50+w/2+w/8+20+50 + xOffset, 0 + yOffset, w/4-50, 18, "Monitored:");
		channel_monitored_filter->when(FL_WHEN_CHANGED);
		channel_monitored_filter->callback(FilterMonitoredChannelItem, this);

		channel_monitor = new Fl_Multi_Browser(50+w/2+w/8+20 + xOffset, 20 + yOffset, w/4, 120, "");
		channel_monitor->when(FL_WHEN_CHANGED);
		channel_monitor->callback(refreshBold, this);
		refreshMonitoredChannelsWidget(this);

		channel_add = new Fl_Button(50+w/2+w/16+10 + xOffset, 20 + yOffset, w/16, 20, ">>>");
		channel_add->callback(addMonitoredChannel, this);

		channel_remove = new Fl_Button(50+w/2+w/16+10 + xOffset, 50 + yOffset, w/16, 20, "<<<");
		channel_remove->callback(removeMonitoredChannel, this);

	firstGroup->end();
	this->resizable(firstGroup);


	yOffset =  h/4+30; 
	
	Fl_Group* secondGroup = new Fl_Group(xOffset, yOffset, w - 20, h/2 - 40, "");
	secondGroup->begin();
		frame_num = new Fl_Input(120 + xOffset, 0 + yOffset, 40, 18, "No. of frames shown:");
		sprintf(value, "%d", num_of_frames);
		frame_num->when(FL_WHEN_ENTER_KEY);
		frame_num->callback(refreshMaxSize, this);
		frame_num->value(value);

		quat = new Fl_Choice(300 + xOffset, 0 + yOffset, (int)(1.5f*w/8), 18, "Rotation shown as:");
		quat->callback(refreshQuat, this);

		show_x = new Fl_Check_Button(300+(int)(1.5f*w/8) + xOffset, 0 + yOffset, 18, 18, "X");
		show_x->callback(refreshShowX, this);
		show_x->value(1);
		show_x->activate();
		show_y = new Fl_Check_Button(300+(int)(1.5f*w/8)+30 + xOffset, 0 + yOffset, 18, 18, "Y");
		show_y->callback(refreshShowY, this);
		show_y->value(1);
		show_y->activate();
		show_z = new Fl_Check_Button(300+(int)(1.5f*w/8)+60 + xOffset, 0 + yOffset, 18, 18, "Z");
		show_z->callback(refreshShowZ, this);
		show_z->value(1);
		show_z->activate();
		show_w = new Fl_Check_Button(300+(int)(1.5f*w/8)+90 + xOffset, 0 + yOffset, 18, 18, "W");
		show_w->callback(refreshShowW, this);
		show_w->value(1);
		show_w->activate();

		initQuat();

		freeze = new Fl_Button(w-200 + xOffset, 0 + yOffset, 80, 18, "Freeze");
		freeze->callback(freezeView, this);
		is_freezed = false;

		reset_camera = new Fl_Button(w-100 + xOffset, 0 + yOffset, 80, 18, "Reset Camera");
		reset_camera->callback(resetCamera, this);

		chartview = new GlChartView(0 + xOffset, 20 + yOffset, w-20, 3*h/4-60, (char*)"");
	secondGroup->end();
	secondGroup->resizable(chartview);
	this->resizable(secondGroup);
	this->size_range(800, 480);

}


void ChannelBufferWindow::reset()
{
	refreshCharacters(NULL, this);
	refreshCharacters(NULL, this);
	resetCamera(NULL, this);	
}

ChannelBufferWindow::~ChannelBufferWindow()
{
	LOG("ChannelBufferWindow::destructor");
	clearChannelItem(this);
}

void ChannelBufferWindow::clearChannelItem(ChannelBufferWindow* window)
{
	for(size_t i = 0; i < window->Channel_item_list.size(); ++i)
	{
		delete window->Channel_item_list[i].label;
	}
	window->Channel_item_list.clear();
}

void ChannelBufferWindow::initChannelItem(ChannelBufferWindow* window, int num)
{
	window->Channel_item_list.resize(num);
	for(int i = 0; i < num; ++i)
	{
		window->Channel_item_list[i].label = new std::string();
		window->Channel_item_list[i].name = new std::string();
	}
}

void ChannelBufferWindow::refreshBold(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	for(int i = 0; i < window->chartview->get_archive()->GetSeriesCount(); ++i)
	{
		window->chartview->get_archive()->GetSeries(i)->bold = false;
	}
	for(int j = 0; j < window->channel_monitor->size(); ++j)
	{
		if(window->channel_monitor->selected(j+1))
		{
			const char* label = window->channel_monitor->text(j+1);//child(j)->label();
			for(size_t i = 0; i < window->Channel_item_list.size(); ++i)
			{
				ChannelItem& item = window->Channel_item_list[i];
				if(item.monitored)
				{
					if(strcmp(item.label->c_str(), label) == 0)
					{
						for(int k = 0; k < window->chartview->get_archive()->GetSeriesCount(); ++k)
						{
							if(strcmp(window->chartview->get_archive()->GetSeries(k)->title.c_str(), item.label->c_str())==0)
							{
								window->chartview->get_archive()->GetSeries(k)->bold = true;
								break;
							}
						}
					}
				}
			}
		}
	}
}

void ChannelBufferWindow::FilterChannelItem(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	FilterItem(window, window->channel_list, window->channel_filter, false);
	refreshChannelsWidget(window);
}

void ChannelBufferWindow::FilterMonitoredChannelItem(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	FilterItem(window, window->channel_monitor, window->channel_monitored_filter, true);
	refreshMonitoredChannelsWidget(window);
}

void ChannelBufferWindow::FilterItem(ChannelBufferWindow* window, Fl_Browser* list, Fl_Input* filter, bool monitored)
{
	const char* keyword = filter->value();
	if(keyword[0] == '\0')
	{
		for(size_t i = 0; i < window->Channel_item_list.size(); ++i)
		{
			if(window->Channel_item_list[i].monitored == monitored)
			{
				window->Channel_item_list[i].not_in_search = false;
			}
		}
		return;
	}
	for(size_t i = 0; i < window->Channel_item_list.size(); ++i)
	{
		if(window->Channel_item_list[i].monitored != monitored) continue;
		const char* item = window->Channel_item_list[i].label->c_str();
		if(strstr(item, keyword)!= NULL) 
		{
			window->Channel_item_list[i].not_in_search = false;
		}
		else 
		{
			window->Channel_item_list[i].not_in_search = true;
		}
	}
}

void ChannelBufferWindow::resetCamera(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	int i = -1;
	if(strcmp(window->quat->mvalue()->label(), "Quaternion") == 0) i = 0;
	else if(strcmp(window->quat->mvalue()->label(), "Euler angle") == 0) i = 1;
	else if (strcmp(window->quat->mvalue()->label(), "Swing twist") == 0) i = 3;
	window->chartview->init_camera(i);
	window->chartview->update_coordinate = true;
}

void ChannelBufferWindow::freezeView(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	window->is_freezed = !window->is_freezed;
	if(window->is_freezed) window->freeze->label("Unfreeze");
	else window->freeze->label("Freeze");

}

void ChannelBufferWindow::refreshShowX(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	window->chartview->show_x = !window->chartview->show_x;
}

void ChannelBufferWindow::refreshShowY(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	window->chartview->show_y = !window->chartview->show_y;
}

void ChannelBufferWindow::refreshShowZ(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	window->chartview->show_z = !window->chartview->show_z;
}

void ChannelBufferWindow::refreshShowW(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	window->chartview->show_w = !window->chartview->show_w;
}

void ChannelBufferWindow::refreshQuat(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	int i = 0;

	if(strcmp(window->quat->mvalue()->label(), "Quaternion") == 0) i = 0;
	else if(strcmp(window->quat->mvalue()->label(), "Euler angle") == 0) i = 1;
	else if (strcmp(window->quat->mvalue()->label(), "Axis-Angle") == 0) i = 3;
	else if (strcmp(window->quat->mvalue()->label(), "Quat Velocity") == 0) i = 4;
	else if (strcmp(window->quat->mvalue()->label(), "Euler Velocity") == 0) i = 5;
	else if (strcmp(window->quat->mvalue()->label(), "Axis-Angle Velocity") == 0) i = 6;
	
	window->chartview->set_quat_show_type(i);
	setXYZVisibility(window);
}

void ChannelBufferWindow::initQuat()
{
	quat->add("Quaternion");
	quat->add("Euler angle");
	quat->add("Axis-Angle");	
	quat->add("Quat Velocity");
	quat->add("Euler Velocity");
	quat->add("Axis-Angle Velocity");
	quat->value(0);
	setXYZVisibility(this);
}

void ChannelBufferWindow::setXYZVisibility(ChannelBufferWindow* window)
{
	Fl_Color color;
	const Fl_Menu_Item* menuValue = window->quat->mvalue();
	if (!menuValue)
		return;
	if(strcmp(menuValue->label(), "Quaternion") == 0)
	{
		color = 0x38;
		window->show_x->activate();
		window->show_y->activate();
		window->show_z->activate();
		window->show_w->activate();
		window->show_x->labelcolor(color);
		window->show_y->labelcolor(color);
		window->show_z->labelcolor(color);
		window->show_w->labelcolor(color);
	}
	else if(strcmp(menuValue->label(), "Euler angle") == 0)
	{
		color = 47;
		window->show_x->activate();
		window->show_y->activate();
		window->show_z->activate();
		window->show_w->deactivate();
		window->show_w->labelcolor(color);
	}
	else if(strcmp(menuValue->label(), "Axis-Angle") == 0)
	{
		color = 47;
		window->show_x->activate();
		window->show_y->activate();
		window->show_z->activate();
		window->show_w->deactivate();
		window->show_w->labelcolor(color);		
	}
}

void ChannelBufferWindow::refreshMaxSize(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	if(window->mode == 2) 
	{
		window->num_of_frames = atoi(window->frame_num->value());
		return;
	}
	refreshMaxSize(window, atoi(window->frame_num->value()));
}

void ChannelBufferWindow::refreshMaxSize(ChannelBufferWindow* window, int num)
{
	window->num_of_frames = num;
	int series_count = window->chartview->get_archive()->GetSeriesCount();
	for(int i = 0; i < series_count; ++i)
	{
		window->chartview->get_archive()->GetSeries(i)->SetMaxSize(window->num_of_frames);
	}
	window->chartview->coordinate.SetXSize((float)window->num_of_frames);
}

void ChannelBufferWindow::set_default_values()
{
	num_of_frames = 800;
	mode = 1;
	hide_other_channels = true;
	no_motion = "------";
}

void ChannelBufferWindow::loadMotions(ChannelBufferWindow* window)
{
	window->motion->clear();
	window->motion->add(window->no_motion.c_str());
	std::vector<std::string> motionNames = SmartBody::SBScene::getScene()->getMotionNames();
	for (size_t i = 0; i < motionNames.size(); i++)
	{
		const std::string & name = motionNames[i];
		window->motion->add(name.c_str());
	}
	window->motion->value(0);
}

void ChannelBufferWindow::loadCharacters(ChannelBufferWindow* window)
{	
	Fl_Choice* characterChoice = window->character;
	characterChoice->clear();
	characterChoice->add(window->no_motion.c_str());
	const std::vector<std::string>& charNames = SmartBody::SBScene::getScene()->getCharacterNames();
	for (size_t i = 0; i < charNames.size(); i++)
	{
		const std::string & charName = charNames[i];
		characterChoice->add(charName.c_str());
	}
	int ind = 0;
	characterChoice->value(0);
}

void ChannelBufferWindow::loadControllers(Fl_Choice* controller, Fl_Choice* character)
{	
	if (!character || !controller)
		return;
	if(character->mvalue()== NULL) return;

	controller->clear();
	SmartBody::SBCharacter* actor = SmartBody::SBScene::getScene()->getCharacter(character->mvalue()->label());

	if (!actor) return;

	controller->add("All controllers");

	int ct_num = actor->ct_tree_p->count_controllers();
	for(int i = 0; i < ct_num; ++i)
	{
		controller->add(actor->ct_tree_p->controller(i)->getName().c_str());
		actor->ct_tree_p->controller(i)->record_buffer_changes(true);
	}
	if (controller->mvalue() == NULL)
		controller->value(0);
}

void ChannelBufferWindow::refreshChannelsWidget(ChannelBufferWindow* window)
{
	window->channel_list->clear();
	int num = window->Channel_item_list.size();
	Fl_Color color;
	color = 45;
	for(int i = 0; i < num; ++i)
	{
		ChannelItem& item = window->Channel_item_list[i];
		if(!item.monitored && !item.not_in_search && !item.motion_filtered)
		{
			if(window->mode == 2)
			{
				window->channel_list->add(item.label->c_str());
			}
			else 
			{
				if(!item.channel_filtered)
				{
					window->channel_list->add(item.label->c_str());
				}
				else if(!window->hide_other_channels)
				{
					window->channel_list->add(item.label->c_str());
					window->channel_list->child(window->channel_list->size()-1)->labelcolor(color);
				}
			}
		}
	}
}

void ChannelBufferWindow::refreshMonitoredChannelsWidget(ChannelBufferWindow* window)
{
	Fl_Color color;
	color = 47;
	window->channel_monitor->clear();
	int num = window->Channel_item_list.size();
	for(int i = 0; i < num; ++i)
	{
		ChannelItem& item = window->Channel_item_list[i];
		if(item.monitored && !item.not_in_search)
		{
			window->channel_monitor->add(item.label->c_str());
			if(item.motion_filtered) window->channel_monitor->child(window->channel_monitor->size()-1)->labelcolor(color);
		}
	}
}

void ChannelBufferWindow::loadChannels(ChannelBufferWindow* window)
{
	Fl_Choice* character = window->character;
	if(character->mvalue()== NULL) return;
	SmartBody::SBCharacter* actor = SmartBody::SBScene::getScene()->getCharacter(character->mvalue()->label());
	SmartBody::SBMotion* motion = SmartBody::SBScene::getScene()->getMotion(window->motion->mvalue()->label());
	if (!actor && !motion) return;
	
	SkChannelArray* pchannel = NULL;
	if (actor)
	{
		if (!actor->getSkeleton())
			return;
		pchannel = &actor->getSkeleton()->channels(); 
	}
	else if (motion)
	{
		pchannel = &motion->channels();
	}

	SkChannelArray& channels = *pchannel;//skeleton->channels();
	int numChannels = channels.size();
	clearChannelItem(window);
	initChannelItem(window, numChannels);

	SkJoint* joint = NULL;
	char str[100];
	int channel_index = 0;
	char ext[3];

	for (int i = 0; i < numChannels; i++)
	{
		//joint = channels.joint(i);
		//if(joint == NULL) continue;
		
		SkChannel& channel = channels[i];
		int channelSize = channel.size();
		if(channel.type == SkChannel::XPos) sprintf(ext, "_x");
		else if(channel.type == SkChannel::YPos) sprintf(ext, "_y");
		else if(channel.type == SkChannel::ZPos) sprintf(ext, "_z");
		else ext[0] = '\0';

		//sprintf(str, "%s%s (%d)", joint->jointName().c_str(), ext, channelSize);
		sprintf(str, "%s%s (%d)", channels.mappedName(i).c_str(), ext, channelSize);
		ChannelItem& item = window->Channel_item_list[i];
		item.channel_filtered = false;
		item.motion_filtered = false;
		item.monitored = false;
		item.not_in_search = false;
		item.index = channel_index;
		item.label->assign(str);
		//item.name->assign(joint->jointName());
		item.name->assign(channels.mappedName(i));
		item.type = channel.type;
		channel_index += channelSize;
	}

	
	
}

const char* ChannelBufferWindow::getSelectedCharacterName()
{
	const Fl_Menu_Item* item = character->mvalue();
	if(item == NULL) return NULL;
	return character->mvalue()->label();
}

void ChannelBufferWindow::refreshCharacters(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	loadCharacters(window);
	loadControllers(window->controller, window->character);
	loadMotions(window);
}

void ChannelBufferWindow::refreshControllers(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	loadControllers(window->controller, window->character);
}

void ChannelBufferWindow::refreshChannels(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	loadChannels(window);
	loadControllers(window->controller,window->character);
	window->chartview->get_archive()->ClearSeries();
	refreshChannelsWidget(window);
	refreshMonitoredChannelsWidget(window);
}

void ChannelBufferWindow::refreshMotionChannels(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	size_t j = 0;
	refreshControllerVisibilities(window);
	
	if(strcmp(window->motion->mvalue()->label(), window->no_motion.c_str()) == 0)
	{
		for(j = 0; j < window->Channel_item_list.size(); ++j)
		{
			ChannelItem& item = window->Channel_item_list[j];
			item.motion_filtered = false;
			if(item.monitored) 
			{
				window->chartview->get_archive()->GetSeries(item.label->c_str())->Reset();
			}
		}
		window->mode = 1;
		refreshMaxSize(window, atoi(window->frame_num->value()));
		refreshChannelsWidget(window);
		return;
	}
	window->mode = 2;
	SmartBody::SBCharacter* actor = SmartBody::SBScene::getScene()->getCharacter(window->character->mvalue()->label());
	SmartBody::SBMotion * motion = SmartBody::SBScene::getScene()->getMotion(window->motion->mvalue()->label());
	if (motion)
	{
		loadChannels(window);
		//motion->connect(actor->getSkeleton());
		refreshMaxSize(window, motion->frames());
		SkChannelArray& channels = motion->channels();
		for(size_t i = 0; i < window->Channel_item_list.size(); ++i)
		{
			window->Channel_item_list[i].motion_filtered = true;
		}
		for(int i = 0; i < channels.size(); ++i)
		{
#if 0
			if(channels.joint(i) == NULL) continue;
			std::string name = channels.joint(i)->jointName();
			SkChannel::Type type = channels.get(i).type;
			for(j = 0; j < window->Channel_item_list.size(); ++j)
			{
				if(strcmp(window->Channel_item_list[j].name->c_str(), name.c_str()) == 0
					&& window->Channel_item_list[j].type == type)
				{
					window->Channel_item_list[j].motion_filtered = false;
					if(window->Channel_item_list[j].monitored) 
						fillSeriesWithMotionData(window, motion, NULL, window->Channel_item_list[j]);
					break;
				}
			}
#else
			//if(channels.mappedName(i) == NULL) continue;
			std::string name = channels.mappedName(i);//joint(i)->jointName();
			SkChannel::Type type = channels.get(i).type;
			for(j = 0; j < window->Channel_item_list.size(); ++j)
			{
				if(strcmp(window->Channel_item_list[j].name->c_str(), name.c_str()) == 0
					&& window->Channel_item_list[j].type == type)
				{
					window->Channel_item_list[j].motion_filtered = false;
					if(window->Channel_item_list[j].monitored) 
						fillSeriesWithMotionData(window, motion, NULL, window->Channel_item_list[j]);
					break;
				}
			}
#endif
		}
	}
	refreshChannelsWidget(window);
}

void ChannelBufferWindow::refreshHideOtherChannels(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	window->hide_other_channels = !window->hide_other_channels;
	refreshChannelsWidget(window);
}

void ChannelBufferWindow::refreshControllerVisibilities(ChannelBufferWindow* window)
{
	if (!window->motion->mvalue() || !window->controller->mvalue())
		return;

	if(strcmp(window->motion->mvalue()->label(), window->no_motion.c_str()) == 0)
	{
		window->controller->activate();
	}
	else 
	{
		window->controller->deactivate();
		window->check_hide_other_channels->deactivate();
		return;
	}
	if(strcmp(window->controller->mvalue()->label(), "All controllers") == 0)
	{
		window->check_hide_other_channels->deactivate();
	}
	else window->check_hide_other_channels->activate();
}

void ChannelBufferWindow::refreshControllerChannels(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	if(window->character->mvalue() == NULL) return;
	Fl_Browser* channels = window->channel_list;

	refreshControllerVisibilities(window);
	if(strcmp(window->controller->mvalue()->label(), "All controllers") == 0)
	{
		for(size_t i = 0; i < window->Channel_item_list.size(); ++i)
		{
			window->Channel_item_list[i].channel_filtered = false;
		}
		refreshChannelsWidget(window);
		return;
	}
	SbmCharacter* actor = SmartBody::SBScene::getScene()->getCharacter(window->character->mvalue()->label());
	
	for(size_t i = 0; i < window->Channel_item_list.size(); ++i)
	{
		window->Channel_item_list[i].channel_filtered = true;
	}
	
	int ct_num = actor->ct_tree_p->count_controllers();
	for(int i = 0; i < ct_num; ++i)
	{
		if(actor->ct_tree_p->controller(i)->getName() == window->controller->mvalue()->label())
		{
			std::vector<float> buff = actor->ct_tree_p->controller(i)->get_buffer_changes();
			SkChannelArray& channelsInUse = actor->ct_tree_p->controller(i)->controller_channels();
			for(int j = 0; j < channelsInUse.size(); ++j)
			{
				int index = actor->ct_tree_p->controller(i)->getContextChannel(j);
				if (index >= 0 && index < (int)window->Channel_item_list.size())
					window->Channel_item_list[index].channel_filtered = false;
			}
		}
	}
	refreshChannelsWidget(window);
}

void ChannelBufferWindow::addMonitoredChannel(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	GlChartViewSeries* series = NULL;
	std::map<std::string, SkMotion*>::iterator motionIter;
	SmartBody::SBMotion * motion = NULL;
	if(window->mode == 2)
	{
		SmartBody::SBCharacter* actor = SmartBody::SBScene::getScene()->getCharacter(window->character->mvalue()->label());
		std::string moName = window->motion->mvalue()->label();
		motion = SmartBody::SBScene::getScene()->getMotion(moName);
		if (motion && actor)
		{
			motion->connect(actor->getSkeleton());
		}
	}

	for(int i = 0; i < window->channel_list->size(); ++i)
	{
		if(window->channel_list->selected(i+1))
		{
			for(size_t j = 0; j < window->Channel_item_list.size(); ++j)
			{
				if(strcmp(window->Channel_item_list[j].label->c_str(), window->channel_list->text(i+1)) == 0)
				{
					ChannelItem& item = window->Channel_item_list[j];
					item.monitored = true;
					const char* label = item.label->c_str();
					window->chartview->get_archive()->NewSeries(label, get_size(label), window->Channel_item_list[j].index);
					series = window->chartview->get_archive()->GetLastSeries();
					series->SetMaxSize(window->num_of_frames);
					if(window->mode == 2)
					{
						fillSeriesWithMotionData(window, motion, series, item);
					}
					break;
				}
			}
		}
	}
	refreshChannelsWidget(window);
	refreshMonitoredChannelsWidget(window);
}

void ChannelBufferWindow::fillSeriesWithMotionData(ChannelBufferWindow* window, SkMotion* motion, GlChartViewSeries* series, ChannelItem& item)
{
	if(motion == NULL) return;

	if(series == NULL)
	{
		series = window->chartview->get_archive()->GetSeries(item.label->c_str());
	}
	float val[4];
	int index = 0;
	SkChannelArray& channels = motion->channels();
	for(index = 0; index < channels.size(); ++index)
	{
		//if(channels[index].joint == NULL) continue;
		std::string name = channels.mappedName(index);//channels[index].joint->jointName().c_str();
		if(strcmp(name.c_str(), item.name->c_str()) == 0 && item.type == channels[index].type)
		{
			break;
		}
	}
	if(index == channels.size()) 
	{
		series->Clear();
		return;
	}
	float motionDt = motion->duration()/(float)motion->frames();
	series->dt = motionDt;
	for(int k = 0; k < window->num_of_frames; ++k)
	{
		//motion->apply_frame(k);			
		//channels[index].get(val);
		motion->getChannelValueAtFrame(k,index,val); // provide a more direct API for getting the channel values
		if(series->data_type == 1)
		{
			series->Push(val[0]);
		}
		else if(series->data_type == 3)
		{
			series->Push(val[0], val[1], val[2]);
		}
		else if(series->data_type == 4)
		{
			series->Push(val[0], val[1], val[2], val[3]);
		}
		
	}
}

void ChannelBufferWindow::removeMonitoredChannel(Fl_Widget* widget, void* data)
{
	ChannelBufferWindow* window = (ChannelBufferWindow*) data;
	for(int i = 0; i < window->channel_monitor->size(); ++i)
	{
		if(window->channel_monitor->selected(i+1))
		{
			for(size_t j = 0; j < window->Channel_item_list.size(); ++j)
			{
				if(strcmp(window->Channel_item_list[j].label->c_str(), window->channel_monitor->text(i+1)) == 0)
				{
					ChannelItem& item = window->Channel_item_list[j];
					const char* label = item.label->c_str();
					item.monitored = false;
					window->chartview->get_archive()->DeleteSeries(label);
					break;
				}
			}
		}
	}
	refreshChannelsWidget(window);
	refreshMonitoredChannelsWidget(window);
}

int ChannelBufferWindow::get_size(const char* title)
{
	int len = strlen(title);
	bool number = false;

	for(int i = len-1; i >= 0; --i)
	{
		if(number)
		{
			return title[i]-48;
		}
		if(title[i] == ')') number = true;
	}
	return -1;
}

void ChannelBufferWindow::draw()
{
	Fl_Double_Window::draw();
	chartview->render();
}

void ChannelBufferWindow::updateGUI()
{
}

void ChannelBufferWindow::label_viewer(std::string name)
{
	this->label(strdup(name.c_str()));
}

void ChannelBufferWindow::show_viewer()
{
	this->show();
}

void ChannelBufferWindow::hide_viewer()
{
	this->hide();
}

void ChannelBufferWindow::update_viewer()
{
}

void ChannelBufferWindow::show()
{
	SBWindowListener::windowShow();
	Fl_Double_Window::show();
}

void ChannelBufferWindow::hide()
{
	SBWindowListener::windowHide();
	Fl_Double_Window::hide();
}

void ChannelBufferWindow::update()
{
	if(!is_freezed)
	{
		if (character->size() == 0)
			return;

		SmartBody::SBCharacter* char_p = NULL;
		SmartBody::SBCharacter* actor = SmartBody::SBScene::getScene()->getCharacter(character->mvalue()->label());
		const std::vector<std::string>& charNames = SmartBody::SBScene::getScene()->getCharacterNames();
		for (size_t i = 0; i < charNames.size(); i++)
		{
			SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(charNames[i]);
			const char* name = getSelectedCharacterName();
			if( name && strcmp(character->getName().c_str(), name) == 0)
			{
				char_p = character;
				break;
			}
		}
		if(char_p != NULL)
		{
			if(mode != 2)
			{
				SrBuffer<float>& buffer = char_p->ct_tree_p->getLastFrame().buffer();
				chartview->get_archive()->Update(buffer);
			}
			if(mode == 1 && controller->mvalue())
			{
				int buff_counter = 0;
				int ct_num = actor->ct_tree_p->count_controllers();
				for(int i = 0; i < ct_num; ++i)
				{
					if(actor->ct_tree_p->controller(i)->getName() == controller->mvalue()->label())
					{
						std::vector<float> buff = actor->ct_tree_p->controller(i)->get_buffer_changes();
						SkChannelArray& channelsInUse = actor->ct_tree_p->controller(i)->controller_channels();
						for(int j = 0; j < channelsInUse.size(); ++j)
						{
							int index = actor->ct_tree_p->controller(i)->getContextChannel(j);
							if (index < 0 || index >= (int)Channel_item_list.size())
								continue;

							if(Channel_item_list[index].monitored)
							{
								if(Channel_item_list[index].type == SkChannel::XPos
								|| Channel_item_list[index].type == SkChannel::YPos
								|| Channel_item_list[index].type == SkChannel::ZPos)
								{
									chartview->get_archive()->GetSeries(Channel_item_list[index].label->c_str())->SetLast(buff[buff_counter]);
								}
								else if(Channel_item_list[index].type == SkChannel::Quat) 
								{
									chartview->get_archive()->GetSeries(Channel_item_list[index].label->c_str())->SetLast(buff[buff_counter], buff[buff_counter+1], buff[buff_counter+2], buff[buff_counter+3]);
								}
							}
							if(Channel_item_list[index].type == SkChannel::XPos
							|| Channel_item_list[index].type == SkChannel::YPos
							|| Channel_item_list[index].type == SkChannel::ZPos)
							{
								++buff_counter;
							}
							else if(Channel_item_list[index].type == SkChannel::Quat) 
							{	
								buff_counter+= 4;
							}
						}
						break;
					}
				}
			}
		}
	}
	chartview->render();
}

void ChannelBufferWindow::OnCharacterDelete( const std::string & name )
{
	if (name == getSelectedCharacterName())
	{
		character->value(0);
		clearChannelItem(this);
		refreshChannels(this->character, this);
		refreshChannelsWidget(this);
	}
}

void ChannelBufferWindow::OnCharacterUpdate( const std::string & name )
{
	if (name == getSelectedCharacterName())
	{
		character->value(0);
		clearChannelItem(this);
		refreshChannels(this->character, this);
		refreshChannelsWidget(this);
	}
}
 
void ChannelBufferWindow::OnSimulationStart()
{
	character->value(0);
	clearChannelItem(this);
	refreshChannels(this->character, this);
	refreshChannelsWidget(this);
}

void ChannelBufferWindow::OnSimulationUpdate()
{
	update();
}

ChannelBufferViewerFactory::ChannelBufferViewerFactory()
{
}

GenericViewer* ChannelBufferViewerFactory::create(int x, int y, int w, int h)
{
	ChannelBufferWindow* channelbufferWindow = new ChannelBufferWindow(x, y, w, h, (char*)"Channel Buffer");
	return channelbufferWindow;
}

void ChannelBufferViewerFactory::destroy(GenericViewer* viewer)
{
	delete viewer;
}


