#ifndef _VISEMECURVEEDITOR_H_
#define _VISEMECURVEEDITOR_H_

#include <FL/Fl_Widget.H>
#include <vector>
#include <sr/sr_vec.h>
#include "VisemeCurve.h"
#include "VisemeViewerWindow.h"

class VisemeViewerWindow;
class VisemeCurveEditor : public Fl_Widget
{
	public:
		VisemeCurveEditor(int x, int y, int w, int h, char* name);
		~VisemeCurveEditor();

		void draw();
		void clear();
		int handle(int event);
		void refresh();
		
		void setVisemeWindow(VisemeViewerWindow* w);
		void setVisibility(int viseme, bool isVisible);
		void selectLine(int viseme);
		//Temporary
		void generateCurves(int count);
		void changeCurve(int viseme, std::vector<float>& curveData, const std::vector<float>& phonemeCurve1, const std::vector<float>& phonemeCurve2);
		SrVec mapCurveData(SrVec& origData);
		SrVec mapDrawData(SrVec& origData);
		std::vector<VisemeCurve>& getCurves();

		float getMinimumDist(SrVec& pt, SrVec& a, SrVec& b, SrVec& minimumPt);

	protected:
		std::vector<VisemeCurve> _curves;
		std::map<int, VisemeCurve> _copiedCurveMap;

		VisemeViewerWindow* visemeWindow;

		int _selectedPoint;
		int _selectedLine;
		std::vector<int> _selectedLines;
		
		int _gridSizeX;
		int _gridSizeY;
		int _gridPosX;
		int _gridPosY;
		int _gridWidth;
		int _gridHeight;

		bool _pointIsSelected;
		bool _lineIsSelected;

		bool isPointSelected(int mousex, int mousey);
		bool isLineSelected(int mousex, int mousey);
		int getInsertionIndex(int mousex, int mousey);

		void drawAxes();
		void drawPoints();
		void drawCurve();
		void drawGrid();
		void drawName();

		std::vector<Fl_Color> _colors;
};

#endif