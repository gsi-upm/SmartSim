def femaleStepSetup(skeletonName, baseJoint, preFix, statePreFix):
	#mirrorMotion = scene.getMotion("ChrHarmony@Idle01_StepBackwardRt01")
	#mirrorMotion.mirror("ChrHarmony@Idle01_StepBackwardLf01", "ChrHarmony.sk")
	#mirrorMotion1 = scene.getMotion("ChrHarmony@Idle01_StepForwardRt01")
	#mirrorMotion1.mirror("ChrHarmony@Idle01_StepForwardLf01", "ChrHarmony.sk")
	#mirrorMotion2 = scene.getMotion("ChrHarmony@Idle01_StepSidewaysRt01")
	#mirrorMotion2.mirror("ChrHarmony@Idle01_StepSidewaysLf01", "ChrHarmony.sk")

	stateManager = scene.getStateManager()

	print "** State: harmonyStep"
	stateName = statePreFix + "Step"
	state = stateManager.createState2D(stateName)
	state.setBlendSkeleton('ChrHarmony.sk')

	# add motions
	motions = StringVec()
	motions.append(preFix + "ChrHarmony@IdleHandOnHip01")
	motions.append(preFix + "ChrHarmony@Idle01_StepBackwardRt01")
	motions.append(preFix + "ChrHarmony@Idle01_StepForwardRt01")
	motions.append(preFix + "ChrHarmony@Idle01_StepSidewaysRt01")
	motions.append(preFix + "ChrHarmony@Idle01_StepBackwardLf01")
	motions.append(preFix + "ChrHarmony@Idle01_StepForwardLf01")
	motions.append(preFix + "ChrHarmony@Idle01_StepSidewaysLf01")
	for i in range(0, len(motions)):
		state.addMotion(motions[i], 0, 0)

	# add correspondance points
	points1 = DoubleVec()
	points1.append(0)
	points1.append(0)
	points1.append(0)
	points1.append(0)
	points1.append(0)
	points1.append(0)
	points1.append(0)
	state.addCorrespondancePoints(motions, points1)
	points2 = DoubleVec()
	points2.append(0.556322)
	points2.append(0.556322)
	points2.append(0.543678)
	points2.append(0.482989)
	points2.append(0.395402)
	points2.append(0.531034)
	points2.append(0.473563)
	state.addCorrespondancePoints(motions, points2)
	points3 = DoubleVec()
	points3.append(1.46414)
	points3.append(1.46414)
	points3.append(1.46414)
	points3.append(1.46414)
	points3.append(1.33333)
	points3.append(1.33333)
	points3.append(1.33103)
	state.addCorrespondancePoints(motions, points3)

	# reset parameters (because it needs context of correspondance points)
	skeleton = scene.getSkeleton(skeletonName)
	joint = skeleton.getJointByName(baseJoint)
	for i in range(0, len(motions)):
		motion = scene.getMotion(motions[i])
		motion.connect(skeleton)
		correspondancePoints = state.getCorrespondancePoints(i)
		lenCorrespondancePoints = len(correspondancePoints)
		jointTransition = motion.getJointTransition(joint, correspondancePoints[0], correspondancePoints[lenCorrespondancePoints - 1])
		state.setParameter(motions[i], jointTransition[0], jointTransition[2])
		motion.disconnect()

	# add triangles
	state.addTriangle(preFix + "ChrHarmony@IdleHandOnHip01", preFix + "ChrHarmony@Idle01_StepBackwardLf01", preFix + "ChrHarmony@Idle01_StepSidewaysLf01")
	state.addTriangle(preFix + "ChrHarmony@IdleHandOnHip01", preFix + "ChrHarmony@Idle01_StepForwardLf01", preFix + "ChrHarmony@Idle01_StepSidewaysLf01")
	state.addTriangle(preFix + "ChrHarmony@IdleHandOnHip01", preFix + "ChrHarmony@Idle01_StepBackwardRt01", preFix + "ChrHarmony@Idle01_StepSidewaysRt01")
	state.addTriangle(preFix + "ChrHarmony@IdleHandOnHip01", preFix + "ChrHarmony@Idle01_StepForwardRt01", preFix + "ChrHarmony@Idle01_StepSidewaysRt01")
