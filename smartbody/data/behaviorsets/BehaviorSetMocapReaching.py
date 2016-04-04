scene.run("BehaviorSetCommon.py")

def setupBehaviorSet():
	print "Setting up behavior set for gestures..."
	#scene.loadAssetsFromPath("behaviorsets/MocapReaching/skeletons")
	#scene.loadAssetsFromPath("behaviorsets/MocapReaching/motions")
	scene.addAssetPath("script", "behaviorsets/MocapReaching/scripts")
	# map the zebra2 skeleton
	
	assetManager = scene.getAssetManager()	
	motionPath = "behaviorsets/MocapReaching/motions/"
	skel = scene.getSkeleton("ChrGarza.sk")
	if skel == None:
		scene.loadAssetsFromPath("behaviorsets/MocapReaching/skeletons")
		
	scene.run("zebra2-map.py")
	zebra2Map = scene.getJointMapManager().getJointMap("zebra2")
	garzaSkeleton = scene.getSkeleton("ChrGarza.sk")
	zebra2Map.applySkeleton(garzaSkeleton)
	
	
	mocapRReachMotions = StringVec();
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachBackFloor01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachBackHigh01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachBackLow01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachBackMediumFar01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachBackMediumMid01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachBackMediumNear01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachForwardFloor01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachForwardHigh01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachForwardLow01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachForwardMediumFar01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachForwardMediumMid01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachLeft30Floor01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachLeft30High01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachLeft30Low01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachLeft30MediumFar01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachLeft30MediumMid01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachLeft30MediumNear01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachLeft60Floor01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachLeft60High01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachLeft60Low01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachLeft60MediumFar01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachLeft60MediumMid01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachLeft60MediumNear01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachRight120Floor01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachRight120High01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachRight120Low01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachRight120MediumFar01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachRight120MediumMid01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachRight120MediumNear01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachRight30Floor01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachRight30High01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachRight30Low01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachRight30MediumFar01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachRight30MediumMid01")
	mocapRReachMotions.append("ChrGarza@IdleStand01_ReachRight30MediumNear01")
	
	mocapRReachMotions.append("ChrHarmony_Relax001_HandGraspSmSphere_Grasp")
	mocapRReachMotions.append("ChrHarmony_Relax001_HandGraspSmSphere_Reach")
	mocapRReachMotions.append("ChrHarmony_Relax001_HandGraspSmSphere_Release")
	mocapRReachMotions.append("HandsAtSide_RArm_GestureYou")
	
	
	mocapLReachMotions = StringVec();
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachBackFloor01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachBackHigh01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachBackLow01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachBackMediumFar01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachBackMediumMid01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachBackMediumNear01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachForwardFloor01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachForwardHigh01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachForwardLow01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachForwardMediumFar01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachForwardMediumMid01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachLeft30Floor01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachLeft30High01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachLeft30Low01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachLeft30MediumFar01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachLeft30MediumMid01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachLeft30MediumNear01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachLeft60Floor01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachLeft60High01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachLeft60Low01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachLeft60MediumFar01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachLeft60MediumMid01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachLeft60MediumNear01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachRight120Floor01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachRight120High01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachRight120Low01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachRight120MediumFar01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachRight120MediumMid01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachRight120MediumNear01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachRight30Floor01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachRight30High01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachRight30Low01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachRight30MediumFar01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachRight30MediumMid01")
	mocapLReachMotions.append("ChrGarza@IdleStand01_LReachRight30MediumNear01")
	
	mocapLReachMotions.append("ChrHarmony_Relax001_LHandGraspSmSphere_Grasp")	
	mocapLReachMotions.append("ChrHarmony_Relax001_LHandGraspSmSphere_Reach")	
	mocapLReachMotions.append("ChrHarmony_Relax001_LHandGraspSmSphere_Release")	
	mocapLReachMotions.append("HandsAtSide_LArm_GestureYou")


	# mirror the right hand motions to the left hand side
	for i in range(0,len(mocapRReachMotions)):
		motion = scene.getMotion(mocapRReachMotions[i])
		if motion == None:
			assetManager.loadAsset(motionPath+mocapRReachMotions[i]+'.skm')
			motion = scene.getMotion(mocapRReachMotions[i])
		#print 'motionName = ' + locoMotions[i]
		if motion != None:
			motion.setMotionSkeletonName("ChrGarza.sk")
			zebra2Map.applyMotion(motion)		
		mirrorMotion1 = scene.getMotion(mocapRReachMotions[i])
		mirrorMotion1.mirror(mocapLReachMotions[i], "ChrGarza.sk")	
		
	zebra2Map.applyMotion(scene.getMotion("ChrGarza@IdleStand01"))


def retargetBehaviorSet(charName):
	mocapReachMotions = StringVec();
	mocapReachMotions.append("ChrGarza@IdleStand01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachBackFloor01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachBackHigh01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachBackLow01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachBackMediumFar01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachBackMediumMid01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachBackMediumNear01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachForwardFloor01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachForwardHigh01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachForwardLow01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachForwardMediumFar01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachForwardMediumMid01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachLeft30Floor01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachLeft30High01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachLeft30Low01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachLeft30MediumFar01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachLeft30MediumMid01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachLeft30MediumNear01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachLeft60Floor01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachLeft60High01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachLeft60Low01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachLeft60MediumFar01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachLeft60MediumMid01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachLeft60MediumNear01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachRight120Floor01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachRight120High01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachRight120Low01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachRight120MediumFar01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachRight120MediumMid01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachRight120MediumNear01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachRight30Floor01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachRight30High01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachRight30Low01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachRight30MediumFar01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachRight30MediumMid01")
	mocapReachMotions.append("ChrGarza@IdleStand01_ReachRight30MediumNear01")
	
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachBackFloor01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachBackHigh01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachBackLow01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachBackMediumFar01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachBackMediumMid01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachBackMediumNear01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachForwardFloor01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachForwardHigh01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachForwardLow01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachForwardMediumFar01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachForwardMediumMid01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachLeft30Floor01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachLeft30High01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachLeft30Low01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachLeft30MediumFar01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachLeft30MediumMid01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachLeft30MediumNear01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachLeft60Floor01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachLeft60High01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachLeft60Low01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachLeft60MediumFar01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachLeft60MediumMid01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachLeft60MediumNear01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachRight120Floor01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachRight120High01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachRight120Low01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachRight120MediumFar01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachRight120MediumMid01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachRight120MediumNear01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachRight30Floor01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachRight30High01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachRight30Low01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachRight30MediumFar01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachRight30MediumMid01")
	mocapReachMotions.append("ChrGarza@IdleStand01_LReachRight30MediumNear01")	
	
	grabMotions = StringVec()
	grabMotions.append("ChrHarmony_Relax001_HandGraspSmSphere_Grasp")
	grabMotions.append("ChrHarmony_Relax001_LHandGraspSmSphere_Grasp")	
	
	grabMotions.append("ChrHarmony_Relax001_HandGraspSmSphere_Reach")
	grabMotions.append("ChrHarmony_Relax001_LHandGraspSmSphere_Reach")	
	
	grabMotions.append("ChrHarmony_Relax001_HandGraspSmSphere_Release")
	grabMotions.append("ChrHarmony_Relax001_LHandGraspSmSphere_Release")	
	
	grabMotions.append("HandsAtSide_LArm_GestureYou")
	grabMotions.append("HandsAtSide_RArm_GestureYou")
	
	#outDir = scene.getMediaPath() + '/retarget/motion/' + skelName + '/';
	#print 'outDir = ' + outDir ;
	#if not os.path.exists(outDir):
	#	os.makedirs(outDir)
	sbChar = scene.getCharacter(charName)
	if sbChar == None:
		return
	skelName = sbChar.getSkeleton().getName()
	
	createRetargetInstance('ChrGarza.sk', skelName)
	
	assetManager = scene.getAssetManager()
	for i in range(0, len(mocapReachMotions)):		
		sbMotion = assetManager.getMotion(mocapReachMotions[i])
		if sbMotion != None:
			sbMotion.setMotionSkeletonName('ChrGarza.sk')

	for i in range(0, len(grabMotions)):		
		sbMotion = assetManager.getMotion(grabMotions[i])
		if sbMotion != None:
			sbMotion.setMotionSkeletonName('common.sk')
			
	# retarget mocap reaching
	#for n in range(0, len(mocapReachMotions)):
	#	motion = scene.getMotion(mocapReachMotions[n])
	#	if motion is not None:
	#		retargetMotion(mocapReachMotions[n], 'ChrGarza.sk', skelName, outDir + 'MocapReaching/');
	#	else:
	#		print "Cannot find motion " + mocapReachMotions[n] + ", it will be excluded from the reach setup..."

	scene.run("init-example-reach-mocap.py")
	reachSetup(charName, "KNN", 'ChrGarza.sk', '')
		


		