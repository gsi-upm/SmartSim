#include "vhcl.h"
#include "SBPython.h"
#include "SBPythonClass.h"
#include "controllers/me_ct_scheduler2.h"
#include "controllers/me_ct_gaze.h"
#include "controllers/me_ct_eyelid.h"
#include "controllers/me_ct_curve_writer.hpp"
#include "SBFaceDefinition.h"
#include <sb/nvbg.h>
#include "SBBehavior.h"
#include <sb/SBMotion.h>
#include <sb/SBParseNode.h>

#include <sb/SBScene.h>
#include <sb/SBScript.h>
#include <sb/SBService.h>
#include <sb/SBServiceManager.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBBmlProcessor.h>
#include <sb/SBAnimationState.h>
#include <sb/SBMotionBlendBase.h>
#include <sb/SBAnimationTransition.h>
#include <sb/SBAnimationTransitionRule.h>
#include <sb/SBAnimationStateManager.h>
#include <sb/SBSteerManager.h>
#include <sb/SBPhysicsManager.h>
#include <sb/SBReach.h>
#include <sb/SBReachManager.h>
#include <sb/SBGestureMap.h>
#include <sb/SBGestureMapManager.h>
#include <sb/SBAssetManager.h>
#include <sb/SBJointMap.h>
#include <sb/SBJointMapManager.h>
#include <sb/SBParser.h>
#include <sb/SBBoneBusManager.h>
#include <sb/SBCollisionManager.h>
#include <sb/SBSteerAgent.h>
#include <sb/SBPhoneme.h>
#include <sb/SBPhonemeManager.h>
#include <sb/SBBehaviorSet.h>
#include <sb/SBBehaviorSetManager.h>
#include <sb/SBRetarget.h>
#include <sb/SBRetargetManager.h>
#include <sb/SBEvent.h>
#include <sb/SBSceneListener.h>
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

void pythonFuncsMath()
{

	boost::python::class_<SrVec>("SrVec")
		.def(boost::python::init<>())
		.def(boost::python::init<float, float, float>())
		.def("__str__", &SrVec::toString)
		.def("getData", &SrVec::getData, "gets the x,y,z values")
		.def("setData", &SrVec::setData, "sets the x,y,z values")
		.def("len", &SrVec::norm, "gets the length of the vector")
		.def("normalize", &SrVec::normalize, "normalizes the vector")
		.def("isZero", &SrVec::iszero, "returns True if the vector is zero")
		.def("rotY", &SrVec::rotY, "rotate vector around Y axis (radian)")
		.def("vecAngle", &SrVec::vecAngle, "Returns the angle between v1 and v2 (radian)")
		.def("vecYaw", &SrVec::vecYaw, "Returns Yaw angle on X-Z plane of given vec (radian)")
		.def("addVec", &SrVec::operator+=, "Add the input SrVec to the current SrVec")
		;
	boost::python::def("rotatePoint", rotatePoint, "Rotate a point along a center");
	boost::python::class_<SrMat>("SrMat")
		.def(boost::python::init<>())
		.def("__str__", &SrMat::toString)
		.def("getData", &SrMat::getData, "gets the data in the matrix at r,c")
		.def("setData", &SrMat::setData, "sets the data in the matrix at r,c")
		.def("identity", &SrMat::identity, "sets the data in the matrix to an identity matrix")
		.def("transpose", &SrMat::transpose, "transposes the data in the matrix")
		.def("multVec", &SrMat::multVec, "multiply the matrix with a vector")
		.def("getTranslation", &SrMat::get_translation, "get the translation component of the matrix")
		;

	boost::python::class_<SrQuat>("SrQuat")
		.def(boost::python::init<>())
		.def(boost::python::init<float, float, float, float>())
		.def(boost::python::init<SrVec, float>())
		.def(boost::python::init<SrMat>())
		.def("__str__", &SrQuat::toString)
		.def("getAxis", &SrQuat::axis, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the axis of the quaternion.")
		.def("getAngle", &SrQuat::angle, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the angle of the quaternion.")
		.def("getData", &SrQuat::getData, "gets the data in the quaterion at location indicated by the index w,x,y,z")
		.def("setData", &SrQuat::setData, "sets the data in the quaterion at location indicated by the index w,x,y,z")
		.def("multVec", &SrQuat::multVec, "multiply the quaternion with a vector")
		.def("normalize", &SrQuat::normalize, "normalize the quaternion")
		.def("setAxisAngle", &SrQuat::setAxisAngle, "sets the axis and angle of the quaternion")
		;	
	
	boost::python::class_<SrBox>("SrBox")
		.def(boost::python::init<>())
		.def(boost::python::init<SrVec, SrVec>())
		.def(boost::python::init<SrBox>())
		.def("setMinimum", &SrBox::setMinimum, "sets the minimum values of the box")
		.def("setMaximum", &SrBox::setMaximum,  "sets the maximum values of the box")
		.def("getMinimum", &SrBox::getMinimum, boost::python::return_value_policy<boost::python::return_by_value>(), "gets the minimum values of the box")
		.def("getMaximum", &SrBox::getMaximum, boost::python::return_value_policy<boost::python::return_by_value>(), "gets the maximum values of the box")
		.def("getCenter", &SrBox::getCenter, boost::python::return_value_policy<boost::python::return_by_value>(), "gets center of the box")
		.def("getMinSize", &SrBox::min_size, "gets the minimum dimension of the box")
		.def("getMaxSize", &SrBox::max_size, "gets the maximum dimension of the box")
		.def("getSize", &SrBox::getSize, boost::python::return_value_policy<boost::python::return_by_value>(), "returns the size of each dimension")
		.def("doesContain", &SrBox::contains, "returns the center of the box")
		.def("doesIntersect", &SrBox::intersects, "returns the center of the box")
		.def("getVolume", &SrBox::volume, "returns the volume of the box")
		.def("isEmpty", &SrBox::empty, "returns true if the box is empty")
		;

}
}


#endif
