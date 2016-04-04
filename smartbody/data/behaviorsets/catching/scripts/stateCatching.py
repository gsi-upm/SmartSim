def catchingSetup(origSkelName, skeletonName, baseJoint, prefix, statePreFix):
	blendManager = scene.getBlendManager()

	assetManager = scene.getAssetManager()
	
	blendCatchingRight = blendManager.createMotionBlendBase(prefix + "CatchRight", skeletonName, 3)
	blendCatchingRight.setBlendSkeleton(skeletonName)

	rightMotions = StringVec()
	rightMotions.append("Center.bvh")
	rightMotions.append("Center2.bvh")
	rightMotions.append("Far_Center_Up.bvh")
	rightMotions.append("Far_Left.bvh")
	rightMotions.append("Far_Left2.bvh")
	rightMotions.append("Far_Low_Left.bvh")
	rightMotions.append("Far_Lower_Right.bvh")
	rightMotions.append("Far_Lower_Right2.bvh")
	rightMotions.append("Far_Right.bvh")
	rightMotions.append("Left.bvh")
	rightMotions.append("Low_Center.bvh")
	rightMotions.append("Low_Left.bvh")
	rightMotions.append("Low_Right.bvh")
	rightMotions.append("Low_Right2.bvh")
	rightMotions.append("Right.bvh")
	rightMotions.append("Up_Center.bvh")
	rightMotions.append("Up_Left.bvh")
	rightMotions.append("Up_Left_Center.bvh")
	rightMotions.append("Up_Right.bvh")
	rightMotions.append("Up_Right_Far.bvh")
	
	rmotions = StringVec()
	for i in range(0, len(rightMotions)):
		rmotions.append(prefix + rightMotions[i])
		sbMotion = assetManager.getMotion(rightMotions[i])
		if sbMotion != None:
			sbMotion.setMotionSkeletonName(origSkelName)
	
	para = DoubleVec();
	for i in range(0,3):
		para.append(0)
	
	for i in range(0, len(rmotions)):
		blendCatchingRight.addMotion(rmotions[i], para)		
	
			
			
	blendCatchingLeft = blendManager.createMotionBlendBase(prefix + "CatchLeft", skeletonName, 3)
	blendCatchingLeft.setBlendSkeleton(skeletonName)
	
	leftMotions = StringVec()
	leftMotions.append("Center.bvh" + "Lt")
	leftMotions.append("Center2.bvh" + "Lt")
	leftMotions.append("Far_Center_Up.bvh" + "Lt")
	leftMotions.append("Far_Left.bvh" + "Lt")
	leftMotions.append("Far_Left2.bvh" + "Lt")
	leftMotions.append("Far_Low_Left.bvh" + "Lt")
	leftMotions.append("Far_Lower_Right.bvh" + "Lt")
	leftMotions.append("Far_Lower_Right2.bvh" + "Lt")
	leftMotions.append("Far_Right.bvh" + "Lt")
	leftMotions.append("Left.bvh" + "Lt")
	leftMotions.append("Low_Center.bvh" + "Lt")
	leftMotions.append("Low_Left.bvh" + "Lt")
	leftMotions.append("Low_Right.bvh" + "Lt")
	leftMotions.append("Low_Right2.bvh" + "Lt")
	leftMotions.append("Right.bvh" + "Lt")
	leftMotions.append("Up_Center.bvh" + "Lt")
	leftMotions.append("Up_Left.bvh" + "Lt")
	leftMotions.append("Up_Left_Center.bvh" + "Lt")
	leftMotions.append("Up_Right.bvh" + "Lt")
	leftMotions.append("Up_Right_Far.bvh" + "Lt")
	
	lmotions = StringVec()
	for i in range(0, len(leftMotions)):
		lmotions.append(prefix + leftMotions[i])
		sbMotion = assetManager.getMotion(leftMotions[i])
		if sbMotion != None:
			sbMotion.setMotionSkeletonName(origSkelName)
			
	for i in range(0, len(lmotions)):
		blendCatchingLeft.addMotion(lmotions[i], para)			
		
	points0 = DoubleVec()
	points0.append(0) # Center 0
	points0.append(0) # Center2 0
	points0.append(0) # Far_Center_Up 0
	points0.append(0) # Far_Left 0
	points0.append(0) # Far_Left2 0
	points0.append(0) # Far_Low_Left 0
	points0.append(0) # Far_Lower_Right 0
	points0.append(0) # Far_Lower_Right2 0
	points0.append(0) # Far_Right 0
	points0.append(0) # Left 0
	points0.append(0) # Low_Center 0
	points0.append(0) # Low_Left 0
	points0.append(0) # Low_Right 0
	points0.append(0) # Low_Right2 0
	points0.append(0) # Right 0
	points0.append(0) # Up_Center 0
	points0.append(0) # Up_Left 0
	points0.append(0) # Up_Left_Center 0
	points0.append(0) # Up_Right 0
	points0.append(0) # Up_Right_Far 0
	blendCatchingRight.addCorrespondencePoints(motions, points0)
	blendCatchingLeft.addCorrespondencePoints(motions, points0)
	
	points1 = DoubleVec()
	points1.append(1.18) # Center 1
	points1.append(1.05) # Center2 1
	points1.append(.83) # Far_Center_Up 1
	points1.append(.91) # Far_Left 1
	points1.append(1.16) # Far_Left2 1
	points1.append(.68) # Far_Low_Left 1
	points1.append(1.32) # Far_Lower_Right 1
	points1.append(1.60) # Far_Lower_Right2 1
	points1.append(.89) # Far_Right 1
	points1.append(1.14) # Left 1
	points1.append(.92) # Low_Center 1
	points1.append(1.11) # Low_Left 1
	points1.append(1.40) # Low_Right 1
	points1.append(1.41) # Low_Right2 1
	points1.append(1.53) # Right 1
	points1.append(.83) # Up_Center 1
	points1.append(1.30) # Up_Left 1
	points1.append(.99) # Up_Left_Center 1
	points1.append(1.0) # Up_Right 1
	points1.append(.96) # Up_Right_Far 1
	blendCatchingRight.addCorrespondencePoints(motions, points1)
	blendCatchingLeft.addCorrespondencePoints(motions, points1)
	
	points2 = DoubleVec()
	points2.append(3.597) # Center 2
	points2.append(2.475) # Center2 2
	points2.append(2.277) # Far_Center_Up 2
	points2.append(1.848) # Far_Left 2
	points2.append(3.069) # Far_Left2 2
	points2.append(2.343) # Far_Low_Left 2
	points2.append(3.102) # Far_Lower_Right 2
	points2.append(3.399) # Far_Lower_Right2 2
	points2.append(1.551) # Far_Right 2
	points2.append(1.617) # Left 2
	points2.append(1.881) # Low_Center 2
	points2.append(2.772) # Low_Left 2
	points2.append(2.673) # Low_Right 2
	points2.append(2.574) # Low_Right2 2
	points2.append(5.148) # Right 2
	points2.append(2.013) # Up_Center 2
	points2.append(2.376) # Up_Left 2
	points2.append(1.947) # Up_Left_Center 2
	points2.append(3.168) # Up_Right 2
	points2.append(2.112) # Up_Right_Far 2
	blendCatchingRight.addCorrespondencePoints(motions, points2)
	blendCatchingLeft.addCorrespondencePoints(motions, points2)
	
	# generate the 3-dimensional parameters for the blend based
	# on the location of the catching point
	
	skeleton = scene.getSkeleton(skeletonName)
	rhand = skeleton.getJointByName("r_wrist")
	for r in range(0, 9):
		m = scene.getMotion(rightMotions[r])
		m.connect(skeleton)
		rpos = m.getJointPosition(rhand, points1[r])
		m.disconnect()
		params = DoubleVec()
		params.append(rpos.getData(0))
		params.append(rpos.getData(1))
		params.append(rpos.getData(2))
		blendCatchingRight.setParameter(rightMotions[r], params)	
#		print rightMotions[r] + " " + str(params[0]) + " " + str(params[1]) + " " +str(params[2])

	lhand = skeleton.getJointByName("l_wrist")
	for l in range(0, 9):
		m = scene.getMotion(leftMotions[l])
		m.connect(skeleton)
		lpos = m.getJointPosition(lhand, points1[l])
		m.disconnect()
		params = DoubleVec()
		params.append(lpos.getData(0))
		params.append(lpos.getData(1))
		params.append(lpos.getData(2))
		blendCatchingLeft.setParameter(leftMotions[l], params)
#		print leftMotions[l] + " " + str(params[0]) + " " + str(params[1]) + " " +str(params[2])
	
	#blendCatching.buildBlendBase("catch", "RBF" ,True);	
	#blendCatching.buildBlendBase("catch", "Inverse" ,True);	
	blendCatchingRight.buildBlendBase("catch", "KNN" ,True);	
	blendCatchingLeft.buildBlendBase("catch", "KNN" ,True);	
	
	

