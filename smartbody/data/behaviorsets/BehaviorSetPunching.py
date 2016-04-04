scene.run("BehaviorSetCommon.py")

def setupBehaviorSet():
	print "Setting up behavior set for Punching..."
	#scene.loadAssetsFromPath("behaviorsets/punching/skeletons")
	#scene.loadAssetsFromPath("behaviorsets/punching/motions")
	scene.addAssetPath("script", "behaviorsets/punching/scripts")
	
	assetManager = scene.getAssetManager()	
	motionPath = "behaviorsets/punching/motions/"
	skel = scene.getSkeleton("ChrGarza.sk")
	if skel == None:
		scene.loadAssetsFromPath("behaviorsets/punching/skeletons")
		
	scene.run("zebra2-map.py")
	zebra2Map = scene.getJointMapManager().getJointMap("zebra2")
	zebra2Skeleton = scene.getSkeleton("ChrGarza.sk")
	zebra2Map.applySkeleton(zebra2Skeleton)
	
	punchMotions = StringVec()
	
	punchMotions.append("ChrGarza@IdleFight01")	
	punchMotions.append("ChrGarza@IdleFight01_PunchForwardFloor01")
	punchMotions.append("ChrGarza@IdleFight01_PunchForwardMediumFar01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft45Floor01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft45High01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft45Low01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft45MediumFar01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft90Floor01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft90High01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft90Low01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft90MediumFar01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft90MediumNear01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight45Floor01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight45High01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight45Low01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight45MediumFar01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight45MediumNear01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight90Floor01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight90High01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight90Low01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight90MediumFar01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight90MediumNear01")
	
	for i in range(0, len(punchMotions)):
		motion = scene.getMotion(punchMotions[i])
		if motion == None:
			assetManager.loadAsset(motionPath+punchMotions[i]+'.skm')
			motion = scene.getMotion(punchMotions[i])
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
	punchMotions = StringVec()
	
	punchMotions.append("ChrGarza@IdleFight01")	
	punchMotions.append("ChrGarza@IdleFight01_PunchForwardFloor01")
	punchMotions.append("ChrGarza@IdleFight01_PunchForwardMediumFar01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft45Floor01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft45High01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft45Low01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft45MediumFar01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft90Floor01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft90High01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft90Low01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft90MediumFar01")
	punchMotions.append("ChrGarza@IdleFight01_PunchLeft90MediumNear01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight45Floor01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight45High01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight45Low01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight45MediumFar01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight45MediumNear01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight90Floor01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight90High01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight90Low01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight90MediumFar01")
	punchMotions.append("ChrGarza@IdleFight01_PunchRight90MediumNear01")
	
	
	#outDir = scene.getMediaPath() + '/retarget/motion/' + skelName + '/';
	#if not os.path.exists(outDir):
	#	os.makedirs(outDir)
	sbChar = scene.getCharacter(charName)
	if sbChar == None:
		return
	skelName = sbChar.getSkeleton().getName()

	createRetargetInstance('ChrGarza.sk', skelName)
	
	# retarget kicking
	#for n in range(0, len(punchMotions)):
	#	retargetMotion(punchMotions[n], 'ChrGarza.sk', skelName, outDir + 'punching/');
		


	# setup standard locomotion
	scene.run("statePunching.py")
	#punchingSetup(skelName, "base", skelName, skelName)
	punchingSetup('ChrGarza.sk','ChrGarza.sk', "base", '', '')
	
		
