#include "vhcl.h"
#include "ParameterVisualization.h"
#include <sb/SBCharacter.h>
#include <controllers/me_ct_param_animation.h>
#include "ParameterGroup.h"
#include "PanimationWindow.h"


ParameterVisualization::ParameterVisualization(bool isInteractive, int x, int y, int w, int h, char* name, PABlendData* s, ParameterGroup* group) : Fl_Group(x, y, w, h, name), blendData(s), paramGroup(group)
{
	paramX = -9999;
	paramY = -9999;
	interactiveMode = isInteractive;
	this->begin();
	this->end();

	setup();
}

ParameterVisualization::~ParameterVisualization()
{
}

void ParameterVisualization::draw()
{
	Fl_Group::draw();

	setup();
	int xmin = centerX - width/2;
	int xmax = centerX + width/2;
	int ymin = centerY - height/2;
	int ymax = centerY + height/2;
	fl_rectf(xmin,ymin,width,height,FL_GRAY); // manually clean up the drawing area
	
	// draw axis
	fl_color(FL_BLACK);
	fl_line(xmin, centerY, xmax, centerY);
	fl_line(centerX, ymin, centerX, ymax);
	int recX, recY, recW, recH;
	getBound(centerX, centerY, recX, recY, recW, recH);	
	fl_rectf(recX, recY, recW, recH);

	// draw grid
	fl_color(FL_WHITE);
	int numLinesX = width / gridSizeX;
	for (int i = -numLinesX / 2; i <= numLinesX / 2; i++)
		fl_line(centerX + i * gridSizeX, ymin, centerX + i * gridSizeX, ymax);
	int numLinesY = height / gridSizeY;
	for (int i = -numLinesY / 2; i <=  numLinesY / 2; i++)
		fl_line(xmin, centerY + i * gridSizeY, xmax, centerY + i * gridSizeY);

	// draw parameters
	bool highLight = false;
	if (selectedParameters.size() == blendData->state->getNumParameters())
		highLight = true;

	fl_color(FL_GREEN);
	for (int i = 0; i < blendData->state->getNumParameters(); i++)
	{
		if (highLight && selectedParameters[i])
			fl_color(FL_RED);
		else
			fl_color(FL_GREEN);

		int recX, recY, recW, recH;
		SrVec vec =  blendData->state->getVec(i);
		int x = 0;
		if (fabs(scaleX) > 0.0001)
			x = int(vec.x / scaleX);
		int y = 0;
		if (fabs(scaleY) > 0.0001)
			y = int(vec.y / scaleY);
		getBound(centerX + x, centerY - y, recX, recY, recW, recH);
		fl_rectf(recX, recY, recW, recH);
	}

	// draw lines connecting parameters
	fl_color(FL_GREEN);
	for (int i = 0; i <  blendData->state->getNumTriangles(); i++)
	{
		SrVec vec1 = blendData->state->getTriangle(i).a;
		SrVec vec2 = blendData->state->getTriangle(i).b;
		SrVec vec3 = blendData->state->getTriangle(i).c;
		int x1, y1, x2, y2, x3, y3;
		getActualPixel(vec1.x, vec1.y, x1, y1);
		getActualPixel(vec2.x, vec2.y, x2, y2);
		getActualPixel(vec3.x, vec3.y, x3, y3);

		fl_line(x1, y1, x2, y2);
		fl_line(x1, y1, x3, y3);
		fl_line(x3, y3, x2, y2);
	}

	if (selectedTriangles.size() == blendData->state->getNumTriangles())
	{
		fl_color(FL_MAGENTA);
		for (int i = 0; i <  blendData->state->getNumTriangles(); i++)
		{
			SrVec vec1 = blendData->state->getTriangle(i).a;
			SrVec vec2 = blendData->state->getTriangle(i).b;
			SrVec vec3 = blendData->state->getTriangle(i).c;
			int x1, y1, x2, y2, x3, y3;
			getActualPixel(vec1.x, vec1.y, x1, y1);
			getActualPixel(vec2.x, vec2.y, x2, y2);
			getActualPixel(vec3.x, vec3.y, x3, y3);

			if (selectedTriangles[i])
			{
				fl_line(x1, y1, x2, y2);
				fl_line(x1, y1, x3, y3);
				fl_line(x3, y3, x2, y2);
			}
		}
	}

	// draw parameters info
	fl_color(FL_BLACK);
	for (int i = 0; i < blendData->state->getNumParameters(); i++)
	{
		if (highLight && selectedParameters[i])
			fl_color(FL_BLUE);
		else
			fl_color(FL_BLACK);

		SrVec vec = blendData->state->getVec(i);
		int x = int(vec.x / scaleX);
		int y = int(vec.y / scaleY);
		char buff[200];
//		sprintf(buff, "%s(%d,%d)", state->getMotionName(i).c_str(), x, y);
		sprintf(buff, "%s", blendData->state->getMotionName(i).c_str());
		Fl_Font prevFont = fl_font();
		Fl_Fontsize prevSize = fl_size();

		fl_font(FL_COURIER,12);		
		fl_draw(buff, centerX + x, centerY - y - 10);
		// restore the previous font size
		fl_font(prevFont,prevSize);
	}

	// draw parameter
	fl_color(FL_RED);
	if (paramX != -9999 && paramY != -9999)
	{
		int recX, recY, recW, recH;
		getBound(paramX, paramY, recX, recY, recW, recH);
		fl_rectf(recX, recY, recW, recH);		
	}
}

int ParameterVisualization::handle(int event)
{
	if (!interactiveMode)
		return Fl_Group::handle(event);

	int mousex = Fl::event_x();
	int mousey = Fl::event_y();
	switch (event)
	{
		case FL_DRAG:
		{
			bool altKeyPressed = (Fl::event_state(FL_BUTTON1) || Fl::event_state(FL_BUTTON3));
			if (altKeyPressed)
			{
				float valX, valY;
				getActualParam(valX, valY, mousex, mousey);
				updateSlider(valX, valY);
				updateStateData(valX, valY);
				getActualPixel(valX, valY, paramX, paramY);
				redraw();
				break;
			}
		}
	}
	if (event == FL_PUSH)
		return 1;
	return Fl_Group::handle(event);
}

void ParameterVisualization::setup()
{
	centerX = w() / 2  + x();//paramGroup->x();
	centerY = h() / 2  + y();//paramGroup->y();
	width = w();
	height = h();

	SrVec vec = blendData->state->getVec(blendData->state->getMaxVecX());
	float maxX = vec.x;
	vec = blendData->state->getVec(blendData->state->getMinVecX());
	float minX = vec.x;
	if (fabs(maxX) < fabs(minX)) maxX = minX;
	vec = blendData->state->getVec(blendData->state->getMaxVecY());
	float maxY = vec.y;
	vec = blendData->state->getVec(blendData->state->getMinVecY());
	float minY = vec.y;
	if (fabs(maxY) < fabs(minY)) maxY = minY;
	scaleX = fabs(maxX * 3 / (float)width);
	scaleY = fabs(maxY * 3 / (float)height);
}

void ParameterVisualization::resize(int x, int y, int w, int h)
{
	Fl_Group::resize(x, y, w, h);
	setup();
	redraw();
}

void ParameterVisualization::getBound(int ptX, int ptY, int& x, int& y, int& w, int& h)
{
	x = ptX - pad;
	y = ptY - pad;
	w = 2 * pad;
	h = 2 * pad;
}

// input actual parameter, returning pixel on the screen
void ParameterVisualization::getActualPixel(float paramX, float paramY, int& x, int& y)
{
	if (fabs(scaleX) > 0.0001)
		x = int(paramX / scaleX);
	else
		x = int(paramX);
	x = centerX + x;
	if (fabs(scaleY) > 0.0001)
		y = int(paramY / scaleY);
	else
		y = int(paramY);
	y = centerY - y;
}

void ParameterVisualization::getActualParam(float& outX, float& outY, int x, int y)
{
	outX = (x - centerX) * scaleX;
	outY = (centerY - y) * scaleY;
}


// set the parameter location on the screen
void ParameterVisualization::setPoint(int x, int y)
{
	paramX = x;
	paramY = y;
	redraw();
}

void ParameterVisualization::updateSlider(float param1, float param2)
{
	paramGroup->xAxis->value(param1);
	if (paramGroup->yAxis)
		paramGroup->yAxis->value(param2);
	redraw();
}

void ParameterVisualization::updateStateData(float param1, float param2)
{
	if (blendData->state->getType() == 0)
		blendData->state->getWeightsFromParameters(param1, blendData->weights);
	if (blendData->state->getType() == 1)
		blendData->state->getWeightsFromParameters(param1, param2, blendData->weights);
	SmartBody::SBCharacter* character = paramGroup->getCurrentCharacter();
	character->param_animation_ct->updateWeights(blendData->weights);
}

void ParameterVisualization::setSelectedTriangles(std::vector<bool>& selected)
{
	selectedTriangles = selected;
	redraw();
}


void ParameterVisualization::setSelectedParameters(std::vector<bool>& selected)
{
	selectedParameters = selected;
	redraw();
}
