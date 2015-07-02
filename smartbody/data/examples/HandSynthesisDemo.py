print "|--------------------------------------------|"
print "|        Setup Hand Demo 			        |"
print "|--------------------------------------------|"
print ""
print "Demonstrates using a set of gestures to synthesize hand and finger motion on top of a source motion that does not contain hand or finger animation data."
		
		

		
def setMotionNameSkeleton(motionName, skelName):
	motion = scene.getMotion(motionName)
	if motion != None:		
		#motion.scale(1)
		motion.setMotionSkeletonName(skelName)
	
def createRetargetInstance(srcSkelName, tgtSkelName):
	# replace retarget each animation with just a simple retarget instance
	
	# these joints and their children are not retargeted
	endJoints = StringVec();
	endJoints.append('l_forefoot')
	endJoints.append('l_toe')
	endJoints.append('l_wrist')
	endJoints.append('r_forefoot')	
	endJoints.append('r_toe')	
	endJoints.append('r_wrist')

	# these joints are skipped during skeleton alignment
	relativeJoints = StringVec();
	relativeJoints.append('spine1')
	relativeJoints.append('spine2')
	relativeJoints.append('spine3')
	relativeJoints.append('spine4')
	relativeJoints.append('spine5')
	relativeJoints.append('r_sternoclavicular')
	relativeJoints.append('l_sternoclavicular')
	relativeJoints.append('r_acromioclavicular')
	relativeJoints.append('l_acromioclavicular')	
	
	retargetManager = scene.getRetargetManager()
        retarget = retargetManager.getRetarget(srcSkelName,tgtSkelName)
	if retarget == None:
		retarget = 	retargetManager.createRetarget(srcSkelName,tgtSkelName)
		retarget.initRetarget(endJoints,relativeJoints)
		
# Add asset paths
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('motion', 'ChrMaarten')
scene.addAssetPath('motion', 'Hand')
scene.addAssetPath('script', 'behaviorsets')
scene.addAssetPath('script', 'scripts')
scene.loadAssets()


# configure camera
# Set scene parameters and camera
print 'Configuring scene parameters and camera'
scene.setScale(1.0)
scene.setBoolAttribute('internalAudio', True)
scene.run('default-viewer.py')
camera = getCamera()
camera.setEye(0, 1.71, 1.86)
camera.setCenter(0, 1, 0.01)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(100)
camera.setNearPlane(0.1)
camera.setAspectRatio(0.966897)
cameraPos = SrVec(0, 1.6, 10)
scene.getPawn('camera').setPosition(cameraPos)

# Runs the default viewer for camera
scene.setScale(1)

# Set up joint map for character  
print 'Setting up joint map and configuring Brad\'s skeleton'
scene.run('zebra2-map.py')
zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
bradSkeleton = scene.getSkeleton('ChrBrad.sk')
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse('ChrBrad')

# setting up retargetting between the source motion and the current character
print 'Setting up joint map and configuring Brad\'s skeleton'
scene.run('ipisoft-map.py')
ipisoftMap = scene.getJointMapManager().getJointMap('ipisoft')


# retargetting the motion (iPi motion to SmartBody skeleton)
m = scene.getMotion("Adil_Gesture1.bvh")
ipisoftMap.applyMotion(m)
ipiSk = scene.getSkeleton("Adil_Gesture1.bvh")
ipisoftMap.applySkeleton(ipiSk)
setMotionNameSkeleton('Adil_Gesture1.bvh', 'Adil_Gesture1.bvh')
createRetargetInstance('Adil_Gesture1.bvh','ChrBrad.sk')
# at this point, the source motion has been retargeted to the character,
# so it can be used to generate the hand animation

# run behavior set to load motions
scene.run('BehaviorSetGestures.py')
setupBehaviorSet()
retargetBehaviorSet('ChrBrad')

# create a new hand configuration
configManager = scene.getHandConfigurationManager()
handConfig = configManager.createHandConfiguration("singlegesture")
handConfig.addMotion("ChrBrad@Idle01_NegativeBt01")
configMotions = handConfig.getMotionNames()
#print "Motions in singlegesture configuration:"
#for m in range(0, len(configMotions)):
#	print configMotions[m]
	
#create another hand configuration
handConfig2 = configManager.createHandConfiguration("multiplegestures")
handConfig2.addMotion("ChrBrad@Idle01_YouPointLf01")
handConfig2.addMotion("ChrBrad@Idle01_BeatHighBt01")
handConfig2.addMotion("ChrBrad@Idle01_IndicateRightBt01")
print "Motions in multiplegestures configuration:"
#configMotions = handConfig2.getMotionNames()
#for m in range(0, len(configMotions)):
#	print configMotions[m]
	
# create a third detailed hand configuration
print 'Setting up all_config'
handConfig3 = configManager.createHandConfiguration("allgestures")
handConfig3.addMotion("ChrBrad@Idle01_YouPointLf01")
handConfig3.addMotion("ChrBrad@Idle01_BeatHighBt01")
handConfig3.addMotion("ChrBrad@Idle01_ArmStretch01")
handConfig3.addMotion("ChrBrad@Idle01_BeatHighBt02")
handConfig3.addMotion("ChrBrad@Idle01_BeatHighLf01")
handConfig3.addMotion("ChrBrad@Idle01_BeatLowBt01")
handConfig3.addMotion("ChrBrad@Idle01_BeatLowLf01")
handConfig3.addMotion("ChrBrad@Idle01_BeatLowLf02")
handConfig3.addMotion("ChrBrad@Idle01_BeatMidBt01")
handConfig3.addMotion("ChrBrad@Idle01_BeatMidLf01")
handConfig3.addMotion("ChrBrad@Idle01_ChopBoth01")
handConfig3.addMotion("ChrBrad@Idle01_ChopBt01")
handConfig3.addMotion("ChrBrad@Idle01_ChopLf01")
handConfig3.addMotion("ChrBrad@Idle01_Contemplate01")
handConfig3.addMotion("ChrBrad@Idle01_ExampleLf01")
handConfig3.addMotion("ChrBrad@Idle01_HoweverLf01")
handConfig3.addMotion("ChrBrad@Idle01_IndicateLeftBt01")
handConfig3.addMotion("ChrBrad@Idle01_IndicateLeftLf01")
handConfig3.addMotion("ChrBrad@Idle01_IndicateRightBt01")
handConfig3.addMotion("ChrBrad@Idle01_IndicateRightRt01")
handConfig3.addMotion("ChrBrad@Idle01_MeLf01")
handConfig3.addMotion("ChrBrad@Idle01_NegativeBt01")
handConfig3.addMotion("ChrBrad@Idle01_NegativeBt02")
handConfig3.addMotion("ChrBrad@Idle01_NegativeLf01")
handConfig3.addMotion("ChrBrad@Idle01_NegativeRt01")
handConfig3.addMotion("ChrBrad@Idle01_OfferBoth01")
handConfig3.addMotion("ChrBrad@Idle01_OfferLf01")
handConfig3.addMotion("ChrBrad@Idle01_PleaBt02")
handConfig3.addMotion("ChrBrad@Idle01_PointLf01")
handConfig3.addMotion("ChrBrad@Idle01_ReceiveLf01")
handConfig3.addMotion("ChrBrad@Idle01_ReceiveRt01")
handConfig3.addMotion("ChrBrad@Idle01_SafeLf01")
handConfig3.addMotion("ChrBrad@Idle01_SafeRt01")
handConfig3.addMotion("ChrBrad@Idle01_ScratchChest01")
handConfig3.addMotion("ChrBrad@Idle01_ScratchHeadLf01")
handConfig3.addMotion("ChrBrad@Idle01_YouLf01")
print "Motions in multiplegestures configuration:"
#configMotions = handConfig3.getMotionNames()
#for m in range(0, len(configMotions)):
#	print configMotions[m]

# Add ChrBrad (source character )
source = scene.createCharacter('source', '')
sourceSkeleton = scene.createSkeleton('ChrBrad.sk')
source.setSkeleton(sourceSkeleton)
sourcePos = SrVec(0, 0, 0)
source.setPosition(sourcePos)
source.createStandardControllers()

# Deformable mesh
source.setDoubleAttribute('deformableMeshScale', 0.01)

# enable these to show the deformable mesh, otherwise only the skeleton will be shown
#source.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
#source.setStringAttribute("displayType", "GPUmesh")


# set the hand configuration debug to display what is happening
#source.setBoolAttribute("hand.debug", True)
# decide which hand configuration to use
source.setStringAttribute("hand.config", "allgestures")
source.setStringAttribute("hand.config", "multiplegestures")
source.setStringAttribute("hand.config", "singlegestures")
# set the name of the motion to use
source.setStringAttribute("hand.motionName", "Adil_Gesture1.bvh")







