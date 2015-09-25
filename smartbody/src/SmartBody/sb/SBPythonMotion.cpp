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
#include <sb/SBMotionGraph.h>
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

void pythonFuncsMotion()
{

	boost::python::class_<SBMotion, boost::python::bases<SBObject> >("SBMotion")
		//.def(boost::python::init<std::string>())
		.def("addChannel", &SBMotion::addChannel, "Adds a channel to the motion with a given type (XPos, YPos, ZPos, Quat)")
		.def("addFrame", &SBMotion::addFrame, "Adds a frame to the motion a given time.")
		.def("addKeyFrameChannel", &SBMotion::addKeyFrameChannel, "Adds a time/value for a particular translation or other channel for non-baked animation.")
		.def("addKeyFrameQuat", &SBMotion::addKeyFrameQuat, "Adds a time/value for a particular quaternion channel for non-baked animation.")
		.def("bakeFrames", &SBMotion::bakeFrames, "Bakes frames added from addkeyFrameChannel and addKeyFrameQuat.")
		.def("getMotionFileName", &SBMotion::getMotionFileName, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the motion file name. \n Input: NULL \n Output: motion file name")
		.def("getNumFrames", &SBMotion::getNumFrames, "Returns the number of frames inside this motion. \n Input: NULL \n Output: number of frames in the motion")
		.def("getFrameData", &SBMotion::getFrameData, "Returns the frame data given frame index. \n Input: frame index \n Output: a list of frame data")
		.def("getFrameSize", &SBMotion::getFrameSize, "Returns the frame size. \n Input: NULL \n Output: frame size (how many data does one frame include)")
		.def("getNumChannels", &SBMotion::getNumChannels, "Returns the number of channels for this motion. \n Input: NULL \n Output: number of channels for this motion")
		.def("getChannels", &SBMotion::getChannels, "Returns the channels + type inside the skeleton. \n Input: NULL \n Output: channel name and type")
		.def("checkSkeleton", &SBMotion::checkSkeleton, "Print out all the motion channels and compare it with the given skeleton channels. Mark '+' in front if the skeleton channel exists in the motion. \n Input: skeleton file name \n Output: NULL")
		.def("connect", &SBMotion::connect, "Connect current motion to a skeleton object so the channels inside the motion are mapped to the channels inside skeleton. \n Input: Skeleton Object \n Output: NULL")
		.def("disconnect", &SBMotion::disconnect, "Disconnect current motion with current skeleton object. ")
		.def("mirror", &SBMotion::mirror, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Mirrors the motion.")
		.def("getOffset", &SBMotion::getOffset, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Offset the current motion to input base motion.")
		.def("mirrorChildren", &SBMotion::mirrorChildren, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Mirrors only the child joints of a specific parent joint.")
		.def("retarget", &SBMotion::retarget, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Retarget the motion to a different skeleton.")
		.def("footSkateCleanUp", &SBMotion::footSkateCleanUp, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Retarget the motion to a different skeleton.")
		.def("constrain", &SBMotion::constrain, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Constrain the retargeted motion to based on the source skeleton and motion.")
		.def("smoothCycle", &SBMotion::smoothCycle, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Build the smooth cycle the motion.")
		.def("alignToEnd", &SBMotion::alignToEnd, "Cut the first x number of frames and stitch them to the end. x is the input number")
		.def("alignToBegin", &SBMotion::alignToBegin, "Cut the last x number of frames and stitch them to the begin. x is the input number")
		.def("duplicateCycle", &SBMotion::duplicateCycle, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Duplicate motion by x amount of cycles.")
		.def("getJointSpeed", &SBMotion::getJointSpeed, "Get the accumulative joint speed. \n Input: SBJoint, start time, end time \n Output: joint speed(unit: same with the skeleton)")
		.def("getJointSpeedAxis", &SBMotion::getJointSpeedAxis, "Get the accumulative joint speed of a given axis: X, Y or Z. \n Input: SBJoint, axis, start time, end time \n Output: joint speed(unit: same with the skeleton)")
		.def("getJointAngularSpeed", &SBMotion::getJointAngularSpeed, "Get the joint accumulative angular speed. \n Input: SBJoint, start time, end time \n Output: joint angular speed(unit: degree/sec)")		
		.def("getJointAngularSpeedAxis", &SBMotion::getJointAngularSpeedAxis, "Get the joint accumulative angular speed of a given axis: X, Y or Z. \n Input: SBJoint, axis, start time, end time \n Output: joint angular speed(unit: degree/sec)")		
		.def("getJointTransition", &SBMotion::getJointTransition, "Get the joint transition vector. \n Input: SBJoint, start time, end time \n Output: joint transition vector containing x, y, z value (unit: same with the skeleton)")		
		.def("getJointPosition", &SBMotion::getJointPosition, "Get the joint position. \n Input: SBJoint, time \n Output: joint position containing x, y, z value (unit: same with the skeleton)")		
		.def("translate", &SBMotion::translate, "Translates the base joint name by x,y,z values.")		
		.def("rotate", &SBMotion::rotate, "Rotates the base joint name by x,y,z axis.")			
		.def("scale", &SBMotion::scale, "Scales all translations in skeleton by scale factor.")		
		.def("trim", &SBMotion::trim, "Trims the starting and ending frames in the motion.")	
		.def("downsample", &SBMotion::downsample, "Downsample the number of frames by factor N.")	
		.def("removeMotionChannelsByEndJoints", &SBMotion::removeMotionChannelsByEndJoints, "Remove all the motion channels corresponding to the descendents of end joints. \n Input : skeleton name, end joint names")	
		.def("pertainMotionChannelsByEndJoints", &SBMotion::pertainMotionChannelsByEndJoints, "Pertain all the motion channels corresponding to the descendents of end joints. \n Input : skeleton name, end joint names")	
		.def("removeMotionChannels", &SBMotion::removeMotionChannels, "Remove the motion channels based on input joint names. \n Input : names of channels to be removed")	
		.def("saveToSkm", &SBMotion::saveToSkm, "Saves the file in .skm format to a given file name.")	
		.def("saveToBVH", &SBMotion::saveToBVH, "Saves the file in .bvh format to a given file name and skeleton name.")	
		.def("saveToSkmb", &SBMotion::saveToSkb, "Saves the file in .skmb (binary) format to a given file name.")	
		.def("saveToSkmByFrames", &SBMotion::saveToSkmByFrames, "Saves the file in .skm format to a given file name, start and end frame.")	
		.def("getTimeStart", &SBMotion::getTimeStart, "Returns the start time of the motion.")
		.def("getTimeReady", &SBMotion::getTimeReady, "Returns the ready time of the motion.")
		.def("getTimeStrokeStart", &SBMotion::getTimeStrokeStart, "Returns the stroke start time of the motion.")
		.def("getTimeStroke", &SBMotion::getTimeStroke, "Returns the stroke time of the motion.")
		.def("getTimeStrokeEnd", &SBMotion::getTimeStrokeEnd, "Returns the stroke end time of the motion.")
		.def("getTimeRelax", &SBMotion::getTimeRelax, "Returns the relax time of the motion.")
		.def("getTimeStop", &SBMotion::getTimeStop, "Returns the stop time of the motion.")	
		.def("getDuration", &SBMotion::getDuration, "Return the duration of the motion")
		.def("setSyncPoint", &SBMotion::setSyncPoint, "Set the gestrure syncpoint for this motion")
		.def("addEvent", &SBMotion::addEvent, "Adds an event associated with this motion that will be triggered at the given time. The last paramter determines if the event will be triggered only once, or every time the motion is looped.")
		.def("addMetaData", &SBMotion::addMetaData, "Add a tagged metadata as string to the motion.")
		.def("buildJointTrajectory", &SBMotion::buildJointTrajectory, "Build the positional trajectory of a given joint in this motion.")
		.def("removeMetaData", &SBMotion::removeMetaData, "Remove a tagged metadata from the motion.")
		.def("getMetaDataString", &SBMotion::getMetaDataString, "Get the first metadata based on tag name")
		.def("getMetaDataDouble", &SBMotion::getMetaDataDouble, "Get the first metadata based on tag name")
		.def("getMetaDataTags", &SBMotion::getMetaDataTags, "Get all tag names in the metadata map.")
		.def("getMotionSkeletonName", &SBMotion::getMotionSkeletonName, boost::python::return_value_policy<boost::python::return_by_value>(),"Get the skeleton associated with this motion.")
		.def("setMotionSkeletonName", &SBMotion::setMotionSkeletonName, "Set the skeleton associated with this motion.")
		.def("setEmptyMotion", &SBMotion::setEmptyMotion, "Set this motion to be empty given duration and number of frames")
		.def("addSimilarPose", &SBMotion::addSimilarPose, "Marks another motion as being similar to this motion. This will be used by the gesture system to retarget gestures from one poses to a similar one.")
		.def("removeSimilarPose", &SBMotion::removeSimilarPose, "Removes a pose as being similar to this motion. This will be used by the gesture system to retarget gestures from one poses to a similar one.")
		.def("getSimilarPoses", &SBMotion::getSimilarPoses, "Retrieves all the similar pose.")
		.def("unrollPrerotation", &SBMotion::unrollPrerotation, "Remove pre-rotation from joints.")		
		.def("addTemporalRotationOffset", &SBMotion::addTemporalRotationOffset, "Add a rotation offset on a joint.")		
		;

	boost::python::class_<SBMotionNode>("SBMotionNode")
		.def("getName", &SBMotionNode::getName, "Get the node name.")
		.def("getAnimBlend", &SBMotionNode::getAnimBlend, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Get the animation blend associated with this node.")
		;

	boost::python::class_<SBMotionTransitionEdge>("SBMotionTransitionEdge")
		.def("getName", &SBMotionTransitionEdge::getName, "Get the transition edge name.")
		.def("getSrcNode", &SBMotionTransitionEdge::getSrcNode, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Get the source motion node.")
		.def("getTgtNode", &SBMotionTransitionEdge::getTgtNode, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Get the target motion node.")
		;

	boost::python::class_<SBMotionGraph>("SBMotionGraph")
		//.def(boost::python::init<std::string>())
		.def("addMotionNodeFromBlend", &SBMotionGraph::addMotionNodeFromBlend, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Adds a motion node to the motion graph.")
		.def("addMotionEdge", &SBMotionGraph::addMotionEdge, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Adds a motion edge to the motion graph.")
		.def("getMotionNode", &SBMotionGraph::getMotionNode, boost::python::return_value_policy<boost::python::reference_existing_object>(), "get the motion node based on its name.")
		.def("getMotionEdge", &SBMotionGraph::getMotionEdge, boost::python::return_value_policy<boost::python::reference_existing_object>(), "get the motion edge based on its name.")
		.def("buildAutomaticMotionGraph", &SBMotionGraph::buildAutomaticMotionGraph, "build the motion graph automatically from a set of motions.")
		;
	boost::python::class_<SBMotionGraphManager>("SBMotionGraphManager")
		.def("createMotionGraph", &SBMotionGraphManager::createMotionGraph, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Create a motion graph.")
		.def("getMotionGraph", &SBMotionGraphManager::getMotionGraph, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Get the motion graph based on its name.")
		.def("getMotionGraphNames", &SBMotionGraphManager::getMotionGraphNames, "Get the names of all motion graphs.")
		;	
}
}


#endif
