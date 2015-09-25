/*
 *  ParamAnimEditorWidget.cpp - part of SmartBody-lib's Test Suite
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

#include "vhcl.h"
#include "ParamAnimEditorWidget.h"
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationStateManager.h>
#include <sb/SBScene.h>
#include "ParamAnimBlock.h"
#include "ParamAnimStateEditor.h"
#include "ParamAnimTransitionEditor.h"

ParamAnimEditorWidget::ParamAnimEditorWidget(Fl_Group* g, int x, int y, int w, int h, char* name) : EditorWidget(x, y, w, h, name)
{
	blockSelectionChanged = false;
	trackSelectionChanged = false;
	this->lockBlockFunc(true);
	showScrubLine = false;
	showAlignLine = true;
	parentGroup = g;

}
	
void ParamAnimEditorWidget::drawBlock(nle::Block* block, int trackNum, int blockNum)
{
	EditorWidget::drawBlock(block, trackNum, blockNum);
}

void ParamAnimEditorWidget::drawMark(nle::Block* block, nle::Mark* mark, int trackNum, int blockNum, int markNum)
{
	CorrespondenceMark* cMark = dynamic_cast<CorrespondenceMark*>(mark);
	IntervalMark* iMark = dynamic_cast<IntervalMark*>(mark);
	if (cMark || iMark)
	{
		int viewableStart = this->convertTimeToPosition(getViewableTimeStart());
		int viewableEnd = this->convertTimeToPosition(getViewableTimeEnd());	
		
		int trackTop = padding + timeWindowHeight + trackNum * trackHeight;// + yOffset;
		
		int bounds[4];
		mark->getBounds(bounds[0], bounds[1], bounds[2], bounds[3]);
		//bounds[1] += yOffset;

		if (viewableStart > bounds[0] + bounds[2])
			return;
		if (viewableEnd < bounds[0])
			return;

		if (mark->isSelected())
			fl_color(FL_GREEN);
		else
			fl_color(mark->getColor());

		bool drawText = true;
		if (viewableStart > bounds[0])
		{
			int diff = viewableStart - bounds[0];
			bounds[0] += diff;
			bounds[2] -= diff;
			drawText = false;
		}
		//fltk::Rectangle rec(bounds[0] - 2, bounds[1], bounds[2] + 4, bounds[3]);
		fl_rect(bounds[0] - 2, bounds[1], bounds[2] + 4, bounds[3]);

		
		if (mark->isShowName())
		{
			if (drawText)
			{
				int width, height;
				std::string markName = mark->getName();
				fl_measure(markName.c_str(), width, height);

				int startTextX = bounds[0];
				int startTextY = bounds[1];
				if (startTextX + width <= viewableEnd)
				{
					fl_color(FL_BLACK);
					fl_draw(markName.c_str(), startTextX, startTextY);
				}		
			}
		}

		if (cMark)
		{
			// connect to the attached correspondence mark
			// get the position of the other mark
			
			std::vector<CorrespondenceMark*>& attached = cMark->getAttachedMark();
			for (size_t i = 0; i < attached.size(); i++)
			{
				if (attached[i])
				{
					int abounds[4];
					attached[i]->getBounds(abounds[0], abounds[1], abounds[2], abounds[3]);
					fl_color(FL_BLUE);
					fl_line(bounds[0], bounds[1], abounds[0], abounds[1]);
				}
			}
		}
		else if (iMark)
		{
			if (trackNum == 0)
			{
				// connect to ease in mark
				nle::Track* secondTrack = model->getTrack(1);
				if (block)
				{
					nle::Block* block = secondTrack->getBlock(0);
					if (block)
					{
						nle::Mark* easeInMark = block->getMark(0);
						if (easeInMark)
						{
							int outBounds[4];
							iMark->getBounds(outBounds[0], outBounds[1], outBounds[2], outBounds[3]);
							int inBounds[4];
							easeInMark->getBounds(inBounds[0], inBounds[1], inBounds[2], inBounds[3]);
							fl_color(FL_BLUE);
							fl_line(outBounds[0], outBounds[1] + outBounds[3], inBounds[0], inBounds[1]);
							fl_line(outBounds[0] + outBounds[2], outBounds[1] + outBounds[3], inBounds[0] + inBounds[2], inBounds[1]);
						}
					}
				}
				
			}
		}
	}
}

void ParamAnimEditorWidget::changeBlockSelectionEvent(nle::Block* block)
{
	blockSelectionChanged = true;
}

void ParamAnimEditorWidget::changeTrackSelectionEvent(nle::Track* track)
{
	trackSelectionChanged = true;
	// select the motions associated with this track change
	bool isTrackSelected = track->isSelected();
	std::string trackName = track->getName();
	PABlendEditor* stateEditor = dynamic_cast<PABlendEditor*>(parentGroup);
	if (stateEditor)
	{
		int numItems = stateEditor->stateAnimationList->size();
		bool changed = false;
		for (int i = 0; i < numItems; i++)
		{
			const char* motionName = stateEditor->stateAnimationList->text(i + 1);
			if (trackName == motionName)
			{
				int isMotionSelected  = stateEditor->stateAnimationList->selected(i + 1);
				if (isTrackSelected && !isMotionSelected)
				{
					stateEditor->stateAnimationList->select(i + 1, 1);
					changed = true;
				}
				else if (!isTrackSelected && isMotionSelected)
				{
					stateEditor->stateAnimationList->select(i + 1, 0);
					changed = true;
				}
			}
		}
		if (changed)
		{
			stateEditor->selectStateAnimations(stateEditor->stateAnimationList, stateEditor);
		}
	}
}

void ParamAnimEditorWidget::changeMarkSelectionEvent(nle::Mark* mark)
{
	markSelectionChanged = true;
}

void ParamAnimEditorWidget::releaseBlockEvent(nle::Block* block)
{
}

void ParamAnimEditorWidget::releaseTrackEvent(nle::Track* track)
{
}

void ParamAnimEditorWidget::releaseMarkEvent(nle::Mark* mark)
{
	// a mark was moved should also result in a moved 
	// correspandence point associated with that point
	CorrespondenceMark* cMark = dynamic_cast<CorrespondenceMark*>(mark);
	nle::Block* block = mark->getBlock();
	nle::Track* track = block->getTrack();
	int trackIndex = model->getTrackIndex(track);
	int markIndex = block->getMarkIndex(cMark);
	// update that correspondance point on the state
	PABlendEditor* stateEditor = dynamic_cast<PABlendEditor*>(parentGroup);
	if (stateEditor)
	{
		std::string currentStateName = stateEditor->stateList->menu()[stateEditor->stateList->value()].label();
		SmartBody::SBAnimationBlend* currentState = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(currentStateName);
		if (currentState)
		{
			currentState->setCorrespondencePoints(trackIndex, markIndex, cMark->getStartTime());
			setup();
		}
	}
	else
	{
		PATransitionEditor* transEditor = dynamic_cast<PATransitionEditor*>(parentGroup);
		if (transEditor)
		{
		}
	}
}

void ParamAnimEditorWidget::setBlockSelectionChanged(bool val)
{
	blockSelectionChanged = val;
}

bool ParamAnimEditorWidget::getBlockSelectionChanged()
{
	return blockSelectionChanged;
}

void ParamAnimEditorWidget::setup()
{
	EditorWidget::setup();
	
	double maxEndTime = 0.0f;
	// adjust the viewable end time
	for (int t = 0; t < model->getNumTracks(); t++)
	{
		nle::Track* track = model->getTrack(t);
		for (int b = 0; b < track->getNumBlocks(); b++)
		{
			nle::Block* block = track->getBlock(b);
			double endTime = block->getEndTime();
			if (maxEndTime < endTime)
				maxEndTime = endTime;
		}
	}

	//	this->setViewableTimeEnd(maxEndTime + 1.0);
}

int ParamAnimEditorWidget::handle(int event)
{
	PABlendEditor* stateEditor = dynamic_cast<PABlendEditor*>(parentGroup);
	if (stateEditor)
	{
		PABlend* currentState = stateEditor->getCurrentState();
		const std::vector<std::string>& selectedMotions = stateEditor->getSelectedMotions();
		if (selectedMotions.size() == 1)
		{
			switch (event)
			{
			case FL_DRAG:
				nle::Mark* selectedMarker = NULL;
				int selectedMarkerId = -1;
				for (int t = 0; t < model->getNumTracks(); t++)
				{
					nle::Track* track = model->getTrack(t);
					for (int b = 0; b < track->getNumBlocks(); b++)
					{
						nle::Block* block = track->getBlock(b);
						for (int m = 0; m < block->getNumMarks(); m++)
						{
							nle::Mark* mark = block->getMark(m);
							if (mark->isSelected())
							{
								selectedMarker = mark;
								selectedMarkerId = m;
								break;
							}
						}
					}
				}

				if (selectedMarker)
				{
					int motionId = currentState->getMotionId(selectedMotions[0]);
					if (motionId >= 0)
					{
						int prevMarker = selectedMarkerId - 1;
						if (prevMarker >= 0)
						{
							double prevMarkerTime = currentState->keys[motionId][prevMarker];
							if (selectedMarker->getStartTime() < prevMarkerTime)
							{
								selectedMarker->setStartTime(prevMarkerTime + 0.01f);
								selectedMarker->setEndTime(prevMarkerTime + 0.01f);
								redraw();
							}
						}
						int nextMarker = selectedMarkerId + 1;
						if (nextMarker <= (currentState->getNumKeys() - 1))
						{
							double nextMarkerTime = currentState->keys[motionId][nextMarker];
							if (selectedMarker->getStartTime() > nextMarkerTime)
							{
								selectedMarker->setStartTime(nextMarkerTime - 0.01f);
								selectedMarker->setEndTime(nextMarkerTime - 0.01f);
								redraw();
							}
						}
					}

				}
				break;
			}
		}
	}
	return nle::EditorWidget::handle(event);
}

void ParamAnimEditorWidget::setTrackSelectionChanged(bool val)
{
	trackSelectionChanged = val;
}

bool ParamAnimEditorWidget::getTrackSelectionChanged()
{
	return trackSelectionChanged;
}

void ParamAnimEditorWidget::setMarkSelectionChanged(bool val)
{
	markSelectionChanged = val;
}

bool ParamAnimEditorWidget::getMarkSelectionChanged()
{
	return markSelectionChanged;
}

void ParamAnimEditorWidget::draw()
{
	nle::EditorWidget::draw();

	// draw scrub line if applicable
	if (showScrubLine)
	{
		if (scrubTimes.size() != model->getNumTracks())
			return;
		fl_color(FL_BLACK);
		// draw the scrub line
		for (int t = 0; t < model->getNumTracks(); t++)
		{
			int xPos = convertTimeToPosition(scrubTimes[t]);
			nle::Track* track = model->getTrack(t);
			int x1, y1, w1, h1;
			track->getBounds(x1, y1, w1, h1);
			fl_line(xPos, y1, xPos, y1 + h1);	
		}
	}
	if (showAlignLine)
	{
		if (alignTimes.size() != model->getNumTracks())
			return;
		fl_color(FL_BLACK);
		int triangleWidth = 1;
		// draw the align triangles
		for (int t = 0; t < model->getNumTracks(); t++)
		{
			int xPos = convertTimeToPosition(alignTimes[t]);
			if (alignTimes[t] == 0)
				continue;

			nle::Track* track = model->getTrack(t);
			int x1, y1, w1, h1;
			track->getBounds(x1, y1, w1, h1);
			/*
			fl_line(xPos, y1 + h1 / 2, xPos - 1, y1);
			fl_line(xPos, y1 + h1 / 2, xPos + 1, y1);
			fl_line(xPos + 1, y1, xPos - 1, y1);
			fl_line(xPos, y1 + h1 / 2, xPos - 1, y1 + h1);
			fl_line(xPos, y1 + h1 / 2, xPos + 1, y1 + h1);
			fl_line(xPos + 1, y1 + h1, xPos - 1, y1 + h1);
			*/

			fl_polygon(xPos, y1 + h1 / 2, xPos - triangleWidth, y1, xPos + triangleWidth, y1);
			fl_polygon(xPos, y1 + h1 / 2, xPos - triangleWidth, y1 + h1, xPos + triangleWidth, y1 + h1);
			//fl_line(xPos, y1, xPos, y1 + h1);	
		}
	}
}

void ParamAnimEditorWidget::setLocalTimes(std::vector<double>& t)
{
	scrubTimes = t;
}

std::vector<double>& ParamAnimEditorWidget::getLocalTimes()
{
	return scrubTimes;
}

void ParamAnimEditorWidget::setShowScrubLine(bool val)
{
	showScrubLine = val;
}

void ParamAnimEditorWidget::setAlignTimes(std::vector<double>& t)
{
	alignTimes = t;
}

std::vector<double>& ParamAnimEditorWidget::getAlignTimes()
{
	return alignTimes;
}

void ParamAnimEditorWidget::setShowAlignLine(bool val)
{
	showAlignLine = val;
}

nle::Mark* ParamAnimEditorWidget::getSelectedCorrespondancePointIndex(int& selectedIndex)
{
	selectedIndex = -1;
	for (int t = 0; t < model->getNumTracks(); t++)
	{
		nle::Track* track = model->getTrack(t);
		for (int b = 0; b < track->getNumBlocks(); b++)
		{
			nle::Block* block = track->getBlock(b);
			for (int m = 0; m < block->getNumMarks(); m++)
			{
				nle::Mark* mark = block->getMark(m);
				if (mark->isSelected())
				{
					selectedIndex = m;
					return mark;
				}
			}
		}
	}

	return NULL;
}

