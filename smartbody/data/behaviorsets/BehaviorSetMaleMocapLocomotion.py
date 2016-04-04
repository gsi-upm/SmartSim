scene.run("BehaviorSetCommon.py")

def setupBehaviorSet():
	print "Setting up behavior set for MaleMocapLocomotion..."
	
	#scene.loadAssetsFromPath("behaviorsets/MaleMocapLocomotion/skeletons")
	#scene.loadAssetsFromPath("behaviorsets/MaleMocapLocomotion/motions")
	assetManager = scene.getAssetManager()
	scene.addAssetPath("script", "behaviorsets/MaleMocapLocomotion/scripts")
	motionPath = "behaviorsets/MaleMocapLocomotion/motions/"
	skel = scene.getSkeleton("ChrBackovic.sk")
	if skel == None:
		scene.loadAssetsFromPath("behaviorsets/MaleMocapLocomotion/skeletons")
		
	
	# map the zebra2 skeleton
	scene.run("zebra2-map.py")
	zebra2Map = scene.getJointMapManager().getJointMap("zebra2")
	bradSkeleton = scene.getSkeleton("ChrBackovic.sk")
	zebra2Map.applySkeleton(bradSkeleton)	
	locoMotions = StringVec()
	locoMotions.append("ChrMarine@Idle01")
	locoMotions.append("ChrMarine@Jog01")
	locoMotions.append("ChrMarine@Meander01")
	locoMotions.append("ChrMarine@Run01")
	#locoMotions.append("ChrMarine@RunCircleLf01")
	locoMotions.append("ChrMarine@RunCircleRt01")
	#locoMotions.append("ChrMarine@RunTightCircleLf01")
	locoMotions.append("ChrMarine@RunTightCircleRt01")
	#locoMotions.append("ChrMarine@StrafeSlowLf01")
	locoMotions.append("ChrMarine@StrafeSlowRt01")
	locoMotions.append("ChrMarine@Walk01")
	#locoMotions.append("ChrMarine@WalkCircleLf01")
	locoMotions.append("ChrMarine@WalkCircleRt01")
	#locoMotions.append("ChrMarine@WalkTightCircleLf01")
	locoMotions.append("ChrMarine@WalkTightCircleRt01")
	#locoMotions.append("ChrMarine@Turn360Lf01")
	locoMotions.append("ChrMarine@Turn360Rt01")
	#locoMotions.append("ChrMarine@StrafeFastLf01")
	locoMotions.append("ChrMarine@StrafeFastRt01")
	#locoMotions.append("ChrMarine@Idle01_StepBackwardsLf01")
	locoMotions.append("ChrMarine@Idle01_StepBackwardsRt01")
	#locoMotions.append("ChrMarine@Idle01_StepForwardLf01")
	locoMotions.append("ChrMarine@Idle01_StepForwardRt01")
	#locoMotions.append("ChrMarine@Idle01_StepSidewaysLf01")
	locoMotions.append("ChrMarine@Idle01_StepSidewaysRt01")
	#locoMotions.append("ChrMarine@Turn90Lf01")	
	#locoMotions.append("ChrMarine@Turn180Lf01")
	locoMotions.append("ChrMarine@Turn90Rt01")
	locoMotions.append("ChrMarine@Turn180Rt01")
	#locoMotions.append("ChrMarine@Idle01_ToWalkLf01")
	#locoMotions.append("ChrMarine@Idle01_ToWalk01_Turn90Lf01")
	#locoMotions.append("ChrMarine@Idle01_ToWalk01_Turn180Lf01")
	locoMotions.append("ChrMarine@Idle01_ToWalk01")
	locoMotions.append("ChrMarine@Idle01_ToWalk01_Turn90Rt01")
	locoMotions.append("ChrMarine@Idle01_ToWalk01_Turn180Rt01")
	
	
	for i in range(0, len(locoMotions)):
		motion = scene.getMotion(locoMotions[i])
		if motion == None:
			assetManager.loadAsset(motionPath+locoMotions[i]+'.skm')
			motion = scene.getMotion(locoMotions[i])
		#print 'motionName = ' + locoMotions[i]
		if motion != None:
			motion.setMotionSkeletonName('ChrBackovic.sk')
			zebra2Map.applyMotion(motion)
			motion.buildJointTrajectory('l_forefoot','base')
			motion.buildJointTrajectory('r_forefoot','base')
			motion.buildJointTrajectory('l_ankle','base')
			motion.buildJointTrajectory('r_ankle','base')


def retargetBehaviorSet(charName):

	scene.run('locomotion-ChrMarine-init.py')
	
	locoMotions = StringVec()
	locoMotions.append("ChrMarine@Idle01")
	locoMotions.append("ChrMarine@Jog01")
	locoMotions.append("ChrMarine@Meander01")
	locoMotions.append("ChrMarine@Run01")
	locoMotions.append("ChrMarine@RunCircleLf01_smooth")
	locoMotions.append("ChrMarine@RunCircleRt01_smooth")
	locoMotions.append("ChrMarine@RunTightCircleLf01")
	locoMotions.append("ChrMarine@RunTightCircleRt01")
	locoMotions.append("ChrMarine@StrafeSlowLf01")
	locoMotions.append("ChrMarine@StrafeSlowRt01")
	locoMotions.append("ChrMarine@Walk01")
	locoMotions.append("ChrMarine@WalkCircleLf01_smooth")
	locoMotions.append("ChrMarine@WalkCircleRt01_smooth")
	locoMotions.append("ChrMarine@WalkTightCircleLf01_smooth")
	locoMotions.append("ChrMarine@WalkTightCircleRt01_smooth")
	locoMotions.append("ChrMarine@Turn360Lf01")
	locoMotions.append("ChrMarine@Turn360Rt01")
	locoMotions.append("ChrMarine@StrafeFastLf01_smooth")
	locoMotions.append("ChrMarine@StrafeFastRt01_smooth")
	locoMotions.append("ChrMarine@Idle01_StepBackwardsLf01")
	locoMotions.append("ChrMarine@Idle01_StepBackwardsRt01")
	locoMotions.append("ChrMarine@Idle01_StepForwardLf01")
	locoMotions.append("ChrMarine@Idle01_StepForwardRt01")
	locoMotions.append("ChrMarine@Idle01_StepSidewaysLf01")
	locoMotions.append("ChrMarine@Idle01_StepSidewaysRt01")
	locoMotions.append("ChrMarine@Turn90Lf01")
	locoMotions.append("ChrMarine@Turn180Lf01")
	locoMotions.append("ChrMarine@Turn90Rt01")
	locoMotions.append("ChrMarine@Turn180Rt01")
	locoMotions.append("ChrMarine@Idle01_ToWalkLf01")
	locoMotions.append("ChrMarine@Idle01_ToWalk01_Turn90Lf01")
	locoMotions.append("ChrMarine@Idle01_ToWalk01_Turn180Lf01")
	locoMotions.append("ChrMarine@Idle01_ToWalk01")
	locoMotions.append("ChrMarine@Idle01_ToWalk01_Turn90Rt01")
	locoMotions.append("ChrMarine@Idle01_ToWalk01_Turn180Rt01")
	
	
			
	
	'''	
	outDir = scene.getMediaPath() + '/retarget/motion/' + skelName + '/';
	if not os.path.exists(outDir):
		os.makedirs(outDir)
		
	# retarget standard locomotions
	for n in range(0, len(locoMotions)):
		curMotion = scene.getMotion(locoMotions[n])
		if curMotion is not None:
			retargetMotion(locoMotions[n], 'ChrBackovic.sk', skelName, outDir + 'MaleMocapLocomotion/');	

	# setup standard locomotion
	scene.run("locomotion-ChrMarine-state-Locomotion.py")
	marineLocomotionSetup('ChrBackovic.sk', skelName, "base", skelName, skelName)
	
	# starting state, starting locomotion with different angle
	scene.run("locomotion-ChrMarine-state-StartingLeft.py")
	mocapStartLeftSetup(skelName, "base", skelName, skelName)
	
	# starting state, starting locomotion with different angle
	scene.run("locomotion-ChrMarine-state-StartingRight.py")
	mocapStartRightSetup(skelName, "base", skelName, skelName)

	# idle turn state, facing adjusting
	scene.run("locomotion-ChrMarine-state-IdleTurn.py")
	mocapIdleTurnSetup(skelName, "base", skelName, skelName)

	# step state, stepping adjusting
	scene.run("locomotion-ChrMarine-state-Step.py")
	mocapStepSetup(skelName, "base", skelName, skelName)
	
	'''
	
	sbChar = scene.getCharacter(charName)
	if sbChar == None:
		return
	skelName = sbChar.getSkeleton().getName()
	createRetargetInstance('ChrBackovic.sk', skelName)
	stateManager = scene.getStateManager()
	mocapLocomotionState = stateManager.getBlend('mocapLocomotion')
	if mocapLocomotionState == None:
		# compute motion trajectory
		for i in range(0, len(locoMotions)):
			motion = scene.getMotion(locoMotions[i])
			#print 'motionName = ' + locoMotions[i]
			if motion != None:
				motion.buildJointTrajectory('l_forefoot','base')
				motion.buildJointTrajectory('r_forefoot','base')
				motion.buildJointTrajectory('l_ankle','base')
				motion.buildJointTrajectory('r_ankle','base')
		# setup standard locomotion
		scene.run("locomotion-ChrMarine-state-Locomotion.py")
		#marineLocomotionSetup('ChrBackovic.sk', 'ChrBackovic.sk', "base", '', 'mocap')	
		
		# starting state, starting locomotion with different angle
		scene.run("locomotion-ChrMarine-state-StartingLeft.py")
		#mocapStartLeftSetup('ChrBackovic.sk','ChrBackovic.sk', "base", '', 'mocap')
		
		# starting state, starting locomotion with different angle
		scene.run("locomotion-ChrMarine-state-StartingRight.py")
		#mocapStartRightSetup('ChrBackovic.sk','ChrBackovic.sk', "base", '', 'mocap')

		# idle turn state, facing adjusting
		scene.run("locomotion-ChrMarine-state-IdleTurn.py")
		#mocapIdleTurnSetup('ChrBackovic.sk','ChrBackovic.sk', "base", '', 'mocap')

		# step state, stepping adjusting
		scene.run("locomotion-ChrMarine-state-Step.py")
		#mocapStepSetup('ChrBackovic.sk','ChrBackovic.sk', "base", '', 'mocap')
		
		# setup transitions
		scene.run("locomotion-ChrMarine-transitions.py")
	
	# add IK constraint for foot automatically
	sbChar.addJointTrajectoryConstraint('l_forefoot','base')
	sbChar.addJointTrajectoryConstraint('r_forefoot','base')
	#sbChar.addJointTrajectoryConstraint('l_ankle','base')
	#sbChar.addJointTrajectoryConstraint('r_ankle','base')	
	# setup steering
	scene.run("init-steer-agents.py")
	steerManager = scene.getSteerManager()
	steerManager.setEnable(False)
	setupSteerAgent(charName, 'mocap')	
	steerManager.setEnable(True)
	
