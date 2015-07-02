def femaleStartingSetup(skeletonName, baseJoint, preFix, statePreFix):
	#mirrorMotion = scene.getMotion("ChrHarmony@Idle01_ToWalkRt01")
	#mirrorMotion.mirror("ChrHarmony@Idle01_ToWalkLf01", "ChrHarmony.sk")
	#mirrorMotion1 = scene.getMotion("ChrHarmony@Idle01_ToWalk01_Turn90Rt01")
	#mirrorMotion1.mirror("ChrHarmony@Idle01_ToWalk01_Turn90Lf01", "ChrHarmony.sk")
	#mirrorMotion2 = scene.getMotion("ChrHarmony@Idle01_ToWalk01_Turn180Rt01")
	#mirrorMotion2.mirror("ChrHarmony@Idle01_ToWalk01_Turn180Lf01", "ChrHarmony.sk")


	stateManager = scene.getStateManager()

	print "** State: allStartingLeft"
	state1 = stateManager.createState1D(statePreFix + "StartingLeft")
	state1.setBlendSkeleton('ChrHarmony.sk')
	motions1 = StringVec()
	motions1.append(preFix + "ChrHarmony@Idle01_ToWalkLf01")
	motions1.append(preFix + "ChrHarmony@Idle01_ToWalk01_Turn90Lf01")
	motions1.append(preFix + "ChrHarmony@Idle01_ToWalk01_Turn180Lf01")
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
#	points2.append(0.71)
#	points2.append(0.82)
#	points2.append(0.90)
	points2.append(0.76)
	points2.append(0.82)
	points2.append(0.94)
	state1.addCorrespondancePoints(motions1, points2)
	points3 = DoubleVec()
#	points3.append(0.943716)
#	points3.append(1.24)
#	points3.append(1.48)
	points3.append(1.37)
	points3.append(1.64)
	points3.append(2.06)
	state1.addCorrespondancePoints(motions1, points3)
	
	points4 = DoubleVec()
#	points3.append(0.943716)
#	points3.append(1.24)
#	points3.append(1.48)
	points4.append(2.06)
	points4.append(2.10)
	points4.append(2.59)
	state1.addCorrespondancePoints(motions1, points4)

	print "** State: allStartingRight"
	state2 = stateManager.createState1D(statePreFix + "StartingRight")
	state2.setBlendSkeleton('ChrHarmony.sk')
	motions2 = StringVec()
	motions2.append(preFix + "ChrHarmony@Idle01_ToWalkRt01")
	motions2.append(preFix + "ChrHarmony@Idle01_ToWalk01_Turn90Rt01")
	motions2.append(preFix + "ChrHarmony@Idle01_ToWalk01_Turn180Rt01")
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
	state2.addCorrespondancePoints(motions2, points4)

