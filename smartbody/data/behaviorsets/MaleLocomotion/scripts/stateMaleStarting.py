def startingSetup(origSkelName, skeletonName, baseJoint, preFix, statePreFix):
	
	mirrorMotion = scene.getMotion("ChrUtah_StopToWalkRt01")
	mirrorMotion.mirror("ChrUtah_StopToWalkLf01", skeletonName)
	
	mirrorMotion1 = scene.getMotion("ChrUtah_Idle01_ToWalk01_Turn90Lf01")
	mirrorMotion1.mirror("ChrUtah_Idle01_ToWalk01_Turn90Rt01", skeletonName)
	mirrorMotion2 = scene.getMotion("ChrUtah_Idle01_ToWalk01_Turn180Lf01")
	mirrorMotion2.mirror("ChrUtah_Idle01_ToWalk01_Turn180Rt01", skeletonName)

	originalMotions1 = StringVec()
	originalMotions2 = StringVec()
	originalMotions1.append("ChrUtah_StopToWalkLf01")
	originalMotions1.append("ChrUtah_Idle01_ToWalk01_Turn90Lf01")
	originalMotions1.append("ChrUtah_Idle01_ToWalk01_Turn180Lf01")
	originalMotions2.append("ChrUtah_StopToWalkRt01")
	originalMotions2.append("ChrUtah_Idle01_ToWalk01_Turn90Rt01")
	originalMotions2.append("ChrUtah_Idle01_ToWalk01_Turn180Rt01")
	
	motions1 = StringVec()
	motions2 = StringVec()
	assetManager = scene.getAssetManager()
	for i in range(0, len(originalMotions1)):
		motions1.append(preFix + originalMotions1[i])
		motions2.append(preFix + originalMotions2[i])
		sbMotion1 = assetManager.getMotion(originalMotions1[i])
		if sbMotion1 != None:
			sbMotion1.setMotionSkeletonName(origSkelName)
			sbMotion1.buildJointTrajectory('l_forefoot','base')
			sbMotion1.buildJointTrajectory('r_forefoot','base')
			sbMotion1.buildJointTrajectory('l_ankle','base')
			sbMotion1.buildJointTrajectory('r_ankle','base')
		sbMotion2 = assetManager.getMotion(originalMotions2[i])
		if sbMotion2 != None:
			sbMotion2.setMotionSkeletonName(origSkelName)
			sbMotion2.buildJointTrajectory('l_forefoot','base')
			sbMotion2.buildJointTrajectory('r_forefoot','base')
			sbMotion2.buildJointTrajectory('l_ankle','base')
			sbMotion2.buildJointTrajectory('r_ankle','base')
		
	stateManager = scene.getStateManager()

	print "** State: allStartingLeft"
	startLefStateName = statePreFix+"StartingLeft"
	if (stateManager.getBlend(startLefStateName) == None): # don't create duplicate state		
		state1 = stateManager.createState1D(statePreFix+"StartingLeft")
		state1.setBlendSkeleton(origSkelName)
		
		# motions1.append("ChrUtah_StopToWalkLf01")
		# motions1.append("ChrUtah_Idle01_ToWalk01_Turn90Lf01")
		# motions1.append("ChrUtah_Idle01_ToWalk01_Turn180Lf01")
		params1 = DoubleVec()
		params1.append(0)
		params1.append(-90)
		params1.append(-180)

		for i in range(0, len(motions1)):
			state1.addMotion(motions1[i], params1[i])

		points1 = DoubleVec()
		points1.append(0)
		points1.append(0)
		points1.append(0)
		state1.addCorrespondancePoints(motions1, points1)
		points2 = DoubleVec()
		points2.append(0.57541)
		points2.append(1.2)
		points2.append(1.35)
		state1.addCorrespondancePoints(motions1, points2)
		points3 = DoubleVec()
		points3.append(0.943716)
		points3.append(1.41)
		points3.append(1.6)
		state1.addCorrespondancePoints(motions1, points3)

	print "** State: allStartingRight"
	
	startRightStateName = statePreFix+"StartingRight"
	if (stateManager.getBlend(startRightStateName) == None):
		state2 = stateManager.createState1D(statePreFix+"StartingRight")
		state2.setBlendSkeleton(origSkelName)
		
		# motions2.append("ChrUtah_StopToWalkRt01")
		# motions2.append("ChrUtah_Idle01_ToWalk01_Turn90Rt01")
		# motions2.append("ChrUtah_Idle01_ToWalk01_Turn180Rt01")
		params2 = DoubleVec()
		params2.append(0)
		params2.append(90)
		params2.append(180)

		for i in range(0, len(motions2)):
			state2.addMotion(motions2[i], params2[i])

		# Since the right is symetric with the left, so the correspondance points are the same
		state2.addCorrespondancePoints(motions2, points1)
		state2.addCorrespondancePoints(motions2, points2)
		state2.addCorrespondancePoints(motions2, points3)

