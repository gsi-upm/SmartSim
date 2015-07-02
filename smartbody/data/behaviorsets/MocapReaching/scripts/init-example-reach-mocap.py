def reachSetup(characterName, interpolatorType, originalSkeleton, preFix):
	print "Setting up reach database for " + characterName
	rightHandMotions = StringVec();
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachBackFloor01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachBackHigh01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachBackLow01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachBackMediumFar01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachBackMediumMid01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachBackMediumNear01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachForwardFloor01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachForwardHigh01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachForwardLow01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachForwardMediumFar01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachForwardMediumMid01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachLeft30Floor01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachLeft30High01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachLeft30Low01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachLeft30MediumFar01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachLeft30MediumMid01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachLeft30MediumNear01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachLeft60Floor01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachLeft60High01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachLeft60Low01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachLeft60MediumFar01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachLeft60MediumMid01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachLeft60MediumNear01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachRight120Floor01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachRight120High01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachRight120Low01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachRight120MediumFar01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachRight120MediumMid01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachRight120MediumNear01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachRight30Floor01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachRight30High01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachRight30Low01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachRight30MediumFar01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachRight30MediumMid01")
	rightHandMotions.append(preFix+"ChrGarza@IdleStand01_ReachRight30MediumNear01")	
	
		
	leftHandMotions = StringVec();
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachBackFloor01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachBackHigh01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachBackLow01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachBackMediumFar01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachBackMediumMid01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachBackMediumNear01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachForwardFloor01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachForwardHigh01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachForwardLow01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachForwardMediumFar01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachForwardMediumMid01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachLeft30Floor01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachLeft30High01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachLeft30Low01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachLeft30MediumFar01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachLeft30MediumMid01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachLeft30MediumNear01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachLeft60Floor01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachLeft60High01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachLeft60Low01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachLeft60MediumFar01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachLeft60MediumMid01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachLeft60MediumNear01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachRight120Floor01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachRight120High01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachRight120Low01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachRight120MediumFar01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachRight120MediumMid01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachRight120MediumNear01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachRight30Floor01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachRight30High01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachRight30Low01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachRight30MediumFar01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachRight30MediumMid01")
	leftHandMotions.append(preFix+"ChrGarza@IdleStand01_LReachRight30MediumNear01")		
	
	scene = getScene()
	
	character = scene.getCharacter(characterName)
	skeleton = character.getSkeleton()
	reachManager = scene.getReachManager()
	reach = reachManager.createReachWithTag(characterName,'mocap')	
	reach.setInterpolatorType(interpolatorType)
	for i in range(0,len(rightHandMotions)):
		right = scene.getMotion(rightHandMotions[i])
		left = scene.getMotion(leftHandMotions[i])
		reach.addMotion("right",scene.getMotion(rightHandMotions[i]))
		reach.addMotion("left",scene.getMotion(leftHandMotions[i]))
		
	reach.setGrabHandMotion("right",scene.getMotion(preFix+"ChrHarmony_Relax001_HandGraspSmSphere_Grasp"));
	reach.setGrabHandMotion("left",scene.getMotion(preFix+"ChrHarmony_Relax001_LHandGraspSmSphere_Grasp"));
	
	reach.setReachHandMotion("right",scene.getMotion(preFix+"ChrHarmony_Relax001_HandGraspSmSphere_Reach"));
	reach.setReachHandMotion("left",scene.getMotion(preFix+"ChrHarmony_Relax001_LHandGraspSmSphere_Reach"));
	
	reach.setReleaseHandMotion("right",scene.getMotion(preFix+"ChrHarmony_Relax001_HandGraspSmSphere_Release"));
	reach.setReleaseHandMotion("left",scene.getMotion(preFix+"ChrHarmony_Relax001_LHandGraspSmSphere_Release"));
	
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















