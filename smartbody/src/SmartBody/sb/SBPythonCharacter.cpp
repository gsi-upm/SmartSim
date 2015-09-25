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
#include <sb/SBRigNode.h>

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

void pythonFuncsCharacter()
{

	boost::python::class_<SBPawn, boost::python::bases<SBObject> >("SBPawn")
		.def("getName", &SBPawn::getName, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the name of the pawn..")
		.def("getSkeleton", &SBPawn::getSkeleton, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the skeleton object of the pawn.")
		.def("setSkeleton", &SBPawn::setSkeleton, "Attaches the skeleton to the character.")
		.def("setName", &SBPawn::setName, "Sets or changes the name of the character.")
		.def("getPosition", &SBPawn::getPosition, "Returns the current position of the character's world offset.")
		.def("getOrientation", &SBPawn::getOrientation, "Returns the current orientation of the character's world offset.")
		.def("setPosition", &SBPawn::setPosition, "Sets the current position of the character's world offset.")
		.def("setPositionSmooth", &SBPawn::setPositionSmooth, "Sets the current position of the character's world offset. The character will be translated smoothly overtime to avoid popping.")
		.def("setOrientation", &SBPawn::setOrientation, "Set the current orientation of the character's world offset.")
		.def("setHPR", &SBPawn::setHPR, "Sets the heading, pitch and roll of the character's world offset.")
		.def("getHPR", &SBPawn::getHPR, "Gets the heading, pitch and roll of the character's world offset.")
		.def("setHPRSmooth", &SBPawn::setHPRSmooth, "Sets the heading, pitch and roll of the character's world offset. The character will be rotated smoothly overtime to avoid popping.")
		.def("getBoundingBox", &SBCharacter::getBoundingBox, "Gets the boundary dimensions of the character.")
		.def("createMeshFromCollisionSurface", &SBPawn::createMeshFromCollisionSurface, "Creates a mesh based on the current collision surface.")
	;

	boost::python::class_<SrCamera, boost::python::bases<SBPawn> >("Camera")
		.def("print", &SrCamera::print, "Shows all the camera statistics. ")
		.def("reset", &SrCamera::reset, "Reset camera with camera eye (0 166 185), camera center (0 92 0). ")
		.def("setEye", &SrCamera::setEye, "Set camera eye position. \n Input: camera eye position(should only have three number in the input list) e.g. [0, 0, 0] \n Output: NULL")
		.def("getEye", &SrCamera::getEye, "Get camera eye position.")
		.def("setCenter", &SrCamera::setCenter, "Set camera center. \n Input: camera center position(should only have three number in the input list) e.g. [0, 0, 0] \n Output: NULL")
		.def("getCenter", &SrCamera::getCenter, "Get camera center.")
		.def("setScale", &SrCamera::setScale, "Set camera scale. \n camera scale: NULL \n Output: NULL")
		.def("getScale", &SrCamera::getScale, "Get camera scale.")
		.def("setUpVector", &SrCamera::setUpVector, "Set camera up vector.")
		.def("getUpVector", &SrCamera::getUpVector, "Returns the camera up vector.")
		.def("setFov", &SrCamera::setFov, "Set's the camera's field of view.")
		.def("getFov", &SrCamera::getFov, "Get's the camera's field of view.")
		.def("setNearPlane", &SrCamera::setNearPlane, "Set's the camera's near plane.")
		.def("getNearPlane", &SrCamera::getNearPlane, "Get's the camera's near plane.")
		.def("setFarPlane", &SrCamera::setFarPlane, "Set's the camera's far plane.")
		.def("getFarPlane", &SrCamera::getFarPlane, "Get's the camera's far plane.")
		.def("setAspectRatio", &SrCamera::setAspectRatio, "Set's the camera's aspect ratio.")
		.def("getAspectRatio", &SrCamera::getAspectRatio, "Get's the camera's aspect ratio.")
		.def("setCameraParameterSmooth", &SrCamera::setCameraParameterSmooth, "Smoothly interpolate the current camera to target camera within given time.")		
		;

	boost::python::class_<SBCharacter, boost::python::bases<SBPawn, SBObject> >("SBCharacter")
		//.def(boost::python::init<std::string, std::string>())
		.def("addJointTrajectoryConstraint", &SBCharacter::addJointTrajectoryConstraint, "Add IK constraint to a specific end effector joint to be enforced in the post-processing.")
		.def("isAutomaticPruning", &SBCharacter::isAutomaticPruning, "Returns true if the character's cotnroller are automatically pruned.")
		.def("setAutomaticPruning", &SBCharacter::setAutomaticPruning, "Toggles the automatic pruning mechanism on or off.")
		.def("pruneControllers", &SBCharacter::pruneControllers, "Prunes the controller tree.")
		.def("startMotionRecord", &SBCharacter::startMotionRecord, "Start recording the output motion of this character.")
		.def("stopMotionRecord", &SBCharacter::stopMotionRecord, "Stop the recording and output the motion to a file given the type, type can be one of the following: skm, bvh.")
		.def("writeMotionRecord", &SBCharacter::writeMotionRecord, "Wrtie the recorded motion to a file given the type, type can be one of the following: skm, bvh.")
		.def("setSoftEyes", &SBCharacter::setSoftEyes, "Sets the soft eyes feature.")
		.def("isSoftEyes", &SBCharacter::isSoftEyes, "Returns the value of the soft eyes feature.")
		.def("setUseVisemeCurves", &SBCharacter::setUseVisemeCurves, "Use curves when interpreting visemes.")
		.def("isUseVisemeCurves", &SBCharacter::isUseVisemeCurves, "Are curves used when interpreting visemes.")
		.def("setVisemeTimeOffset", &SBCharacter::setVisemeTimeOffset, "Set the time delay for viseme curve mode.")
		.def("getVisemeTimeOffset", &SBCharacter::getVisemeTimeOffset, "Get the time delay for viseme curve mode.")
		.def("getNumControllers", &SBCharacter::getNumControllers, "Returns number of top level controllers inside this character.")
		.def("addController", &SBCharacter::addController, "Adds a controller to the character.")
		.def("removeController", &SBCharacter::removeController, "Removes a controller from the character.")
		.def("createStandardControllers", &SBCharacter::createStandardControllers, "Creates a full set of controllers for the character.")		
		.def("createMinimalControllers", &SBCharacter::createMinimalControllers, "Creates a basic set of controllers for posture, animation, gaze, head and face control.")		
		//.def("getNumVisemes", &SBCharacter::getNumVisemes, "Returns the number of visemes.")
		.def("getControllerByIndex", &SBCharacter::getControllerByIndex, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the ith controller.")
		.def("getControllerByName", &SBCharacter::getControllerByName, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the controller with the given name.")
		.def("getControllerNames", &SBCharacter::getControllerNames, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the controller name vector.")
		.def("setVoice", &SBCharacter::setVoice, "Sets the voice type: remote, audiofile, text or none (use \"\").")
		.def("setVoiceCode", &SBCharacter::setVoiceCode, "Sets the voice code. For audiofile type, this is a path.")
		.def("setVoiceBackup", &SBCharacter::setVoiceBackup, "Sets the voice backup type: remote, audiofile, text or none (use \"\").")
		.def("setVoiceBackupCode", &SBCharacter::setVoiceBackupCode, "Sets the voice backup code. For audiofile type, this is a path.")
		.def("getVoice", &SBCharacter::getVoice, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the voice of the character..")
		.def("getVoiceCode", &SBCharacter::getVoiceBackupCode, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the voice code. For audiofile type, this is a path.")
		.def("getVoiceBackup", &SBCharacter::getVoiceBackup, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the voice backup type: remote, audiofile, text or none (use \"\").")
		.def("setFaceDefinition", &SBCharacter::setFaceDefinition, "Sets face definition (visemes, action units) for a character.")
		.def("getFaceDefinition", &SBCharacter::getFaceDefinition, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Gets face definition (visemes, action units) for a character.")
		.def("getHeight", &SBCharacter::getHeight, "Gets the height of the character.")
		.def("getNumBehaviors", &SBCharacter::getNumBehaviors, "Returns the number of behaviors of the character.")
		.def("getBehavior", &SBCharacter::getBehavior, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the ith behavior of the character.")
		//.def("getFaceDefinition", &SBCharacter::getFaceDefinition, "Gets face definition (visemes, action units) for a character.")
		.def("setNvbg", &SBCharacter::setNvbg, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Sets the NVBG handler for this character.")
		.def("getNvbg", &SBCharacter::getNvbg, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Gets the NVBG handler for this character.")
		.def("setMiniBrain", &SBCharacter::setMiniBrain, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Sets the mini brain handler for this character.")
		.def("getMiniBrain", &SBCharacter::getMiniBrain, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Gets the mini brain handler for this character.")
		.def("interruptFace", &SBCharacter::interruptFace, "Stops any action units or lip syncing movements on a character by fading them out over a period of seconds specified.")
		.def("hasSpeechBehavior", &SBCharacter::hasSpeechBehavior, "Returns speech behavior msg id if any, otherwise return empty string.")
		.def("createBlendShapeChannel", &SBCharacter::createBlendShapeChannel, "Create a blend shape channel given its name.")
		.def("setDeformableMeshName", &SBCharacter::setDeformableMeshName, "Sets the name of the deformable mesh. Same as setting the deformableMesh attribute.")
		.def("setMotionGraph", &SBCharacter::setMotionGraph, "Sets the motion graph for this character")
		.def("startMotionGraph", &SBCharacter::startMotionGraph, "Start running the motion graph given a starting node name")
		.def("startMotionGraphRandomWalk", &SBCharacter::startMotionGraphRandomWalk, "Start running the motion graph from a random node.")
		.def("startMotionGraphWithPath", &SBCharacter::startMotionGraphWithPath, "Start running the motion graph and synthesize a graph walk based on input path.")
		.def("updateFaceDefinition", &SBCharacter::updateFaceDefinition, "Updates the face definition.")
		.def("updateDefaultFacePose", &SBCharacter::updateDefaultFacePose, "Updates the default face pose.")
		.def("getReach", &SBCharacter::getReach, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Get current reach instance used by the character.")
		.def("setReach", &SBCharacter::setReach, "Get current reach instance used by the character.")
		.def("getReachAttachedPawnName", &SBCharacter::getReachAttachedPawnName, "Get the name of currently attached pawn in the reach controller.")
		.def("addParserListener", &SBCharacter::addParserListener, "Adds a parser listener for processing BML based on an utterance.")
		.def("removeParserListener", &SBCharacter::removeParserListener, "Rrmoves the parser listener for processing BML based on an utterance.")
		.def("getParserListener", &SBCharacter::getParserListener, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the parser listener for processing BML based on an utterance.")
		;


		boost::python::class_<SBSetDrivenKeyNode>("SBSetDrivenKeyNode")
		.def(boost::python::init<>())
		.def("setRange", &SBSetDrivenKeyNode::setRange, "Sets the range of this linear mapping.")
		.def("getValue", &SBSetDrivenKeyNode::getValue, "Gets the value based on the input.")
		;

		boost::python::class_<SBSetDrivenKeySequenceNode>("SBSetDrivenKeySequenceNode")
		.def(boost::python::init<>())
		.def("setRange", &SBSetDrivenKeySequenceNode::setRange, "Sets the range of this linear mapping.")
		.def("getValue", &SBSetDrivenKeySequenceNode::getValue, "Gets the value based on the input.")
		;


}
}


#endif
