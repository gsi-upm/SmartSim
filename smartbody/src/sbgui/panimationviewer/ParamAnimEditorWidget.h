/*
 *  ParamAnimEditorWidget.h - part of SmartBody-lib's Test Suite
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
 *      Yuyu Xu, USC
 */

#ifndef _PARAMANIMEDITORWIDGET_
#define _PARAMANIMEDITORWIDGET_

#include <FL/Fl_Slider.H>
#include "nle/NonLinearEditorWidget.h"

class PABlendEditor;

class ParamAnimEditorWidget : public nle::EditorWidget
{
	public:
		ParamAnimEditorWidget(Fl_Group* e, int x, int y, int w, int h, char* name);

		virtual void changeBlockSelectionEvent(nle::Block* block);
		virtual void changeTrackSelectionEvent(nle::Track* track);
		virtual void changeMarkSelectionEvent(nle::Mark* mark);
		virtual void releaseBlockEvent(nle::Block* block);
		virtual void releaseTrackEvent(nle::Track* track);
		virtual void releaseMarkEvent(nle::Mark* mark);
		virtual void setup();
		virtual int handle(int event);

		void setBlockSelectionChanged(bool val);
		bool getBlockSelectionChanged();
		void setTrackSelectionChanged(bool val);
		bool getTrackSelectionChanged();
		void setMarkSelectionChanged(bool val);
		bool getMarkSelectionChanged();

		virtual void draw();

		void setLocalTimes(std::vector<double>& t);
		std::vector<double>& getLocalTimes();
		void setShowScrubLine(bool val);

		void setAlignTimes(std::vector<double>& t);
		std::vector<double>& getAlignTimes();
		void setShowAlignLine(bool val);

		nle::Mark* getSelectedCorrespondancePointIndex(int& index);

protected:
		virtual void drawBlock(nle::Block* block, int trackNum, int blockNum);
		virtual void drawMark(nle::Block* block, nle::Mark* mark, int trackNum, int blockNum, int markNum);

		bool blockSelectionChanged;
		bool trackSelectionChanged;
		bool markSelectionChanged;

		std::vector<double> scrubTimes;
		bool showScrubLine;
		std::vector<double> alignTimes;
		bool showAlignLine;
		Fl_Group* parentGroup;

};

#endif
