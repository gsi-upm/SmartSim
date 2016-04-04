#include "vhcl.h"
#include "SBPythonClass.h"
#include "controllers/me_ct_reach.hpp"

#include <sb/sbm_character.hpp>
#include <sk/sk_skeleton.h>
#include <sk/sk_joint.h>
#include <sbm/sbm_test_cmds.hpp>
#include <sb/SBScene.h>
#include <sb/SBSimulationManager.h>
#include <controllers/me_ct_param_animation.h>
#include <controllers/me_ct_scheduler2.h>
#include <controllers/me_ct_gaze.h>
#include <controllers/me_ct_eyelid.h>
#include <controllers/me_ct_face.h>
#include <controllers/me_ct_curve_writer.hpp>

namespace SmartBody 
{

SrViewer* getViewer()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	if (!scene->getViewer())
	{
		if (scene->getViewerFactory())
		{
			SrViewerFactory* viewerFactory = scene->getViewerFactory();
			scene->setViewer(viewerFactory->create(viewerFactory->getX(), viewerFactory->getY(), viewerFactory->getW(), viewerFactory->getH()));
			scene->getViewer()->label_viewer("Visual Debugger");
			SmartBody::SBScene::getScene()->createCamera("cameraDefault");
		}
	}
	return scene->getViewer();
}

#ifndef SB_NO_PYTHON


std::string PyLogger::strBuffer = "";

void pythonExit()
{
	
}

void quitSbm()
{
	SmartBody::SBScene::getScene()->getSimulationManager()->stop();
}

void reset()
{
	SmartBody::SBScene::destroyScene();
}

void printLog(const std::string& message)
{
	string s = message;
	if (vhcl::EndsWith(s, "\n"))
		s.erase(s.length() - 1);
	if (s.length() > 0)
		LOG(message.c_str());
}



SBController* createController(std::string controllerType, std::string controllerName)
{
	SBController* controller = NULL;

	if (controllerType == "schedule")
	{
		controller = new MeCtScheduler2();
		
	}
	else if (controllerType == "gaze")
	{
		controller = new MeCtGaze();
	}
	else if (controllerType == "eyelid")
	{
		controller = new MeCtEyeLidRegulator();
	}
	else if (controllerType == "face")
	{
		controller = new MeCtFace();
	}
	else if (controllerType == "paramanimation")
	{
		controller = new MeCtParamAnimation();
	}
	else if (controllerType == "curvewriter")
	{
		controller = new MeCtCurveWriter();
	}

	if (controller)
		controller->setName(controllerName);

	return controller;
}

SrCamera* getCamera()
{
	return SmartBody::SBScene::getScene()->getActiveCamera();
}



////////////////////////////////////////////

void PyLogger::pa()
{
	strBuffer += "a";		
}

void PyLogger::pb()
{
	strBuffer += "b";		
}
void PyLogger::pc()
{
	strBuffer += "c";		
}

void PyLogger::pd()
{
	strBuffer += "d";		
}
void PyLogger::pe()
{
	strBuffer += "e";		
}

void PyLogger::pf()
{
	strBuffer += "f";		
}
void PyLogger::pg()
{
	strBuffer += "g";		
}

void PyLogger::ph()
{
	strBuffer += "h";		
}
void PyLogger::pi()
{
	strBuffer += "i";		
}

void PyLogger::pj()
{
	strBuffer += "j";		
}
void PyLogger::pk()
{
	strBuffer += "k";		
}

void PyLogger::pl()
{
	strBuffer += "l";		
}

void PyLogger::pm()
{
	strBuffer += "m";		
}
void PyLogger::pn()
{
	strBuffer += "n";		
}

void PyLogger::po()
{
	strBuffer += "o";		
}
void PyLogger::pp()
{
	strBuffer += "p";		
}

void PyLogger::pq()
{
	strBuffer += "q";		
}
void PyLogger::pr()
{
	strBuffer += "r";		
}

void PyLogger::ps()
{
	strBuffer += "s";		
}
void PyLogger::pt()
{
	strBuffer += "t";		
}

void PyLogger::pu()
{
	strBuffer += "u";		
}
void PyLogger::pv()
{
	strBuffer += "v";		
}

void PyLogger::pw()
{
	strBuffer += "w";		
}
void PyLogger::px()
{
	strBuffer += "x";		
}

void PyLogger::py()
{
	strBuffer += "y";		
}
void PyLogger::pz()
{
	strBuffer += "z";		
}
void PyLogger::pspace()
{
	strBuffer +=" ";
}

void PyLogger::p1()
{
	strBuffer +="1";
}
void PyLogger::p2()
{
	strBuffer +="2";
}
void PyLogger::p3()
{
	strBuffer +="3";
}
void PyLogger::p4()
{
	strBuffer +="4";
}
void PyLogger::p5()
{
	strBuffer +="5";
}
void PyLogger::p6()
{
	strBuffer +="6";
}
void PyLogger::p7()
{
	strBuffer +="7";
}
void PyLogger::p8()
{
	strBuffer +="8";
}
void PyLogger::p9()
{
	strBuffer +="9";
}
void PyLogger::p0()
{
	strBuffer +="0";
}




void PyLogger::pnon()
{
	strBuffer +=".";
}

void PyLogger::outlog()
{
	if (strBuffer.size() > 1)
	{
		LOG("pyLog : %s",strBuffer.c_str());
		strBuffer = "";
	}	
}
#endif

}