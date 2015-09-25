#ifndef _VISEMECURVE_H
#define _VISEMECURVE_H

#include <vector>
#include <sr/sr_vec.h>
#include <FL/Fl.H>

class VisemeCurve : public std::vector<SrVec>
{
public:
	VisemeCurve();
	~VisemeCurve();

	void copy(VisemeCurve& v);

	bool isVisible();
	void setVisibilty(bool isVisible);

	Fl_Color getLineColor();
	void SetLineColor(Fl_Color color);

	Fl_Color getPointColor();
	void SetPointColor(Fl_Color color);

	int getPointRadius();
	void setPointRadius(int size);

protected:
	bool _isVisible;
	Fl_Color _lineColor;
	Fl_Color _pointColor;
	int _pointRadius;
};

#endif