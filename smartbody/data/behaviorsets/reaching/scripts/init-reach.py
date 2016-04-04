def reachSetup(characterName, interpolatorType, originalSkeleton, preFix):
	print "Setting up reach database for " + characterName
	rightHandMotions = StringVec();
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachRtHigh")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachRtMidHigh")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachRtMidLow")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachLfLow")
	
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachLfHigh")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachLfMidHigh")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachRtMidLow")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachRtLow")
	
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachMiddleHigh")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachMiddleMidHigh")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachMiddleMidLow")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachMiddleLow")
	
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachClose_Lf")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachClose_Rt")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachClose_MiddleHigh")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachClose_MiddleLow")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachClose_MiddleMidHigh")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachClose_MiddleMidLow")
	
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachBehind_High1")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachBehind_High2")	
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachBehind_Low1")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachBehind_Low2")	
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachBehind_MidHigh1")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachBehind_MidHigh2")	
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachBehind_MidLow1")
	rightHandMotions.append(preFix+"ChrHarmony_Relax001_ArmReachBehind_MidLow2")	
	
	leftHandMotions = StringVec();
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachRtHigh")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachRtMidHigh")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachRtMidLow")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachLfLow")
	
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachLfHigh")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachLfMidHigh")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachRtMidLow")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachRtLow")
	
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachMiddleHigh")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachMiddleMidHigh")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachMiddleMidLow")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachMiddleLow")
	
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachClose_Lf")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachClose_Rt")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachClose_MiddleHigh")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachClose_MiddleLow")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachClose_MiddleMidHigh")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachClose_MiddleMidLow")
	
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachBehind_High1")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachBehind_High2")	
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachBehind_Low1")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachBehind_Low2")	
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachBehind_MidHigh1")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachBehind_MidHigh2")	
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachBehind_MidLow1")
	leftHandMotions.append(preFix+"ChrHarmony_Relax001_LArmReachBehind_MidLow2")	
	
	scene = getScene()
	
	character = scene.getCharacter(characterName)
	skeleton = character.getSkeleton()
	reachManager = scene.getReachManager()
	reach = reachManager.createReachWithTag(characterName,'default')	
	print 'setting up reach for ' + characterName + ' , numReach()=' + str(reachManager.getNumReaches())
	reach.setInterpolatorType(interpolatorType)
	for i in range(0,len(rightHandMotions)):
		#mirrorMotion1 = scene.getMotion(rightHandMotions[i])
		#mirrorMotion1.mirror(leftHandMotions[i], originalSkeleton)
		reach.addMotion("right",scene.getMotion(rightHandMotions[i]))
		reach.addMotion("left",scene.getMotion(leftHandMotions[i]))
		
	
	#grabMirrorMotion = scene.getMotion(preFix+"ChrHarmony_Relax001_HandGraspSmSphere_Grasp")
	#grabMirrorMotion.mirror(preFix+"ChrHarmony_Relax001_LHandGraspSmSphere_Grasp", originalSkeleton)
	reach.setGrabHandMotion("right",scene.getMotion(preFix+"ChrHarmony_Relax001_HandGraspSmSphere_Grasp"));
	reach.setGrabHandMotion("left",scene.getMotion(preFix+"ChrHarmony_Relax001_LHandGraspSmSphere_Grasp"));
	
	#reachMirrorMotion = scene.getMotion(preFix+"ChrHarmony_Relax001_HandGraspSmSphere_Reach")
	#reachMirrorMotion.mirror(preFix+"ChrHarmony_Relax001_LHandGraspSmSphere_Reach", originalSkeleton)
	reach.setReachHandMotion("right",scene.getMotion(preFix+"ChrHarmony_Relax001_HandGraspSmSphere_Reach"));
	reach.setReachHandMotion("left",scene.getMotion(preFix+"ChrHarmony_Relax001_LHandGraspSmSphere_Reach"));
	
	#releaseMirrorMotion = scene.getMotion(preFix+"ChrHarmony_Relax001_HandGraspSmSphere_Release")
	#releaseMirrorMotion.mirror(preFix+"ChrHarmony_Relax001_LHandGraspSmSphere_Release", originalSkeleton)
	reach.setReleaseHandMotion("right",scene.getMotion(preFix+"ChrHarmony_Relax001_HandGraspSmSphere_Release"));
	reach.setReleaseHandMotion("left",scene.getMotion(preFix+"ChrHarmony_Relax001_LHandGraspSmSphere_Release"));
	
	#pointMirrorMotion = scene.getMotion(preFix+"HandsAtSide_RArm_GestureYou")
	#pointMirrorMotion.mirror(preFix+"HandsAtSide_LArm_GestureYou", originalSkeleton)
	reach.setPointHandMotion("right",scene.getMotion(preFix+"HandsAtSide_RArm_GestureYou"));
	reach.setPointHandMotion("left",scene.getMotion(preFix+"HandsAtSide_LArm_GestureYou"));
	
	reach.build(scene.getCharacter(characterName))		

# handle grasp event
# grasp event currently uses old-style command interface
# by storing the command as the parameter of the event
class GraspHandler(SBEventHandler):
	def executeAction(this, event):
		params = event.getParameters()
		scene.command(params)

# now create the event handler for the 'reach' event
graspHandler = GraspHandler()
em = scene.getEventManager()
em.addEventHandler("reach", graspHandler)















