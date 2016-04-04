scene.setScale(.01)

getViewer().show()
getCamera().reset()
getCamera().setEye(-257.385, 218.578, 355.538)
getCamera().setCenter(36.1478, 67.7661, 47.408)
getCamera().setUpVector(SrVec(0, 1, 0))

getCamera().setFov(1.0472)
getCamera().setFarPlane(10000)
getCamera().setNearPlane(1)
getCamera().setAspectRatio(0.879121)

scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('motion', 'ChrBrad')
scene.addAssetPath("script", "behaviorsets")
scene.addAssetPath('script', 'scripts')
scene.addAssetPath('script', 'examples/CEGUI')
scene.loadAssets()

scene.run('zebra2-map.py')
zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
bradSkeleton = scene.getSkeleton('ChrBrad.sk')
bradSkeleton.rescale(100)
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse('ChrBrad')

scale = 100.0

allJoints = StringVec()
allJoints.append("HIP_CENTER")
allJoints.append("SPINE")
allJoints.append("SHOULDER_CENTER")
allJoints.append("SHOULDER_LEFT")
allJoints.append("HEAD")
allJoints.append("ELBOW_LEFT")
allJoints.append("WRIST_LEFT")
allJoints.append("HAND_LEFT")
allJoints.append("SHOULDER_RIGHT")
allJoints.append("ELBOW_RIGHT")
allJoints.append("WRIST_RIGHT")
allJoints.append("HAND_RIGHT")
allJoints.append("HIP_LEFT")
allJoints.append("KNEE_LEFT")
allJoints.append("ANKLE_LEFT")
allJoints.append("FOOT_LEFT")
allJoints.append("HIP_RIGHT")
allJoints.append("KNEE_RIGHT")
allJoints.append("ANKLE_RIGHT")
allJoints.append("FOOT_RIGHT")

# script to control the pawn positions

class ShowKinectSkeleton (SBScript):

	def start(this):
		# create a global position marker that can be used to move the kinect skeleton around
		kinectroot = scene.createPawn("kinectroot")
		kinectroot.setPosition(SrVec(0, 100, -50))
		for j in range(0, len(allJoints)):
			p = scene.createPawn(allJoints[j])
			p.setStringAttribute("collisionShape", "sphere")
			p.setVec3Attribute("collisionShapeScale", 5, 5, 5)
			
	
	def update(this, time):
		kinectroot = scene.getPawn("kinectroot")
		globalpos = kinectroot.getPosition()
		globalrot = kinectroot.getOrientation()
		realtime = scene.getRealtimeManager()
		channels = realtime.getChannelNames()
		for c in range(0, len(channels)):
			chan = channels[c]
			if (chan.startswith("_pos_")):
				pos = realtime.getDataVec(chan)
				name = chan[5:]
				p = scene.getPawn(name)
				scaledPos = SrVec(pos.getData(0) * scale, pos.getData(1) * scale, pos.getData(2) * scale)
				# adjust the pawn location via the kinect root object
				temppos = globalrot.multVec(scaledPos)
				temppos.addVec(globalpos)
				p.setPosition(temppos)
			
showKinectScript = ShowKinectSkeleton()
scene.addScript("showskeleton", showKinectScript)
	
brad = scene.createCharacter("ChrBrad", "")
bradSkel = scene.createSkeleton("ChrBrad.sk")
brad.setSkeleton(bradSkel)
brad.createStandardControllers()
# disable the postprocessing (foot correction) controller when using reaching
brad.getControllerByName("ChrBrad_postprocessController").setIgnore(True)

# setup locomotion
scene.run('BehaviorSetMaleMocapLocomotion.py')
setupBehaviorSet()
retargetBehaviorSet('ChrBrad')

# setup reach 
scene.run('BehaviorSetReaching.py')
setupBehaviorSet()
retargetBehaviorSet('ChrBrad')

# set up the user interface to allow switching between different capabilities

print 'Setting up GUI'
scene.run('GUIUtil.py')

gui = GUIManager()
class GUIHandler:
	def __init__(self):
		print "Loading Kinect GUI"

	def handleLeftHandButton(self,args):		
		btn = args.window
		bml.execBML('*', '<sbm:reach sbm:reach-type="right" target="HAND_LEFT"/>')
		
	def handleRightHandButton(self,args):		
		btn = args.window
		bml.execBML('*', '<sbm:reach sbm:reach-type="left" target="HAND_RIGHT"/>')
		
	def handleNoHandsButton(self,args):		
		btn = args.window		
		bml.execBML('*', '<sbm:reach sbm:reach-finish="true"/>')
	
	def handleStepForward(self,args):
		btn = args.window
		bml.execBML('*', '<blend name="mocapStep" sbm:wrap-mode="Once" x="0" y="1"/>')
		
	def handleStepBack(self,args):
		btn = args.window		
		bml.execBML('*', '<blend name="mocapStep" sbm:wrap-mode="Once" x="0" y="-1"/>')
		
	def handleStepLeft(self,args):
		btn = args.window		
		bml.execBML('*', '<blend name="mocapStep" sbm:wrap-mode="Once" x="-1" y="0"/>')
		
	def handleStepRight(self,args):
		btn = args.window	
		bml.execBML('*', '<blend name="mocapStep" sbm:wrap-mode="Once" x="1" y="0"/>')		

guiHandler = GUIHandler()

alphaVal = .5
buttonRightHand = gui.createButton('righthand', 'Right hand -> Left',  10, 10, 150)
buttonRightHand.subscribeEvent(PushButton.EventClicked, guiHandler.handleRightHandButton)
buttonRightHand.setAlpha(alphaVal)

buttonLeftHand = gui.createButton('lefthand', 'Left hand -> Right', 170, 10, 150)
buttonLeftHand.subscribeEvent(PushButton.EventClicked, guiHandler.handleLeftHandButton)
buttonLeftHand.setAlpha(alphaVal)

buttonNoHands = gui.createButton('nohands', 'No hands',  260, 10, 150)
buttonNoHands.subscribeEvent(PushButton.EventClicked, guiHandler.handleNoHandsButton)
buttonNoHands.setAlpha(alphaVal)

buttonStepForward = gui.createButton( 'stepforward','Step Forward', 120, 40, 150)
buttonStepForward.subscribeEvent(PushButton.EventClicked, guiHandler.handleStepForward)
buttonStepForward.setAlpha(alphaVal)

buttonStepBack = gui.createButton('stepback','Step back',  120, 100, 150)
buttonStepBack.subscribeEvent(PushButton.EventClicked, guiHandler.handleStepBack)
buttonStepBack.setAlpha(alphaVal)

buttonStepLeft = gui.createButton('stepleft','Step Left',  10, 70, 150)
buttonStepLeft.subscribeEvent(PushButton.EventClicked, guiHandler.handleStepLeft)
buttonStepLeft.setAlpha(alphaVal)

buttonStepRight = gui.createButton('stepright','Step Right',  180, 70, 150)
buttonStepRight.subscribeEvent(PushButton.EventClicked, guiHandler.handleStepRight)
buttonStepRight.setAlpha(alphaVal)


bml.execBML('*', '<body posture="ChrMarine@Idle01"/>')		

print "Please start the program named SBKinectViewerPositions.exe"
print "Then choose the appropriate action from the buttons above."
		