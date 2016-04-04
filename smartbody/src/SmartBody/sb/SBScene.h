#ifndef SBSCENE_H
#define SBSCENE_H

#include <vhcl.h>
#include <sb/SBTypes.h>
#include <sb/SBObject.h>
#include <sb/SBScript.h>
#include <map>
#include <sstream>
#include <sr/sr_viewer.h>
#include <sbm/general_param_setting.h>

#ifndef SB_NO_PYTHON
#ifndef __native_client__
#include <boost/python.hpp>
#endif
#endif


class SBDebuggerUtility;
class SrCamera;
class CameraTrack;
class SrSnGroup;
class SbmPawn;
class SbmCharacter;
class Heightfield;
class KinectProcessor;
class SkJoint;

namespace SmartBody {

class SBSceneListener;
class SBPawn;
class SBCharacter;
class SBSkeleton;
class SBFaceDefinition;
class SBMotion;
class SBEventManager;
class SBSimulationManager;
class SBProfiler;
class SBBmlProcessor;
class SBAnimationBlendManager;
class SBReachManager;
class SBSteerManager;
class SBRealtimeManager;
class SBFaceShiftManager;
class SBServiceManager;
class SBPhysicsManager;
class SBBoneBusManager;
class SBGestureMapManager;
class SBJointMapManager;
class SBCollisionManager;
class SBPhonemeManager;
class SBBehaviorSetManager;
class SBRetargetManager;
class SBAssetManager;
class SBSpeechManager;
class SBNavigationMeshManager;
class SBParser;
class SBSubject;
class SBController;
class SBVHMsgManager;
class SBCommandManager;
class SBNavigationMesh;
class SBMotionGraphManager;
class SBHandConfigurationManager;
class SBDebuggerServer;
class SBDebuggerClient;

class SBScene : public SBObject
{
	public:
		SBAPI SBScene(void);
		SBAPI ~SBScene(void);

		SBAPI void setProcessId(const std::string& id);
		SBAPI const std::string& getProcessId();

		SBAPI void setMediaPath(const std::string& path);
		SBAPI const std::string& getMediaPath();

		SBAPI void update();

		SBAPI std::string getStringFromObject(SmartBody::SBObject* object);
		SBAPI SmartBody::SBObject* getObjectFromString(const std::string& value);

		SBAPI std::string save(bool remoteSetup = false, std::string mediaPath = "");
		SBAPI std::string saveSceneSetting();
		SBAPI std::string exportScene(const std::vector<std::string>& aspects, std::string mediaPath = "", bool remoteSetup = false);

		#if !defined(__FLASHPLAYER__)
		SBAPI void exportScenePackage(std::string outDir, std::string outZipArchiveName = "");
		SBAPI void exportCharacter(std::string charName, std::string outDir);
		#endif

		SBAPI static SBScene* getScene();		
		SBAPI static void destroyScene();

		SBAPI void setScale(float val);
		SBAPI float getScale();

		SBAPI bool command(const std::string& command);
		SBAPI bool commandAt(float seconds, const std::string& command);
		SBAPI void sendVHMsg(const std::string& message);
		SBAPI void sendVHMsg2(const std::string&, const std::string& encodedMessage);		

		SBAPI SBCharacter* createCharacter(const std::string&, const std::string&);
		SBAPI SBCharacter* copyCharacter(const std::string& origCharacter, const std::string& newCharacter);
		SBAPI SBPawn* createPawn(const std::string& pawnName);
		SBAPI SBPawn* copyPawn(const std::string& origPawn, const std::string& newPawn);

		SBAPI void removeAllCharacters();
		SBAPI void removeCharacter(const std::string& charName);
		SBAPI void removePawn(const std::string& pawnName);
		SBAPI void removeAllPawns();
		SBAPI int getNumCharacters();
		SBAPI int getNumPawns();
		SBAPI SBCharacter* getCharacter(const std::string& name);
		SBAPI SBPawn* getPawn(const std::string& name);

		SBAPI SBFaceDefinition* createFaceDefinition(const std::string& name);
		SBAPI void removeFaceDefinition(const std::string& name);
		SBAPI SBFaceDefinition* getFaceDefinition(const std::string& name);
		SBAPI int getNumFaceDefinitions();
		SBAPI std::vector<std::string> getFaceDefinitionNames();

		SBAPI const std::vector<std::string>& getPawnNames();
		SBAPI const std::vector<std::string>& getCharacterNames();

		SBAPI void removePendingCommands();

		SBAPI std::vector<std::string> getEventHandlerNames();

		SBAPI void startFileLogging(const std::string& filename);
		SBAPI void stopFileLogging();
	
		SBAPI bool run(const std::string& command);
		SBAPI bool runScript(const std::string& script);

		SBAPI void setDefaultCharacter(const std::string& character);
		SBAPI void setDefaultRecipient(const std::string& recipient);

		SBAPI void reset();

		SBAPI std::string getLastScriptDirectory();
		SBAPI void setLastScriptDirectory(std::string dir);

		SBAPI void addScript(const std::string& name, SBScript* script);
		SBAPI void removeScript(const std::string& name);
		SBAPI int getNumScripts();
		SBAPI SBScript* getScript(const std::string& name);
		SBAPI std::vector<std::string> getScriptNames();
		SBAPI std::map<std::string, SBScript*>& getScripts();

		SBAPI SBEventManager* getEventManager();		
		SBAPI SBSimulationManager* getSimulationManager();
		SBAPI SBProfiler* getProfiler();
		SBAPI SBBmlProcessor* getBmlProcessor();
		SBAPI SBAnimationBlendManager* getBlendManager();
		SBAPI SBReachManager* getReachManager();
		SBAPI SBSteerManager* getSteerManager();
		SBAPI SBRealtimeManager* getRealtimeManager();
		SBAPI SBFaceShiftManager* getFaceShiftManager();
		SBAPI SBServiceManager* getServiceManager();
		SBAPI SBPhysicsManager* getPhysicsManager();
		SBAPI SBBoneBusManager* getBoneBusManager();
		SBAPI SBGestureMapManager* getGestureMapManager();
		SBAPI SBJointMapManager* getJointMapManager();
		SBAPI SBCollisionManager* getCollisionManager();
		SBAPI SBPhonemeManager* getDiphoneManager();
		SBAPI SBBehaviorSetManager* getBehaviorSetManager();
		SBAPI SBRetargetManager* getRetargetManager();
		SBAPI SBAssetManager* getAssetManager();
		SBAPI SBSpeechManager* getSpeechManager();
		SBAPI SBVHMsgManager* getVHMsgManager();
		SBAPI SBCommandManager* getCommandManager();
		SBAPI SBNavigationMeshManager* getNavigationMeshManager();
		SBAPI SBMotionGraphManager* getMotionGraphManager();
		SBAPI SBHandConfigurationManager* getHandConfigurationManager();

		SBAPI SBParser* getParser();

		SBAPI SBDebuggerServer * getDebuggerServer();
		SBAPI SBDebuggerClient * getDebuggerClient();
		SBAPI SBDebuggerUtility* getDebuggerUtility();
		SBAPI bool isRemoteMode();
		SBAPI void setRemoteMode(bool val);

		SBAPI void addSceneListener(SBSceneListener* listener);
		SBAPI std::vector<SBSceneListener*>& getSceneListeners();
		SBAPI void removeSceneListener(SBSceneListener* listener);
		SBAPI void removeAllSceneListeners();

		SBAPI void notify(SBSubject* subject);

		SBAPI static void setSystemParameter(const std::string& name, const std::string& value);
		SBAPI static std::string getSystemParameter(const std::string& name);
		SBAPI static void removeSystemParameter(const std::string& name);
		SBAPI static void removeAllSystemParameters();
		SBAPI static std::vector<std::string> getSystemParameterNames();

		SBAPI SrCamera* createCamera(const std::string& name);
		SBAPI void removeCamera(SrCamera* camera);
		SBAPI void setActiveCamera(SrCamera* camera);
		SBAPI SrCamera* getActiveCamera();
		SBAPI SrCamera* getCamera(const std::string& name);
      SBAPI void SetCameraLocked(bool locked);
      SBAPI bool IsCameraLocked();
		SBAPI int getNumCameras();
		SBAPI std::vector<std::string> getCameraNames();
		SBAPI std::vector<SBController*>& getDefaultControllers();
		SBAPI bool hasCameraTrack();
		SBAPI void setCameraTrack(const std::string& characterName, const std::string& jointName);
		SBAPI void removeCameraTrack();
		SBAPI void updateTrackedCameras();

		SBAPI void updateConeOfSight();
		SBAPI bool setCameraConeOfSight(const std::string& characterName);
		SBAPI bool hasConeOfSight();
		SBAPI void removeConeOfSight();

		SBAPI std::vector<std::string> checkVisibility(const std::string& characterName);
		SBAPI std::vector<std::string> checkVisibility_current_view();
		std::vector<std::string> occlusionTest(const std::vector<std::string>& testPawns);
		std::vector<std::string> frustumTest(const std::vector<std::string>& testPawnNames);

		// deprecated
		SBAPI SBSkeleton* createSkeleton(const std::string&char_name);
		SBAPI SBSkeleton* getSkeleton(const std::string& name);		
		SBAPI void addAssetPath(const std::string& type, const std::string& path);
		SBAPI std::vector<std::string> getAssetPaths(const std::string& type);
		SBAPI std::vector<std::string> getLocalAssetPaths(const std::string& type);
		SBAPI void removeAssetPath(const std::string& type, const std::string& path);
		SBAPI void removeAllAssetPaths(const std::string& type);
		SBAPI void loadAssets();
		SBAPI void loadAsset(const std::string& assetPath);
		SBAPI void loadAssetsFromPath(const std::string& assetPath);
		SBAPI void loadMotions();
		SBAPI void addMotions(const std::string& path, bool recursive);
		SBAPI SBMotion* addMotionDefinition(const std::string& motionName, double duration, int numFrames );
		SBAPI SBSkeleton* addSkeletonDefinition(const std::string& skelName);
		SBAPI void removeSkeletonDefinition(const std::string& skelName);
		SBAPI SBMotion* createMotion(const std::string& motionName);
		SBAPI SBMotion* getMotion(const std::string& name);
		SBAPI int getNumMotions();
		SBAPI std::vector<std::string> getMotionNames();
		SBAPI int getNumSkeletons();
		SBAPI std::vector<std::string> getSkeletonNames();

		SBAPI SrSnGroup* getRootGroup();
		SBAPI std::string getValidName(const std::string& name);
		SBAPI void updatePawnNames();
		SBAPI void updateCharacterNames();

#if 0 // should be creating navigation mesh directly in AssetManager
		SBAPI bool createNavigationMesh(const std::string& meshfilename);
#endif
		SBAPI void setNavigationMesh(const std::string& naviMeshName);
		SBAPI SBNavigationMesh* getNavigationMesh();
		
		SBAPI Heightfield* createHeightfield();
		SBAPI void removeHeightfield();
		SBAPI Heightfield* getHeightfield();
		SBAPI float queryTerrain( float x, float z, float *normal_p );

		SBAPI KinectProcessor* getKinectProcessor();
		SBAPI std::map<std::string, GeneralParam*>& getGeneralParameters();

#ifndef SB_NO_PYTHON
#ifndef __native_client__
		SBAPI void setPythonMainModule(boost::python::object& pyobject);
		SBAPI void setPythonMainDict(boost::python::object& pyobject);
		SBAPI boost::python::object& getPythonMainModule();
		SBAPI boost::python::object& getPythonMainDict();
#endif
#endif
	
		SBAPI SrViewer* getViewer();
		SBAPI SrViewer* getOgreViewer();
		SBAPI void setViewer(SrViewer* viewer);
		SBAPI void setOgreViewer(SrViewer* viewer);
		SBAPI void setViewerFactory(SrViewerFactory* viewerFactory);
		SBAPI void setOgreViewerFactory(SrViewerFactory* viewerFactory);
		SBAPI SrViewerFactory* getViewerFactory();
		SBAPI SrViewerFactory* getOgreViewerFactory();


				
	protected:

		void initialize();
		void cleanup();
		void saveScene(std::stringstream& strstr, bool remoteSetup);
		void saveAssets(std::stringstream& strstr, bool remoteSetup, std::string mediaPath);
		void saveCameras(std::stringstream& strstr, bool remoteSetup);
		void savePawns(std::stringstream& strstr, bool remoteSetup);
		void saveCharacters(std::stringstream& strstr, bool remoteSetup);
		void saveLights(std::stringstream& strstr, bool remoteSetup);
		void saveRetargets(std::stringstream& strstr, bool remoteSetup);
		void saveBlends(std::stringstream& strstr, bool remoteSetup);
		void saveJointMaps(std::stringstream& strstr, bool remoteSetup);
		void saveFaceDefinitions(std::stringstream& strstr, bool remoteSetup);
		void saveGestureMaps(std::stringstream& strstr, bool remoteSetup);
		void saveLipSyncing(std::stringstream& strstr, bool remoteSetup);
		void saveServices(std::stringstream& strstr, bool remoteSetup);
		void savePositions(std::stringstream& strstr, bool remoteSetup);
		void createDefaultControllers();
		void removeDefaultControllers();


		SBSimulationManager* _sim;
		SBProfiler* _profiler;
		SBBmlProcessor* _bml;
		SBAnimationBlendManager* _blendManager;
		SBReachManager* _reachManager;
		SBSteerManager* _steerManager;
		SBRealtimeManager* _realtimeManager;
		SBFaceShiftManager* _faceShiftManager;
		SBServiceManager* _serviceManager;
		SBPhysicsManager* _physicsManager;
		SBBoneBusManager* _boneBusManager;
		SBGestureMapManager* _gestureMapManager;
		SBJointMapManager* _jointMapManager;
		SBCollisionManager* _collisionManager;
		SBPhonemeManager* _phonemeManager;
		SBRetargetManager* _retargetManager;
		SBEventManager* _eventManager;
		SBAssetManager* _assetManager;
		SBSpeechManager* _speechManager;
		SBVHMsgManager* _vhmsgManager;
		SBCommandManager* _commandManager;
		SBNavigationMeshManager* _naviMeshManager;
		SBMotionGraphManager* _motionGraphManager;
		SBHandConfigurationManager* _handConfigManager;

		SBParser* _parser;

		std::vector<SBSceneListener*> _sceneListeners;

		SBBehaviorSetManager* _behaviorSetManager;

		std::map<std::string, SBScript*> _scripts;
		float _scale;
		bool _isRemoteMode;
		bool _isCameraLocked;
		static bool _firstTime;

		SBDebuggerServer*	_debuggerServer;
		SBDebuggerClient*	_debuggerClient;
		SBDebuggerUtility*	_debuggerUtility;
		std::map<std::string, SrCamera*> _cameras;
		std::string _activeCamera;
		std::vector<CameraTrack*> _cameraTracking;

		bool		_coneOfSight;
		SkJoint *	_coneOfSight_leftEye;
		SkJoint *	_coneOfSight_rightEye;
		std::string	_coneOfSight_character;

		std::map<std::string, SmartBody::SBFaceDefinition*> _faceDefinitions;

		std::string _mediaPath;
		std::vector<SBController*> _defaultControllers;
		
		std::string _processId;
		static SBScene* _scene;
		static std::map<std::string, std::string> _systemParameters;

		SrSnGroup* _rootGroup;
		std::map<std::string, SbmPawn*>	_pawnMap;
		std::vector<std::string> _pawnNames;
		std::map<std::string, SbmCharacter*> _characterMap;
		std::vector<std::string> _characterNames;

		SrViewer* _viewer;
		SrViewer* _ogreViewer;
		SrViewerFactory* _viewerFactory;
		SrViewerFactory* _ogreViewerFactory;

		vhcl::Log::Listener* _logListener;

		KinectProcessor* _kinectProcessor;
		Heightfield* _heightField;
		SBNavigationMesh* _navigationMesh;
		std::map<std::string, GeneralParam*> _generalParams;

		std::string _lastScriptDirectory;

#ifndef SB_NO_PYTHON
#ifndef __native_client__
		boost::python::object _mainModule;
		boost::python::object _mainDict;
#endif
#endif
};


};



#endif
