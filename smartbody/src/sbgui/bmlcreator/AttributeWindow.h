/**************************************************
Copyright 2005 by Ari Shapiro and Petros Faloutsos

DANCE
Dynamic ANimation and Control Environment

 ***************************************************************
 ******General License Agreement and Lack of Warranty ***********
 ****************************************************************

This software is distributed for noncommercial use in the hope that it will 
be useful but WITHOUT ANY WARRANTY. The author(s) do not accept responsibility
to anyone for the consequences	of using it or for whether it serves any 
particular purpose or works at all. No warranty is made about the software 
or its performance. Commercial use is prohibited. 

Any plugin code written for DANCE belongs to the developer of that plugin,
who is free to license that code in any manner desired.

Content and code development by third parties (such as FLTK, Python, 
ImageMagick, ODE) may be governed by different licenses.
You may modify and distribute this software as long as you give credit 
to the original authors by including the following text in every file 
that is distributed: */

/*********************************************************
	Copyright 2005 by Ari Shapiro and Petros Faloutsos

	DANCE
	Dynamic ANimation and Control Environment
	-----------------------------------------
	AUTHOR:
		Ari Shapiro (ashapiro@cs.ucla.edu)
	ORIGINAL AUTHORS: 
		Victor Ng (victorng@dgp.toronto.edu)
		Petros Faloutsos (pfal@cs.ucla.edu)
	CONTRIBUTORS:
		Yong Cao (abingcao@cs.ucla.edu)
		Paco Abad (fjabad@dsic.upv.es)
**********************************************************/

#ifndef _ATTRIBUTEWINDOW_
#define _ATTRIBUTEWINDOW_

#include <FL/Fl_Scroll.H>
#include "vhcl.h"
#include "sb/SBObserver.h"
#include "sb/SBAttribute.h"
#include <map>

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Choice.H>
#include "../flu/Flu_Collapsable_Group.h"

class SBObject;

class AttributeWindow : public Fl_Group, public SmartBody::SBObserver
{
public:
	AttributeWindow(SmartBody::SBObject*, int,int,int,int,const char*, bool upDownBox = true);
	~AttributeWindow();

	void setDirty(bool val);
	void draw();
	//void show();
	//void layout();

	void setOffset(int pixels);
	int getOffset();
	void setObject(SmartBody::SBObject* g);
	SmartBody::SBObject* getObject();

	void addChoice(Fl_Choice* choice, const std::string& val);
	
	void setAttributeInfo(Fl_Widget* widget, SmartBody::SBAttributeInfo* attrInfo);
	void reorderAttributes();
	void cleanUpAttributesInfo();
	void cleanUpWidgets();
	virtual void notify(SmartBody::SBSubject* subject);

	static void BoolCB(Fl_Widget* w, void *data);
	static void IntCB(Fl_Widget* w, void *data);
	static void DoubleCB(Fl_Widget* w, void *data);
	static void StringCB(Fl_Widget* w, void *data);
	static void Vec3CB(Fl_Widget* w, void *data);
	static void ActionCB(Fl_Widget* w, void *data);

	static void ExpandAllCB(Fl_Widget* w, void *data);
	static void CollapseAllCB(Fl_Widget* w, void *data);

	static void EditStringCB(Fl_Widget* w, void *data);

	static const uchar ATTRIBUTEWINDOWTYPE = (uchar)240;

	SmartBody::SBObject* object;

	bool dirty;
	std::map<std::string, Fl_Widget*> widgetMap;
	std::map<Fl_Widget*, std::string> reverseWidgetMap;
	std::map<std::string, Flu_Collapsable_Group*> widgetGroupMap;
	std::map<Fl_Widget*, std::string> reverseWidgetGroupMap;
	Fl_Scroll* mainGroup;
	Fl_Pack* mainPack;
	int _offset;
};

#endif
