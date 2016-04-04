print "|--------------------------------------------|"
print "|         Starting Character Demo            |"
print "|--------------------------------------------|"

# Add asset paths
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('motion', 'ChrMaarten')
scene.addAssetPath("script", "behaviorsets")
scene.addAssetPath('script', 'scripts')
scene.loadAssets()

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

# Set up joint map for Brad
print 'Setting up joint map and configuring Brad\'s skeleton'
scene.run('zebra2-map.py')
zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
bradSkeleton = scene.getSkeleton('ChrBrad.sk')
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse('ChrMaarten')

# Establish lip syncing data set
print 'Establishing lip syncing data set'
scene.run('init-diphoneDefault.py')

# Set up face definition
print 'Setting up face definition'
# Brad's face definition
bradFace = scene.createFaceDefinition('ChrBrad')
bradFace.setFaceNeutral('ChrBrad@face_neutral')
bradFace.setAU(1,  "left",  "ChrBrad@001_inner_brow_raiser_lf")
bradFace.setAU(1,  "right", "ChrBrad@001_inner_brow_raiser_rt")
bradFace.setAU(2,  "left",  "ChrBrad@002_outer_brow_raiser_lf")
bradFace.setAU(2,  "right", "ChrBrad@002_outer_brow_raiser_rt")
bradFace.setAU(4,  "left",  "ChrBrad@004_brow_lowerer_lf")
bradFace.setAU(4,  "right", "ChrBrad@004_brow_lowerer_rt")
bradFace.setAU(5,  "both",  "ChrBrad@005_upper_lid_raiser")
bradFace.setAU(6,  "both",  "ChrBrad@006_cheek_raiser")
bradFace.setAU(7,  "both",  "ChrBrad@007_lid_tightener")
bradFace.setAU(10, "both",  "ChrBrad@010_upper_lip_raiser")
bradFace.setAU(12, "left",  "ChrBrad@012_lip_corner_puller_lf")
bradFace.setAU(12, "right", "ChrBrad@012_lip_corner_puller_rt")
bradFace.setAU(25, "both",  "ChrBrad@025_lips_part")
bradFace.setAU(26, "both",  "ChrBrad@026_jaw_drop")
bradFace.setAU(45, "left",  "ChrBrad@045_blink_lf")
bradFace.setAU(45, "right", "ChrBrad@045_blink_rt")

bradFace.setViseme("open",    "ChrBrad@open")
bradFace.setViseme("W",       "ChrBrad@W")
bradFace.setViseme("ShCh",    "ChrBrad@ShCh")
bradFace.setViseme("PBM",     "ChrBrad@PBM")
bradFace.setViseme("FV",      "ChrBrad@FV")
bradFace.setViseme("wide",    "ChrBrad@wide")
bradFace.setViseme("tBack",   "ChrBrad@tBack")
bradFace.setViseme("tRoof",   "ChrBrad@tRoof")
bradFace.setViseme("tTeeth",  "ChrBrad@tTeeth")

print 'Adding character into scene'
# Set up Brad
brad = scene.createCharacter('ChrBrad', '')
bradSkeleton = scene.createSkeleton('ChrBrad.sk')
brad.setSkeleton(bradSkeleton)
# Set position
bradPos = SrVec(0, 0, 0)
brad.setPosition(bradPos)
# Set facing direction
bradFacing = SrVec(0, 0, 0)
brad.setHPR(bradFacing)
# Set face definition
brad.setFaceDefinition(bradFace)
# Set standard controller
brad.createStandardControllers()
# Deformable mesh
brad.setDoubleAttribute('deformableMeshScale', .01)
brad.setStringAttribute('deformableMesh', 'ChrMaarten.dae')

# Turn on GPU deformable geometry
brad.setStringAttribute("displayType", "GPUmesh")

# Retarget 
#setup locomotion
scene.run('BehaviorSetMaleMocapLocomotion.py')
setupBehaviorSet()
retargetBehaviorSet('ChrBrad')

# setup GUI
print 'Setting up GUI'
scene.run('GUIUtil.py')
gui = GUIManager()
class GUIHandler:
	def __init__(self):
		self.speed = 0.0	
		self.turnAngle = 0.0
		self.strafeDir = 0.0	

	def handleStartLocoButton(self,args):
	
		bml.execBML('ChrBrad', '<blend name="mocapLocomotion"/>')
		#animationList = [ 'ChrBrad@Idle01_YouLf01', 'ChrBrad@Idle01_BeatLowLf01', 'ChrBrad@Idle01_Shrug01', 'ChrBrad@Idle01_NegativeBt01']
		#which = random.randrange(0, len(animationList), 1)
		#bml.execBML('ChrBrad', '<animation name="' + animationList[which] +'"/>')
	
	def handleStopLocoButton(self,args):		
		#print 'handleStopLoco'
		bml.execBML('ChrBrad', '<blend name="null" sbm:schedule-mode="Now" sbm:wrap-mode="Loop"/>')
		#bml.execBML('ChrBrad', '<sbm:reach target="touchPawn" sbm:reach-duration="0.2"/>')
	def handleSpeedSlider(self,args):
		slider = args.window		
		#print 'speed slider value = ' + str(slider.getCurrentValue())	
		self.speed = slider.getCurrentValue()
		bml.execBML('ChrBrad', '<blend mode="update" name="mocapLocomotion" sbm:schedule-mode="Now" sbm:wrap-mode="Loop" x="'+str(self.speed)+'" y="'+str(self.turnAngle)+'" z="' +str(self.strafeDir)+ '"/>')
		
		
	def handleTurnSlider(self,args):
		slider = args.window
		#print 'turn slider value = ' + str(slider.getCurrentValue())
		self.turnAngle = slider.getCurrentValue()-180.0
		#bml.execBML('ChrBrad', '<blend mode="update" name="mocapLocomotion" sbm:schedule-mode="Now" sbm:wrap-mode="Loop" y="'+str(turnAngle)+'"/>')
		bml.execBML('ChrBrad', '<blend mode="update" name="mocapLocomotion" sbm:schedule-mode="Now" sbm:wrap-mode="Loop" x="'+str(self.speed)+'" y="'+str(self.turnAngle)+'" z="' +str(self.strafeDir)+ '"/>')
		
	def handleStrafeSlider(self,args):
		slider = args.window
		#print 'strafe slider value = ' + str(slider.getCurrentValue())
		self.strafeDir = slider.getCurrentValue()-90.0
		#bml.execBML('ChrBrad', '<blend mode="update" name="mocapLocomotion" sbm:schedule-mode="Now" sbm:wrap-mode="Loop" z="'+str(strafeDir)+'"/>')
		bml.execBML('ChrBrad', '<blend mode="update" name="mocapLocomotion" sbm:schedule-mode="Now" sbm:wrap-mode="Loop" x="'+str(self.speed)+'" y="'+str(self.turnAngle)+'" z="' +str(self.strafeDir)+ '"/>')		

alphaVal = 0.5
guiHandler = GUIHandler()
startLocoBtn = gui.createButton('startLocoBtn','Start Locomotion')
startLocoBtn.subscribeEvent(PushButton.EventClicked, guiHandler.handleStartLocoButton)
startLocoBtn.setAlpha(alphaVal)
stopLocoBtn = gui.createButton('stopLocoBtn','Stop Locomotion')
stopLocoBtn.subscribeEvent(PushButton.EventClicked, guiHandler.handleStopLocoButton)
stopLocoBtn.setAlpha(alphaVal)

#text1 = gui.createStaticText('text1','Speed')
speedSlider = gui.createSlider('speedSlider',4.0,0.0)
speedSlider.subscribeEvent(Slider.EventValueChanged, guiHandler.handleSpeedSlider)
speedSlider.setAlpha(alphaVal)
turnSlider = gui.createSlider('turnSlider',360.0,180.0)
turnSlider.subscribeEvent(Slider.EventValueChanged, guiHandler.handleTurnSlider)
turnSlider.setAlpha(alphaVal)
strafeSlider = gui.createSlider('strafeSlider',180.0,90.0)
strafeSlider.subscribeEvent(Slider.EventValueChanged, guiHandler.handleStrafeSlider)
strafeSlider.setAlpha(alphaVal)


# Start the simulation
print 'Starting the simulation'
sim.start()

bml.execBML('ChrBrad', '<body posture="ChrBrad@Idle01"/>')
bml.execBML('ChrBrad', '<saccade mode="listen"/>')
#bml.execBML('ChrBrad', '<gaze sbm:handle="brad" target="camera"/>')

sim.resume()
