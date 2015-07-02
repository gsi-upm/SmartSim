def femaleIdleTurnSetup(skeletonName, baseJoint, prefix, statePreFix):
	#mirrorMotion = scene.getMotion("ChrHarmony@Turn90Rt01")
	#mirrorMotion.mirror("ChrHarmony@Turn90Lf01", "ChrHarmony.sk")
	#mirrorMotion1 = scene.getMotion("ChrHarmony@Turn180Rt01")
	#mirrorMotion1.mirror("ChrHarmony@Turn180Lf01", "ChrHarmony.sk")


	stateManager = scene.getStateManager()

	print "** State: harmonyIdleTurn"
	stateName = statePreFix + "IdleTurn"
	state = stateManager.createState1D(stateName)
	state.setBlendSkeleton('ChrHarmony.sk')
	motions = StringVec()
	motions.append(prefix + "ChrHarmony@IdleHandOnHip01")
	motions.append(prefix + "ChrHarmony@Turn90Lf01")
	motions.append(prefix + "ChrHarmony@Turn180Lf01")
	motions.append(prefix + "ChrHarmony@Turn90Rt01")
	motions.append(prefix + "ChrHarmony@Turn180Rt01")
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

	