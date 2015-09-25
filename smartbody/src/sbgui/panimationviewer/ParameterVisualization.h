#ifndef _PARAMETERVISUALIZATION_H_
#define _PARAMETERVISUALIZATION_H_

#include <FL/Fl_Group.H>
#include <sb/PABlend.h>


class PABlendData;
class ParameterGroup;


class ParameterVisualization : public Fl_Group
{
	public:
		ParameterVisualization(bool isInteractive, int x, int y, int w, int h, char* name, PABlendData* s, ParameterGroup* window);
		~ParameterVisualization();

		virtual void draw();
		virtual int handle(int event);
		virtual void setup();
		virtual void resize(int x, int y, int w, int h);
		void getActualPixel(float paramX, float paramY, int& x, int& y);
		void getActualParam(float& paramX, float& paramY, int x, int y);
		void setPoint(int x, int y);

		void updateSlider(float param1, float param2);
		void updateStateData(float param1, float param2);

		void setSelectedTriangles(std::vector<bool>& selected);
		void setSelectedParameters(std::vector<bool>& selected);

	private:
		void getBound(int ptX, int ptY, int& x, int& y, int& w, int& h);

	private:
		static const int pad = 5;
		static const int margin = 10;
		static const int gridSizeX = 50;
		static const int gridSizeY = 30;
		float scaleX;
		float scaleY;
		int centerX;
		int centerY;
		int width;
		int height;
		int paramX;
		int paramY;
		PABlendData* blendData;
		ParameterGroup* paramGroup;
		bool interactiveMode;
		std::vector<bool> selectedTriangles;
		std::vector<bool> selectedParameters;
};

#endif
