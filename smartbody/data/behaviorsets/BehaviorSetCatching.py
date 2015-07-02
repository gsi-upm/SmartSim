scene.run("BehaviorSetCommon.py")

def setupBehaviorSet():
	print "Setting up behavior set for Catching..."
	#scene.loadAssetsFromPath("behaviorsets/catching/skeletons")
	#scene.loadAssetsFromPath("behaviorsets/catching/motions")
	scene.addAssetPath("script", "behaviorsets/catching/scripts")
	
	assetManager = scene.getAssetManager()	
	motionPath = "behaviorsets/catching/motions/"
	skel = scene.getSkeleton("CatchSkeleton.bvh")
	if skel == None:
		scene.loadAssetsFromPath("behaviorsets/catching/skeletons")
	# map the catch skeleton
	scene.run("ipisoft-map.py")
	ipiMap = scene.getJointMapManager().getJointMap("ipisoft")
	catchSkeleton = scene.getSkeleton("CatchSkeleton.bvh")
	ipiMap.applySkeleton(catchSkeleton)
	
	catchMotions = StringVec()
	
	catchMotions.append("Center.bvh")
	catchMotions.append("Center2.bvh")
	catchMotions.append("Far_Center_Up.bvh")
	catchMotions.append("Far_Left.bvh")
	catchMotions.append("Far_Left2.bvh")
	catchMotions.append("Far_Low_Left.bvh")
	catchMotions.append("Far_Lower_Right.bvh")
	catchMotions.append("Far_Lower_Right2.bvh")
	catchMotions.append("Far_Right.bvh")
	catchMotions.append("Left.bvh")
	catchMotions.append("Low_Center.bvh")
	catchMotions.append("Low_Left.bvh")
	catchMotions.append("Low_Right.bvh")
	catchMotions.append("Low_Right2.bvh")
	catchMotions.append("Right.bvh")
	catchMotions.append("Up_Center.bvh")
	catchMotions.append("Up_Left.bvh")
	catchMotions.append("Up_Left_Center.bvh")
	catchMotions.append("Up_Right.bvh")
	catchMotions.append("Up_Right_Far.bvh")
	
	for i in range(0, len(catchMotions)):
		motion = scene.getMotion(catchMotions[i])
		if motion == None:
			assetManager.loadAsset(motionPath+catchMotions[i])
			motion = scene.getMotion(catchMotions[i])		
		if motion != None:
			motion.setMotionSkeletonName("CatchSkeleton.bvh")
			ipiMap.applyMotion(motion)
	
	mirroredMotions = StringVec()
	mirroredMotions.append("Center.bvh")
	mirroredMotions.append("Center2.bvh")
	mirroredMotions.append("Far_Center_Up.bvh")
	mirroredMotions.append("Far_Left.bvh")
	mirroredMotions.append("Far_Left2.bvh")
	mirroredMotions.append("Far_Low_Left.bvh")
	mirroredMotions.append("Far_Lower_Right.bvh")
	mirroredMotions.append("Far_Lower_Right2.bvh")
	mirroredMotions.append("Far_Right.bvh")
	mirroredMotions.append("Left.bvh")
	mirroredMotions.append("Left.bvh")
	mirroredMotions.append("Low_Center.bvh")
	mirroredMotions.append("Low_Left.bvh")
	mirroredMotions.append("Low_Right.bvh")
	mirroredMotions.append("Low_Right2.bvh")
	mirroredMotions.append("Right.bvh")
	mirroredMotions.append("Up_Center.bvh")
	mirroredMotions.append("Up_Left.bvh")
	mirroredMotions.append("Up_Left_Center.bvh")
	mirroredMotions.append("Up_Right.bvh")
	mirroredMotions.append("Up_Right_Far.bvh")
	
	for i in range(0,len(mirroredMotions)):
		mirrorMotion = scene.getMotion(mirroredMotions[i])
		if mirrorMotion != None:
			mirrorMotion.mirror(mirroredMotions[i]+"Lt", "CatchSkeleton.bvh")


def retargetBehaviorSet(charName):
	catchMotions = StringVec()
	
	catchMotions.append("Center.bvh")
	catchMotions.append("Center2.bvh")
	catchMotions.append("Far_Center_Up.bvh")
	catchMotions.append("Far_Left.bvh")
	catchMotions.append("Far_Left2.bvh")
	catchMotions.append("Far_Low_Left.bvh")
	catchMotions.append("Far_Low_Left2.bvh")
	catchMotions.append("Far_Lower_Right.bvh")
	catchMotions.append("Far_Lower_Right2.bvh")
	catchMotions.append("Far_Right.bvh")
	catchMotions.append("Left.bvh")
	catchMotions.append("Low_Center.bvh")
	catchMotions.append("Low_Left.bvh")
	catchMotions.append("Low_Right.bvh")
	catchMotions.append("Low_Right2.bvh")
	catchMotions.append("Right.bvh")
	catchMotions.append("Up_Center.bvh")
	catchMotions.append("Up_Left.bvh")
	catchMotions.append("Up_Left_Center.bvh")
	catchMotions.append("Up_Right.bvh")
	catchMotions.append("Up_Right_Far.bvh")
	#include the mirrored motions
	catchMotions.append("Center.bvh"+"Lt")
	catchMotions.append("Center2.bvh"+"Lt")
	catchMotions.append("Far_Center_Up.bvh"+"Lt")
	catchMotions.append("Far_Left.bvh"+"Lt")
	catchMotions.append("Far_Left2.bvh"+"Lt")
	catchMotions.append("Far_Low_Left.bvh"+"Lt")
	catchMotions.append("Far_Lower_Right.bvh"+"Lt")
	catchMotions.append("Far_Lower_Right2.bvh"+"Lt")
	catchMotions.append("Far_Right.bvh"+"Lt")
	catchMotions.append("Left.bvh"+"Lt")
	catchMotions.append("Low_Center.bvh"+"Lt")
	catchMotions.append("Low_Left.bvh"+"Lt")
	catchMotions.append("Low_Right.bvh"+"Lt")
	catchMotions.append("Low_Right2.bvh"+"Lt")
	catchMotions.append("Right.bvh"+"Lt")
	catchMotions.append("Up_Center.bvh"+"Lt")
	catchMotions.append("Up_Left.bvh"+"Lt")
	catchMotions.append("Up_Left_Center.bvh"+"Lt")
	catchMotions.append("Up_Right.bvh"+"Lt")
	catchMotions.append("Up_Right_Far.bvh"+"Lt")
	
	scene.loadAssetsFromPath("catching")

	sbChar = scene.getCharacter(charName)
	if sbChar == None:
		return
	skelName = sbChar.getSkeleton().getName()
	
	createRetargetInstance("CatchSkeleton.bvh", skelName)

	# setup standard locomotion
	scene.run("stateCatching.py")
	catchingSetup('CatchSkeleton.bvh','CatchSkeleton.bvh', "base", '','')
	
		
