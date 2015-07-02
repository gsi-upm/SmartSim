scene.run("BehaviorSetCommon.py")

def setupBehaviorSet():
	print "Setting up behavior set for gestures..."
	#scene.loadAssetsFromPath("behaviorsets/FemaleGestures/skeletons")
	#scene.loadAssetsFromPath("behaviorsets/FemaleGestures/motions")
	scene.addAssetPath("script", "behaviorsets/FemaleGestures/scripts")
	
	assetManager = scene.getAssetManager()	
	motionPath = "behaviorsets/FemaleGestures/motions/"
	skel = scene.getSkeleton("ChrConnor.sk")
	if skel == None:
		scene.loadAssetsFromPath("behaviorsets/FemaleGestures/skeletons")
	# map the zebra2 skeleton
	scene.run("zebra2-map.py")
	zebra2Map = scene.getJointMapManager().getJointMap("zebra2")
	bradSkeleton = scene.getSkeleton("ChrConnor.sk")
	zebra2Map.applySkeleton(bradSkeleton)
	
	gestureMotions = StringVec()
	gestureMotions.append("ChrConnor@IdleStand01")
	gestureMotions.append("ChrConnor@IdleCross01_ThrowAwayRt01")
	gestureMotions.append("ChrConnor@BeatFlipBt01")
	gestureMotions.append("ChrConnor@IdleStand01_BeatFlipRt01")
	gestureMotions.append("ChrConnor@IdleStand01_BeatForwardBt01")
	gestureMotions.append("ChrConnor@IdleStand01_BeatForwardRt01")
	gestureMotions.append("ChrConnor@IdleStand01_BeatMidBt01")
	gestureMotions.append("ChrConnor@IdleStand01_BeatMidRt01")
	gestureMotions.append("ChrConnor@IdleStand01_ChopRt01")
	gestureMotions.append("ChrConnor@IdleStand01_MeBt01")
	gestureMotions.append("ChrConnor@IdleStand01_MeRt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateForwardLowBt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateForwardLowRt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateForwardMidBt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateForwardMidRt01")
	gestureMotions.append("ChrConnor@IdleStand01_YouForwardBt01")

	gestureMotions.append("ChrConnor@IdleStand01_NegateLeftLowBt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateLeftLowRt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateLeftMidBt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateLeftMidRt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateRightLowBt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateRightLowRt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateRightMidBt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateRightMidRt01")
	gestureMotions.append("ChrConnor@IdleStand01_NoForwardBt01")	
	gestureMotions.append("ChrConnor@IdleStand01_NoForwardRt01")
	gestureMotions.append("ChrConnor@IdleStand01_NoLeftBt01")
	gestureMotions.append("ChrConnor@IdleStand01_NoLeftRt01")
	gestureMotions.append("ChrConnor@IdleStand01_NoRightBt01")
	gestureMotions.append("ChrConnor@IdleStand01_NoRightRt01")
	gestureMotions.append("ChrConnor@IdleStand01_ScrewYouBt01")
	gestureMotions.append("ChrConnor@IdleStand01_ScrewYouRt01")
	gestureMotions.append("ChrConnor@IdleStand01_StopForwardBt01")
	gestureMotions.append("ChrConnor@IdleStand01_StopForwardRt01")
	gestureMotions.append("ChrConnor@IdleStand01_StopLeftBt01")
	gestureMotions.append("ChrConnor@IdleStand01_StopLeftRt01")
	gestureMotions.append("ChrConnor@IdleStand01_StopRightBt01")
	gestureMotions.append("ChrConnor@IdleStand01_StopRightRt01")
	gestureMotions.append("ChrConnor@IdleStand01_YouForwardRt01")
	gestureMotions.append("ChrConnor@IdleStand01_YouLeftBt01")
	gestureMotions.append("ChrConnor@IdleStand01_YouLeftRt01")
	gestureMotions.append("ChrConnor@IdleStand01_YouRightBt01")
	gestureMotions.append("ChrConnor@IdleStand01_YouRightRt01")
	gestureMotions.append("ChrConnor@IdleUpright01_KickChair01")
	
	for i in range(0, len(gestureMotions)):
		motion = scene.getMotion(gestureMotions[i])	
		if motion == None:
			assetManager.loadAsset(motionPath+gestureMotions[i]+'.skm')
			motion = scene.getMotion(gestureMotions[i])		
		if motion != None:
			motion = scene.getMotion(gestureMotions[i])
			zebra2Map.applyMotion(motion)
		
	mirroredMotions = StringVec()
	
	mirroredMotions.append("ChrConnor@IdleCross01_ThrowAwayRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_BeatFlipRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_BeatForwardRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_BeatMidRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_ChopRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_MeRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_NegateForwardLowRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_NegateForwardMidRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_NegateLeftLowRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_NegateLeftMidRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_NegateRightLowRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_NegateRightMidRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_NoForwardRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_NoLeftRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_NoRightRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_ScrewYouRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_StopForwardRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_StopLeftRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_StopRightRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_YouForwardRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_YouLeftRt01")
	mirroredMotions.append("ChrConnor@IdleStand01_YouRightRt01")
	
	for i in range(0,len(mirroredMotions)):
		mirrorMotion = scene.getMotion(mirroredMotions[i])
		if mirrorMotion != None:
			mirrorMotion.mirror(mirroredMotions[i]+"Lf", "ChrConnor.sk")


def retargetBehaviorSet(charName):
	gestureMotions = StringVec()
	gestureMotions.append("ChrConnor@IdleStand01")
	gestureMotions.append("ChrConnor@IdleCross01_ThrowAwayRt01")
	gestureMotions.append("ChrConnor@IdleStand01_BeatFlipRt01")
	gestureMotions.append("ChrConnor@IdleStand01_BeatForwardRt01")
	gestureMotions.append("ChrConnor@IdleStand01_BeatMidRt01")
	gestureMotions.append("ChrConnor@IdleStand01_ChopRt01")
	gestureMotions.append("ChrConnor@IdleStand01_MeRt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateLeftLowBt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateLeftLowRt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateLeftMidBt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateLeftMidRt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateRightLowBt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateRightLowRt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateRightMidBt01")
	gestureMotions.append("ChrConnor@IdleStand01_NegateRightMidRt01")
	gestureMotions.append("ChrConnor@IdleStand01_NoForwardBt01")	
	gestureMotions.append("ChrConnor@IdleStand01_NoForwardRt01")
	gestureMotions.append("ChrConnor@IdleStand01_NoLeftBt01")
	gestureMotions.append("ChrConnor@IdleStand01_NoLeftRt01")
	gestureMotions.append("ChrConnor@IdleStand01_NoRightBt01")
	gestureMotions.append("ChrConnor@IdleStand01_NoRightRt01")
	gestureMotions.append("ChrConnor@IdleStand01_ScrewYouBt01")
	gestureMotions.append("ChrConnor@IdleStand01_ScrewYouRt01")
	gestureMotions.append("ChrConnor@IdleStand01_StopForwardBt01")
	gestureMotions.append("ChrConnor@IdleStand01_StopForwardRt01")
	gestureMotions.append("ChrConnor@IdleStand01_StopLeftBt01")
	gestureMotions.append("ChrConnor@IdleStand01_StopLeftRt01")
	gestureMotions.append("ChrConnor@IdleStand01_StopRightBt01")
	gestureMotions.append("ChrConnor@IdleStand01_StopRightRt01")
	gestureMotions.append("ChrConnor@IdleStand01_YouForwardRt01")
	gestureMotions.append("ChrConnor@IdleStand01_YouLeftBt01")
	gestureMotions.append("ChrConnor@IdleStand01_YouLeftRt01")
	gestureMotions.append("ChrConnor@IdleStand01_YouRightBt01")
	gestureMotions.append("ChrConnor@IdleStand01_YouRightRt01")
	gestureMotions.append("ChrConnor@IdleUpright01_KickChair01")
	
	# mirrored motions
	gestureMotions.append("ChrConnor@IdleCross01_ThrowAwayRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_BeatFlipRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_BeatForwardRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_BeatMidRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_ChopRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_MeRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_NegateForwardLowRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_NegateForwardMidRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_NegateLeftLowRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_NegateLeftMidRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_NegateRightLowRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_NegateRightMidRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_NoForwardRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_NoLeftRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_NoRightRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_ScrewYouRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_StopForwardRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_StopLeftRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_StopRightRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_YouForwardRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_YouLeftRt01Lf")
	gestureMotions.append("ChrConnor@IdleStand01_YouRightRt01Lf")
	
	#outDir = scene.getMediaPath() + '/retarget/motion/' +  '/';
	#print 'outDir = ' + outDir ;
	#if not os.path.exists(outDir):
	#	os.makedirs(outDir)
	
	sbChar = scene.getCharacter(charName)
	if sbChar == None:
		return
	skelName = sbChar.getSkeleton().getName()
	
	assetManager = scene.getAssetManager()	
	for i in range(0, len(gestureMotions)):
		sbMotion = assetManager.getMotion(gestureMotions[i])
		if sbMotion != None:
			sbMotion.setMotionSkeletonName("ChrConnor.sk")
	
	createRetargetInstance('ChrConnor.sk', skelName)			
	
	# set up the gesture map
	gestureMap = scene.getGestureMapManager().createGestureMap(charName)
	# you
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_YouForwardRt01", "YOU", "", "RIGHT_HAND", "", "ChrConnor@IdleStand01")
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_YouForwardRt01Lf", "YOU", "", "LEFT_HAND", "",  "ChrConnor@IdleStand01")
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_YouForwardBt01", "YOU", "", "BOTH_HANDS", "",  "ChrConnor@IdleStand01")

	# me
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_MeRt01", "ME", "", "RIGHT_HAND", "",  "ChrConnor@IdleStand01")
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_MeRt01Lf", "ME", "", "LEFT_HAND", "",  "ChrConnor@IdleStand01")
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_MeBt01", "ME", "", "BOTH_HANDS", "",  "ChrConnor@IdleStand01")

	# beat 
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_BeatForwardRt01", "BEAT", "", "RIGHT_HAND", "",  "ChrConnor@IdleStand01")
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_BeatForwardRt01Lf", "BEAT", "", "LEFT_HAND", "",  "ChrConnor@IdleStand01")
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_BeatForwardBt01", "BEAT", "", "BOTH_HANDS", "",  "ChrConnor@IdleStand01")
	
	# chop 
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_ChopRt01", "CHOP", "", "RIGHT_HAND", "",  "ChrConnor@IdleStand01")
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_ChopRt01Lf", "CHOP", "", "LEFT_HAND", "",  "ChrConnor@IdleStand01")
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_ChopBt01", "CHOP", "", "BOTH_HANDS", "",  "ChrConnor@IdleStand01")

	# negate 
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_NegateRightMidRt01", "NEGATE", "", "RIGHT_HAND", "",  "ChrConnor@IdleStand01")
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_NegateRightMidRt01Lf", "NEGATE", "", "LEFT_HAND", "",  "ChrConnor@IdleStand01")
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_NegateRightMidBt01", "NEGATE", "", "BOTH_HANDS", "",  "ChrConnor@IdleStand01")
	
	# no 
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_NoForwardRt01", "NO", "", "RIGHT_HAND", "",  "ChrConnor@IdleStand01")
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_NoForwardRt01Lf", "NO", "", "LEFT_HAND", "",  "ChrConnor@IdleStand01")
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_NoForwardBt01", "NO", "", "BOTH_HANDS", "",  "ChrConnor@IdleStand01")
		
	# stop 
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_StopRightRt01", "STOP", "", "RIGHT_HAND", "",  "ChrConnor@IdleStand01")
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_StopRightRt01Lf", "STOP", "", "LEFT_HAND", "",  "ChrConnor@IdleStand01")
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_StopRightBt01", "STOP", "", "BOTH_HANDS", "",  "ChrConnor@IdleStand01")
	
	# throw away 
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_ThrowAwayRt01", "THROWAWAY", "", "RIGHT_HAND", "",  "ChrConnor@IdleStand01")
	gestureMap.addGestureMapping( "ChrConnor@IdleStand01_ThrowAwayRt01Lf", "THROWAWAY", "", "LEFT_HAND", "",  "ChrConnor@IdleStand01")
	
	print "Setting gesture map attribute..."
	sbChar.setStringAttribute("gestureMap", charName)
		
	
	
	
	

		
