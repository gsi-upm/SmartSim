def jumpingSetup(origSkelName, skeletonName, baseJoint, prefix, statePreFix):
	blendManager = scene.getBlendManager()

	blendjumping = blendManager.createMotionBlendBase(prefix + "Jump", skeletonName, 3)
	
	blendjumping.setBlendSkeleton(skeletonName)

	originalMotions = StringVec()
	originalMotions.append("ChrGarza@IdleStand01_JumpForwardHighMid01")
	originalMotions.append("ChrGarza@IdleStand01_JumpForwardHighNear01")
	originalMotions.append("ChrGarza@IdleStand01_JumpForwardLowFar01")
	originalMotions.append("ChrGarza@IdleStand01_JumpForwardLowMid01")
	originalMotions.append("ChrGarza@IdleStand01_JumpForwardLowNear01")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft45HighMid01")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft45HighNear01")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft45LowFar01")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft45LowMid01")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft45LowNear01")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft90HighMid01")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft90LowFar01")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft90LowMid01")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft90LowNear01")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft45HighMid01"+"Rt")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft45HighNear01"+"Rt")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft45LowFar01"+"Rt")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft45LowMid01"+"Rt")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft45LowNear01"+"Rt")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft90HighMid01"+"Rt")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft90LowFar01"+"Rt")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft90LowMid01"+"Rt")
	originalMotions.append("ChrGarza@IdleStand01_JumpLeft90LowNear01"+"Rt")
	originalMotions.append("ChrGarza@IdleStand01_JumpUpHigh01")
	originalMotions.append("ChrGarza@IdleStand01_JumpUpLow01")
	originalMotions.append("ChrGarza@IdleStand01_JumpUpMedium01")
	
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
		blendjumping.addMotion(motions[i], para)
		
	points0 = DoubleVec()
	points0.append(0) # ChrGarza@IdleStand01_JumpForwardHighMid01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpForwardHighNear01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpForwardLowFar01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpForwardLowMid01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpForwardLowNear01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft45HighMid01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft45HighNear01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft45LowFar01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft45LowMid01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft45LowNear01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft90HighMid01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft90LowFar01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft90LowMid01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft90LowNear01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft45HighMid01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft45HighNear01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft45LowFar01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft45LowMid01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft45LowNear01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft90HighMid01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft90LowFar01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft90LowMid01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpLeft90LowNear01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpUpHigh01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpUpLow01 0
	points0.append(0) # ChrGarza@IdleStand01_JumpUpMedium01 0
	blendjumping.addCorrespondencePoints(motions, points0)
	points1 = DoubleVec()
	points1.append(2.42667) # ChrGarza@IdleStand01_JumpForwardHighMid01 1
	points1.append(2.59) # ChrGarza@IdleStand01_JumpForwardHighNear01 1
	points1.append(2.77667) # ChrGarza@IdleStand01_JumpForwardLowFar01 1
	points1.append(2.345) # ChrGarza@IdleStand01_JumpForwardLowMid01 1
	points1.append(2.24) # ChrGarza@IdleStand01_JumpForwardLowNear01 1
	points1.append(2.695) # ChrGarza@IdleStand01_JumpLeft45HighMid01 1
	points1.append(2.415) # ChrGarza@IdleStand01_JumpLeft45HighNear01 1
	points1.append(2.905) # ChrGarza@IdleStand01_JumpLeft45LowFar01 1
	points1.append(2.33333) # ChrGarza@IdleStand01_JumpLeft45LowMid01 1
	points1.append(2.15833) # ChrGarza@IdleStand01_JumpLeft45LowNear01 1
	points1.append(2.15833) # ChrGarza@IdleStand01_JumpLeft90HighMid01 1
	points1.append(2.205) # ChrGarza@IdleStand01_JumpLeft90LowFar01 1
	points1.append(2.135) # ChrGarza@IdleStand01_JumpLeft90LowMid01 1
	points1.append(2.15833) # ChrGarza@IdleStand01_JumpLeft90LowNear01 1
	points1.append(2.695) # ChrGarza@IdleStand01_JumpLeft45HighMid01 1
	points1.append(2.415) # ChrGarza@IdleStand01_JumpLeft45HighNear01 1
	points1.append(2.905) # ChrGarza@IdleStand01_JumpLeft45LowFar01 1
	points1.append(2.33333) # ChrGarza@IdleStand01_JumpLeft45LowMid01 1
	points1.append(2.15833) # ChrGarza@IdleStand01_JumpLeft45LowNear01 1
	points1.append(2.15833) # ChrGarza@IdleStand01_JumpLeft90HighMid01 1
	points1.append(2.205) # ChrGarza@IdleStand01_JumpLeft90LowFar01 1
	points1.append(2.135) # ChrGarza@IdleStand01_JumpLeft90LowMid01 1
	points1.append(2.15833) # ChrGarza@IdleStand01_JumpLeft90LowNear01 1
	points1.append(2.98667) # ChrGarza@IdleStand01_JumpUpHigh01 1
	points1.append(2.21667) # ChrGarza@IdleStand01_JumpUpLow01 1
	points1.append(2.45) # ChrGarza@IdleStand01_JumpUpMedium01 1
	blendjumping.addCorrespondencePoints(motions, points1)
	points2 = DoubleVec()
	points2.append(3.71228) # ChrGarza@IdleStand01_JumpForwardHighMid01 2
	points2.append(3.69314) # ChrGarza@IdleStand01_JumpForwardHighNear01 2
	points2.append(3.74492) # ChrGarza@IdleStand01_JumpForwardLowFar01 2
	points2.append(3.35657) # ChrGarza@IdleStand01_JumpForwardLowMid01 2
	points2.append(3.13494) # ChrGarza@IdleStand01_JumpForwardLowNear01 2
	points2.append(3.73921) # ChrGarza@IdleStand01_JumpLeft45HighMid01 2
	points2.append(3.52543) # ChrGarza@IdleStand01_JumpLeft45HighNear01 2
	points2.append(3.85163) # ChrGarza@IdleStand01_JumpLeft45LowFar01 2
	points2.append(3.24583) # ChrGarza@IdleStand01_JumpLeft45LowMid01 2
	points2.append(3.01368) # ChrGarza@IdleStand01_JumpLeft45LowNear01 2
	points2.append(3.25313) # ChrGarza@IdleStand01_JumpLeft90HighMid01 2
	points2.append(3.20607) # ChrGarza@IdleStand01_JumpLeft90LowFar01 2
	points2.append(2.9961) # ChrGarza@IdleStand01_JumpLeft90LowMid01 2
	points2.append(3.0284) # ChrGarza@IdleStand01_JumpLeft90LowNear01 2
	points2.append(3.73921) # ChrGarza@IdleStand01_JumpLeft45HighMid01 2
	points2.append(3.52543) # ChrGarza@IdleStand01_JumpLeft45HighNear01 2
	points2.append(3.85163) # ChrGarza@IdleStand01_JumpLeft45LowFar01 2
	points2.append(3.24583) # ChrGarza@IdleStand01_JumpLeft45LowMid01 2
	points2.append(3.01368) # ChrGarza@IdleStand01_JumpLeft45LowNear01 2
	points2.append(3.25313) # ChrGarza@IdleStand01_JumpLeft90HighMid01 2
	points2.append(3.20607) # ChrGarza@IdleStand01_JumpLeft90LowFar01 2
	points2.append(2.9961) # ChrGarza@IdleStand01_JumpLeft90LowMid01 2
	points2.append(3.0284) # ChrGarza@IdleStand01_JumpLeft90LowNear01 2
	points2.append(4.52539) # ChrGarza@IdleStand01_JumpUpHigh01 2
	points2.append(3.10069) # ChrGarza@IdleStand01_JumpUpLow01 2
	points2.append(3.50333) # ChrGarza@IdleStand01_JumpUpMedium01 2
	blendjumping.addCorrespondencePoints(motions, points2)
	points3 = DoubleVec()
	points3.append(5.3) # ChrGarza@IdleStand01_JumpForwardHighMid01 3
	points3.append(4.96667) # ChrGarza@IdleStand01_JumpForwardHighNear01 3
	points3.append(5.96667) # ChrGarza@IdleStand01_JumpForwardLowFar01 3
	points3.append(4.63333) # ChrGarza@IdleStand01_JumpForwardLowMid01 3
	points3.append(5.03333) # ChrGarza@IdleStand01_JumpForwardLowNear01 3
	points3.append(5.3) # ChrGarza@IdleStand01_JumpLeft45HighMid01 3
	points3.append(4.96667) # ChrGarza@IdleStand01_JumpLeft45HighNear01 3
	points3.append(5.8) # ChrGarza@IdleStand01_JumpLeft45LowFar01 3
	points3.append(4.63333) # ChrGarza@IdleStand01_JumpLeft45LowMid01 3
	points3.append(4.3) # ChrGarza@IdleStand01_JumpLeft45LowNear01 3
	points3.append(4.63333) # ChrGarza@IdleStand01_JumpLeft90HighMid01 3
	points3.append(4.63333) # ChrGarza@IdleStand01_JumpLeft90LowFar01 3
	points3.append(3.96667) # ChrGarza@IdleStand01_JumpLeft90LowMid01 3
	points3.append(4.46667) # ChrGarza@IdleStand01_JumpLeft90LowNear01 3
	points3.append(5.3) # ChrGarza@IdleStand01_JumpLeft45HighMid01 3
	points3.append(4.96667) # ChrGarza@IdleStand01_JumpLeft45HighNear01 3
	points3.append(5.8) # ChrGarza@IdleStand01_JumpLeft45LowFar01 3
	points3.append(4.63333) # ChrGarza@IdleStand01_JumpLeft45LowMid01 3
	points3.append(4.3) # ChrGarza@IdleStand01_JumpLeft45LowNear01 3
	points3.append(4.63333) # ChrGarza@IdleStand01_JumpLeft90HighMid01 3
	points3.append(4.63333) # ChrGarza@IdleStand01_JumpLeft90LowFar01 3
	points3.append(3.96667) # ChrGarza@IdleStand01_JumpLeft90LowMid01 3
	points3.append(4.46667) # ChrGarza@IdleStand01_JumpLeft90LowNear01 3
	points3.append(6.63333) # ChrGarza@IdleStand01_JumpUpHigh01 3
	points3.append(4.3) # ChrGarza@IdleStand01_JumpUpLow01 3
	points3.append(4.76667) # ChrGarza@IdleStand01_JumpUpMedium01 3
	blendjumping.addCorrespondencePoints(motions, points3)
	#for i in range(0, len(motions)):
	#	blendjumping.addMotion(motions[i], paramsX[i], paramsY[i], paramsZ[i])	
		
	blendjumping.buildBlendBase("jump", "RBF" ,True);	
	#blendjumping.buildBlendBase("jump", "KNN" ,True);	
	#blendjumping.buildBlendBase("jump", "Inverse" ,True);	
		

