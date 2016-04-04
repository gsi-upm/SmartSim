#include "vhcl.h"
#include "SBPython.h"
#include "SBPythonClass.h"
#include <sr/sr_camera.h>
#include <sb/SBScene.h>
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationTransition.h>
#include <sb/SBAnimationTransitionRule.h>
#include <sb/SBEvent.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <sb/SBVersion.hpp>

#ifndef SB_NO_PYTHON
#include <boost/python/suite/indexing/vector_indexing_suite.hpp> 
#include <boost/python/return_internal_reference.hpp>
#include <boost/python/args.hpp>
#endif

#include "SBPythonInternal.h"


typedef std::map<std::string,SrQuat> QuatMap;
typedef std::map<std::string,SrVec> VecMap;
typedef std::map<std::string, std::string> StringMap;


#ifndef SB_NO_PYTHON


namespace SmartBody
{

void pythonFuncsSystem()
{
	// viewers
	boost::python::def("getCamera", getCamera, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the camera object for the viewer. \n Input: NULL \n Output: camera object");
	boost::python::def("getViewer", getViewer, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the visual debugger. \n Input: NULL \n Output: visual debugger");
	

	// assets
//	boost::python::def("execScripts", execScripts, "Execute a chain of scripts. \n Input: list of script name string e.g. [\"script1 name\", \"script2 name\", ...] \n Output: NULL");
//	boost::python::def("getScript", getScript, boost::python::return_value_policy<boost::python::manage_new_object>(), "Returns the sequence file object. \n Input: script name \n Output: script object");
	


	// system
	boost::python::def("pythonexit", pythonExit, "Exits the Python interpreter. ");
	boost::python::def("reset", reset, "Reset SBM. ");
	boost::python::def("quit", quitSbm, "Quit SBM. ");	
	boost::python::def("getScene", SBScene::getScene, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Gets the SmartBody scene object.");
	boost::python::def("getVersion", getVersion, "Gets the SmartBody version.");

}
}


#endif
