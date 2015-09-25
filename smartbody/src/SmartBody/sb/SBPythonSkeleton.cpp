#include "vhcl.h"
#include "SBPython.h"
#include "SBPythonClass.h"
#include <sb/SBJoint.h>
#include <sb/SBSkeleton.h>
#include <sb/SBScript.h>
#include <sb/SBEvent.h>
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationTransition.h>
#include <sb/SBAnimationTransitionRule.h>
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

void pythonFuncsSkeleton()
{

	boost::python::class_<SBSkeleton, boost::python::bases<SBObject> >("SBSkeleton")
	//	.def(boost::python::init<>())
		.def(boost::python::init<std::string>())
		.def("load", &SBSkeleton::load, "Loads the skeleton definition from the given skeleton name.")
		.def("save", &SBSkeleton::save, "Saves the skeleton definition with the given skeleton name.")
		.def("loadFromString",  &SBSkeleton::loadFromString, "Loads the skeleton definition from a string.")
		.def("rescale", &SBSkeleton::rescale, "Adjust the skeleton size to scale ratio")
		.def("getName", &SBSkeleton::getName, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the name of the skeleton.")
		.def("getFileName", &SBSkeleton::getFileName, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the original filename where the skeleton was loaded from.")
		.def("getNumJoints", &SBSkeleton::getNumJoints, "Returns the number of joints for this skeleton.")
		.def("getJointNames", &SBSkeleton::getJointNames, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the joint names for this skeleton.")
		.def("getJointByName", &SBSkeleton::getJointByName, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the joint of a given name.")
		.def("getJoint", &SBSkeleton::getJoint, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the joint with a given index.")
		.def("getNumChannels", &SBSkeleton::getNumChannels, "Returns the number of the channels inside the skeleton.")
		.def("getChannelType", &SBSkeleton::getChannelType, "Returns the type of the channel of a given index.")
		.def("getChannelSize", &SBSkeleton::getChannelSize, "Returns the size of the channel given index.")
		.def("createSkelWithoutPreRot", &SBSkeleton::createSkelWithoutPreRot, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Create a new standard T-pose skel from source but without pre-rotations")	
		.def("orientJointsLocalAxesToWorld", &SBSkeleton::orientJointsLocalAxesToWorld, "Orient skeleton joints local axes to match world coordinate axes (Y-up Z-front)")
		.def("createJoint", &SBSkeleton::createJoint, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Adds a joint to the skeleton. First parameter is the joint, second is the joints parent (null for root).")
		.def("createStaticJoint", &SBSkeleton::createStaticJoint, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Adds a joint to the skeleton. First parameter is the joint, second is the joints parent (null for root).")
		.def("createChannel", &SBSkeleton::createChannel, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Adds a channel to the skeleton. First parameter is the joint, second is the channels parent (null for root).")		
		.def("update", &SBSkeleton::update, "Creates channels from the joints. Used after all the joints have been created.")
		; 

	boost::python::class_<SBJoint, boost::python::bases<SBObject> >("SBJoint")
		.def(boost::python::init<>())
		.def("setName", &SBJoint::setName, "Set the name of the joint.")
		.def("getName", &SBJoint::getName, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the name of the joint.")
		.def("getParent", &SBJoint::getParent, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the parent joint.")
		.def("setParent", &SBJoint::setParent, "Sets the parent joint.")
		.def("getNumChildren", &SBJoint::getNumChildren, "Returns the number of child joints.")
		.def("getChild", &SBJoint::getChild, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the child joint with a given index.")
		.def("getSkeleton", &SBJoint::getSkeleton, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the skeleton.")
		.def("setSkeleton", &SBJoint::setSkeleton, "Sets the skeleton.")
		.def("getOffset", &SBJoint::getOffset, "Returns the offset of the joint from the parent joint.") 
		.def("setOffset", &SBJoint::setOffset, "Sets the offset of the joint from the parent joint.")
		.def("getIndex", &SBJoint::getIndex, "Returns the index of the joint in current skeleton.")
		.def("getPosition", &SBJoint::getPosition, "Returns the current position of the joint in global coordinates.")
		.def("getQuat", &SBJoint::getQuaternion, "Returns the current quaterion of the joint in global coordinates.")
		.def("getMatrixGlobal", &SBJoint::getMatrixGlobal, "Returns the matrix of the joint in global coordinates.")
		.def("getMatrixLocal", &SBJoint::getMatrixLocal, "Returns the matrix of the joint in local coordinates.")
		.def("addChild", &SBJoint::addChild, "Add a child joint to current joint.")
		.def("setUseRotation", &SBJoint::setUseRotation, "Allows the joint to use rotation channels.")	
		.def("getUseRotation", &SBJoint::isUseRotation, "Determines if the joint uses rotation channels.")	
		.def("setUsePosition", &SBJoint::setUsePosition, "Allows the joint to use position channels.")	
		.def("isUsePosition", &SBJoint::isUsePosition, "Determines if the joint uses position channels.")	
		.def("getMass", &SBJoint::getMass, "Gets the mass of the joint.")
		.def("setMass", &SBJoint::setMass, "Sets the mass of the joint.")
		.def("getPrerotation", &SBJoint::getPrerotation, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the prerotation values for the joint.")
		.def("setPrerotation", &SBJoint::setPrerotation, "Sets the prerotation values for the joint.")
		.def("getPostrotation", &SBJoint::getPostrotation, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the postrotation values for the joint.")
		.def("setPostrotation", &SBJoint::setPostrotation, "Sets the postrotation values for the joint.")		
		.def("getJointType", &SBJoint::getJointType, "Returns the type of the joint.");
		

}
}


#endif
