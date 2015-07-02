def idleTurnSetup(origSkelName, skeletonName, baseJoint, preFix, statePreFix):
	print "** State: allIdleTurn"
	stateManager = scene.getStateManager()	
	stateName = statePreFix+"IdleTurn"
	if (stateManager.getBlend(stateName) != None): # don't create duplicate state
		return	
	state = stateManager.createState1D(stateName)
	state.setBlendSkeleton(origSkelName)
	originalMotions = StringVec()
	originalMotions.append("ChrUtah_Idle001")
	originalMotions.append("ChrUtah_Turn90Lf01")
	originalMotions.append("ChrUtah_Turn180Lf01")
	originalMotions.append("ChrUtah_Turn90Rt01")
	originalMotions.append("ChrUtah_Turn180Rt01")
	motions = StringVec()
	assetManager = scene.getAssetManager()
	for i in range(0, len(originalMotions)):
		motions.append(preFix + originalMotions[i])
		sbMotion = assetManager.getMotion(originalMotions[i])
		if sbMotion != None:
			sbMotion.setMotionSkeletonName(origSkelName)
			
	params = DoubleVec()
	params.append(0)
	params.append(-90)
	params.append(-180)
	params.append(90)
	params.append(180)

	for i in range(0, len(motions)):
		state.addMotion(motions[i], params[i])

	points1 = DoubleVec()
	points1.append(0)
	points1.append(0)
	points1.append(0)
	points1.append(0)
	points1.append(0)
	state.addCorrespondancePoints(motions, points1)
	points2 = DoubleVec()
	points2.append(0.255738)
	points2.append(0.762295)
	points2.append(0.87541)
	points2.append(0.757377)
	points2.append(0.821311)
	state.addCorrespondancePoints(motions, points2)
	points3 = DoubleVec()
	points3.append(0.633333)
	points3.append(1.96667)
	points3.append(2.46667)
	points3.append(1.96667)
	points3.append(2.46667)
	state.addCorrespondancePoints(motions, points3)