#include "VisemeCurve.h"

VisemeCurve::VisemeCurve()
{
	_lineColor = FL_BLACK;
	_pointColor = FL_BLACK;
	_isVisible = false;
	_pointRadius = 5;
}

VisemeCurve::~VisemeCurve()
{
}

void VisemeCurve::copy(VisemeCurve& v)
{
	_lineColor = v._lineColor;
	_pointColor = v._pointColor;
	_isVisible = v._isVisible;
	_pointRadius = v._pointRadius;

	for (size_t i = 0; i < v.size(); i++)
		this->push_back(v[i]);
}

bool VisemeCurve::isVisible()
{
	return _isVisible;
}

void VisemeCurve::setVisibilty(bool isVisible)
{
	_isVisible = isVisible;
}

Fl_Color VisemeCurve::getLineColor()
{
	return _lineColor;
}

void VisemeCurve::SetLineColor(Fl_Color color)
{
	_lineColor = color;
}

Fl_Color VisemeCurve::getPointColor()
{
	return _pointColor;
}

void VisemeCurve::SetPointColor(Fl_Color color)
{
	_pointColor = color;
}

void VisemeCurve::setPointRadius(int size)
{
	_pointRadius = size;
}

int VisemeCurve::getPointRadius()
{
	return _pointRadius;
}
