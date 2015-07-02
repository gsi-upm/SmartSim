def swattingSetup(origSkelName, skeletonName, baseJoint, prefix, statePreFix):
	print "SKELETONNAME = " + skeletonName
	blendManager = scene.getBlendManager()

	assetManager = scene.getAssetManager()
	
	blendSwattingRight = blendManager.createMotionBlendBase(prefix + "SwatRight", skeletonName, 3)
	blendSwattingRight.setBlendSkeleton(skeletonName)

	rightMotions = StringVec()
	rightMotions.append("SwatCenter.bvh")
	rightMotions.append("SwatLeft.bvh")
	rightMotions.append("SwatLow_Far_Left.bvh")
	rightMotions.append("SwatLow_Right.bvh")
	rightMotions.append("SwatLow_Right_Far.bvh")
	rightMotions.append("SwatLow_Right2.bvh")
	rightMotions.append("SwatRight.bvh")
	rightMotions.append("SwatUp_Center.bvh")
	rightMotions.append("SwatUp_Center2.bvh") 
	rightMotions.append("SwatUp_Far_Center.bvh")
	rightMotions.append("SwatUp_Far_Left.bvh")
	rightMotions.append("SwatUp_Left.bvh")
	rightMotions.append("SwatUp_Right.bvh")
	
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
		blendSwattingRight.addMotion(rmotions[i], para)		
			
	blendSwattingLeft = blendManager.createMotionBlendBase(prefix + "SwatLeft", skeletonName, 3)
	blendSwattingLeft.setBlendSkeleton(skeletonName)
	
	leftMotions = StringVec()
	leftMotions.append("SwatCenter.bvh" + "Lt")
	leftMotions.append("SwatLeft.bvh" + "Lt")
	leftMotions.append("SwatLow_Far_Left.bvh" + "Lt")
	leftMotions.append("SwatLow_Right.bvh" + "Lt")
	leftMotions.append("SwatLow_Right_Far.bvh" + "Lt")
	leftMotions.append("SwatLow_Right2.bvh" + "Lt")
	leftMotions.append("SwatRight.bvh" + "Lt")
	leftMotions.append("SwatUp_Center.bvh" + "Lt")
	leftMotions.append("SwatUp_Center2.bvh" + "Lt")
	leftMotions.append("SwatUp_Far_Center.bvh" + "Lt")
	leftMotions.append("SwatUp_Far_Left.bvh" + "Lt")
	leftMotions.append("SwatUp_Left.bvh" + "Lt")
	leftMotions.append("SwatUp_Right.bvh" + "Lt")
	
	lmotions = StringVec()
	for i in range(0, len(leftMotions)):
		lmotions.append(prefix + leftMotions[i])
		sbMotion = assetManager.getMotion(leftMotions[i])
		if sbMotion != None:
			sbMotion.setMotionSkeletonName(origSkelName)
			
	for i in range(0, len(lmotions)):
		blendSwattingLeft.addMotion(lmotions[i], para)			
		
	points0 = DoubleVec()
	points0.append(0) # SwatCenter 0
	points0.append(0) # SwatLeft 0
	points0.append(0) # SwatLow_Far_Left 0
	points0.append(0) # SwatLow_Right 0
	points0.append(0) # SwatLow_Right_Far 0
	points0.append(0) # SwatLow_Right2 0
	points0.append(0) # SwatRight 0
	points0.append(0) # SwatUp_Center 0
	points0.append(0) # SwatUp_Center2 0
	points0.append(0) # SwatUp_Far_Center 0
	points0.append(0) # SwatUp_Far_Left 0
	points0.append(0) # SwatUp_Left 0
	points0.append(0) # SwatUp_Right 0
	blendSwattingRight.addCorrespondencePoints(motions, points0)
	blendSwattingLeft.addCorrespondencePoints(motions, points0)
	
	points1 = DoubleVec()
	points1.append(.70) # SwatCenter 1
	points1.append(1.42) # SwatLeft 1
	points1.append(1.06) # SwatLow_Far_Left 1
	points1.append(.58) # SwatLow_Right 1
	points1.append(.66) # SwatLow_Right_Far 1
	points1.append(.79) # SwatLow_Right2 1
	points1.append(.75) # SwatRight 1
	points1.append(.58) # SwatUp_Center 1
	points1.append(.67) # SwatUp_Center2 1
	points1.append(.89) # SwatUp_Far_Center 1
	points1.append(.67) # SwatUp_Far_Left 1
	points1.append(.96) # SwatUp_Left 1
	points1.append(.66) # SwatUp_Right 1
	blendSwattingRight.addCorrespondencePoints(motions, points1)
	blendSwattingLeft.addCorrespondencePoints(motions, points1)
	
	points2 = DoubleVec()
	points2.append(2.20) # SwatCenter 2
	points2.append(3.07) # SwatLeft 2
	points2.append(1.63) # SwatLow_Far_Left 2
	points2.append(1.93) # SwatLow_Right 2
	points2.append(2.27) # SwatLow_Right_Far 2
	points2.append(1.87) # SwatLow_Right2 2
	points2.append(1.97) # SwatRight 2
	points2.append(1.70) # SwatUp_Center 2
	points2.append(2.33) # SwatUp_Center2 2
	points2.append(2.33) # SwatUp_Far_Center 2
	points2.append(1.90) # SwatUp_Far_Left 2
	points2.append(2.23) # SwatUp_Left 2
	points2.append(1.57) # SwatUp_Right 2
	blendSwattingRight.addCorrespondencePoints(motions, points2)
	blendSwattingLeft.addCorrespondencePoints(motions, points2)
	
	# generate the 3-dimensional parameters for the blend based
	# on the location of the Swatting point
	
	skeleton = scene.getSkeleton(skeletonName)
	rhand = skeleton.getJointByName("r_wrist")
	for r in range(0, 12):
		m = scene.getMotion(rightMotions[r])
		m.connect(skeleton)
		rpos = m.getJointPosition(rhand, points1[r])
		m.disconnect()
		params = DoubleVec()
		params.append(rpos.getData(0))
		params.append(rpos.getData(1))
		params.append(rpos.getData(2))
		blendSwattingRight.setParameter(rightMotions[r], params)	
#		print rightMotions[r] + " " + str(params[0]) + " " + str(params[1]) + " " +str(params[2])

	lhand = skeleton.getJointByName("l_wrist")
	for l in range(0, 12):
		m = scene.getMotion(leftMotions[l])
		m.connect(skeleton)
		lpos = m.getJointPosition(lhand, points1[l])
		m.disconnect()
		params = DoubleVec()
		params.append(lpos.getData(0))
		params.append(lpos.getData(1))
		params.append(lpos.getData(2))
		blendSwattingLeft.setParameter(leftMotions[l], params)
#		print leftMotions[l] + " " + str(params[0]) + " " + str(params[1]) + " " +str(params[2])
	
	#blendSwatting.buildBlendBase("Swat", "RBF" ,True);	
	#blendSwatting.buildBlendBase("Swat", "Inverse" ,True);	
	blendSwattingRight.buildBlendBase("Swat", "KNN" ,True);	
	blendSwattingLeft.buildBlendBase("Swat", "KNN" ,True);

	# add 'concentration' events during the blend
	blendSwattingRight.addEvent(rightMotions[0], points1[0] - .50, "concentration", "", False)
	blendSwattingLeft.addEvent(leftMotions[0], points1[0] - .50, "concentration", "", False)
'''
	# strange crash in the GUI when activating the UI, disabled for now
	class SwatConcentrationEventHandler(SBEventHandler):
	   def executeAction(self, event):
		  str = event.getParameters()
		  print "Concentrate!"
		  bml.execBML('*', '<face amount="1" au="4" side="BOTH" type="facs"/>')

	myHandler = SwatConcentrationEventHandler()
	eventManager = scene.getEventManager()
	eventManager .addEventHandler("concentration", myHandler)
'''	

