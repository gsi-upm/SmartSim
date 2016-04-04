def stepSetup(origSkelName, skeletonName, baseJoint, preFix, statePreFix):
	stateManager = scene.getStateManager()

	print "** State: allStep"
	stateName = statePreFix+"Step"
	if (stateManager.getBlend(stateName) != None): # don't create duplicate state
		return				
	state = stateManager.createState2D(stateName)
	state.setBlendSkeleton(origSkelName)
	# add motions
	originalMotions = StringVec()
	originalMotions.append("ChrUtah_Idle001")
	originalMotions.append("ChrUtah_Idle01_StepBackwardRt01")
	originalMotions.append("ChrUtah_Idle01_StepForwardRt01")
	originalMotions.append("ChrUtah_Idle01_StepSidewaysRt01")
	originalMotions.append("ChrUtah_Idle01_StepBackwardLf01")
	originalMotions.append("ChrUtah_Idle01_StepForwardLf01")
	originalMotions.append("ChrUtah_Idle01_StepSidewaysLf01")
	
	motions = StringVec()
	assetManager = scene.getAssetManager()
	for i in range(0, len(originalMotions)):
		motions.append(preFix + originalMotions[i])
		sbMotion = assetManager.getMotion(originalMotions[i])
		if sbMotion != None:
			sbMotion.setMotionSkeletonName(origSkelName)
			
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
		#print 'motion = ' + motions[i]
		motion = scene.getMotion(motions[i])
		motion.connect(skeleton)
		correspondancePoints = state.getCorrespondancePoints(i)
		lenCorrespondancePoints = len(correspondancePoints)
		jointTransition = motion.getJointTransition(joint, correspondancePoints[0], correspondancePoints[lenCorrespondancePoints - 1])
		state.setParameter(motions[i], jointTransition[0], jointTransition[2])
		motion.disconnect()

	# add triangles
	state.addTriangle(preFix+"ChrUtah_Idle001", preFix+"ChrUtah_Idle01_StepBackwardLf01", preFix+"ChrUtah_Idle01_StepSidewaysLf01")
	state.addTriangle(preFix+"ChrUtah_Idle001", preFix+"ChrUtah_Idle01_StepForwardLf01", preFix+"ChrUtah_Idle01_StepSidewaysLf01")
	state.addTriangle(preFix+"ChrUtah_Idle001", preFix+"ChrUtah_Idle01_StepBackwardRt01", preFix+"ChrUtah_Idle01_StepSidewaysRt01")
	state.addTriangle(preFix+"ChrUtah_Idle001", preFix+"ChrUtah_Idle01_StepForwardRt01", preFix+"ChrUtah_Idle01_StepSidewaysRt01")
	