#ifndef _PARAMETERGROUP_H_
#define _PARAMETERGROUP_H_

#include <FL/Fl_Group.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Value_Slider.H>
#include <sb/PABlend.h>

class PABlendData;
class ParameterVisualization;
class Parameter3DVisualization;
class PanimationWindow;
namespace SmartBody { class SBCharacter; }

class ParameterGroup : public Fl_Group
{
	public:
		ParameterGroup(int x, int y, int w, int h, char* name, PABlendData* s, PanimationWindow* window, bool ex = false);
		~ParameterGroup();

		virtual void resize(int x, int y, int w, int h);
		
		static void updateXAxisValue(Fl_Widget* widget, void* data);
		static void updateXYAxisValue(Fl_Widget* widget, void* data);
		static void updateXYZAxisValue(Fl_Widget* widget, void* data);
		void updateWeight(std::vector<double>& weights);
		PABlendData* getCurrentPABlendData();
		SmartBody::SBCharacter* getCurrentCharacter();

	public:
		PanimationWindow* paWindow;
		PABlendData* blendData;
		ParameterVisualization* paramVisualization;
		Parameter3DVisualization* param3DVisualization;
		bool exec;
		Fl_Value_Slider* xAxis;
		Fl_Value_Slider* yAxis;
		Fl_Value_Slider* zAxis;
};

#endif
