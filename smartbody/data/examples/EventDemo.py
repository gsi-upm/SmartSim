from random import choice
print "|--------------------------------------------|"
print "|           Starting Event Demo              |"
print "|--------------------------------------------|"

# Add asset paths
scene.addAssetPath('script', 'scripts')
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('motion', 'ChrRachel')
scene.addAssetPath("script", "behaviorsets")
scene.loadAssets()

# Set scene parameters to fit new Brad and Rachel
print 'Configuring scene parameters and camera'
scene.setScale(1.0)
scene.setBoolAttribute('internalAudio', True)
scene.run('default-viewer.py')
camera = getCamera()
camera.setEye(-0.140295, 1.51999, 1.01841)
camera.setCenter(-0.362596, 1.31182, -0.0758442)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(100)
camera.setNearPlane(0.1)
camera.setAspectRatio(0.966897)
scene.getPawn('camera').setPosition(SrVec(0, 1.55, 3))

# Set joint map for Rachel
print 'Setting up joint map for Brad and Rachel'
scene.run('zebra2-map.py')
zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
rachelSkeleton = scene.getSkeleton('ChrRachel.sk')
zebra2Map.applySkeleton(rachelSkeleton)
zebra2Map.applyMotionRecurse('ChrRachel')

# Establish lip syncing data set
scene.run('init-diphoneDefault.py')

# Setting up face definition
print 'Setting up face definition'

# Rachel's face definition
rachelFace = scene.createFaceDefinition('ChrRachel')
rachelFace.setFaceNeutral('ChrRachel@face_neutral')
rachelFace.setAU(1,  "left",  "ChrRachel@001_inner_brow_raiser_lf")
rachelFace.setAU(1,  "right", "ChrRachel@001_inner_brow_raiser_rt")
rachelFace.setAU(2,  "left",  "ChrRachel@002_outer_brow_raiser_lf")
rachelFace.setAU(2,  "right", "ChrRachel@002_outer_brow_raiser_rt")
rachelFace.setAU(4,  "left",  "ChrRachel@004_brow_lowerer_lf")
rachelFace.setAU(4,  "right", "ChrRachel@004_brow_lowerer_rt")
rachelFace.setAU(5,  "both",  "ChrRachel@005_upper_lid_raiser")
rachelFace.setAU(6,  "both",  "ChrRachel@006_cheek_raiser")
rachelFace.setAU(7,  "both",  "ChrRachel@007_lid_tightener")
rachelFace.setAU(10, "both",  "ChrRachel@010_upper_lip_raiser")
rachelFace.setAU(12, "left",  "ChrRachel@012_lip_corner_puller_lf")
rachelFace.setAU(12, "right", "ChrRachel@012_lip_corner_puller_rt")
rachelFace.setAU(25, "both",  "ChrRachel@025_lips_part")
rachelFace.setAU(26, "both",  "ChrRachel@026_jaw_drop")
rachelFace.setAU(45, "left",  "ChrRachel@045_blink_lf")
rachelFace.setAU(45, "right", "ChrRachel@045_blink_rt")

rachelFace.setViseme("open",    "ChrRachel@open")
rachelFace.setViseme("W",       "ChrRachel@W")
rachelFace.setViseme("ShCh",    "ChrRachel@ShCh")
rachelFace.setViseme("PBM",     "ChrRachel@PBM")
rachelFace.setViseme("FV",      "ChrRachel@FV")
rachelFace.setViseme("wide",    "ChrRachel@wide")
rachelFace.setViseme("tBack",   "ChrRachel@tBack")
rachelFace.setViseme("tRoof",   "ChrRachel@tRoof")
rachelFace.setViseme("tTeeth",  "ChrRachel@tTeeth")

print 'Setting up Rachel'
rachel = scene.createCharacter('ChrRachel', '')
rachelSkeleton = scene.createSkeleton('ChrRachel.sk')
rachel.setSkeleton(rachelSkeleton)
rachel.setFaceDefinition(rachelFace)
rachelPos = SrVec(-.35, 0, 0)
rachel.setPosition(rachelPos)
rachel.setHPR(SrVec(17, 0, 0))
rachel.createStandardControllers()
# Deformable mesh
rachel.setDoubleAttribute('deformableMeshScale', .01)
rachel.setStringAttribute('deformableMesh', 'ChrRachel.dae')
# Lip syncing diphone setup
rachel.setStringAttribute('lipSyncSetName', 'default')
rachel.setBoolAttribute('usePhoneBigram', True)
rachel.setVoice('remote')

import platform
if platform.system() == "Windows":
	windowsVer = platform.platform()
	if windowsVer.find("Windows-7") == 0:
		rachel.setVoiceCode('Microsoft|Anna')
	else:
		if windowsVer.find("Windows-8") == 0 or windowsVer.find("Windows-post2008Server") == 0:
			rachel.setVoiceCode('Microsoft|Zira|Desktop')
else: # non-Windows platform, use Festival voices
	rachel.setVoiceCode('voice_kal_diphone')

# Gesture map setup
rachel.setStringAttribute('gestureMap', 'ChrRachel')
rachel.setBoolAttribute('gestureRequest.autoGestureTransition', True)

# setup gestures
scene.run('BehaviorSetFemaleGestures.py')
setupBehaviorSet()
retargetBehaviorSet('ChrRachel')

# Turning on deformable GPU
rachel.setStringAttribute("displayType", "GPUmesh")

# Idle pose
bml.execBML('ChrRachel', '<body posture="ChrConnor@IdleStand01" ready="0" relax="0"/>')

# trigger an event at a given motion
motion1 = scene.getMotion('ChrConnor@IdleStand01_BeatMidRt01')
motion1.addEvent(0.5, 'motionSpeech', 'beat', False)
motion2 = scene.getMotion('ChrConnor@IdleStand01_NegateRightMidRt01')
motion2.addEvent(0.3, 'motionSpeech', 'contemplate', False)
motion3 = scene.getMotion('ChrConnor@IdleStand01_NoLeftRt01')
motion3.addEvent(0.3, 'motionSpeech', 'negative', False)

last = 0
delay = 4
rachelTime = 0
class EventDemo(SBScript):
	def update(self, time):
		global last, rachelTime

		diff = time - last
		if diff >= delay:
			diff = 0
			canTime = True
		# If time's up, do action
		diff2 = time - rachelTime
		if diff2 > 12:
			diff2 = 0
			rachelTime = time
			# Animations are triggering event handler which is in turn triggering speech as seen below
			bml.execBMLAt(0, 'ChrRachel', '<gesture name="ChrConnor@IdleStand01_BeatMidRt01"/>')
			bml.execBMLAt(3, 'ChrRachel', '<gesture name="ChrConnor@IdleStand01_NegateRightMidRt01"/>')
			bml.execBMLAt(8, 'ChrRachel', '<gesture name="ChrConnor@IdleStand01_NoLeftRt01"/>')
			
# Run the update script
scene.removeScript('eventdemo')
eventdemo = EventDemo()
scene.addScript('eventdemo', eventdemo)

# add code to respond to events that are triggered by the motion

class MyEventHandler(SBEventHandler):
	
	def executeAction(self, event):
		params = event.getParameters()
		# Do action depending on message received from event handler
		print "Got an event " + params + " at time " + str(sim.getTime())
		if 'beat' in params:
			bml.execBML('ChrRachel', '<speech type="text/plain">What is the meaning of this?</speech><face type="FACS" au="1" amount=".7" start="0" end="2"/><face type="FACS" au="2" amount=".7" start="0" end="2"/>')
		if 'contemplate' in params:
			bml.execBML('ChrRachel', '<speech type="text/plain">No, I don\'t want any more of those</speech><face type="FACS" au="7" amount="1.0" start="0" end="3"/><face type="FACS" au="7" amount="1.0" start="0" end="3"/><face type="FACS" au="2" side="right" amount="1.0" start="0" end="3"/>')
		if 'negative' in params:
			bml.execBML('ChrRachel', '<speech type="text/plain">This is unacceptable</speech><face type="FACS" au="4" amount="1.0" start="0" end="3"/>')
			

myHandler = MyEventHandler()
eventManager = scene.getEventManager()
eventManager.addEventHandler('motionSpeech', myHandler)

print '******************************************************'
print '* This script requires speech relay to work properly *'
print '*    It can be found under Window > Speech Relay     *'
print '******************************************************'
