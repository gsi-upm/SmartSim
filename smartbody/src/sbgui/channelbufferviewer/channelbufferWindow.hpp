/*
 *  channelbufferWindow.hpp - part of SmartBody-lib's Test Suite
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
 */

#ifndef _CHANNEL_BUFFER_WINDOW_H_
#define _CHANNEL_BUFFER_WINDOW_H_

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Multi_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>

#include <sk/sk_channel.h>
#include <sk/sk_motion.h>
#include <sbm/GenericViewer.h>
#include "GlChartView.hpp"
#include <SBWindowListener.h>

class ChannelItem
{
public:
	int index;
	std::string * name;
	std::string * label;
	SkChannel::Type type;
	bool monitored;
	bool channel_filtered;
	bool motion_filtered;
	bool not_in_search;
};


class ChannelBufferWindow : public GenericViewer, public Fl_Double_Window, public SBWindowListener
{
public:
	ChannelBufferWindow(int x, int y, int w, int h, char* name);
	~ChannelBufferWindow();

	virtual void label_viewer(std::string name);
	virtual void show_viewer();
	virtual void hide_viewer();
	virtual void update_viewer();
	void draw();
	int handle(int event) { return Fl_Double_Window::handle(event); }   
	void updateGUI();
	void show();  
	void hide();

	void reset();

	void generateBML(Fl_Widget* widget, void* data);

	GlChartView* chartview;
	Fl_Choice* character;
	Fl_Choice* controller;
	Fl_Choice* motion;
	Fl_Choice* quat;
	Fl_Check_Button* show_x;
	Fl_Check_Button* show_y;
	Fl_Check_Button* show_z;
	Fl_Check_Button* show_w;
	Fl_Check_Button* check_hide_other_channels;
	Fl_Button* refresh;
	Fl_Button* freeze;
	Fl_Button* reset_camera;
	Fl_Multi_Browser* channel_list;
	Fl_Multi_Browser* channel_monitor;
	Fl_Button* channel_add;
	Fl_Button* channel_remove;
	Fl_Input* frame_num;
	Fl_Input* channel_filter;
	Fl_Input* channel_monitored_filter;

	std::vector<ChannelItem> Channel_item_list;

	int num_of_frames;
	bool is_freezed;
	bool hide_other_channels;

	int mode; //0: character; 1: controller; 2: motion

	void OnCharacterDelete( const std::string & name );
	void OnCharacterUpdate( const std::string & name );
	void OnSimulationStart( );
	void OnSimulationUpdate( );

protected:
	std::string no_motion;

public:
	const char* getSelectedCharacterName();
	void update();

	static void clearChannelItem(ChannelBufferWindow* window);
	static void initChannelItem(ChannelBufferWindow* window, int num);
	static void loadCharacters(ChannelBufferWindow* window);
	static void loadControllers(Fl_Choice* controller, Fl_Choice* character);
	static void loadChannels(ChannelBufferWindow* window);
	static void loadMotions(ChannelBufferWindow* window);

	static void refreshMotionChannels(Fl_Widget* widget, void* data);
	static void refreshChannelsWidget(ChannelBufferWindow* window);
	static void refreshMonitoredChannelsWidget(ChannelBufferWindow* window);
	static void refreshMaxSize(ChannelBufferWindow* window, int num);

	static void refreshBold(Fl_Widget* widget, void* data);
	static void refreshCharacters(Fl_Widget* widget, void* data);
	static void refreshControllers(Fl_Widget* widget, void* data);
	static void refreshControllerChannels(Fl_Widget* widget, void* data);
	static void refreshHideOtherChannels(Fl_Widget* widget, void* data);
	static void refreshChannels(Fl_Widget* widget, void* data);
	static void refreshMaxSize(Fl_Widget* widget, void* data);
	static void refreshQuat(Fl_Widget* widget, void* data);
	static void resetCamera(Fl_Widget* widget, void* data);
	static void freezeView(Fl_Widget* widget, void* data);
	static void FilterChannelItem(Fl_Widget* widget, void* data);
	static void FilterMonitoredChannelItem(Fl_Widget* widget, void* data);
	static void refreshControllerVisibilities(ChannelBufferWindow* window);

	static void refreshShowX(Fl_Widget* widget, void* data);
	static void refreshShowY(Fl_Widget* widget, void* data);
	static void refreshShowZ(Fl_Widget* widget, void* data);
	static void refreshShowW(Fl_Widget* widget, void* data);

	static void setXYZVisibility(ChannelBufferWindow* window);

protected:
	void initQuat();
	void set_default_values();
	static void clearMonitoredChannel(ChannelBufferWindow* window);
	static void addMonitoredChannel(Fl_Widget* widget, void* data);
	static void FilterItem(ChannelBufferWindow* window, Fl_Browser* list, Fl_Input* filter, bool monitored);
	static void removeMonitoredChannel(Fl_Widget* widget, void* data);
	static int get_size(const char* title);
	static void fillSeriesWithMotionData(ChannelBufferWindow* window, SkMotion* motion, GlChartViewSeries* series, ChannelItem& item);
};

class ChannelBufferViewerFactory : public GenericViewerFactory
{
public:
	ChannelBufferViewerFactory();

	virtual GenericViewer* create(int x, int y, int w, int h);
	virtual void destroy(GenericViewer* viewer);
};
#endif
