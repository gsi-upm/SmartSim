scene.run("BehaviorSetCommon.py")
def setupBehaviorSet():
	scene = getScene()	
	print "Setting up behavior set for Reaching ..."
	#scene.loadAssetsFromPath("behaviorsets/reaching/skeletons")
	#scene.loadAssetsFromPath("behaviorsets/reaching/motions")
	scene.addAssetPath("script", "behaviorsets/reaching/scripts")
	
	
	assetManager = scene.getAssetManager()	
	motionPath = "behaviorsets/reaching/motions/"
	skel = scene.getSkeleton("common.sk")
	if skel == None:
		scene.loadAssetsFromPath("behaviorsets/reaching/skeletons")
		
	commonSk = scene.getSkeleton("common.sk")
	# mirror all arm and hand motions
	preFix = ""
	rightHandMotions = StringVec();
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachRtHigh")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachRtMidHigh")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachRtMidLow")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachLfLow")
	
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachLfHigh")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachLfMidHigh")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachRtMidLow")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachRtLow")
	
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachMiddleHigh")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachMiddleMidHigh")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachMiddleMidLow")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachMiddleLow")
	
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachClose_Lf")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachClose_Rt")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachClose_MiddleHigh")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachClose_MiddleLow")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachClose_MiddleMidHigh")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachClose_MiddleMidLow")
	
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachBehind_High1")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachBehind_High2")	
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachBehind_Low1")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachBehind_Low2")	
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachBehind_MidHigh1")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachBehind_MidHigh2")	
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachBehind_MidLow1")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachBehind_MidLow2")	
	
	rightHandMotions.append("ChrHarmony_Relax001_HandGraspSmSphere_Grasp")
	rightHandMotions.append("ChrHarmony_Relax001_HandGraspSmSphere_Reach")
	rightHandMotions.append("ChrHarmony_Relax001_HandGraspSmSphere_Release")
	rightHandMotions.append("HandsAtSide_RArm_GestureYou")
	
	leftHandMotions = StringVec();
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachRtHigh")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachRtMidHigh")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachRtMidLow")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachLfLow")
	
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachLfHigh")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachLfMidHigh")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachRtMidLow")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachRtLow")
	
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachMiddleHigh")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachMiddleMidHigh")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachMiddleMidLow")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachMiddleLow")
	
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachClose_Lf")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachClose_Rt")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachClose_MiddleHigh")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachClose_MiddleLow")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachClose_MiddleMidHigh")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachClose_MiddleMidLow")
	
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachBehind_High1")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachBehind_High2")	
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachBehind_Low1")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachBehind_Low2")	
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachBehind_MidHigh1")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachBehind_MidHigh2")	
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachBehind_MidLow1")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachBehind_MidLow2")	
	
	leftHandMotions.append("ChrHarmony_Relax001_LHandGraspSmSphere_Grasp")	
	leftHandMotions.append("ChrHarmony_Relax001_LHandGraspSmSphere_Reach")	
	leftHandMotions.append("ChrHarmony_Relax001_LHandGraspSmSphere_Release")	
	leftHandMotions.append("HandsAtSide_LArm_GestureYou")	
	
	for i in range(0,len(rightHandMotions)):
		motion = scene.getMotion(rightHandMotions[i])
		if motion == None:
			assetManager.loadAsset(motionPath+rightHandMotions[i]+'.skm')
			motion = scene.getMotion(rightHandMotions[i])
		#print 'motionName = ' + locoMotions[i]
		if motion != None:
			motion.setMotionSkeletonName("common.sk")			
		mirrorMotion1 = scene.getMotion(rightHandMotions[i])
		mirrorMotion1.mirror(leftHandMotions[i], 'common.sk')		
	
	
def retargetBehaviorSet(charName):
	reachMotions = StringVec()
	reachMotions.append("ChrHarmony_Relax001_ArmReachRtHigh")
	reachMotions.append("ChrHarmony_Relax001_ArmReachRtMidHigh")
	reachMotions.append("ChrHarmony_Relax001_ArmReachRtMidLow")
	reachMotions.append("ChrHarmony_Relax001_ArmReachLfLow")
	reachMotions.append("ChrHarmony_Relax001_ArmReachLfHigh")
	reachMotions.append("ChrHarmony_Relax001_ArmReachLfMidHigh")
	reachMotions.append("ChrHarmony_Relax001_ArmReachRtMidLow")
	reachMotions.append("ChrHarmony_Relax001_ArmReachRtLow")
	reachMotions.append("ChrHarmony_Relax001_ArmReachMiddleHigh")
	reachMotions.append("ChrHarmony_Relax001_ArmReachMiddleMidHigh")
	reachMotions.append("ChrHarmony_Relax001_ArmReachMiddleMidLow")
	reachMotions.append("ChrHarmony_Relax001_ArmReachMiddleLow")
	reachMotions.append("ChrHarmony_Relax001_ArmReachClose_Lf")
	reachMotions.append("ChrHarmony_Relax001_ArmReachClose_Rt")
	reachMotions.append("ChrHarmony_Relax001_ArmReachClose_MiddleHigh")
	reachMotions.append("ChrHarmony_Relax001_ArmReachClose_MiddleLow")
	reachMotions.append("ChrHarmony_Relax001_ArmReachClose_MiddleMidHigh")
	reachMotions.append("ChrHarmony_Relax001_ArmReachClose_MiddleMidLow")
	reachMotions.append("ChrHarmony_Relax001_ArmReachBehind_High1")
	reachMotions.append("ChrHarmony_Relax001_ArmReachBehind_High2")	
	reachMotions.append("ChrHarmony_Relax001_ArmReachBehind_Low1")
	reachMotions.append("ChrHarmony_Relax001_ArmReachBehind_Low2")	
	reachMotions.append("ChrHarmony_Relax001_ArmReachBehind_MidHigh1")
	reachMotions.append("ChrHarmony_Relax001_ArmReachBehind_MidHigh2")	
	reachMotions.append("ChrHarmony_Relax001_ArmReachBehind_MidLow1")
	reachMotions.append("ChrHarmony_Relax001_ArmReachBehind_MidLow2")	
		
	reachMotions.append("ChrHarmony_Relax001_HandGraspSmSphere_Grasp")
	reachMotions.append("ChrHarmony_Relax001_HandGraspSmSphere_Reach")
	reachMotions.append("ChrHarmony_Relax001_HandGraspSmSphere_Release")
	reachMotions.append("HandsAtSide_RArm_GestureYou")
	
	reachMotions.append("ChrHarmony_Relax001_LArmReachRtHigh")
	reachMotions.append("ChrHarmony_Relax001_LArmReachRtMidHigh")
	reachMotions.append("ChrHarmony_Relax001_LArmReachRtMidLow")
	reachMotions.append("ChrHarmony_Relax001_LArmReachLfLow")	
	reachMotions.append("ChrHarmony_Relax001_LArmReachLfHigh")
	reachMotions.append("ChrHarmony_Relax001_LArmReachLfMidHigh")
	reachMotions.append("ChrHarmony_Relax001_LArmReachRtMidLow")
	reachMotions.append("ChrHarmony_Relax001_LArmReachRtLow")	
	reachMotions.append("ChrHarmony_Relax001_LArmReachMiddleHigh")
	reachMotions.append("ChrHarmony_Relax001_LArmReachMiddleMidHigh")
	reachMotions.append("ChrHarmony_Relax001_LArmReachMiddleMidLow")
	reachMotions.append("ChrHarmony_Relax001_LArmReachMiddleLow")	
	reachMotions.append("ChrHarmony_Relax001_LArmReachClose_Lf")
	reachMotions.append("ChrHarmony_Relax001_LArmReachClose_Rt")
	reachMotions.append("ChrHarmony_Relax001_LArmReachClose_MiddleHigh")
	reachMotions.append("ChrHarmony_Relax001_LArmReachClose_MiddleLow")
	reachMotions.append("ChrHarmony_Relax001_LArmReachClose_MiddleMidHigh")
	reachMotions.append("ChrHarmony_Relax001_LArmReachClose_MiddleMidLow")	
	reachMotions.append("ChrHarmony_Relax001_LArmReachBehind_High1")
	reachMotions.append("ChrHarmony_Relax001_LArmReachBehind_High2")	
	reachMotions.append("ChrHarmony_Relax001_LArmReachBehind_Low1")
	reachMotions.append("ChrHarmony_Relax001_LArmReachBehind_Low2")	
	reachMotions.append("ChrHarmony_Relax001_LArmReachBehind_MidHigh1")
	reachMotions.append("ChrHarmony_Relax001_LArmReachBehind_MidHigh2")	
	reachMotions.append("ChrHarmony_Relax001_LArmReachBehind_MidLow1")
	reachMotions.append("ChrHarmony_Relax001_LArmReachBehind_MidLow2")	
	
	reachMotions.append("ChrHarmony_Relax001_LHandGraspSmSphere_Grasp")	
	reachMotions.append("ChrHarmony_Relax001_LHandGraspSmSphere_Reach")	
	reachMotions.append("ChrHarmony_Relax001_LHandGraspSmSphere_Release")	
	reachMotions.append("HandsAtSide_LArm_GestureYou")
	
	
	
	#outDir = scene.getMediaPath() + 'retarget/motion/' + skelName + '/';
	#print 'outDir = ' + outDir ;
	#if not os.path.exists(outDir):
	#	os.makedirs(outDir)	
	# retarget reaching
	#for n in range(0, len(reachMotions)):
	#	retargetMotion(reachMotions[n], 'common.sk', skelName, outDir + 'Reaching/');	
	assetManager = scene.getAssetManager()
	for i in range(0, len(reachMotions)):		
		sbMotion = assetManager.getMotion(reachMotions[i])
		if sbMotion != None:
			sbMotion.setMotionSkeletonName('common.sk')
	
	sbChar = scene.getCharacter(charName)
	if sbChar == None:
		return
	skelName = sbChar.getSkeleton().getName()
	createRetargetInstance('common.sk', skelName)

	scene.run("init-reach.py")
	reachSetup(charName, "KNN", 'common.sk', '')	