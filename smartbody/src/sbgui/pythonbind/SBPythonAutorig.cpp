#include "vhcl.h"
#include "SBPythonAutoRig.h"
#include "SBInterfaceListener.h"
#include <sb/SBObject.h>
#include <sb/SBScene.h>
#include <sb/SBAssetManager.h>
#include <sb/SBPawn.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include <sbm/ParserOpenCOLLADA.h>
#include <autorig/SBAutoRigManager.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <vector>
#include <string>

#ifndef SB_NO_PYTHON
#include <boost/python/suite/indexing/vector_indexing_suite.hpp> 
#include <boost/python/return_internal_reference.hpp>
#include <boost/python/args.hpp>
#include <boost/python.hpp>
#endif

#include <RootWindow.h>
#include <fltk_viewer.h>


#ifndef SB_NO_PYTHON


struct SBInterfaceListenerWrap : SBInterfaceListener, boost::python::wrapper<SBInterfaceListener>
{
	virtual void onStart()
	{
		if (boost::python::override o = this->get_override("onStart"))
		{
			try {
				o();
			} catch (...) {
				PyErr_Print();
			}
		}

		SBInterfaceListener::onStart();
	};

	void default_onStart()
	{
		SBInterfaceListener::onStart();
	}

	virtual bool onMouseClick(int x, int y, int button)
	{
		if (boost::python::override o = this->get_override("onMouseClick"))
		{
			try {
				return o(x, y, button);
			} catch (...) {
				PyErr_Print();
			}
		}

		return SBInterfaceListener::onMouseClick(x, y, button);
	};

	bool default_onMouseClick(int x, int y, int button)
	{
		return SBInterfaceListener::onMouseClick(x, y, button);
	}

	virtual bool onMouseMove(int x, int y)
	{
		if (boost::python::override o = this->get_override("onMouseMove"))
		{
			try {
				return o(x, y);
			} catch (...) {
				PyErr_Print();
			}
		}

		return SBInterfaceListener::onMouseMove(x, y);
	};

	bool default_onMouseMove(int x, int y)
	{
		return SBInterfaceListener::onMouseMove(x, y);
	}

	virtual bool onMouseRelease(int x, int y, int button)
	{
		if (boost::python::override o = this->get_override("onMouseRelease"))
		{
			try {
				return o(x, y);
			} catch (...) {
				PyErr_Print();
			}
		}

		return SBInterfaceListener::onMouseRelease(x, y, button);
	};

	bool default_onMouseRelease(int x, int y, int button)
	{
		return SBInterfaceListener::onMouseRelease(x, y, button);
	}

	virtual bool onMouseDrag(int x, int y)
	{
		if (boost::python::override o = this->get_override("onMouseDrag"))
		{
			try {
				return o(x, y);
			} catch (...) {
				PyErr_Print();
			}
		}

		return SBInterfaceListener::onMouseDrag(x, y);
	};

	bool default_onMouseDrag(int x, int y)
	{
		return SBInterfaceListener::onMouseDrag(x, y);
	}

	virtual bool onKeyboardPress(char c)
	{
		if (boost::python::override o = this->get_override("onKeyboardPress"))
		{
			try {
				return o(c);
			} catch (...) {
				PyErr_Print();
			}
		}

		return SBInterfaceListener::onKeyboardPress(c);
	};

	bool default_onKeyboardPress(char c)
	{
		return SBInterfaceListener::onKeyboardPress(c);
	}

	virtual bool onKeyboardRelease(char c)
	{
		if (boost::python::override o = this->get_override("onKeyboardRelease"))
		{
			try {
				return o(c);
			} catch (...) {
				PyErr_Print();
			}
		}

		return SBInterfaceListener::onKeyboardRelease(c);
	};

	bool default_onKeyboardRelease(char c)
	{
		return SBInterfaceListener::onKeyboardRelease(c);
	}

	virtual void onEnd()
	{
		if (boost::python::override o = this->get_override("onEnd"))
		{
			try {
				o();
			} catch (...) {
				PyErr_Print();
			}
		}

		SBInterfaceListener::onEnd();
	};

	void default_onEnd()
	{
		SBInterfaceListener::onEnd();
	}

};




void setPawnMesh(const std::string& pawnName, const std::string& meshName, SrVec meshScale)
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBAssetManager* assetManager = scene->getAssetManager();
	SmartBody::SBPawn* pawn = scene->getPawn(pawnName);
	if (!pawn)
		return;

	DeformableMesh* mesh = assetManager->getDeformableMesh(meshName);
	if (!mesh)
	{
		return;
	}
	if (mesh)
	{
		pawn->dStaticMeshInstance_p = new SbmDeformableMeshGPUInstance();
		pawn->dStaticMeshInstance_p->setToStaticMesh(true);
		DeformableMeshInstance* meshInsance = pawn->dStaticMeshInstance_p;
		meshInsance->setDeformableMesh(mesh);
		//meshInsance->setSkeleton(pawn->getSkeleton());	
		meshInsance->setPawn(pawn);
		meshInsance->setMeshScale(meshScale);
	}
}

void saveDeformableMesh(const std::string& meshName, const std::string& skelName, const std::string& outDir)
{
	std::vector<std::string> moNames;
	double scale = 1.0;
	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(skelName);
	if (character)
	{
		scale = character->getDoubleAttribute("deformableMeshScale");
	}

	ParserOpenCOLLADA::exportCollada(outDir, skelName, meshName, moNames, true, true, false, scale);
}

//	Callback function for Python module Misc to run the checkVisibility function
std::vector<std::string> checkVisibility(const std::string& character)
{
	bool DEBUG_CHECK_VISIBILITY			= true;
	
	SmartBody::SBScene* scene			= SmartBody::SBScene::getScene();

	BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
	if (window && window->curViewer)
		window->curViewer->make_current(); // make sure the OpenGL context is current

	std::vector<std::string> visible	= scene->checkVisibility(character);
	
	if(DEBUG_CHECK_VISIBILITY) {
		LOG ("Visible pawns from %s: ", character.c_str());
		for( std::vector<std::string>::const_iterator i = visible.begin(); 
			 i != visible.end(); 
			 i++)
		{
			LOG ("%s, ", (*i).c_str());
		}
	}

	return visible;
}

//	Callback function for Python module Misc to run the checkVisibility function
std::vector<std::string> checkVisibility_current_view()
{
	bool DEBUG_CHECK_VISIBILITY			= true;
	
	SmartBody::SBScene* scene			= SmartBody::SBScene::getScene();

	// make current
	BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
	if (window && window->curViewer)
		window->curViewer->make_current(); // make sure the OpenGL context is current
	
	std::vector<std::string> visible	= scene->checkVisibility_current_view();

	if(DEBUG_CHECK_VISIBILITY) {
		LOG ("Visible pawns: ");
		for( std::vector<std::string>::const_iterator i = visible.begin(); i != visible.end(); ++i)  {
			LOG ("%s, ", i);
		}
	}

	return visible;
}

void addPoint(const std::string& pointName, SrVec point, SrVec color, int size)
{
	BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
	if (window->curViewer)
	{
		window->curViewer->addPoint(pointName, point, color, size);
	}
}

void removePoint(const std::string& pointName)
{
	BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
	if (window->curViewer)
	{
		window->curViewer->removePoint(pointName);
	}
}

void removeAllPoints()
{
	BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
	if (window->curViewer)
	{
		window->curViewer->removeAllPoints();
	}
}

void addLine(const std::string& lineName, std::vector<SrVec>& points, SrVec color, int width)
{
	BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
	if (window->curViewer)
	{
		window->curViewer->addLine(lineName, points, color, width);
	}
}

void removeLine(const std::string& lineName)
{
	BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
	if (window->curViewer)
	{
		window->curViewer->removeLine(lineName);
	}
}

void removeAllLines()
{
	BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
	if (window->curViewer)
	{
		window->curViewer->removeAllLines();
	}
}

BOOST_PYTHON_MODULE(GUIInterface)
{
	boost::python::def("addPoint", addPoint, "addPoint");
	boost::python::def("removePoint", removePoint, "removePoint");
	boost::python::def("removeAllPoints", removePoint, "removeAllPoints");
	boost::python::def("addLine", addLine, "addLine");
	boost::python::def("removeLine", removeLine, "removeLine");
	boost::python::def("removeAllLines", removeAllLines, "removeAllLines");



	boost::python::class_<SBInterfaceListenerWrap, boost::noncopyable> ("SBInterfaceListener")
		.def(boost::python::init<>())
		.def("onStart", &SBInterfaceListener::onStart, "onStart")
		.def("onMouseClick", &SBInterfaceListener::onMouseClick, &SBInterfaceListenerWrap::default_onMouseClick, "onMouseClick")
		.def("onMouseMove", &SBInterfaceListener::onMouseMove, &SBInterfaceListenerWrap::default_onMouseMove, "onMouseMove")
		.def("onMouseRelease", &SBInterfaceListener::onMouseRelease, &SBInterfaceListenerWrap::default_onMouseRelease, "onMouseRelease")
		.def("onMouseDrag", &SBInterfaceListener::onMouseDrag, &SBInterfaceListenerWrap::default_onMouseDrag, "onMouseDrag")
		.def("onKeyboardPress", &SBInterfaceListener::onKeyboardPress, &SBInterfaceListenerWrap::default_onKeyboardPress, "onKeyboardPress")
		.def("onKeyboardRelease", &SBInterfaceListener::onKeyboardRelease, &SBInterfaceListenerWrap::default_onKeyboardRelease, "onKeyboardRelease")
		.def("onEnd", &SBInterfaceListener::onEnd, "onEnd")
	;

	boost::python::def("getInterfaceManager", SBInterfaceManager::getInterfaceManager, boost::python::return_value_policy<boost::python::reference_existing_object>(),"Gets the interface manager.");
	
	boost::python::class_<SBInterfaceManager, boost::noncopyable> ("SBInterfaceManager")
		.def("addInterfaceListener", &SBInterfaceManager::addInterfaceListener, "Adds an interface listener.")
		.def("removeInterfaceListener", &SBInterfaceManager::removeInterfaceListener, "Removes an interface listener.")
		.def("convertScreenSpaceTo3D", &SBInterfaceManager::convertScreenSpaceTo3D, "Converts screen space to 3D space given a point on a plane and a normal to that plane.")
		.def("getSelectedObject", &SBInterfaceManager::getSelectedObject, "Returns the name of the currently selection object.")
;
}


BOOST_PYTHON_MODULE(AutoRig)
{	
	boost::python::def("saveDeformableMesh", saveDeformableMesh, "Save the deformable model to the target directory");
	boost::python::def("setPawnMesh", setPawnMesh, "Set the deformable model to the target pawn");

	boost::python::class_<SBAutoRigManager>("SBAutoRigManager")
		.def("getAutoRigManager", &SBAutoRigManager::singletonPtr, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Get the autorigging manager")
		.staticmethod("getAutoRigManager")
		.def("buildAutoRiggingFromPawnMesh", &SBAutoRigManager::buildAutoRiggingFromPawnMesh, "Build the rigging from a pawn with mesh")
		;
		//.def("setHPRSmooth", &SBAutoRigManager::setHPRSmooth, "Sets the heading, pitch and roll of the character's world offset. The character will be rotated smoothly overtime to avoid popping.")
		//;	
}

BOOST_PYTHON_MODULE(Misc)
{	
	boost::python::def("checkVisibility", checkVisibility, boost::python::return_value_policy<boost::python::return_by_value>(), "Lists visible pawns for a given character");
	boost::python::def("checkVisibility_current_view", checkVisibility_current_view, boost::python::return_value_policy<boost::python::return_by_value>(), "Lists visible pawns from current viewport");

}

void initGUIInterfacePythonModule()
{
	initGUIInterface();
}

void initMiscPythonModule()
{
	initMisc();
}

void initAutoRigPythonModule()
{
	initAutoRig();
}



#endif
