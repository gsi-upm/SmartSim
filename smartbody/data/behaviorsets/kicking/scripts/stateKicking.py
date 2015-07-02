def kickingSetup(origSkelName, skeletonName, baseJoint, prefix, statePreFix):
	blendManager = scene.getBlendManager()

	blendKicking = blendManager.createMotionBlendBase(prefix + "Kick", skeletonName, 3)
	blendKicking.setBlendSkeleton(skeletonName)

	originalMotions = StringVec()
	originalMotions.append( "ChrGarza@IdleFight01_KickBackHigh01")
	originalMotions.append( "ChrGarza@IdleFight01_KickBackLow01")
	originalMotions.append( "ChrGarza@IdleFight01_KickBackMedium01")
	originalMotions.append( "ChrGarza@IdleFight01_KickForwardHigh01")
	originalMotions.append( "ChrGarza@IdleFight01_KickForwardLow01")
	originalMotions.append( "ChrGarza@IdleFight01_KickForwardMedium01")
	originalMotions.append( "ChrGarza@IdleFight01_KickLeftSideHigh01")
	originalMotions.append( "ChrGarza@IdleFight01_KickLeftSideLow01")
	originalMotions.append( "ChrGarza@IdleFight01_KickLeftSideMedium01")
	originalMotions.append( "ChrGarza@IdleFight01_KickRightSideHigh01")
	originalMotions.append( "ChrGarza@IdleFight01_KickRightSideLow01")
	originalMotions.append( "ChrGarza@IdleFight01_KickRightSideMedium01")
	
	motions = StringVec()
	assetManager = scene.getAssetManager()
	for i in range(0, len(originalMotions)):
		motions.append(prefix + originalMotions[i])
		sbMotion = assetManager.getMotion(originalMotions[i])
		if sbMotion != None:
			sbMotion.setMotionSkeletonName(origSkelName)
	
	para = DoubleVec();
	for i in range(0,3):
		para.append(0)
	para.append(0)
	for i in range(0, len(motions)):
		blendKicking.addMotion(motions[i], para)		
		
	points0 = DoubleVec()
	points0.append(0) # ChrGarza@IdleFight01_KickBackHigh01 0
	points0.append(0) # ChrGarza@IdleFight01_KickBackLow01 0
	points0.append(0) # ChrGarza@IdleFight01_KickBackMedium01 0
	points0.append(0) # ChrGarza@IdleFight01_KickForwardHigh01 0
	points0.append(0) # ChrGarza@IdleFight01_KickForwardLow01 0
	points0.append(0) # ChrGarza@IdleFight01_KickForwardMedium01 0
	points0.append(0) # ChrGarza@IdleFight01_KickLeftSideHigh01 0
	points0.append(0) # ChrGarza@IdleFight01_KickLeftSideLow01 0
	points0.append(0) # ChrGarza@IdleFight01_KickLeftSideMedium01 0
	points0.append(0) # ChrGarza@IdleFight01_KickRightSideHigh01 0
	points0.append(0) # ChrGarza@IdleFight01_KickRightSideLow01 0
	points0.append(0) # ChrGarza@IdleFight01_KickRightSideMedium01 0
	blendKicking.addCorrespondencePoints(motions, points0)
	points1 = DoubleVec()
	points1.append(2.45) # ChrGarza@IdleFight01_KickBackHigh01 1
	points1.append(2.65504) # ChrGarza@IdleFight01_KickBackLow01 1
	points1.append(2.93741) # ChrGarza@IdleFight01_KickBackMedium01 1
	points1.append(2.27813) # ChrGarza@IdleFight01_KickForwardHigh01 1
	points1.append(2.21763) # ChrGarza@IdleFight01_KickForwardLow01 1
	points1.append(2.04748) # ChrGarza@IdleFight01_KickForwardMedium01 1
	points1.append(2.02763) # ChrGarza@IdleFight01_KickLeftSideHigh01 1
	points1.append(1.78496) # ChrGarza@IdleFight01_KickLeftSideLow01 1
	points1.append(2.30662) # ChrGarza@IdleFight01_KickLeftSideMedium01 1
	points1.append(1.98374) # ChrGarza@IdleFight01_KickRightSideHigh01 1
	points1.append(2.55) # ChrGarza@IdleFight01_KickRightSideLow01 1
	points1.append(2.12374) # ChrGarza@IdleFight01_KickRightSideMedium01 1
	blendKicking.addCorrespondencePoints(motions, points1)
	points2 = DoubleVec()
	points2.append(4.63333) # ChrGarza@IdleFight01_KickBackHigh01 2
	points2.append(5.96667) # ChrGarza@IdleFight01_KickBackLow01 2
	points2.append(5.93333) # ChrGarza@IdleFight01_KickBackMedium01 2
	points2.append(4.8) # ChrGarza@IdleFight01_KickForwardHigh01 2
	points2.append(4.66667) # ChrGarza@IdleFight01_KickForwardLow01 2
	points2.append(3.96667) # ChrGarza@IdleFight01_KickForwardMedium01 2
	points2.append(4.66667) # ChrGarza@IdleFight01_KickLeftSideHigh01 2
	points2.append(3.3) # ChrGarza@IdleFight01_KickLeftSideLow01 2
	points2.append(4.4) # ChrGarza@IdleFight01_KickLeftSideMedium01 2
	points2.append(4.3) # ChrGarza@IdleFight01_KickRightSideHigh01 2
	points2.append(4.63333) # ChrGarza@IdleFight01_KickRightSideLow01 2
	points2.append(4.3) # ChrGarza@IdleFight01_KickRightSideMedium01 2
	blendKicking.addCorrespondencePoints(motions, points2)
	
	blendKicking.buildBlendBase("kick", "RBF" ,True);	
	
	

