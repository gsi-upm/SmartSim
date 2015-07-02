scene.run("BehaviorSetCommon.py")

def setupBehaviorSet():
	print "Setting up behavior set for Female Locomotion..."
	#scene.loadAssetsFromPath("behaviorsets/kicking/skeletons")
	#scene.loadAssetsFromPath("behaviorsets/kicking/motions")
	scene.addAssetPath("script", "behaviorsets/kicking/scripts")
	
	
	assetManager = scene.getAssetManager()	
	motionPath = "behaviorsets/kicking/motions/"
	skel = scene.getSkeleton("ChrGarza.sk")
	if skel == None:
		scene.loadAssetsFromPath("behaviorsets/kicking/skeletons")
		
	scene.run("zebra2-map.py")
	zebra2Map = scene.getJointMapManager().getJointMap("zebra2")
	zebra2Skeleton = scene.getSkeleton("ChrGarza.sk")
	zebra2Map.applySkeleton(zebra2Skeleton)
	
	kickMotions = StringVec()
	
	kickMotions.append("ChrGarza@IdleFight01")	
	kickMotions.append("ChrGarza@IdleFight01_KickBackHigh01")
	kickMotions.append("ChrGarza@IdleFight01_KickBackLow01")
	kickMotions.append("ChrGarza@IdleFight01_KickBackMedium01")
	kickMotions.append("ChrGarza@IdleFight01_KickForwardHigh01")
	kickMotions.append("ChrGarza@IdleFight01_KickForwardLow01")
	kickMotions.append("ChrGarza@IdleFight01_KickForwardMedium01")
	kickMotions.append("ChrGarza@IdleFight01_KickLeftSideHigh01")
	kickMotions.append("ChrGarza@IdleFight01_KickLeftSideLow01")
	kickMotions.append("ChrGarza@IdleFight01_KickLeftSideMedium01")
	kickMotions.append("ChrGarza@IdleFight01_KickRightSideHigh01")
	kickMotions.append("ChrGarza@IdleFight01_KickRightSideLow01")
	kickMotions.append("ChrGarza@IdleFight01_KickRightSideMedium01")
	
	for i in range(0, len(kickMotions)):
		motion = scene.getMotion(kickMotions[i])
		if motion == None:
			assetManager.loadAsset(motionPath+kickMotions[i]+'.skm')
			motion = scene.getMotion(kickMotions[i])
		#print 'motionName = ' + locoMotions[i]
		if motion != None:
			motion.setMotionSkeletonName("ChrGarza.sk")
			zebra2Map.applyMotion(motion)
	'''
	mirroredMotions = StringVec()
	mirroredMotions.append("ChrGarza@IdleStand01_JumpLeft45HighMid01")
	mirroredMotions.append("ChrGarza@IdleStand01_JumpLeft45HighNear01")
	mirroredMotions.append("ChrGarza@IdleStand01_JumpLeft45LowFar01")
	mirroredMotions.append("ChrGarza@IdleStand01_JumpLeft45LowMid01")
	mirroredMotions.append("ChrGarza@IdleStand01_JumpLeft45LowNear01")
	mirroredMotions.append("ChrGarza@IdleStand01_JumpLeft90HighMid01")
	mirroredMotions.append("ChrGarza@IdleStand01_JumpLeft90LowFar01")
	mirroredMotions.append("ChrGarza@IdleStand01_JumpLeft90LowMid01")
	mirroredMotions.append("ChrGarza@IdleStand01_JumpLeft90LowNear01")	
	
	for i in range(0,len(mirroredMotions)):
		mirrorMotion = scene.getMotion(mirroredMotions[i])
		if mirrorMotion != None:
			mirrorMotion.mirror(mirroredMotions[i]+"Rt", "ChrGarza.sk")
	'''


def retargetBehaviorSet(charName):
	kickMotions = StringVec()
	
	kickMotions.append("ChrGarza@IdleFight01")	
	kickMotions.append("ChrGarza@IdleFight01_KickBackHigh01")
	kickMotions.append("ChrGarza@IdleFight01_KickBackLow01")
	kickMotions.append("ChrGarza@IdleFight01_KickBackMedium01")
	kickMotions.append("ChrGarza@IdleFight01_KickForwardHigh01")
	kickMotions.append("ChrGarza@IdleFight01_KickForwardLow01")
	kickMotions.append("ChrGarza@IdleFight01_KickForwardMedium01")
	kickMotions.append("ChrGarza@IdleFight01_KickLeftSideHigh01")
	kickMotions.append("ChrGarza@IdleFight01_KickLeftSideLow01")
	kickMotions.append("ChrGarza@IdleFight01_KickLeftSideMedium01")
	kickMotions.append("ChrGarza@IdleFight01_KickRightSideHigh01")
	kickMotions.append("ChrGarza@IdleFight01_KickRightSideLow01")
	kickMotions.append("ChrGarza@IdleFight01_KickRightSideMedium01")
	
	
	#outDir = scene.getMediaPath() + '/retarget/motion/' + skelName + '/';
	#if not os.path.exists(outDir):
	#	os.makedirs(outDir)
	'''	
	# retarget kicking
	for n in range(0, len(kickMotions)):
		retargetMotion(kickMotions[n], 'ChrGarza.sk', skelName, outDir + 'kicking/');
	# setup standard locomotion
	scene.run("stateKicking.py")
	kickingSetup(skelName, "base", skelName, skelName)
	'''
	sbChar = scene.getCharacter(charName)
	if sbChar == None:
		return
	skelName = sbChar.getSkeleton().getName()	
	createRetargetInstance('ChrGarza.sk', skelName)
	scene.run("stateKicking.py")
	kickingSetup('ChrGarza.sk','ChrGarza.sk', "base", '', '')
	
		
