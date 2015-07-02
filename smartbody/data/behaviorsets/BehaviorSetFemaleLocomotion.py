scene.run("BehaviorSetCommon.py")

def setupBehaviorSet():
	print "Setting up behavior set for Female Locomotion..."
	#scene.loadAssetsFromPath("behaviorsets/FemaleLocomotion/skeletons")
	#scene.loadAssetsFromPath("behaviorsets/FemaleLocomotion/motions")
	scene.addAssetPath("script", "behaviorsets/FemaleLocomotion/scripts")
	
	assetManager = scene.getAssetManager()	
	motionPath = "behaviorsets/FemaleLocomotion/motions/"
	skel = scene.getSkeleton("ChrHarmony.sk")
	if skel == None:
		scene.loadAssetsFromPath("behaviorsets/FemaleLocomotion/skeletons")
	# map the Harmony skeleton
	scene.run("zebra2-map.py")
	zebra2Map = scene.getJointMapManager().getJointMap("zebra2")
	harmonySkeleton = scene.getSkeleton("ChrHarmony.sk")
	zebra2Map.applySkeleton(harmonySkeleton)
	
	
	locoMotions = StringVec()	
	locoMotions.append("ChrHarmony@Idle01_StepBackwardLf01")	
	locoMotions.append("ChrHarmony@Idle01_StepBackwardRt01")
	locoMotions.append("ChrHarmony@Idle01_StepForwardLf01")
	locoMotions.append("ChrHarmony@Idle01_StepForwardRt01")
	locoMotions.append("ChrHarmony@Idle01_StepSidewaysLf01")
	locoMotions.append("ChrHarmony@Idle01_StepSidewaysRt01")
	locoMotions.append("ChrHarmony@Idle01_ToWalk01")
	locoMotions.append("ChrHarmony@Idle01_ToWalk01_Turn90Lf01")
	locoMotions.append("ChrHarmony@Idle01_ToWalk01_Turn90Rt01")
	locoMotions.append("ChrHarmony@Idle01_ToWalk01_Turn180Lf01")
	locoMotions.append("ChrHarmony@Idle01_ToWalk01_Turn180Rt01")
	locoMotions.append("ChrHarmony@Idle01_ToWalk01_Turn360Rt01")
	locoMotions.append("ChrHarmony@Idle01_ToWalkLf01")
	locoMotions.append("ChrHarmony@Idle01_ToWalkRt01")
	locoMotions.append("ChrHarmony@IdleHandOnHip01")
	#locoMotions.append("ChrHarmony@StrafeSlowLf01")
	locoMotions.append("ChrHarmony@StrafeSlowRt01")
	locoMotions.append("ChrHarmony@Turn90Lf01")
	locoMotions.append("ChrHarmony@Turn90Rt01")
	locoMotions.append("ChrHarmony@Turn180Lf01")
	locoMotions.append("ChrHarmony@Turn180Rt01")
	locoMotions.append("ChrHarmony@Walk01")
	locoMotions.append("ChrHarmony@Walk01_ToIdle01")
	locoMotions.append("ChrHarmony@Walk02")
	#locoMotions.append("ChrHarmony@WalkCircleLf01")
	locoMotions.append("ChrHarmony@WalkCircleRt01")
	#locoMotions.append("ChrHarmony@WalkTightCircleLf01")
	locoMotions.append("ChrHarmony@WalkTightCircleRt01")	
	
	for i in range(0, len(locoMotions)):
		motion = scene.getMotion(locoMotions[i])
		if motion == None:
			assetManager.loadAsset(motionPath+locoMotions[i]+'.skm')
			motion = scene.getMotion(locoMotions[i])		
		if motion != None:
			motion.setMotionSkeletonName('ChrHarmony.sk')
			zebra2Map.applyMotion(motion)	
			
		
	mirrorMotion1 = scene.getMotion("ChrHarmony@WalkCircleRt01")
	if scene.getMotion("ChrHarmony@WalkCircleLf01") == None:
		zebra2Map.applyMotion(mirrorMotion1)	
		mirrorMotion1.mirror("ChrHarmony@WalkCircleLf01", "ChrHarmony.sk")
		
	mirrorMotion1 = scene.getMotion("ChrHarmony@WalkTightCircleRt01")
	if scene.getMotion("ChrHarmony@WalkTightCircleLf01") == None:
		zebra2Map.applyMotion(mirrorMotion1)	
		mirrorMotion1.mirror("ChrHarmony@WalkTightCircleLf01", "ChrHarmony.sk")
		
	mirrorMotion1 = scene.getMotion("ChrHarmony@StrafeSlowRt01")
	if scene.getMotion("ChrHarmony@StrafeSlowLf01") == None:
		zebra2Map.applyMotion(mirrorMotion1)	
		mirrorMotion1.mirror("ChrHarmony@StrafeSlowLf01", "ChrHarmony.sk")
	
	

def retargetBehaviorSet(charName):
	locoMotions = StringVec()
	
	locoMotions.append("ChrHarmony@Idle01_StepBackwardLf01")	
	locoMotions.append("ChrHarmony@Idle01_StepBackwardRt01")
	locoMotions.append("ChrHarmony@Idle01_StepForwardLf01")
	locoMotions.append("ChrHarmony@Idle01_StepForwardRt01")
	locoMotions.append("ChrHarmony@Idle01_StepSidewaysLf01")
	locoMotions.append("ChrHarmony@Idle01_StepSidewaysRt01")
	locoMotions.append("ChrHarmony@Idle01_ToWalk01")
	locoMotions.append("ChrHarmony@Idle01_ToWalk01_Turn90Lf01")
	locoMotions.append("ChrHarmony@Idle01_ToWalk01_Turn90Rt01")
	locoMotions.append("ChrHarmony@Idle01_ToWalk01_Turn180Lf01")
	locoMotions.append("ChrHarmony@Idle01_ToWalk01_Turn180Rt01")
	locoMotions.append("ChrHarmony@Idle01_ToWalk01_Turn360Rt01")
	locoMotions.append("ChrHarmony@Idle01_ToWalkLf01")
	locoMotions.append("ChrHarmony@Idle01_ToWalkRt01")
	locoMotions.append("ChrHarmony@IdleHandOnHip01")
	locoMotions.append("ChrHarmony@StrafeSlowLf01")
	locoMotions.append("ChrHarmony@StrafeSlowRt01")
	locoMotions.append("ChrHarmony@Turn90Lf01")
	locoMotions.append("ChrHarmony@Turn90Rt01")
	locoMotions.append("ChrHarmony@Turn180Lf01")
	locoMotions.append("ChrHarmony@Turn180Rt01")
	locoMotions.append("ChrHarmony@Walk01")
	locoMotions.append("ChrHarmony@Walk01_ToIdle01")
	locoMotions.append("ChrHarmony@Walk02")
	locoMotions.append("ChrHarmony@WalkCircleLf01")
	locoMotions.append("ChrHarmony@WalkCircleRt01")
	locoMotions.append("ChrHarmony@WalkTightCircleLf01")
	locoMotions.append("ChrHarmony@WalkTightCircleRt01")
	
	#outDir = scene.getMediaPath() + '/retarget/motion/' + skelName + '/';
	#if not os.path.exists(outDir):
	#	os.makedirs(outDir)
		
	# retarget female locomotions
	#for n in range(0, len(locoMotions)):
	#	retargetMotion(locoMotions[n], 'ChrHarmony.sk', skelName, outDir + 'FemaleLocomotion/');

	sbChar = scene.getCharacter(charName)
	if sbChar == None:
		return
	charSkelName = sbChar.getSkeleton().getName()
	createRetargetInstance('ChrHarmony.sk', charSkelName)
	
	# setup standard locomotion
	scene.run("stateFemaleLocomotion.py")
	femaleLocomotionSetup(charSkelName, "base", '', 'female')
	
	# starting state, starting locomotion with different angle
	scene.run("stateFemaleStarting.py")
	femaleStartingSetup(charSkelName, "base", '', 'female')

	# idle turn state, facing adjusting
	scene.run("stateFemaleIdleTurn.py")
	femaleIdleTurnSetup(charSkelName, "base", '', 'female')

	# step state, stepping adjusting
	scene.run("stateFemaleStep.py")
	femaleStepSetup(charSkelName, "base", '', 'female')

	# transitions
	scene.run("transitionsFemale.py")
	femaleTransitionSetup('', 'female')
	
	# setup steering
	scene.run("init-steer-agents.py")
	steerManager = scene.getSteerManager()
	steerManager.setEnable(False)
	setupSteerAgent(charName, 'female')	
	steerManager.setEnable(True)
