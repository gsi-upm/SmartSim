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
#include <sb/SBRealtimeManager.h>
#include <sb/SBFaceShiftManager.h>
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
#include <sb/SBVHMsgManager.h>
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
#include <sb/SBNavigationMesh.h>
#include <sb/SBSceneListener.h>
#include <sb/SBNavigationMeshManager.h>
#include <sb/SBHandConfigurationManager.h>
#include <sb/SBDebuggerServer.h>
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


void pythonFuncsScene()
{
		boost::python::class_<SBNavigationMesh>("SBNavigationMesh")		
		.def("buildNavigationMesh", &SBNavigationMesh::buildNavigationMesh, "Build the navigation mesh based on input mesh name")		
		.def("findPath", &SBNavigationMesh::findPath, boost::python::return_value_policy<boost::python::return_by_value>(), "Find a path from start position to end position.")
		.def("queryMeshPointByRayCast", &SBNavigationMesh::queryMeshPointByRayCast, boost::python::return_value_policy<boost::python::return_by_value>(), "Query the position on navigation mesh by ray casting.")
		.def("queryFloorHeight", &SBNavigationMesh::queryFloorHeight, boost::python::return_value_policy<boost::python::return_by_value>(), "Query the height of floor on navigation mesh given a position and search radius.")
		;

		boost::python::class_<SBScene, boost::python::bases<SBObject> >("SBScene")
		.def("update", &SBScene::update, "Updates the simulation at the given time.")
		.def("setProcessId", &SBScene::setProcessId, "Sets the process id of the SmartBody instance.")
		.def("getProcessId", &SBScene::getProcessId,  boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the process id of the SmartBody instance.")
		.def("getStringFromObject", &SBScene::getStringFromObject, "Retrieves a string representing a SmartBody object's name given the object.")
		.def("getObjectFromString", &SBScene::getObjectFromString, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Retrieves a SmartBody object based on its name as a string.")
		.def("createCharacter", &SBScene::createCharacter, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates a new character given character name. \n Input: character name \nOutput: character object")
		.def("copyCharacter", &SBScene::copyCharacter, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Create a new character copied from an existing character. \n Input: original character name, copy character name \nOutput: character object")
		.def("createPawn", &SBScene::createPawn, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates a new pawn.")
		.def("copyPawn", &SBScene::copyPawn, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Create a new pawn copied from an existing pawn. \n Input: original pawn name, copy pawn name \nOutput: pawn object")
		.def("createFaceDefinition", &SBScene::createFaceDefinition, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates a new face definition with a given name.")
		.def("getFaceDefinition", &SBScene::getFaceDefinition, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns a face definition with a given name.")
		.def("getNumFaceDefinitions", &SBScene::getNumFaceDefinitions, "Returns the number of face definitions.")
		.def("getFaceDefinitionNames", &SBScene::getFaceDefinitionNames, "Return a list of all face definition names. \n Input: NULL \nOutput: list of face definition names.")
		.def("removeCharacter", &SBScene::removeCharacter, "Remove the character given its name. \n Input: character name \n Output: NULL")
		.def("removeAllCharacters", &SBScene::removeAllCharacters, "Removes all the characters.")
		.def("removePawn", &SBScene::removePawn, "Remove the pawn given its name. \n Input: pawn name \n Output: NULL")
		.def("removeAllPawns", &SBScene::removeAllPawns, "Removes all the pawns.")
		.def("getNumPawns", &SBScene::getNumPawns, "Returns the number of pawns.\n Input: NULL \nOutput: number of pawns.")
		.def("getNumCharacters", &SBScene::getNumCharacters, "Returns the number of characters.\n Input: NULL \nOutput: number of characters.")
		.def("getPawn", &SBScene::getPawn, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the pawn object given its name. \n Input: pawn name \nOutput: pawn object")
		.def("getCharacter", &SBScene::getCharacter, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the character object given its name. \n Input: character name \nOutput: character object")
		.def("getPawnNames", &SBScene::getPawnNames, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns a list of all character names.\n Input: NULL \nOutput: list of pawn names")
		.def("getCharacterNames", &SBScene::getCharacterNames, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns a list of all character names.\n Input: NULL \nOutput: list of character names")
		.def("getEventHandlerNames", &SBScene::getEventHandlerNames, "Returns a list of names of all event handlers.\n Input: NULL \nOutput: list of event handler names")
		.def("setMediaPath",&SBScene::setMediaPath, "Sets the media path.")
		.def("getMediaPath",&SBScene::getMediaPath, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the media path.")
		.def("setDefaultCharacter", &SBScene::setDefaultCharacter, "Sets the default character.")
		.def("setDefaultRecipient", &SBScene::setDefaultRecipient, "Sets the default recipient.")
		.def("addSkeletonDefinition", &SBScene::addSkeletonDefinition, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Add an new empty skeleton into system. \n Input: skeleton name \nOutput: skeleton object")
		.def("removeSkeletonDefinition", &SBScene::removeSkeletonDefinition, "Removes a skeleton with a given name.")
		.def("createMotion", &SBScene::createMotion, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Add an new empty motion into system. \n Input: motion name\n \nOutput: motion object")
		.def("addScript", &SBScene::addScript, "Adds a script to the scene.")
		.def("removeScript", &SBScene::removeScript, "Returns the number of scripts.")
		.def("getNumScripts", &SBScene::getNumScripts, "Returns the number of scripts.")
		.def("getScriptNames", &SBScene::getScriptNames, "Returns the names of all the scripts.")
		.def("getScript", &SBScene::getScript, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns a script.")
		.def("getScale", &SBScene::getScale, "Returns the scene scale in meters (default is centimeters .01)")
		.def("setScale", &SBScene::setScale, "Sets the scene scale in meters.")
		.def("isRemoteMode", &SBScene::isRemoteMode, "Returns the boolean indicating whether scene is in remote mode.")
		.def("setRemoteMode", &SBScene::setRemoteMode, "Sets the scene remote mode.")
		.def("removePendingCommands", &SBScene::removePendingCommands, "Removes any commands stored in SmartBody awaiting execution.")
		.def("addSceneListener", &SBScene::addSceneListener, "Adds a listener for scene events.")
		.def("removeSceneListener", &SBScene::removeSceneListener, "Removes a scene listener.")
		.def("removeSceneListener", &SBScene::addSceneListener, "Removes a listener for scene events.")
		.def("getSceneListeners", &SBScene::getSceneListeners, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Gets all the scene listeners for scene events.")
		.def("removeAllSceneListeners", &SBScene::removeAllSceneListeners, "Removes all scene listeners.")
		.def("save", &SBScene::save, "Saves the SmartBody configuration. Returns a string containing Python commands representing the configuration.")
		.def("exportScene", &SBScene::exportScene, "Saves the entire SmartBody configuration, including assets paths as a python script into a given file location.")
		#if !defined(__FLASHPLAYER__)
		.def("exportScenePackage", &SBScene::exportScenePackage, "Saves the entire SmartBody configuration and all assets into a given folder, or into a .zip archive")
		.def("exportCharacter", &SBScene::exportCharacter, "Saves the character's skeleton, skinned mesh and all other assets into a given folder in Collada format. ")
		#endif
		.def("setNavigationMesh", &SBScene::setNavigationMesh, "Set the navigation mesh used for steering in the scene.\n Input : Navigation mesh name")
		.def("getNavigationMesh", &SBScene::getNavigationMesh, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Get the navigation mesh in the scene")
		.def("startFileLogging", &SBScene::startFileLogging, "Starts logging SmartBody messages to a given log file.")
		.def("stopFileLogging", &SBScene::stopFileLogging, "Stops logging SmartBody messages to the given log file.")

		// cameras
		.def("createCamera", &SBScene::createCamera, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates a camera with a given name and returns it.")
		.def("removeCamera", &SBScene::removeCamera, "Removes a camera.")
		.def("setActiveCamera", &SBScene::setActiveCamera, "Sets the camera to be used in the viewer.")
		.def("getActiveCamera", &SBScene::getActiveCamera, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Gets the camera currently being used in the viewer.")
		.def("getCamera", &SBScene::getCamera, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns a camera by name.")
		.def("getNumCameras", &SBScene::getNumCameras, "Returns the number of cameras available.")
		.def("getCameraNames", &SBScene::getCameraNames, "Gets the names of all the cameras available.")

		// command processing
		.def("command", &SBScene::command, "Runs an old-Style SmartBody command.")
		.def("commandAt", &SBScene::commandAt, "Runs an old-style SmartBody command at a set time in the future.")
		.def("vhmsg", &SBScene::sendVHMsg, "Sends a virtual human message.")
		.def("vhmsg2", &SBScene::sendVHMsg2, "Sends a virtual human message.")
		.def("run", &SBScene::runScript, "Runs a python script.")
		// managers
		.def("getEventManager", &SBScene::getEventManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the event manager.")
		.def("getSimulationManager", &SBScene::getSimulationManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the simulation manager object.")
		.def("getProfiler", &SBScene::getProfiler, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the  profiler object.")
		.def("getBmlProcessor", &SBScene::getBmlProcessor, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the bml processor object.")
		.def("getStateManager", &SBScene::getBlendManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the state manager object.")
		.def("getBlendManager", &SBScene::getBlendManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the state manager object.")
		.def("getReachManager", &SBScene::getReachManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the reach manager object.")
		.def("getSteerManager", &SBScene::getSteerManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the steer manager object.")
		.def("getServiceManager", &SBScene::getServiceManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the service manager object.")
		.def("getPhysicsManager", &SBScene::getPhysicsManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the physics manager object.")
		.def("getBoneBusManager", &SBScene::getBoneBusManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the Bone Bus manager object.")
		.def("getGestureMapManager", &SBScene::getGestureMapManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the gesture map manager object.")
		.def("getJointMapManager", &SBScene::getJointMapManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the joint mapping manager object.")
		.def("getCollisionManager", &SBScene::getCollisionManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the collision manager object.")
		.def("getDiphoneManager", &SBScene::getDiphoneManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the diphone manager object.")
		.def("getBehaviorSetManager", &SBScene::getBehaviorSetManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the behavior set manager.")
		.def("getRetargetManager", &SBScene::getRetargetManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the retarget manager.")
		.def("getAssetManager", &SBScene::getAssetManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the asset manager.")
		.def("getVHMsgManager", &SBScene::getVHMsgManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the VH message manager.")
		.def("getNavigationMeshManager", &SBScene::getNavigationMeshManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the navigation mesh manager.")
		.def("getMotionGraphManager", &SBScene::getMotionGraphManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the motion graph manager.")
		.def("getHandConfigurationManager", &SBScene::getHandConfigurationManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the hand configuration manager.")
		.def("getRealtimeManager", &SBScene::getRealtimeManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the real time data manager.")
		.def("getFaceShiftManager", &SBScene::getFaceShiftManager, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the FaceShift manager.")
		.def("getDebuggerServer", &SBScene::getDebuggerServer, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the debugger server.")


		.def("getParser", &SBScene::getParser, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the Charniak parser.")

		.def("setSystemParameter", &SBScene::setSystemParameter, "Sets a name/value pair that persists between scene sessions.")
		.def("getSystemParameter", &SBScene::getSystemParameter, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns a value for a given name.")
		.def("removeSystemParameter", &SBScene::removeSystemParameter, "Removes a system parameter.")
		.def("removeAllSystemParameters", &SBScene::removeSystemParameter, "Removes a system parameter.")
		.def("getSystemParameterNames", &SBScene::getSystemParameterNames, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns names of all system parameters.")

		// deprecated
		.def("createSkeleton", &SBScene::createSkeleton, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates a new skeleton given a skeleton definition.")
		.def("getSkeleton", &SBScene::getSkeleton, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the skeleton object given its name. \n Input: skeleton name \nOutput: skeleton object")
		.def("addAssetPath", &SBScene::addAssetPath, "Add path resource given path type and actual path string. \n Input: type(can be seq|me|ME), path \n Output: NULL")
		.def("removeAssetPath", &SBScene::removeAssetPath, "Removes a  path resource given path type and actual path string. \n Input: type(can be cript|motion|audio), path \n Output: NULL")
		.def("removeAllAssetPaths", &SBScene::removeAllAssetPaths, "Removes all paths resource given path type and actual path string. \n Input: type(can be script|motion|audio), path \n Output: NULL")
		.def("getAssetPaths", &SBScene::getAssetPaths, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns a list of all path names for a given type: seq, me, audio, mesh.")
		.def("getLocalAssetPaths", &SBScene::getLocalAssetPaths, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns a list of all path names for a given type excluding the media path: seq, me, audio, mesh.")
		.def("loadAssets", &SBScene::loadAssets, "Loads the skeletons and motions from the asset paths.")
		.def("loadAssetsFromPath", &SBScene::loadAssetsFromPath, "Loads the skeletons and motions from a given path. The path will not be stored for later use.")
		.def("addMotions", &SBScene::addMotions, "Add motion resource given filepath and recursive flag. \n Input: path, recursive flag(boolean variable indicating whether to tranverse all the children directories) \n Output: NULL")
		.def("addMotionDefinition", &SBScene::addMotionDefinition, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Adds a motion definition with blank frames.")
		.def("getMotion", &SBScene::getMotion, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns a the motion of given name.")
		.def("getNumMotions", &SBScene::getNumMotions, "Returns the number of motions available.")
		.def("getMotionNames", &SBScene::getMotionNames, "Returns the names of motions available.")
		.def("getNumSkeletons", &SBScene::getNumSkeletons, "Returns the number of skeletons available.")
		.def("getSkeletonNames", &SBScene::getSkeletonNames, "Returns a list of all skeleton names.\n Input: NULL \nOutput: list of skeleton names")

		.def("setLastScriptDirectory", &SBScene::setLastScriptDirectory, "Sets the path that to run a script.")
		.def("getLastScriptDirectory", &SBScene::getLastScriptDirectory, "Returns the last path that was used to run a script.")


	;
}
}


#endif
