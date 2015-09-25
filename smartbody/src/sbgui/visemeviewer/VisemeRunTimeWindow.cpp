#include "vhcl.h"
#include "VisemeRunTimeWindow.h"
#include "VisemeRTCurveViewer.h"
#include <sb/SBBmlProcessor.h>
#include <bml/bml_speech.hpp>
#include <bml/bml_processor.hpp>
#include <algorithm>

void RunTimeCurveData::insertData(const std::string& id, const std::vector<float>& curve, int group, const std::string info)
{
	CurveData d;
	d.curveInfo = info;
	d.data = curve;
	d.groupId = group;
	d.id = id;
	d.display = false;
	d.highlight = false;
	d.inlist = true;

	// check if the curve already exist 
	_curves.push_back(d);
}

void RunTimeCurveData::insertPhoneme(const std::string& name, double t)
{
	_phonemeNames.push_back(name);
	_phonemeTimes.push_back(t);
}

void RunTimeCurveData::clear()
{
	_curves.clear();
	_phonemeNames.clear();
	_phonemeTimes.clear();
}

int RunTimeCurveData::getCurveDataId(const std::string& id, int group)
{
	for (size_t i = 0; _curves.size(); ++i)
	{
		if (_curves[i].id == id && _curves[i].groupId == group)
			return i;
	}
	return -1;
}

VisemeRunTimeWindow::VisemeRunTimeWindow(int x, int y, int w, int h, char* name) : Fl_Double_Window(x, y, w, h)
{
	_data = new RunTimeCurveData();

	this->label("Lip Sync Runtime Window");
	this->begin();
		_inputCurveFilter = new Fl_Input(60, 10, 150, 20, "Curves:");
		_inputCurveFilter->callback(OnChangeFilter, this);
		_inputCurveFilter->when(FL_WHEN_ENTER_KEY);
		_browserCurveList = new Fl_Multi_Browser(10, 30, 200, 150, "");
		//_inputCurveDisplayedFilter = new Fl_Input(350, 10, 150, 20, "Display:");
		_browserCurveDisplay = new Fl_Multi_Browser(300, 30, 200, 150, "");
		_browserCurveDisplay->callback(OnSelectDisplay, this);
		_buttonAdd = new Fl_Button(220, 40, 70, 20, ">>>");
		_buttonAdd->callback(OnAddDisplayCurve, this);
		_buttonRemove = new Fl_Button(220, 80, 70, 20, "<<<");
		_buttonRemove->callback(OnDelDisplayCurve, this);
		_buttonResetCamera = new Fl_Button(10, 190, 100, 20, "Reset Camera");
		_buttonResetCamera->callback(OnResetCameraCB, this);
		_sliderZoomRes = new Fl_Value_Slider(250, 190, 150, 20, "Zoom Resolution");
		_sliderZoomRes->type(FL_HORIZONTAL);
		_sliderZoomRes->range(0.001f, 0.5f);
		_sliderZoomRes->align(FL_ALIGN_LEFT);
		_sliderZoomRes->callback(OnChangeZoomResolutionCB, this);
		_rtCurveViewer = new VisemeRTCurveViewer(10, 210, 780, 380, "", _data);
	this->end();
	this->resizable(_rtCurveViewer);
	this->size_range(800, 600);
	_sliderZoomRes->value(_rtCurveViewer->getZoomRes());
}

VisemeRunTimeWindow::~VisemeRunTimeWindow()
{
	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

void VisemeRunTimeWindow::show()
{
	Fl_Double_Window::show();
}

void VisemeRunTimeWindow::hide()
{
	Fl_Double_Window::hide();
}

void VisemeRunTimeWindow::draw()
{
	_rtCurveViewer->draw();
	Fl_Double_Window::draw();
}

void VisemeRunTimeWindow::retrievingCurveData(BML::BmlRequest* request)
{
	_rtCurveViewer->_data->clear();

	BML::VecOfBehaviorRequest b = request->behaviors;
	for (BML::VecOfBehaviorRequest::iterator iter = b.begin();
		iter != b.end();
		iter++)
	{
		BML::BehaviorRequestPtr requestPtr = (*iter);
		BML::BehaviorRequest* behavior = requestPtr.get();
		BML::SpeechRequest* speechRequest = dynamic_cast<BML::SpeechRequest*> (behavior);
		if (speechRequest)
		{
			std::vector<SmartBody::VisemeData*>& phonemes = speechRequest->getPhonemes();
			for (size_t i = 0; i < phonemes.size(); ++i)
			{
				const std::string& pName = phonemes[i]->id();
				double t = phonemes[i]->time();
				_rtCurveViewer->_data->insertPhoneme(pName, t);
			}

			std::vector<SmartBody::VisemeData*>& debugVisemeCurves = speechRequest->getDebugVisemeCurves();
			for (size_t i = 0; i < debugVisemeCurves.size(); ++i)
			{
				const std::string id = debugVisemeCurves[i]->id();
				const std::string groupStr = debugVisemeCurves[i]->getCurveInfo();
				int groupId = atoi(groupStr.c_str());
				_rtCurveViewer->_data->insertData(id, debugVisemeCurves[i]->getFloatCurve(), groupId);
			}
		}
	}

	refreshChannel();
}

void VisemeRunTimeWindow::refreshChannel()
{
	_browserCurveList->clear();
	_browserCurveDisplay->clear();
	for (size_t i = 0; i < _data->_curves.size(); ++i)
	{
		const std::string& displayName = constructDisplayName(_data->_curves[i].id, _data->_curves[i].groupId);
		if (_data->_curves[i].display)
			_browserCurveDisplay->add(displayName.c_str());	
		else if (_data->_curves[i].inlist)
			_browserCurveList->add(displayName.c_str());	
	}
	_rtCurveViewer->redraw();
	redraw();
}


const std::string VisemeRunTimeWindow::constructDisplayName(const std::string& id, int group)
{
	std::stringstream ss;
	ss << id << "(" << group << ")";
	return ss.str();
}

void VisemeRunTimeWindow::parseDisplayName(const std::string& displayName, std::string& id, int& group)
{
	int leftBracket = displayName.find_first_of("(");
	if (leftBracket != std::string::npos)
	{
		id = displayName.substr(0, leftBracket);
		std::string groupStr = displayName.substr(leftBracket + 1, displayName.size() - leftBracket - 2);
		group = atoi(groupStr.c_str());
	}
}

bool VisemeRunTimeWindow::matchingPattern(std::string input, std::string pattern, int caseSensitive)
{
	if (caseSensitive == 0)	// convert everything to lower case first
	{
		std::transform(input.begin(), input.end(), input.begin(), ::tolower);
		std::transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);
	}

	std::vector<std::string> tokens;
	vhcl::Tokenize(pattern, tokens);
	if (tokens.size() == 0)
		return true;

	for (size_t i = 0; i < tokens.size(); ++i)
	{
		unsigned found = input.find(tokens[i]);
		if (found == std::string::npos)
			return false;
	}
	return true;
}

void VisemeRunTimeWindow::OnAddDisplayCurve(Fl_Widget* widget, void* data)
{
	VisemeRunTimeWindow* viewer = (VisemeRunTimeWindow*) data;
	for(int i = 0; i < viewer->_browserCurveList->size(); ++i)
	{
		if(viewer->_browserCurveList->selected(i+1))
		{
			int group;
			std::string id;
			viewer->parseDisplayName(viewer->_browserCurveList->text(i+1), id, group);
			int cId = viewer->_data->getCurveDataId(id, group);
			if (cId >= 0)
			{
				viewer->_data->_curves[cId].display = true;
			}
		}
	}
	viewer->refreshChannel();
}

void VisemeRunTimeWindow::OnDelDisplayCurve(Fl_Widget* widget, void* data)
{
	VisemeRunTimeWindow* viewer = (VisemeRunTimeWindow*) data;
	for(int i = 0; i < viewer->_browserCurveDisplay->size(); ++i)
	{
		if(viewer->_browserCurveDisplay->selected(i+1))
		{
			int group;
			std::string id;
			viewer->parseDisplayName(viewer->_browserCurveDisplay->text(i+1), id, group);
			int cId = viewer->_data->getCurveDataId(id, group);
			if (cId >= 0)
			{
				viewer->_data->_curves[cId].display = false;
			}
		}
	}
	viewer->refreshChannel();
}


void VisemeRunTimeWindow::OnResetCameraCB(Fl_Widget* widget, void* data)
{
	VisemeRunTimeWindow* viewer = (VisemeRunTimeWindow*) data;
	viewer->_rtCurveViewer->resetCamera();
	viewer->_sliderZoomRes->value(viewer->_rtCurveViewer->getZoomRes());
}

void VisemeRunTimeWindow::OnChangeZoomResolutionCB(Fl_Widget* widget, void* data)
{
	VisemeRunTimeWindow* viewer = (VisemeRunTimeWindow*) data;
	viewer->_rtCurveViewer->setZoomRes((float)viewer->_sliderZoomRes->value());
}

void VisemeRunTimeWindow::OnSelectDisplay(Fl_Widget* widget, void* data)
{
	VisemeRunTimeWindow* viewer = (VisemeRunTimeWindow*) data;
	for (size_t i = 0; i < viewer->_data->_curves.size(); ++i)
	{
		viewer->_data->_curves[i].highlight = false;
	}

	for(int i = 0; i < viewer->_browserCurveDisplay->size(); ++i)
	{
		int group;
		std::string id;
		viewer->parseDisplayName(viewer->_browserCurveDisplay->text(i+1), id, group);
		int cId = viewer->_data->getCurveDataId(id, group);
		if (cId >= 0 && viewer->_browserCurveDisplay->selected(i+1))
		{
			viewer->_data->_curves[cId].highlight = true;
		}
	}
	viewer->_rtCurveViewer->redraw();
}

void VisemeRunTimeWindow::OnChangeFilter(Fl_Widget* widget, void* data)
{
	VisemeRunTimeWindow* viewer = (VisemeRunTimeWindow*) data;

	if (strcmp(viewer->_inputCurveFilter->value(), "") == 0)
	{
		for (size_t i = 0; i < viewer->_data->_curves.size(); ++i)
		{
			viewer->_data->_curves[i].inlist = true;
		}
		viewer->refreshChannel();
		return;
	}


	for (size_t i = 0; i < viewer->_data->_curves.size(); ++i)
	{
		viewer->_data->_curves[i].inlist = false;
	}
	for (size_t i = 0; i < viewer->_data->_curves.size(); ++i)
	{
		std::string displayName = viewer->constructDisplayName(viewer->_data->_curves[i].id, viewer->_data->_curves[i].groupId);
		if (viewer->matchingPattern(displayName, viewer->_inputCurveFilter->value()))
			viewer->_data->_curves[i].inlist = true;
	}

	viewer->refreshChannel();
}