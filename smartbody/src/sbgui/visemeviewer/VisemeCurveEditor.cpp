#include "VisemeCurveEditor.h"

#include <FL/Fl.H>
#include <FL/Fl_Device.H>
#include <FL/fl_draw.H>
#include <stdlib.h>
#include <sr/sr_vec.h>

VisemeCurveEditor::VisemeCurveEditor(int x, int y, int w, int h, char* name) : Fl_Widget(x, y, w, h)
{
	_pointIsSelected = false;
	_lineIsSelected = false;

	_selectedPoint = -1;
	_selectedLine = -1;

	_gridSizeX = 5;
	_gridSizeY = 5;
	_gridPosX = x + 5;
	_gridPosY = y + 5;
	_gridWidth = w - 12;
	_gridHeight = h - 12;

	visemeWindow = NULL;

	_colors.push_back(FL_GRAY0);
	_colors.push_back(FL_RED);
	//_colors.push_back(FL_GREEN);
	_colors.push_back(FL_BLUE);
	_colors.push_back(FL_DARK_RED);
	_colors.push_back(FL_DARK_GREEN);
	_colors.push_back(FL_CYAN);
	_colors.push_back(FL_DARK_BLUE);
	_colors.push_back(FL_MAGENTA);
}

void VisemeCurveEditor::setVisemeWindow(VisemeViewerWindow* w)
{
	visemeWindow = w;
}

void VisemeCurveEditor::selectLine(int viseme){
	_lineIsSelected = true;
	_selectedLine = viseme;

	_pointIsSelected = false;
	_selectedPoint = -1;
}

VisemeCurveEditor::~VisemeCurveEditor()
{
}

void VisemeCurveEditor::clear()
{
	fl_color(FL_GRAY);
	fl_rectf(x(), y(), w(), h());
}

void VisemeCurveEditor::refresh()
{
	_selectedLines.clear();
	_selectedLine = -1;
	_selectedPoint = -1;
	_pointIsSelected = false;
	_lineIsSelected = false;
	redraw();
}

void VisemeCurveEditor::draw()
{
	clear();

	drawGrid();
	drawAxes();
	drawPoints();
	drawCurve();
	visemeWindow->drawNames();

	//draw scrub line for slider
	fl_color(FL_BLACK);
	if (visemeWindow->isPlayingViseme())
	{
		float sliderVal = visemeWindow->getSliderValue();
		fl_line(_gridPosX + int(_gridWidth * sliderVal), _gridPosY, _gridPosX + int(_gridWidth * sliderVal), _gridPosY + _gridHeight);
	}
}


int VisemeCurveEditor::handle(int event)
{
	int mousex = Fl::event_x();
	int mousey = Fl::event_y();
	bool isCurveDirty = false;

	// copy&paste selected lines
	int isCtrlPressed = Fl::event_state(FL_CTRL);
	int isKeyCPressed = Fl::get_key('c');
	int isKeyVPressed = Fl::get_key('v');
	if (isCtrlPressed && isKeyCPressed)
	{
		for (int i = 0; i < (int)_selectedLines.size(); ++i)
		{
			VisemeCurve copiedCurve;
			copiedCurve.copy(_curves[_selectedLines[i]]);
			_copiedCurveMap.insert(std::make_pair(_selectedLines[i], copiedCurve));
		}
	}
	if (isCtrlPressed && isKeyVPressed)
	{
		for (	std::map<int, VisemeCurve>::iterator iter = _copiedCurveMap.begin();
			iter != _copiedCurveMap.end();
			iter++
			)
		{
			_curves[iter->first] = (iter->second);
			visemeWindow->selectViseme(iter->first + 1);
		}
		isCurveDirty = true;
		_copiedCurveMap.clear();
		redraw();
	}

	switch (event)
	{
		case FL_PUSH:
		{
			int isButton1Pressed = Fl::event_state(FL_BUTTON1);
			int isButtonMidPressed = Fl::event_state(FL_BUTTON2);
			int isButton2Pressed = Fl::event_state(FL_BUTTON3);
			int isShiftPressed = Fl::event_state(FL_SHIFT);

			// middle mouse clear out
			if (isButtonMidPressed)
			{
				_selectedLine = -1;
				_selectedPoint = -1;
				_selectedLines.clear();
				_pointIsSelected = false;
				_lineIsSelected = false;
				redraw();
			}
			
			// select line, multi-select lines, add points
			if (isButton1Pressed)
			{	
				if (isCtrlPressed)
				{
					_selectedLines.clear();
					if (isLineSelected(mousex, mousey))
					{
						redraw();
					}
				}
				else if (isShiftPressed)
				{
					if (_lineIsSelected)
						_selectedLines.push_back(_selectedLine);

					if (isLineSelected(mousex, mousey))
					{
						_selectedLines.push_back(_selectedLine);
						redraw();
					}
				}
				else
				{
					_selectedLines.clear();
					bool pointIsSelected = isPointSelected(mousex, mousey);

					if(pointIsSelected)
					{
						_pointIsSelected = true;
					}
					else
					{
						if (_lineIsSelected)
						{
							int insertIndx = getInsertionIndex(mousex, mousey);

							SrVec point((float) mousex, (float) mousey, 0.f);
							SrVec curvePoint = mapDrawData(point);

							_curves[_selectedLine].insert(_curves[_selectedLine].begin() + insertIndx, curvePoint);
							_selectedPoint = insertIndx;
							_pointIsSelected = true;
							isCurveDirty = true;
							// draw a point in that location
							redraw();
						}
					}
				}
			}
			else if (isButton2Pressed)
			{
				bool pointIsSelected = isPointSelected(mousex, mousey);
				
				if (pointIsSelected)
				{
					_curves[_selectedLine].erase(_curves[_selectedLine].begin() + _selectedPoint);
					isCurveDirty = true;
					_pointIsSelected = false;
					_selectedPoint = -1;
				}

				redraw();
			}
			if (isCurveDirty) // if data changed, refresh it
			{
				visemeWindow->refreshData();
			}
			return 1;
			break;
		}

		case FL_DRAG:
		{
			if(_pointIsSelected)
			{
				SrVec& point = _curves[_selectedLine][_selectedPoint];
				SrVec drawPoint = mapCurveData(point);

				// Enforce Constraints
				float minx = (float)_gridPosX;
				float miny = (float)_gridPosY;
				float maxx = (float)(_gridPosX + _gridWidth);
				float maxy = (float)(_gridPosY + _gridHeight);

				if(_selectedPoint > 0)
				{
					SrVec prevDrawPoint = mapCurveData(_curves[_selectedLine][_selectedPoint - 1]);
					minx =  prevDrawPoint.x;
				}
				if(_selectedPoint < (int)_curves[_selectedLine].size() - 1)
				{
					SrVec nextDrawPoint = mapCurveData(_curves[_selectedLine][_selectedPoint + 1]);
					maxx =  nextDrawPoint.x;
				}

				if(mousex > maxx)
					break;
				else if(mousex< minx )
					break;
				else if(mousey > maxy)
					break;
				else if(mousey < miny )
					break;
				else
				{
					SrVec temp((float)mousex, (float)mousey, 0.0f);
					SrVec newCurvePoint = mapDrawData(temp);
					if (newCurvePoint.x < 0.02f)	// clamp it
						newCurvePoint.x = 0.0f;
					if (newCurvePoint.x > 0.98f)	// clamp it
						newCurvePoint.x = 1.0f;

					point.x = newCurvePoint.x;
					point.y = newCurvePoint.y;
					isCurveDirty = true;
				}
				redraw();
			}
			break;
		}

		case FL_RELEASE:
		{
			//_pointIsSelected = false;
		}
	}

	if (isCurveDirty) // if data changed, refresh it
	{
		visemeWindow->refreshData();
	}

	return Fl_Widget::handle(event);
}

bool VisemeCurveEditor::isPointSelected(int mousex, int mousey)
{
	SrVec mouseClick((float) mousex, (float) mousey, 0.f);
	bool pointIsSelected = false;

	if (_selectedLine < int(0) || _selectedLine >= int(_curves.size()))
		return false;

	if(!_curves[_selectedLine].isVisible())
		return false;

	for(int j = 0; j < (int)_curves[_selectedLine].size(); j++)
	{	
		SrVec point = _curves[_selectedLine][j];
		SrVec newPoint = mapCurveData(point);
		float distance = dist(mouseClick, newPoint);
	
		if(distance <= _curves[_selectedLine].getPointRadius() * 5.0f)
		{
			pointIsSelected = true;
			_pointIsSelected = true;
			_lineIsSelected = true;
			_selectedPoint = j;
			//_selectedLine = i;
			break;
		}
	}
	
	return pointIsSelected;
}

bool VisemeCurveEditor::isLineSelected(int mousex, int mousey)
{
	SrVec mouseClick((float) mousex, (float) mousey, 0.f);
	bool lineIsSelected = false;
	for (int i = 0; i < (int)_curves.size(); i++)
	{
		if(!_curves[i].isVisible())
			continue;

		for(int j = 0; j < (int)_curves[i].size() - 1; j++)
		{	
			// check line segment instead of the point
			SrVec point1 = _curves[i][j];
			SrVec newPoint1 = mapCurveData(point1);
			SrVec point2 = _curves[i][j + 1];
			SrVec newPoint2 = mapCurveData(point2);
			SrVec closestPoint;
			float distance = getMinimumDist(mouseClick, newPoint1, newPoint2, closestPoint);

			float distToPoint1 = dist(closestPoint, newPoint1);
			float distToPoint2 = dist(closestPoint, newPoint2);

			if(distance <= _curves[i].getPointRadius())
			{
				lineIsSelected = true;
				_lineIsSelected = true;
				_selectedLine = i;
				break;
			}
		}
	}

	return lineIsSelected;
}

int VisemeCurveEditor::getInsertionIndex(int mousex, int mousey)
{
	SrVec mouseClick((float) mousex, (float) mousey, 0.f);
	
	int indx = 0;
	if(_curves.size() == 0 || !_lineIsSelected)
		return 0;

	SrVec firstKey = mapCurveData(_curves[_selectedLine][0]);
	if(mouseClick.x < firstKey.x)
		return 0;

	for (indx = 0; indx < (int)_curves[_selectedLine].size() - 1; indx++)
	{	
		SrVec& point1 = _curves[_selectedLine][indx];
		SrVec newPoint1 = mapCurveData(point1);
		SrVec& point2 = _curves[_selectedLine][indx + 1];
		SrVec newPoint2 = mapCurveData(point2);
	
		if(mouseClick.x > newPoint1.x && mouseClick.x < newPoint2.x)
			break;
	}

	return (indx + 1);
}

void VisemeCurveEditor::drawPoints()
{
	fl_begin_complex_polygon();

	// draw points
	for (int i = 0; i < (int)_curves.size(); i++)
	{
		fl_color(_curves[i].getPointColor());

		if(!_curves[i].isVisible())
			continue;

		for(int j = 0; j < (int)_curves[i].size(); j++)
		{
			SrVec& point = _curves[i][j];
			

			if(j == _selectedPoint && i == _selectedLine)
				continue;

			SrVec newPoint = mapCurveData(point);
			fl_circle(newPoint.x, newPoint.y, _curves[i].getPointRadius());
		}
	}

	// draw selected point
	if(_pointIsSelected && _curves[_selectedLine].isVisible())
	{
		SrVec& point = _curves[_selectedLine][_selectedPoint];
		SrVec newPoint = mapCurveData(point);
		fl_color(FL_BLUE);
		fl_circle(newPoint.x, newPoint.y, _curves[_selectedLine].getPointRadius() + 3);
	}

	fl_end_complex_polygon();
}

void VisemeCurveEditor::drawCurve()
{
	// connect the points with a line
	SrVec lastPoint;
	SrVec newLastPoint;
	for (int i = 0; i < (int)_curves.size(); i++)
	{
		fl_color(_curves[i].getLineColor());

		if(!_curves[i].isVisible())
			continue;

		if(i == _selectedLine)
			fl_line_style(0, 3, 0);
		else
		{
			bool hasHighlights = false;
			for (int j = 0; j < (int)_selectedLines.size(); ++j)
			{
				if (_selectedLines[j] == i)
				{
					fl_line_style(0, 3, 0);
					hasHighlights = true;
					break;
				}
			}
			if (!hasHighlights)
				fl_line_style(0, 0, 0);
		}

		for(int j = 0; j < (int)_curves[i].size(); j++)
		{

			if (j == 0)
			{
				lastPoint =_curves[i][0];
				newLastPoint = mapCurveData(lastPoint);
				continue;
			}

			SrVec& point = _curves[i][j];
			SrVec newPoint = mapCurveData(point);

			fl_line((int) newLastPoint.x, (int) newLastPoint.y, (int) newPoint.x, (int) newPoint.y);
			newLastPoint = newPoint;
		}
	}
	fl_line_style(0, 0, 0);
}


void VisemeCurveEditor::drawGrid()
{
	int centerX = x() + w() / 2;
	int centerY = y() + h() / 2;
	int ymax = y();
	int ymin = y() + h();
	int xmin = x();
	int xmax = x() + w();

	// draw grid
	fl_color(FL_WHITE);
	int numLinesX = w() / _gridSizeX;
	for (int i = -numLinesX / 2; i <= numLinesX / 2; i++)
		fl_line(centerX + i *  _gridSizeX, ymin, centerX + i *  _gridSizeX, ymax);
	int numLinesY = h() /  _gridSizeY;
	for (int i = -numLinesY / 2; i <=  numLinesY / 2; i++)
		fl_line(xmin, centerY + i * _gridSizeY, xmax, centerY + i *  _gridSizeY);
}

void VisemeCurveEditor::setVisibility(int viseme, bool isVisible)
{
	_curves[viseme].setVisibilty( isVisible);
}

void VisemeCurveEditor::generateCurves(int count)
{
	for (int i = 0; i < count; i++)
	{
		VisemeCurve curve;

		for(int j = 0; j < 5; j++)
		{
			SrVec point( rand() % 25 + (float)(x() + j * 70), (float)( y() + h()/2.0f + rand() % 70), (float)0);
			curve.push_back(point);
		}
		if (i < (int) _colors.size())
			curve.SetLineColor(_colors[i]);
		else
			curve.SetLineColor( rand() % 256);
		//curve.SetPointColor( rand() % 256);

		_curves.push_back(curve);
	}
}



void VisemeCurveEditor::changeCurve(int viseme, std::vector<float>& curveData, const std::vector<float>& phonemeCurve1, const std::vector<float>& phonemeCurve2)
{
	if (curveData.size() == 0) // generate three points as default
	{
		if (phonemeCurve1.size() > 0 || phonemeCurve2.size() > 0)
		{
			_curves[viseme].clear();
			if (phonemeCurve1.size() > 0)
			{
				for (size_t i = 0; i < phonemeCurve1.size() / 2; i++)
				{
					_curves[viseme]. push_back(SrVec(phonemeCurve1[i * 2 + 0] * 0.5f, phonemeCurve1[i * 2 + 1], 0.0f));
				}
			}
			if (phonemeCurve2.size() > 0)
			{
				for (size_t i = 0; i < phonemeCurve2.size() / 2; i++)
				{
					_curves[viseme]. push_back(SrVec(phonemeCurve2[i * 2 + 0] * 0.5f + 0.5f, phonemeCurve2[i * 2 + 1], 0.0f));
				}
			}
		}
		else
		{
			_curves[viseme].clear();
			_curves[viseme].push_back(SrVec(0.1f, 0, 0));
			_curves[viseme].push_back(SrVec(0.5f, 1.0f, 0));
			_curves[viseme].push_back(SrVec(0.9f, 0, 0));
		}
	}
	else
	{
		_curves[viseme].clear();

		for (size_t i = 0; i < curveData.size() / 2; i++)
		{
			_curves[viseme].push_back(SrVec(curveData[i * 2 + 0], curveData[i * 2 + 1], 0.0f));
		}
	}
}

SrVec VisemeCurveEditor::mapCurveData(SrVec& origData)
{
	SrVec newData;
	newData.x = origData.x * _gridWidth + _gridPosX;
	newData.y = (1.0f - origData.y) * _gridHeight + _gridPosY;
	newData.z = origData.z;
	return newData;
}

SrVec VisemeCurveEditor::mapDrawData(SrVec& origData)
{
	SrVec newData;
	newData.x = (origData.x - _gridPosX) / _gridWidth;
	newData.y = 1.0f - ((origData.y - _gridPosY) / _gridHeight);
	newData.z = origData.z;

	return newData;
}

std::vector<VisemeCurve>& VisemeCurveEditor::getCurves()
{
	return _curves;
}

float VisemeCurveEditor::getMinimumDist(SrVec& c, SrVec& a, SrVec& b, SrVec& minimumPt)
{
	SrVec ab = b - a;
	SrVec ac = c - a;
	float f = dot(ab, ac);
	float d = dot(ab, ab);
	if (f < 0)
	{
		minimumPt = a;
		return dist(c, a);
	}
	if (f > d)
	{
		minimumPt = b;
		return dist(c, b);
	}
	f = f / d;
	minimumPt = a + f * ab;
	return dist(c, minimumPt);
}


void VisemeCurveEditor::drawAxes()
{
	fl_color(FL_BLACK);
	fl_line(_gridPosX, _gridPosY, _gridPosX, _gridPosY + _gridHeight);
	fl_line(_gridPosX , _gridPosY + _gridHeight, _gridPosX + _gridWidth, _gridPosY + _gridHeight);

}
