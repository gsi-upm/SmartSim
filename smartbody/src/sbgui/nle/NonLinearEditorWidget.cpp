#include "NonLinearEditorWidget.h"
#include <iostream>
#include <FL/Enumerations.H>
#include <FL/Fl.H>

namespace nle
{

EditorWidget::EditorWidget(int x, int y, int w, int h, char* name) :
									Fl_Widget(x, y, w, h, name)
{
	model = NULL;
	//xOffset = x;
	//yOffset = y;
	setup();
	setViewableTimeStart(0);
	setViewableTimeEnd(3);
	setTimeWindowSelected(false);
	selectState = STATE_NORMAL;
	cameraState = CAMERASTATE_NORMAL;
	setBlockCandidate(NULL, true);
	blockOpLocked = false;

	padding = 20;
	trackHeight = 30;
    activationSize = 8;
	labelWidth = 100;
	timeWindowHeight = 10;
}

EditorWidget::~EditorWidget()
{
}

void EditorWidget::setup()
{
	left = padding + x();
	right = w() - padding + x();
	width = right - left;
	top = padding + y();

	trackStart = left + labelWidth;
	

	// set up the tracks
	if (!model)
		return;

	if (model->isModelChanged())
	{
		model->update();
	}

	int currentTrackLocation = top + timeWindowHeight + 10;
	for (int t = 0; t < model->getNumTracks(); t++)
	{
		nle::Track* track = model->getTrack(t);

		int trackTop = currentTrackLocation;
		int trackLeft = trackStart;
		int trackEnd = right;
		int trackWidth = trackEnd - trackStart;

		for (int b = 0; b < track->getNumBlocks(); b++)
		{
			nle::Block* block = track->getBlock(b);
			// set up the blocks

			double startTime = block->getStartTime();    
			//if (startTime > this->getViewableTimeEnd())
			//	continue;
			if (startTime < 0)
				startTime = 0;

			double endTime = block->getEndTime();
			//if (endTime < this->getViewableTimeStart())
			//	continue;
			if (endTime > this->getViewableTimeEnd())
				endTime = this->getViewableTimeEnd();
			if (endTime < 0)
				endTime = 0;

			int startPos = convertTimeToViewablePosition(startTime);
			int endPos = convertTimeToViewablePosition(endTime);

			std::vector<int> levelHeight;
			levelHeight.push_back(0);

			for (int m = 0; m < block->getNumMarks(); m++)
			{
				nle::Mark* mark = block->getMark(m);

				double markStartTime = mark->getStartTime();    
				//if (markStartTime > this->getViewableTimeEnd())
				//	continue;
				if (markStartTime < 0)
					markStartTime = 0;

				double markEndTime = mark->getEndTime();
				//if (markEndTime < this->getViewableTimeStart())
				//	continue;
				if (markEndTime > this->getViewableTimeEnd())
					markEndTime = this->getViewableTimeEnd();
				if (markEndTime < 0)
					markEndTime = 0;

				int markStartPos = convertTimeToViewablePosition(markStartTime);
				int markEndPos = convertTimeToViewablePosition(markEndTime);

				// set the mark's bounds
				int width = markEndPos - markStartPos;
				if (width == 0)
					width = 1;
				int halfHeight = int(trackHeight / 2);
				mark->setBounds(markStartPos, trackTop + halfHeight, width, halfHeight);
				levelHeight[0] = trackTop + halfHeight;
			}
			// check for mark overlap
			bool hasOverlap = true;
			while (hasOverlap) 
			{
				hasOverlap = false;
				for (int m = 0; m < block->getNumMarks(); m++)
				{
					nle::Mark* mark = block->getMark(m);
					int bounds[4];
					mark->getBounds(bounds[0], bounds[1], bounds[2], bounds[3]);
					for (int n = m + 1; n < block->getNumMarks(); n++)
					{
						nle::Mark* mark2 = block->getMark(n);
						// make sure these aren't the same marks
						//if (mark->getName() == mark2->getName())
						//	continue;
						int bounds2[4];
						mark2->getBounds(bounds2[0], bounds2[1], bounds2[2], bounds2[3]);
						if (bounds[1] == bounds2[1] && bounds[0] == bounds2[0]) // if these are at the same level and the same location
						{
							hasOverlap = true;
							// on which level is there a conflict?
							int conflictLevel = 0;
							for (unsigned int h = 0; h < levelHeight.size(); h++)
							{
								if (bounds[1] == levelHeight[h])
								{
									conflictLevel = h;
									break;
								}
							}
							if (conflictLevel + 1 == levelHeight.size())
							{
								currentTrackLocation += trackHeight;
								levelHeight.push_back(bounds[1] + trackHeight);
							}
							bounds2[1] += trackHeight;
							mark2->setBounds(bounds2[0], bounds2[1], bounds2[2], bounds2[3]);
							//std::cout << "Conflict between " << block->getName() << " [" << mark->getName() << "]" << " [" << mark2->getName() << "]" << std::endl;
							
						}
					}
				}
			}

			block->setBounds(startPos, trackTop, endPos - startPos, trackHeight * levelHeight.size());
		
		}

		currentTrackLocation += trackHeight;
		track->setBounds(trackStart, trackTop, trackWidth, currentTrackLocation - trackTop);
	}

	trackBottom = currentTrackLocation + trackHeight;
	// make sure that the widget can accomodate all the tracks
	int curHeight = this->h();
	//this->h(currentTrackLocation + trackHeight);

	// time window
	// viewable time window bar will be proportional to the model time
	double modelStart = 0;
	double modelEnd = model->getEndTime();

	int timeWindowLocationStart = trackStart;
	int timeWindowLocationEnd = right;

	// make sure time window is at least of a minimum size
	int timeWindowSize = timeWindowLocationEnd - timeWindowLocationStart;
	if (timeWindowSize < 5)
		timeWindowLocationEnd = timeWindowLocationStart + 5;

	// setup the time window
	int timeWindowTop = 0;
	int timeWindowBottom = timeWindowHeight;
	int timeWindowLeft = timeWindowLocationStart;
	int timeWindowRight = timeWindowLocationEnd;

	int timeWindowWidth = timeWindowRight - timeWindowLeft;

	// draw the time bar
	//fltk::Rectangle timerec(timeWindowLocationStart, timeWindowTop, 
	//	                    timeWindowSize, timeWindowBottom - timeWindowTop);


	// set the time window bounds
	this->setTimeWindowBounds(timeWindowLeft, timeWindowTop, timeWindowLocationEnd - timeWindowLocationStart, timeWindowHeight);

	// set the time slider bounds
	int sliderLeft = 0;
	int sliderRight = 0;

	double sliderStartTime = this->getViewableTimeStart();
	double sliderEndTime = this->getViewableTimeEnd();

	double modelTimeSpan = modelEnd - modelStart;
	double ratioStart = sliderStartTime / modelTimeSpan;
	double ratioEnd = sliderEndTime / modelTimeSpan;
	if (ratioEnd > 1.0)
		ratioEnd = 1.0;
	sliderLeft = int(ratioStart * double(timeWindowWidth)) + timeWindowLeft;
	sliderRight = int(ratioEnd* double(timeWindowWidth)) + timeWindowLeft;
	this->setTimeSliderBounds(sliderLeft, timeWindowTop, sliderRight - sliderLeft, timeWindowHeight);
	damage(FL_DAMAGE_ALL);
}

void EditorWidget::resize(int x, int y, int w, int h)
{	
	Fl_Widget::resize(x, y, w, h);
	
	setup();
}

void EditorWidget::setModel(nle::NonLinearEditorModel* m)
{
	this->model = m;
	if (m)
		notifyModelChanged(m);
}

nle::NonLinearEditorModel* EditorWidget::getModel()
{
	return model;
}

void EditorWidget::initialize()
{
}

void EditorWidget::setWindowStart(double time)
{
	windowStartTime = time;
}

void EditorWidget::setWindowEnd(double time)
{
	windowEndTime = time;
}

void EditorWidget::setViewableTimeStart(double time)
{
	viewableTimeStart = time;
}

double EditorWidget::getViewableTimeStart()
{
	return viewableTimeStart;
}

void EditorWidget::setViewableTimeEnd(double time)
{
	viewableTimeEnd = time;
}

double EditorWidget::getViewableTimeEnd()
{
	return viewableTimeEnd;
}


void EditorWidget::draw()
{
	if (!model)
		return;

	fl_font(FL_COURIER, 8);
	//setup();
	
	// draw the work area
	drawBackground();

	// draw the window sliders
//	drawTimeWindow();

	// draw the grid ticks
	drawTicks();

	// draw each track
	int numTracks = model->getNumTracks();
	for (int t = 0; t < numTracks; t++)
	{
		nle::Track* track = model->getTrack(t);
		drawTrack(track, t);
	}

	// draw each block
	for (int t = 0; t < numTracks; t++)
	{
		nle::Track* track = model->getTrack(t);
		// draw each block
		for (int b = 0; b < track->getNumBlocks(); b++)
		{
			nle::Block* block = track->getBlock(b);
			drawBlock(block, t, b);
		}
	}

	// draw each mark 
	for (int t = 0; t < numTracks; t++)
	{
		nle::Track* track = model->getTrack(t);
		// draw each block
		for (int b = 0; b < track->getNumBlocks(); b++)
		{
			nle::Block* block = track->getBlock(b);
			// draw the marks on the track
			for (int m = 0; m < block->getNumMarks(); m++)
			{
				nle::Mark* mark = block->getMark(m);
				drawMark(block, mark, t, b, m);
			}
		}
	}
	
	// show the mouse hit targets
	if (false)
	{
		fl_color(FL_GREEN);

		int loc[4];
		this->getTimeWindowBounds(loc[0], loc[1], loc[2], loc[3]);
		fl_rectf(loc[0], loc[1], loc[2], loc[3]);

		for (int t = 0; t < numTracks; t++)
		{
			nle::Track* track = model->getTrack(t);
			// draw each block
			for (int b = 0; b < track->getNumBlocks(); b++)
			{
				nle::Block* block = track->getBlock(b);
				block->getBounds(loc[0], loc[1], loc[2], loc[3]);
				fl_rectf(loc[0], loc[1], loc[2], loc[3]);
			}
		}
	}

	// draw the current time
	int timePos = this->convertTimeToPosition(this->getModel()->getTime());
	fl_color(FL_RED);
	fl_line(timePos, top, timePos, trackBottom);

	// draw the time if dragging
	bool leftOrRight;
	nle::Block* blockBeingDragged = getBlockCandidate(leftOrRight);
	if (blockBeingDragged)
	{
		double t = 0.0;
		if (leftOrRight)
			t = blockBeingDragged->getStartTime();
		else
			t = blockBeingDragged->getEndTime();
		char buff[256];
		sprintf(buff, "%8.4f", t);
		fl_color(FL_BLACK);
		int bounds[4];
		blockBeingDragged->getTrack()->getBounds(bounds[0], bounds[1], bounds[2], bounds[3]);
		//bounds[0] += xOffset;
		//bounds[1] += yOffset;
		fl_draw(buff, this->convertTimeToPosition(t), bounds[1] +  5 * bounds[3] / 6);
		
	}
}

void EditorWidget::drawBackground()
{
	fl_color(FL_DARK3);

	fl_rectf(left, top, width, trackBottom - top);
}

void EditorWidget::drawTicks()
{
	if (!this->getModel())
		return;

	fl_color(FL_DARK3);

	// draw the timing every second
	char buff[128];
	int counter = 0;

	double cur =  this->getViewableTimeStart();
	double tickAmount = .25;

	// make sure that there aren't too many tick marks
	bool tickOk = false;
	while (!tickOk)
	{
		int numTicks = int((this->getViewableTimeEnd() - this->getViewableTimeStart()) / tickAmount);
		if (numTicks > 100)
			tickAmount *= 4.0;
		else
			tickOk = true;
	}
	
	while (cur <= this->getViewableTimeEnd())
	{
		int timePos = convertTimeToViewablePosition(cur);
		fl_line(timePos, top, timePos, trackBottom);
		if (counter % 4 == 0)
		{
			sprintf(buff, "%6.2f", cur);
			int textW, textH;
			fl_measure(buff, textW, textH);
			fl_draw(buff, timePos - textW / 2, trackBottom + trackHeight);
			//fl_draw(buff, timePos - textW / 2, top + this->height - textH);
		}
		counter++;
		cur += tickAmount;	
	}
}

void EditorWidget::drawTimeWindow()
{
	int bounds[4];

	this->getTimeSliderBounds(bounds[0], bounds[1], bounds[2], bounds[3]);
		if (bounds[0] > 10000 || 
		bounds[1] > 10000 ||
		bounds[2] > 10000 ||
		bounds[3] > 10000)
		return;

	fl_rectf(bounds[0], bounds[1], bounds[2], bounds[3]);
	fl_color(FL_BLACK);
	fl_rect(bounds[0], bounds[1], bounds[2], bounds[3]);

	// draw the time box
	if (isTimeWindowSelected())
		fl_color(FL_GRAY0);
	else
		fl_color(FL_WHITE);

	fl_rectf(bounds[0], bounds[1], bounds[2], bounds[3]);
	fl_color(FL_BLACK);
	fl_rect(bounds[0], bounds[1], bounds[2], bounds[3]);

	// draw the time label
/*	char buff[128];
	sprintf(buff, "%6.2f", this->getViewableTimeStart());
	fl_draw(buff, float(bounds[0]), float((bounds[1] + (bounds[1] + bounds[3])) / 2 + 5) - 10);
	sprintf(buff, "%6.2f", this->getViewableTimeEnd());
	fl_draw(buff, float(bounds[0] + bounds[2]), float((bounds[1] + (bounds[1] + bounds[3])) / 2 + 5) - 10);
	*/
}

void EditorWidget::drawTrack(nle::Track* track, int trackNum)
{
	if (!track)
		return;

	int bounds[4];
	track->getBounds(bounds[0], bounds[1], bounds[2], bounds[3]);
// 	bounds[0] += xOffset;
// 	bounds[1] += yOffset;
 
    // show the active/inactive box
    if (track->isActive())
    {
        fl_color(FL_BLACK);
        fl_rectf(left, bounds[1] + bounds[3] / 2 - activationSize, activationSize, activationSize);
    }
    else
    {
        fl_color(FL_BLACK);
        fl_rect(left, bounds[1] + bounds[3] / 2 - activationSize, activationSize, activationSize);
    }

	// write the track names
	if (track->isSelected())
	{
		fl_color(FL_YELLOW);
	}
	else
	{
		fl_color(FL_BLACK);
	}
	fl_draw(track->getName().c_str(), left + 15, (bounds[1] + (bounds[1] + bounds[3])) / 2);

	if (track->isSelected())
	{
		//fl_color(FL_DARK2);
		fl_color(FL_YELLOW);
	}
	else
	{
		fl_color(FL_LIGHT1);
	}
	fl_rectf(bounds[0], bounds[1], bounds[2], bounds[3]);

	fl_color(FL_DARK3);
	fl_rect(bounds[0], bounds[1], bounds[2], bounds[3]);
}

void EditorWidget::drawBlock(nle::Block* block, int trackNum, int blockNum)
{
	int viewableStart = this->convertTimeToPosition(getViewableTimeStart());
	int viewableEnd = this->convertTimeToPosition(getViewableTimeEnd());
	
	nle::Track* track = block->getTrack();
	int trackBounds[4];
	track->getBounds(trackBounds[0], trackBounds[1], trackBounds[2], trackBounds[3]);
	int trackTop = trackBounds[1];
	
	int bounds[4];
	block->getBounds(bounds[0], bounds[1], bounds[2], bounds[3]);
	if (viewableStart > bounds[0] + bounds[2])
		return;
	if (viewableEnd < bounds[0])
		return;


	if (block->isSelected())
		fl_color(FL_YELLOW);
	else
		fl_color(block->getColor());

	bool drawText = true;
	if (viewableStart > bounds[0])
	{
		int diff = viewableStart - bounds[0];
		bounds[0] += diff;
		bounds[2] -= diff;
		drawText = false;
	}
// 	bounds[0] += xOffset;
// 	bounds[1] += yOffset;
	fl_rectf(bounds[0], bounds[1], bounds[2], bounds[3]);

	// draw an outline around the block
	fl_color(FL_BLACK);
	fl_rect(bounds[0], bounds[1], bounds[2], bounds[3]);

	
	if (block->isShowName())
	{
		if (drawText)
		{
			// draw the name
			int width, height;
			std::string blockName = block->getName();
			fl_measure(blockName.c_str(), width, height);
			// make sure that the width of the block will fit in the block area
			if (width > bounds[2])
			{
				// show only the first few letters...
				if (blockName.size() > 2)
				{
					blockName = blockName.substr(0, 2);
					blockName.append("..");
				}
				fl_measure(blockName.c_str(), width, height);
				if (width > bounds[2])
					width = bounds[2];
			}

			int startTextX = (bounds[0] + (bounds[0] + bounds[2])) / 2 - width / 2 ;//+ xOffset;
			int startTextY = trackTop + int(3.0 * double(trackHeight) / 4.0);// + yOffset;
			if (startTextX + width <= viewableEnd)
				fl_draw(blockName.c_str(), startTextX, startTextY);
		}
	}		
}

void EditorWidget::drawMark(nle::Block* block, nle::Mark* mark, int trackNum, int blockNum, int markNum)
{
	int viewableStart = this->convertTimeToPosition(getViewableTimeStart());
	int viewableEnd = this->convertTimeToPosition(getViewableTimeEnd());	
	
	int trackTop = padding + timeWindowHeight + trackNum * trackHeight;
	
	int bounds[4];
	mark->getBounds(bounds[0], bounds[1], bounds[2], bounds[3]);
// 	bounds[0] += xOffset;
// 	bounds[1] += yOffset;

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
	fl_rect(bounds[0], bounds[1], bounds[2], bounds[3]);

	
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
}

int EditorWidget::handle(int event)
{
	nle::NonLinearEditorModel* model = this->getModel();

	if (!model)
	{
		return Fl_Widget::handle(event);
	}

	int mousex = Fl::event_x();
	int mousey = Fl::event_y();

	bool mouseHit = false;
	bool found = false;
	bool foundBorder = false;
	bool leftOrRight = false;
	nle::Block* candidateBlock = NULL;             
    nle::Block* selectedBlock = NULL;   
	nle::Mark* selectedMark = NULL;

	switch (event)
	{
		case FL_SHORTCUT:
		case FL_KEYBOARD:
			{
				switch(Fl::event_key())
				{
					case 'f': 
						// bring any selected objects in focus
						// if nothing is selected, bring everything into focus
						std::vector<nle::Block*> selectedBlocks;
						double minTime = 9999999;
						double maxTime = -9999999;
						double selectedMinTime = 0;
						double selectedMaxTime = 1;
						for (int t = 0; t < model->getNumTracks(); t++)
						{
							nle::Track* track = model->getTrack(t);
							for (int b = 0; b < track->getNumBlocks(); b++)
							{
								nle::Block* block = track->getBlock(b);
								if (block->isSelected())
								{
									selectedBlocks.push_back(block);
									if (block->getStartTime() < selectedMinTime)
										selectedMinTime = block->getStartTime();
									if (block->getEndTime() > selectedMaxTime)
										selectedMaxTime = block->getEndTime();
								}
								if (block->getStartTime() < minTime)
									minTime = block->getStartTime();
								if (block->getEndTime() > maxTime)
									maxTime = block->getEndTime();

							}
						}
						if (selectedBlocks.size() > 0)
						{
							if (selectedMinTime < 0)
								selectedMinTime = 0;
							if (selectedMaxTime > model->getEndTime())
								selectedMaxTime = model->getEndTime();
							if (selectedMaxTime - selectedMinTime < 1.0)
								selectedMaxTime = selectedMinTime + 1.0;
							this->setViewableTimeStart(selectedMinTime);
							this->setViewableTimeEnd(selectedMaxTime);
						}
						else
						{
							if (minTime < 0)
								minTime = 0;
							if (maxTime > model->getEndTime())
								maxTime = model->getEndTime();
							if (maxTime - minTime < 1.0)
								maxTime = minTime + 1.0;
							this->setViewableTimeStart(minTime);
							this->setViewableTimeEnd(maxTime);
						}
						redraw();
						break;
				}
			}
			break;
		case FL_DRAG:
			{
				bool altKeyPressed = (Fl::event_key(FL_Left) || Fl::event_key(FL_Right));
				if (altKeyPressed)
				{
					if (cameraState == CAMERASTATE_ZOOM)
					{
						int diff = clickPositionY - mousey;
						double expandAmount = double(diff) * .01;
						double timeSpan = clickViewableTimeEnd - clickViewableTimeStart;
						double newTimeSpan = timeSpan + timeSpan * expandAmount;
						if (newTimeSpan < 1.0) // make sure there is at least one second between min/max times
							newTimeSpan = 1.0;
						double timeSpanDiff = newTimeSpan - timeSpan;
						double newStartViewableTime = clickViewableTimeStart - timeSpanDiff / 2.0;
						double newEndViewableTime =  clickViewableTimeEnd + timeSpanDiff / 2.0;
						if (newStartViewableTime < 0)
						{
							newEndViewableTime += -newStartViewableTime;
							newStartViewableTime = 0;
						}
						if (newEndViewableTime > model->getEndTime())
						{
							newStartViewableTime -= newEndViewableTime -  model->getEndTime();
							newEndViewableTime = model->getEndTime();
						}
						this->setViewableTimeStart(newStartViewableTime);
						this->setViewableTimeEnd(newEndViewableTime);
						redraw();
						break;
					}
					else if (cameraState == CAMERASTATE_PAN)
					{
						int diff = mousex - clickPositionX;
						double panAmount = double(diff) * .01;
						double newStartViewableTime = clickViewableTimeStart + panAmount;
						double newEndViewableTime =  clickViewableTimeEnd + panAmount;
						double span = newEndViewableTime - newStartViewableTime;
						if (newStartViewableTime < 0)
						{	
							newEndViewableTime += -newStartViewableTime;
							newStartViewableTime = 0;				
						}
						if (newEndViewableTime > model->getEndTime())
						{
							newEndViewableTime = model->getEndTime();
							newStartViewableTime = newEndViewableTime - span;
						}

						this->setViewableTimeStart(newStartViewableTime);
						this->setViewableTimeEnd(newEndViewableTime);
						redraw();
						break;
					}
				}
			}
			candidateBlock = this->getBlockCandidate(leftOrRight);
			if (candidateBlock && !blockOpLocked)
			{
                bool shiftKeyPressed = (Fl::event_key(FL_Shift_L) || Fl::event_key(FL_Shift_R));

				double time = this->convertViewablePositionToTime(mousex);
				if (leftOrRight)
				{
					// find the block whose end time matches this start time                    
                    nle::Block* blockToLeft = candidateBlock->getTrack()->getPrevBlock(candidateBlock);
                    
                    if (blockToLeft)
                    {
                        // if the SHIFT key is selected, move the start and end times in concert
                        if (shiftKeyPressed)
                        {
                            blockToLeft->setEndTime(time);
                        }
                        else
                        {
                            // limit to end time of other block
                            if (blockToLeft->getEndTime() > time)
                                time = blockToLeft->getEndTime();
                        }
                    }
                    candidateBlock->setStartTime(time);
				}
				else
				{
                    // if the SHIFT key is selected,
					// find the block whose end time matches this start time
                    nle::Block* blockToRight = candidateBlock->getTrack()->getNextBlock(candidateBlock);
                    
                    if (blockToRight)
                    {
                        // if the SHIFT key is selected, move the start and end times in concert
                        if (shiftKeyPressed)
                        {
                            blockToRight->setStartTime(time);
                        }
                        else
                        {
                            // limit to end time of other block
                            if (blockToRight->getStartTime() < time)
                                time = blockToRight->getStartTime();
                        }
                    }                  
					candidateBlock->setEndTime(time);	
				}
                model->setModelChanged(true);
				redraw();
				return 1;
			}

			if (selectState == STATE_TIMEWINDOWSTART || 
				selectState == STATE_TIMEWINDOWEND ||
				selectState == STATE_TIMEWINDOWMIDDLE)
			{
				double startViewableTime = this->getViewableTimeStart();
				double endViewableTime = this->getViewableTimeEnd();

				int posDiff = mousex - clickPositionX;
				double endTime = this->getModel()->getEndTime();
				int windowBounds[4];
				this->getTimeWindowBounds(windowBounds[0], windowBounds[1], windowBounds[2], windowBounds[3]);
				int width = windowBounds[2];
				double movementRatio = double(posDiff) / double(width);
				double timeMovement = movementRatio * endTime;
				clickPositionX = mousex;

				if (selectState == STATE_TIMEWINDOWSTART)
				{
					double newTime = this->getViewableTimeStart() + timeMovement;
					if (newTime < 0)
						newTime = 0;
					if (newTime > this->getViewableTimeEnd() - 1.0)
						newTime = this->getViewableTimeEnd() - 1.0;
					this->setViewableTimeStart(newTime); 
					redraw();
				}
				if (selectState == STATE_TIMEWINDOWEND)
				{
					double newTime = this->getViewableTimeEnd() + timeMovement;
					if (newTime > this->getModel()->getEndTime())
						newTime = this->getModel()->getEndTime();
					if (newTime < this->getViewableTimeStart() + 1.0)
						newTime = this->getViewableTimeStart() + 1.0;
					this->setViewableTimeEnd(newTime); 
					redraw();
				}
				if (selectState == STATE_TIMEWINDOWMIDDLE)
				{
					double newStartTime = this->getViewableTimeStart() + timeMovement;
					double newEndTime = this->getViewableTimeEnd() + timeMovement;
					if (newStartTime < 0)
					{
						newStartTime = 0;
						newEndTime = (this->getViewableTimeEnd() - this->getViewableTimeStart());
					}
					if (newEndTime > this->getModel()->getEndTime())
					{
						newEndTime = this->getModel()->getEndTime();
						newStartTime = newEndTime - (this->getViewableTimeEnd() - this->getViewableTimeStart());
						if (newStartTime < 0.0)
							newStartTime = 0.0;
					}
					
					this->setViewableTimeStart(newStartTime);
					this->setViewableTimeEnd(newEndTime); 
					redraw();
				}
			}

            // check to see if the user is moving an entire mark 
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
							selectedMark = mark;
							break;
						}
					}
					if (selectedMark)
						break;
                }
				if (selectedMark)
					break;
            }

			if (selectedMark == NULL)
			{
				// check to see if the user is moving an entire block 
				for (int t = 0; t < model->getNumTracks(); t++)
				{
					nle::Track* track = model->getTrack(t);
					for (int b = 0; b < track->getNumBlocks(); b++)
					{
						nle::Block* block = track->getBlock(b);
						if (block->isSelected())
						{
							selectedBlock = block;
							break;
						}
					}
					if (selectedBlock)
						break;
				}
			}
			
			if (selectedMark) // mark is selected 
			{
                // determine where to move the block
                double origTime = this->convertViewablePositionToTime(clickPositionX);
                double newTime = this->convertViewablePositionToTime(mousex);
                double timeDiff = newTime - origTime; // adjust block start/end by this amount
                                
				if (true)
				{
					double startTime = selectedMark->getStartTime();
                    double endTime = selectedMark->getEndTime();
					if (selectedMark->isSelectedBothTime())
					{
						startTime = selectedMark->getStartTime() + timeDiff;
						endTime = selectedMark->getEndTime() + timeDiff;
					}
					else if (selectedMark->isSelectedStartTime())
					{
						startTime = selectedMark->getStartTime() + timeDiff;
					}
					else if (selectedMark->isSelectedEndTime())
					{
						endTime = selectedMark->getEndTime() + timeDiff;
					}
                    
                    // make sure that the mark doesn't go beyond the beginning	
					if (selectedMark->isBoundToBlock())
					{
						if (startTime >= selectedMark->getBlock()->getStartTime() && 
							endTime <= selectedMark->getBlock()->getEndTime())
						{
							if (selectedMark->isSelectedBothTime())
							{
								selectedMark->setStartTime(startTime);
								selectedMark->setEndTime(endTime);
							}
							else if (selectedMark->isSelectedStartTime())
							{
								selectedMark->setStartTime(startTime);
							}
							else if (selectedMark->isSelectedEndTime())
							{
								selectedMark->setEndTime(endTime);
							}

							char buff[256];
							if (selectedMark->getStartTime() != selectedMark->getEndTime())
							{
								sprintf(buff, "%6.2f %6.2f", selectedMark->getStartTime(), selectedMark->getEndTime());
								selectedMark->setName(buff);
							}
							else
							{
								sprintf(buff, "%6.2f", selectedMark->getStartTime());
								selectedMark->setName(buff);
							}							
						}       
						if (startTime < selectedMark->getBlock()->getStartTime())
						{
							selectedMark->setStartTime(selectedMark->getBlock()->getStartTime());
							selectedMark->setEndTime(selectedMark->getBlock()->getStartTime());
						}
						if (startTime > selectedMark->getBlock()->getEndTime())
						{
							selectedMark->setStartTime(selectedMark->getBlock()->getEndTime());
							selectedMark->setEndTime(selectedMark->getBlock()->getEndTime());
						}
					}
					else
					{
						if (selectedMark->isSelectedBothTime())
						{
							selectedMark->setStartTime(startTime);
							selectedMark->setEndTime(endTime);
						}
						else if (selectedMark->isSelectedStartTime())
						{
							selectedMark->setStartTime(startTime);
						}
						else if (selectedMark->isSelectedEndTime())
						{
							selectedMark->setEndTime(endTime);
						}
						
						char buff[256];
						sprintf(buff, "%6.2f", selectedMark->getStartTime());
						selectedMark->setName(buff);
					}

					// make sure the beginning doesn't go past the ending or vice versa
					if (selectedMark->isSelectedStartTime())
					{
						if (selectedMark->getStartTime() > selectedMark->getEndTime())
							selectedMark->setStartTime(selectedMark->getEndTime());

					}
					else if (selectedMark->isSelectedEndTime())
					{
						if (selectedMark->getEndTime() < selectedMark->getStartTime())
							selectedMark->setEndTime(selectedMark->getStartTime());

					}
					
                    // reset the mouse position
                    clickPositionX = mousex;
                    model->setModelChanged(true);
                    redraw();
                }				
			}


            if (selectedBlock && !blockOpLocked) // block is selected and user clicked on the middle of the block
            {
                // determine where to move the block
                double origTime = this->convertViewablePositionToTime(clickPositionX);
                double newTime = this->convertViewablePositionToTime(mousex);
                double timeDiff = newTime - origTime; // adjust block start/end by this amount
                                
                bool okToMove = true;
                if (timeDiff > 0) // if moving to the right, move block to the right until another block has been hit

                {
                    nle::Block* nextBlock = selectedBlock->getTrack()->getNextBlock(selectedBlock);
                    if (nextBlock)
                    {
                        if (nextBlock->getStartTime() < selectedBlock->getEndTime() + timeDiff)
                        {
                            timeDiff = nextBlock->getStartTime() - selectedBlock->getEndTime();
                            if (timeDiff < 0)
                                okToMove = false;
                        }
                    }
                }
                else // if moving to the left, move block to the left until another block has been hit
                {
                    nle::Block* prevBlock = selectedBlock->getTrack()->getPrevBlock(selectedBlock);
                    if (prevBlock)
                    {
                        if (prevBlock->getEndTime() > selectedBlock->getStartTime() + timeDiff)
                        {
                            timeDiff = prevBlock->getEndTime() -  selectedBlock->getStartTime();
                            if (timeDiff > 0)
                                okToMove = false;
                        }
                    }
                }
                if (okToMove)
                {
                    // make sure that the block doesn't go beyond the beginning
                    double startTime = selectedBlock->getStartTime() + timeDiff;
                    double endTime = selectedBlock->getEndTime() + timeDiff;
                    if (startTime >= 0)
                    {
                        selectedBlock->setStartTime(startTime);
                        selectedBlock->setEndTime(endTime);
                    }                        
                        
                    // reset the mouse position
                    clickPositionX = mousex;
                    model->setModelChanged(true);
                    redraw();
                }
                
            }
			break;

		case FL_PUSH:		
            
            // remember this mouse position
            clickPositionX = mousex;
			clickPositionY = mousey;

			{
				// if the alt key is pressed, then zoom then enter zoom mode
				bool alt = (Fl::event_key(FL_Alt_L) || Fl::event_key(FL_Alt_R));
				if (alt)
				{
					int button =  Fl::event_button();
					// right mouse is zoom
					if (button == 3)
					{
						cameraState = CAMERASTATE_ZOOM;
						clickViewableTimeStart = this->getViewableTimeStart();
						clickViewableTimeEnd = this->getViewableTimeEnd();
						return 1;
					}
					// middle mouse is pan
					else if (button == 2)
					{
						cameraState = CAMERASTATE_PAN;
						clickViewableTimeStart = this->getViewableTimeStart();
						clickViewableTimeEnd = this->getViewableTimeEnd();
						return 1;
					}
					// left mouse is ???
					else if (button == 1)
					{
					}

				}
			}

			// first, check for existence of block borders
			if (this->getBlockCandidate(leftOrRight))
			{
				return 1;
			}

			// check to see if the time window was hit		
			{				
				int bounds[4];
				this->getTimeSliderBounds(bounds[0], bounds[1], bounds[2], bounds[3]);
				int minX = bounds[0];
				int maxX = bounds[0] + bounds[2];
				int minY = bounds[1];
				int maxY = bounds[1] + bounds[3];

				if ((mousex >= minX && mousex <= maxX) &&
					(mousey >= minY && mousey <= maxY))
				{
					// if left side was hit, move start time
					if (mousex <= minX + 2)
					{
						selectState = STATE_TIMEWINDOWSTART;
					}
					// if right side was hit, move end time
					else if (mousex + 2 >= maxX)
					{
						selectState = STATE_TIMEWINDOWEND;
					}
					// if middle was move, move entire time
					else
					{
						selectState = STATE_TIMEWINDOWMIDDLE;
						clickStartTime = this->getViewableTimeStart();
					}
					this->setTimeWindowSelected(true);
					mouseHit = true;
					found = true;
                    redraw();
				}
			}

			// check to see if a mark was hit			
			for (int t = 0; t < model->getNumTracks(); t++)
			{
				nle::Track* track = model->getTrack(t);
				for (int b = 0; b < track->getNumBlocks(); b++)
				{
					nle::Block* block = track->getBlock(b);
					for (int m = 0; m < block->getNumMarks(); m++)
					{
						nle::Mark* mark = block->getMark(m);
						int bounds[4];
						mark->getBounds(bounds[0], bounds[1], bounds[2], bounds[3]);
						int minX = bounds[0];
						int maxX = bounds[0] + bounds[2];
						int minY = bounds[1];
						int maxY = bounds[1] + bounds[3];

						if ((mousex >= minX && mousex <= maxX) &&
							(mousey >= minY && mousey <= maxY))
						{
							mark->setSelected(!mark->isSelected());
							if (mark->isSelected())
							{
								if  (mark->isInterval())
								{
									// how to manipulate this mark? 
									// first third = start time
									// middle third = move entire mark
									// last third = end time
									int denom = maxX - minX;
									if (denom != 0)
									{
										int numerator = mousex - minX;
										float d = (float) denom;
										float n = (float) numerator;
										float param = n / d;
										if (param < .33)
										{
											mark->setSelectedStartTime(true);
											mark->setSelectedEndTime(false);
											mark->setSelectedBothTime(false);
										}
										else if (param > .67)
										{
											mark->setSelectedStartTime(false);
											mark->setSelectedEndTime(true);
											mark->setSelectedBothTime(false);
										}
										else
										{
											mark->setSelectedStartTime(false);
											mark->setSelectedEndTime(false);
											mark->setSelectedBothTime(true);
										}
									}
								}
								else
								{
									mark->setSelectedStartTime(false);
									mark->setSelectedEndTime(false);
									mark->setSelectedBothTime(true);
								}
							}

							changeMarkSelectionEvent(mark);
							// modify track selection according to mark hit
							for (int t1 = 0; t1 < model->getNumTracks(); t1++)
							{
								nle::Track* selectTrack = model->getTrack(t1);
								if (selectTrack != mark->getBlock()->getTrack())
								{
									if (selectTrack->isSelected())
									{
										selectTrack->setSelected(false);
										changeTrackSelectionEvent(selectTrack);
									}
								}
								else
								{
									if (!selectTrack->isSelected())
									{
										selectTrack->setSelected(true);
										changeTrackSelectionEvent(selectTrack);
									}
								}
							}
			
							// modify block selection according to mark hit
							for (int b1 = 0; b1 < track->getNumBlocks() ; b1++)
							{
								nle::Block* selectBlock = track->getBlock(b1);
								if (selectBlock != mark->getBlock())
								{
									if (selectBlock->isSelected())
									{
										selectBlock->setSelected(false);
										changeBlockSelectionEvent(selectBlock);
									}
								}
								else
								{
									if (!selectBlock->isSelected())
									{
										selectBlock->setSelected(true);
										changeBlockSelectionEvent(selectBlock);
									}
								}
							}

							// unselect the other marks
							for (int t1 = 0; t1 < model->getNumTracks(); t1++)
							{
								nle::Track* track1 = model->getTrack(t1);
								for (int b1 = 0; b1 < track1->getNumBlocks(); b1++)
								{
									nle::Block* block1 = track1->getBlock(b1);
									for (int m1 = 0; m1 < block1->getNumMarks(); m1++)
									{
										nle::Mark* mark1 = block1->getMark(m1);
										if (mark1 != mark)
										{
											if (mark1->isSelected())
											{
												mark1->setSelected(false);
												changeMarkSelectionEvent(mark1);
											}
										}
									}
								}
							}
							mouseHit = true;
							found = true;
							model->setModelChanged(true);
							redraw();
						}				
					}
				}
			}

			if (!blockOpLocked)
			{
				// check to see if a block was hit			
				for (int t = 0; t < model->getNumTracks(); t++)
				{
					nle::Track* track = model->getTrack(t);
					for (int b = 0; b < track->getNumBlocks(); b++)
					{
						nle::Block* block = track->getBlock(b);
						int bounds[4];
						block->getBounds(bounds[0], bounds[1], bounds[2], bounds[3]);
						int minX = bounds[0];
						int maxX = bounds[0] + bounds[2];
						int minY = bounds[1];
						int maxY = bounds[1] + bounds[3];

						if ((mousex >= minX && mousex <= maxX) &&
							(mousey >= minY && mousey <= maxY))
						{
							block->setSelected(!block->isSelected());
							changeBlockSelectionEvent(block);
							for (int t = 0; t < model->getNumTracks(); t++)
							{
								nle::Track* selectTrack = model->getTrack(t);
								if (selectTrack != block->getTrack())
								{
									if (selectTrack->isSelected())
									{
										selectTrack->setSelected(false);
										changeTrackSelectionEvent(selectTrack);
									}
								}
								else
								{
									if (!selectTrack->isSelected())
									{
										selectTrack->setSelected(true);
										changeTrackSelectionEvent(selectTrack);
									}
								}
							}
							if (block->getTrack()->isSelected() != block->isSelected())
							{
								block->getTrack()->setSelected(block->isSelected());
								changeBlockSelectionEvent(block);
							}
							mouseHit = true;
							// unselect the other blocks
							for (int b2 = 0; b2 < track->getNumBlocks(); b2++)
							{
								nle::Block* block2 = track->getBlock(b2);
								if (block2 != block)
								{
									if (block2->isSelected())
									{
										block2->setSelected(false);
										changeBlockSelectionEvent(block2);
									}
								}
							}
							found = true;
							model->setModelChanged(true);
							redraw();
						}
					}
				}
			}

			// check for track selection
			if (!mouseHit)
			{
				for (int t = 0; t < model->getNumTracks(); t++)
				{
					nle::Track* track = model->getTrack(t);
					int bounds[4];
					track->getBounds(bounds[0], bounds[1], bounds[2], bounds[3]);
					int trackMinX = bounds[0];
					int trackMaxX = bounds[0] + bounds[2];
					int trackMinY = bounds[1];
					int trackMaxY = bounds[1] + bounds[3];

					if ((mousex >= trackMinX && mousex <= trackMaxX) &&
						(mousey >= trackMinY && mousey <= trackMaxY))
					{
						// hit the track
						for (int t = 0; t < model->getNumTracks(); t++)
						{
							nle::Track* selectTrack = model->getTrack(t);
							if (selectTrack != track)
							{
								if (selectTrack->isSelected())
								{
									selectTrack->setSelected(false);
									changeTrackSelectionEvent(selectTrack);
								}
							}
							else
							{
								if (!selectTrack->isSelected())
								{
									selectTrack->setSelected(true);
									changeTrackSelectionEvent(selectTrack);
								}
							}
						}
						mouseHit = true;
					}
                    else if (mousex < left + activationSize && mousex > left &&
                             mousey >= trackMinY && mousey <= trackMaxY) // check for track selection in name area
                    {
                        // hit the track, use activation/deactivation
                        if (track->isActive())
                        {
                            track->setActive(false);
                        }
                        else
                        {
                            track->setActive(true);
                        }
                        mouseHit = true;
                    }
				}
			}
                        
            model->setModelChanged(true);
            if (!mouseHit)
			{
				for (int t = 0; t < model->getNumTracks(); t++)
				{
					nle::Track* track = model->getTrack(t);
					for (int b = 0; b < track->getNumBlocks(); b++)
					{
						nle::Block* block = track->getBlock(b);
						if (block->isSelected())
						{
							block->setSelected(false);
							changeBlockSelectionEvent(block);
						}
						for (int m = 0; m < block->getNumMarks(); m++)
						{
							nle::Mark* mark = block->getMark(m);
							if (mark->isSelected())
							{
								mark->setSelected(false);
								changeMarkSelectionEvent(mark);
							}
						}
					}
				}
			}
			else
			{
				redraw();
			}
			
			if (found)
				return 1;
			break;

	

		case FL_RELEASE:
			selectState = STATE_NORMAL;
			cameraState = CAMERASTATE_NORMAL;
			for (int t = 0; t < model->getNumTracks(); t++)
			{
				nle::Track* track = model->getTrack(t);
				if (track->isSelected())
				{
					releaseTrackEvent(track);
				}
				for (int b = 0; b < track->getNumBlocks(); b++)
				{
					nle::Block* block = track->getBlock(b);
					if (block->isSelected())
					{
						releaseBlockEvent(block);
					}
					for (int m = 0; m < block->getNumMarks(); m++)
					{
						nle::Mark* mark = block->getMark(m);
						if (mark->isSelected())
						{
							releaseMarkEvent(mark);
						}
					}
				}
			}
			this->setTimeWindowSelected(false);
			redraw();
			break;

		case FL_MOVE:
			// if the cursor is above a block border, 
			// then change the cursor type
			if (!blockOpLocked)
			{
				for (int t = 0; t < model->getNumTracks(); t++)
				{
					nle::Track* track = model->getTrack(t);
					for (int b = 0; b < track->getNumBlocks(); b++)
					{
						nle::Block* block = track->getBlock(b);
						int bounds[4];					
						block->getBounds(bounds[0], bounds[1], bounds[2], bounds[3]);
						int leftMinX = bounds[0];
						int leftMaxX = bounds[0] + 4;
						int leftMinY = bounds[1];
						int leftMaxY = bounds[1] + bounds[3];

						int rightMinX = bounds[0] + bounds[2] - 4;
						int rightMaxX = bounds[0] + bounds[2];
						int rightMinY = bounds[1];
						int rightMaxY = bounds[1] + bounds[3];


						if ((mousex >= leftMinX && mousex <= leftMaxX) &&
							(mousey >= leftMinY && mousey <= leftMaxY))
						{	
							this->setBlockCandidate(block, true);
							fl_cursor(FL_CURSOR_CROSS);
							foundBorder = true;
						}
						else if ((mousex >= rightMinX && mousex <= rightMaxX) &&
								 (mousey >= rightMinY && mousey <= rightMaxY))
						{	
							this->setBlockCandidate(block, false);
							fl_cursor(FL_CURSOR_CROSS);
							foundBorder = true;
						}
					}
				}
			}
			break;

		default:
			break;
	}
	if (foundBorder)
	{
		fl_cursor(FL_CURSOR_CROSS);
	}
	else
	{
		fl_cursor(FL_CURSOR_DEFAULT);
		this->setBlockCandidate(NULL, true);
	}


	return Fl_Widget::handle(event);
}

int EditorWidget::convertTimeToPosition(double time)
{
	if (!this->getModel())
		return (right - trackStart) / 2;

	double viewStartTime = this->getViewableTimeStart();
	double viewEndTime = this->getViewableTimeEnd();
	double spanTime = viewEndTime - viewStartTime;

	double ratio = (time - viewStartTime) / spanTime;
	if (ratio < 0)
		ratio = 0;
	if (ratio > 1)
		ratio = 1;

	double dWidth = (double) (right - trackStart);
	double relPosition = dWidth * ratio;

	return (int) relPosition + trackStart;
}

double EditorWidget::convertPositionToTime(int position)
{
	if (!this->getModel())
		return 0.0;

	double viewStartTime = this->getViewableTimeStart();
	double viewEndTime = this->getViewableTimeEnd();
	double spanTime = viewEndTime - viewStartTime;

	int relPosition = position - trackStart;
	if (relPosition < 0)
		relPosition = 0;

	double dWidth = (double) (right - trackStart);
	double ratio = relPosition / dWidth;

	double time = ratio * spanTime + this->getViewableTimeStart();

	return time;
}

int EditorWidget::convertTimeToViewablePosition(double time)
{
	if (!this->getModel())
		return (right - trackStart) / 2;

	// determine the size of the editor
	double ratio = (time - this->getViewableTimeStart()) / (this->getViewableTimeEnd() - this->getViewableTimeStart());

	double dWidth = (double) (right - trackStart);
	double result = dWidth * ratio;

	return (int) result + trackStart;
}

double EditorWidget::convertViewablePositionToTime(int position)
{
	if (!this->getModel())
		return 0.0;

	int relPosition = position - trackStart;
	if (relPosition < 0)
		relPosition = 0;

	double timeSpan = (this->getViewableTimeEnd() - this->getViewableTimeStart());

	double time = ((double) relPosition / (double) (right - trackStart)) * timeSpan;
	time += this->getViewableTimeStart();

	if (time > this->getViewableTimeEnd())
		time = this->getViewableTimeEnd();

	if (time < this->getViewableTimeStart())
		time = this->getViewableTimeStart();

	return time;
}


void EditorWidget::setTimeWindowBounds(int x, int y, int w, int h)
{
	timeWindowBounds[0] = x;
	timeWindowBounds[1] = y;
	timeWindowBounds[2] = w;
	timeWindowBounds[3] = h;
}

void EditorWidget::getTimeWindowBounds(int& x, int& y, int& w, int& h)
{
	x = timeWindowBounds[0];
	y = timeWindowBounds[1];
	w = timeWindowBounds[2];
	h = timeWindowBounds[3];
}

void EditorWidget::setTimeSliderBounds(int x, int y, int w, int h)
{
	timeSliderBounds[0] = x;
	timeSliderBounds[1] = y;
	timeSliderBounds[2] = w;
	timeSliderBounds[3] = h;
}

void EditorWidget::getTimeSliderBounds(int& x, int& y, int& w, int& h)
{
	x = timeSliderBounds[0];
	y = timeSliderBounds[1];
	w = timeSliderBounds[2];
	h = timeSliderBounds[3];
}

bool EditorWidget::isTimeWindowSelected()
{
	return timeWindowSelected;
}

void EditorWidget::setTimeWindowSelected(bool val)
{
	timeWindowSelected = val;
}

nle::Block* EditorWidget::getBlockCandidate(bool& beginning)
{
	beginning = candidateBeginning;
	return blockCandidate;
}

void EditorWidget::setBlockCandidate(nle::Block* block, bool beginning)
{
	blockCandidate = block;
	candidateBeginning = beginning;
}


void EditorWidget::changeBlockSelectionEvent(Block* block)
{
}

void EditorWidget::changeTrackSelectionEvent(Track* track)
{
}

void EditorWidget::changeMarkSelectionEvent(Mark* mark)
{
}

void EditorWidget::releaseBlockEvent(Block* block)
{

}

void EditorWidget::releaseTrackEvent(Track* track)
{

}

void EditorWidget::releaseMarkEvent(Mark* mark)
{

}


void EditorWidget::notifyModelChanged(NonLinearEditorModel* model)
{
	int numTracks = 0;
	if (model)
		numTracks = model->getNumTracks();
	int widgetHeight = padding + (numTracks * trackHeight) + timeWindowHeight + 10 + 50;
	if (widgetHeight != h())
		h(widgetHeight);
	setup();
}


void EditorWidget::lockBlockFunc(bool val)
{
	blockOpLocked = val;
}

bool EditorWidget::getBlockLockedStatus()
{
	return blockOpLocked;
}

}
