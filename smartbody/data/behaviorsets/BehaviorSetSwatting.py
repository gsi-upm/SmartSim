scene.run("BehaviorSetCommon.py")

def setupBehaviorSet():
	print "Setting up behavior set for Swatting..."
	#scene.loadAssetsFromPath("behaviorsets/swatting/skeletons")
	#scene.loadAssetsFromPath("behaviorsets/swatting/motions")
	scene.addAssetPath("script", "behaviorsets/swatting/scripts")
	
	assetManager = scene.getAssetManager()	
	motionPath = "behaviorsets/swatting/motions/"
	skel = scene.getSkeleton("SwatSkeleton.bvh")
	if skel == None:
		scene.loadAssetsFromPath("behaviorsets/swatting/skeletons")
	# map the swat skeleton
	scene.run("ipisoft-map.py")
	ipiMap = scene.getJointMapManager().getJointMap("ipisoft")
	swatSkeleton = scene.getSkeleton("SwatSkeleton.bvh")
	ipiMap.applySkeleton(swatSkeleton)
	
	swatMotions = StringVec()
	
	swatMotions.append("Center.bvh")
	swatMotions.append("SwatCenter.bvh")
	swatMotions.append("SwatLeft.bvh")
	swatMotions.append("SwatLow_Far_Left.bvh")
	swatMotions.append("SwatLow_Right.bvh")
	swatMotions.append("SwatLow_Right_Far.bvh")
	swatMotions.append("SwatLow_Right2.bvh")
	swatMotions.append("SwatRight.bvh")
	swatMotions.append("SwatUp_Center.bvh")
	swatMotions.append("SwatUp_Center2.bvh")
	swatMotions.append("SwatUp_Far_Center.bvh")
	swatMotions.append("SwatUp_Far_Left.bvh")
	swatMotions.append("SwatUp_Left.bvh")
	swatMotions.append("SwatUp_Right.bvh")
	
	for i in range(0, len(swatMotions)):
		motion = scene.getMotion(swatMotions[i])
		if motion == None:
			assetManager.loadAsset(motionPath+swatMotions[i])
			motion = scene.getMotion(swatMotions[i])		
		if motion != None:
			motion.setMotionSkeletonName("SwatSkeleton.bvh")
			ipiMap.applyMotion(motion)
	
	mirroredMotions = StringVec()
	mirroredMotions.append("Center.bvh")
	mirroredMotions.append("SwatCenter.bvh")
	mirroredMotions.append("SwatLeft.bvh")
	mirroredMotions.append("SwatLow_Far_Left.bvh")
	mirroredMotions.append("SwatLow_Right.bvh")
	mirroredMotions.append("SwatLow_Right_Far.bvh")
	mirroredMotions.append("SwatLow_Right2.bvh")
	mirroredMotions.append("SwatRight.bvh")
	mirroredMotions.append("SwatUp_Center.bvh")
	mirroredMotions.append("SwatUp_Center2.bvh")
	mirroredMotions.append("SwatUp_Far_Center.bvh")
	mirroredMotions.append("SwatUp_Far_Left.bvh")
	mirroredMotions.append("SwatUp_Left.bvh")
	mirroredMotions.append("SwatUp_Right.bvh")
	
	for i in range(0,len(mirroredMotions)):
		mirrorMotion = scene.getMotion(mirroredMotions[i])
		if mirrorMotion != None:
			mirrorMotion.mirror(mirroredMotions[i]+"Lt", "SwatSkeleton.bvh")


def retargetBehaviorSet(charName):
	swatMotions = StringVec()
	
	swatMotions.append("Center.bvh")
	swatMotions.append("SwatCenter.bvh")
	swatMotions.append("SwatLeft.bvh")
	swatMotions.append("SwatLow_Far_Left.bvh")
	swatMotions.append("SwatLow_Right.bvh")
	swatMotions.append("SwatLow_Right_Far.bvh")
	swatMotions.append("SwatLow_Right2.bvh")
	swatMotions.append("SwatRight.bvh")
	swatMotions.append("SwatUp_Center.bvh")
	swatMotions.append("SwatUp_Center2.bvh")
	swatMotions.append("SwatUp_Far_Center.bvh")
	swatMotions.append("SwatUp_Far_Left.bvh")
	swatMotions.append("SwatUp_Left.bvh")
	swatMotions.append("SwatUp_Right.bvh")
	
	#include the mirrored motions
	swatMotions.append("Center.bvh" + "Lt")
	swatMotions.append("SwatCenter.bvh" + "Lt")
	swatMotions.append("SwatLeft.bvh" + "Lt")
	swatMotions.append("SwatLow_Far_Left.bvh" + "Lt")
	swatMotions.append("SwatLow_Right.bvh" + "Lt")
	swatMotions.append("SwatLow_Right_Far.bvh" + "Lt")
	swatMotions.append("SwatLow_Right2.bvh" + "Lt")
	swatMotions.append("SwatRight.bvh" + "Lt")
	swatMotions.append("SwatUp_Center.bvh" + "Lt")
	swatMotions.append("SwatUp_Center2.bvh" + "Lt")
	swatMotions.append("SwatUp_Far_Center.bvh" + "Lt")
	swatMotions.append("SwatUp_Far_Left.bvh" + "Lt")
	swatMotions.append("SwatUp_Left.bvh" + "Lt")
	swatMotions.append("SwatUp_Right.bvh" + "Lt")
	
	scene.loadAssetsFromPath("swatting")

	sbChar = scene.getCharacter(charName)
	if sbChar == None:
		return
	skelName = sbChar.getSkeleton().getName()
	
	createRetargetInstance("SwatSkeleton.bvh", skelName)

	# setup standard locomotion
	scene.run("stateSwatting.py")
	swattingSetup('SwatSkeleton.bvh','SwatSkeleton.bvh', "base", '','')
	
		
