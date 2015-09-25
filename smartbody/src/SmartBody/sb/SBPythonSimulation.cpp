#include "vhcl.h"
#include "SBPython.h"
#include "SBPythonClass.h"
#include <sb/SBSimulationManager.h>
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationTransitionRule.h>
#include <sb/SBEvent.h>

#include <sr/sr_box.h>
#include <sr/sr_camera.h>
#include <stdlib.h>
#include <sbm/GenericViewer.h>
#include <controllers/me_ct_motion.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

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

void pythonFuncsSimulation()
{

	boost::python::class_<SBSimulationManager>("SBSimulationManager")
		.def("isRunning", &SBSimulationManager::isRunning, "Returns true if the simulation is currently running.")
		.def("isStarted", &SBSimulationManager::isStarted, "Returns true if the simulation has been started.")
		.def("printInfo", &SBSimulationManager::printInfo, "Print all the timing statistics. ")
		.def("printPerf", &SBSimulationManager::printPerf, "Print performance statistics calculated real time given a time period as input.")
		.def("getTime", &SBSimulationManager::getTime, "Returns the current simulation time.")
		.def("getTimeDt", &SBSimulationManager::getTimeDt, "Returns the current simulation dt.")
		.def("setTime", &SBSimulationManager::setTime, "Sets the current simulation time.")
		.def("start", &SBSimulationManager::start, "Start the simulation.")
		.def("stop", &SBSimulationManager::stop, "Stop the simulation.")
		.def("reset", &SBSimulationManager::reset, "Set the clock time to 0. ")
		.def("pause", &SBSimulationManager::pause, "Pause the clock. ")
		.def("resume", &SBSimulationManager::resume, "Resume the clock. ")
		.def("setupTimer", &SBSimulationManager::setupTimer, "Sets up a real time clock that will be used to update the system.")
		.def("setSleepFps", &SBSimulationManager::setSleepFps, "Set the sleep fps. Sleep fps defines the target loop rate. \n Input: sleep fps \n Output: NULL")
		.def("setEvalFps", &SBSimulationManager::setEvalFps, "Set the eval fps. Define the minimum interval to evaluate the frame. \n Input: evaluation fps \n Output: NULL")
		.def("setSimFps", &SBSimulationManager::setSimFps, "Set the simulation fps. Add a fixed increment to output time every update. \n Input: simulation fps \n Output: NULL")
		.def("setSleepDt", &SBSimulationManager::setSleepDt, "Set the sleep dt. \n Input: sleep dt \n Output: NULL")
		.def("setEvalDt", &SBSimulationManager::setEvalDt, "Set the eval dt. \n Input: evaluation dt \n Output: NULL")
		.def("setSimDt", &SBSimulationManager::setSimDt, "Set the sim dt. \n Input: simulation dt \n Output: NULL")
		.def("setSpeed", &SBSimulationManager::setSpeed, "Set the speed for real clock time. Actual time would be real time times speed.")
		;

	boost::python::class_<SBProfiler>("Profiler")
		.def("printLegend", &SBProfiler::printLegend, "Print time profiler legend. ")
		.def("printStats", &SBProfiler::printStats, "Print time profiler statistics. ")
		.def("printReport", &SBProfiler::printReport, "Print time profiler report. ")
		;

	
	boost::python::class_<SrViewer>("Viewer")
		.def("show", &SrViewer::show_viewer, "Shows the viewer.")
		.def("hide", &SrViewer::hide_viewer, "Hides the viewer.")
		;

	boost::python::class_<GenericViewer>("GenericViewer")
		.def("show", &GenericViewer::show_viewer, "Shows the viewer.")
		.def("hide", &GenericViewer::hide_viewer, "Hides the viewer.")
		;

}
}


#endif
