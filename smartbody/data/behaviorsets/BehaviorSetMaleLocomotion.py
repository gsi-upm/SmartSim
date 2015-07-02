scene.run("BehaviorSetCommon.py")

def setupBehaviorSet():
	print "Setting up behavior set for MaleLocomotion..."
	#scene.loadAssetsFromPath("behaviorsets/MaleLocomotion/skeletons")
	#scene.loadAssetsFromPath("behaviorsets/MaleLocomotion/motions")
	scene.addAssetPath("script", "behaviorsets/MaleLocomotion/scripts")
	
	assetManager = scene.getAssetManager()	
	motionPath = "behaviorsets/MaleLocomotion/motions/"
	skel = scene.getSkeleton("test_utah.sk")
	if skel == None:
		scene.loadAssetsFromPath("behaviorsets/MaleLocomotion/skeletons")
	
	locoMotions = StringVec()
	locoMotions.append("ChrUtah_Walk001")
	locoMotions.append("ChrUtah_Idle001")
	locoMotions.append("ChrUtah_Idle01_ToWalk01_Turn360Lf01")
	locoMotions.append("ChrUtah_Idle01_ToWalk01_Turn360Rt01")
	locoMotions.append("ChrUtah_Meander01")
	locoMotions.append("ChrUtah_Shuffle01")
	locoMotions.append("ChrUtah_Jog001")
	locoMotions.append("ChrUtah_Run001")
	locoMotions.append("ChrUtah_WalkInCircleLeft001")
	locoMotions.append("ChrUtah_WalkInCircleRight001")
	locoMotions.append("ChrUtah_WalkInTightCircleLeft001")
	locoMotions.append("ChrUtah_WalkInTightCircleRight001")
	locoMotions.append("ChrUtah_RunInCircleLeft001")
	locoMotions.append("ChrUtah_RunInCircleRight001")
	locoMotions.append("ChrUtah_RunInTightCircleLeft01")
	locoMotions.append("ChrUtah_RunInTightCircleRight01")
	locoMotions.append("ChrUtah_StrafeSlowRt01")
	locoMotions.append("ChrUtah_StrafeSlowLf01")
	locoMotions.append("ChrUtah_StrafeFastRt01")
	locoMotions.append("ChrUtah_StrafeFastLf01")
	locoMotions.append("ChrUtah_Idle001")
	locoMotions.append("ChrUtah_Turn90Lf01")
	locoMotions.append("ChrUtah_Turn180Lf01")
	locoMotions.append("ChrUtah_Turn90Rt01")
	locoMotions.append("ChrUtah_Turn180Rt01")
	locoMotions.append("ChrUtah_StopToWalkRt01")
	locoMotions.append("ChrUtah_Idle01_ToWalk01_Turn90Lf01")
	locoMotions.append("ChrUtah_Idle01_ToWalk01_Turn180Lf01")
	locoMotions.append("ChrUtah_Idle01_StepBackwardRt01")
	locoMotions.append("ChrUtah_Idle01_StepForwardRt01")
	locoMotions.append("ChrUtah_Idle01_StepSidewaysRt01")
	locoMotions.append("ChrUtah_Idle01_StepBackwardLf01")
	locoMotions.append("ChrUtah_Idle01_StepForwardLf01")
	locoMotions.append("ChrUtah_Idle01_StepSidewaysLf01")
	
	for i in range(0, len(locoMotions)):
		motion = scene.getMotion(locoMotions[i])
		if motion == None:
			assetManager.loadAsset(motionPath+locoMotions[i]+'.skm')
			motion = scene.getMotion(locoMotions[i])
		#print 'motionName = ' + locoMotions[i]
		if motion != None:
			motion.setMotionSkeletonName('test_utah.sk')
			motion.buildJointTrajectory('l_forefoot','base')
			motion.buildJointTrajectory('r_forefoot','base')
			motion.buildJointTrajectory('l_ankle','base')
			motion.buildJointTrajectory('r_ankle','base')

def retargetBehaviorSet(charName):			
	#outDir = scene.getMediaPath() + '/retarget/motion/' + skelName + '/';
	#if not os.path.exists(outDir):
	#	os.makedirs(outDir)
		
	# retarget standard locomotions
	#for n in range(0, len(locoMotions)):
	#	curMotion = scene.getMotion(locoMotions[n])
	#	if curMotion is not None:
	#		retargetMotion(locoMotions[n], 'test_utah.sk', skelName, outDir + 'MaleLocomotion/');
	sbChar = scene.getCharacter(charName)
	if sbChar == None:
		return
	skelName = sbChar.getSkeleton().getName()

	createRetargetInstance('test_utah.sk', skelName)
	# setup standard locomotion
	scene.run("stateMaleLocomotion.py")
	locomotionSetup('test_utah.sk', 'test_utah.sk', "base", '', 'all')
	
	# starting state, starting locomotion with different angle
	scene.run("stateMaleStarting.py")
	startingSetup('test_utah.sk', 'test_utah.sk', "base", '', 'all')

	# idle turn state, facing adjusting
	scene.run("stateMaleIdleTurn.py")
	idleTurnSetup('test_utah.sk', 'test_utah.sk', "base", '', 'all')

	# step state, stepping adjusting
	scene.run("stateMaleStep.py")
	stepSetup('test_utah.sk', 'test_utah.sk', "base", '', 'all')

	# transitions
	scene.run("transitions.py")
	transitionSetup('', 'all')
	
	# add IK constraint for foot automatically
	sbChar.addJointTrajectoryConstraint('l_forefoot','base')
	sbChar.addJointTrajectoryConstraint('r_forefoot','base')
	sbChar.addJointTrajectoryConstraint('l_ankle','base')
	sbChar.addJointTrajectoryConstraint('r_ankle','base')	
	
	# setup steering
	scene.run("init-steer-agents.py")
	steerManager = scene.getSteerManager()
	steerManager.setEnable(False)
	setupSteerAgent(charName, 'all')	
	steerManager.setEnable(True)
	
