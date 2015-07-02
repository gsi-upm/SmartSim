import random
print "|--------------------------------------------|"
print "|          Starting Steering Demo            |"
print "|--------------------------------------------|"

# Add asset paths
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('motion', 'ChrMaarten')
scene.addAssetPath('motion', 'ChrRachel')
scene.addAssetPath("script", "behaviorsets")
scene.addAssetPath('script', 'scripts')
scene.loadAssets()

# Set scene parameters and camera
print 'Configuring scene parameters and camera'
scene.setScale(1.0)
scene.setBoolAttribute('internalAudio', True)
scene.run('default-viewer.py')
camera = getCamera()
camera.setEye(0, 15.49, 24.47)
camera.setCenter(0, 14.37, 22.82)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(100)
camera.setNearPlane(0.1)
camera.setAspectRatio(0.966897)
scene.getPawn('camera').setPosition(SrVec(0, -5, 0))

# Set joint map for Brad and Rachel
print 'Setting up joint map for Brad and Rachel'
scene.run('zebra2-map.py')
zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
bradSkeleton = scene.getSkeleton('ChrBrad.sk')
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse('ChrBrad')
rachelSkeleton = scene.getSkeleton('ChrRachel.sk')
zebra2Map.applySkeleton(rachelSkeleton)
zebra2Map.applyMotionRecurse('ChrRachel')

# Retarget setup
steerManager = scene.getSteerManager()
# Setting up group of Brads
print 'Setting up Brads'
bradList = []
bradPosX = -500.0
for i in range(15):
	baseName = 'ChrBrad%s' % i
	brad = scene.createCharacter(baseName, '')
	bradSkeleton = scene.createSkeleton('ChrBrad.sk')
	brad.setSkeleton(bradSkeleton)
	# Set position
	bradPos = SrVec((bradPosX + (i * 50.0))/100, 0, -1)
	brad.setPosition(bradPos)
	# Set up standard controllers
	brad.createStandardControllers()
	# Set collision shape scale
	brad.getAttribute('collisionShapeScale').setValue(SrVec(.01, .01, .01))
	# Set defomable mesh
	brad.setDoubleAttribute('deformableMeshScale', .01)
	brad.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
	# Play idle animation
	bml.execBML(baseName, '<body posture="ChrBrad@Idle01"/>')
	# Retarget character
	#retargetCharacter(baseName, 'ChrBrad.sk', False)
	if i== 0 : 
		scene.run('BehaviorSetMaleMocapLocomotion.py')
		setupBehaviorSet()
		retargetBehaviorSet(baseName)
	steerAgent = steerManager.createSteerAgent(baseName)
	steerAgent.setSteerStateNamePrefix("mocap")
	steerAgent.setSteerType("example")
	# Add current Brad into list
	bradList.append(brad)
	
# Setting up group of Rachels
print 'Setting up Rachels'
rachelList = []
rachelPosX = -500.0
for i in range(15):
	baseName = 'ChrRachel%s' % i
	rachel = scene.createCharacter(baseName, '')
	rachelSkeleton = scene.createSkeleton('ChrRachel.sk')
	rachel.setSkeleton(rachelSkeleton)
	# Set position
	rachelPos = SrVec((rachelPosX + (i * 50.0))/100, 0, 1)
	rachel.setPosition(rachelPos)
	# Set up standard controllers
	rachel.createStandardControllers()
	# Set collision shape scale
	rachel.getAttribute('collisionShapeScale').setValue(SrVec(.01, .01, .01))
	# Set deformable mesh
	rachel.setDoubleAttribute('deformableMeshScale', .01)
	rachel.setStringAttribute('deformableMesh', 'ChrRachel.dae')
	# Play idle animation
	bml.execBML(baseName, '<body posture="ChrRachel_ChrBrad@Idle01"/>')
	# Retarget character
	if i == 0:
 		retargetBehaviorSet(baseName)
	steerAgent = steerManager.createSteerAgent(baseName)
	steerAgent.setSteerStateNamePrefix("mocap")
	steerAgent.setSteerType("example")    
	#retargetCharacter(baseName, 'ChrRachel.sk', False)
	# Add Rachel into list
	rachelList.append(rachel)
  
steerManager.setEnable(False)
steerManager.setEnable(True)

# Turn on GPU deformable geometry for all
for name in scene.getCharacterNames():
	scene.getCharacter(name).setStringAttribute("displayType", "GPUmesh")

# Paths for Brad and Rachel

print 'Setting up GUI'
scene.run('GUIUtil.py')
gui = GUIManager()
bradSpeed = 0.0
rachelSpeed = 0.0
class GUIHandler:
	def __init__(self):
		self.speed = 0.0	
		self.turnAngle = 0.0
		self.strafeDir = 0.0	

	def handleBradSteerButton(self,args):
		print 'handleBradSteer'
		btn = args.window
		for brad in bradList:
			bmlCmd = '<locomotion target="' + str(btn.getText()) + '" speed="' + str(bradSpeed) + '"/>'
			#print bmlCmd
			bml.execBML(brad.getName(), bmlCmd)
	def handleRachelSteerButton(self,args):
		print 'handleRachelSteer'
		btn = args.window
		for rachel in rachelList:
			bmlCmd = '<locomotion target="' + str(btn.getText()) + '" speed="' + str(rachelSpeed) + '"/>'
			#print bmlCmd
			bml.execBML(rachel.getName(), bmlCmd)
	def handleBradSpeedSlider(self,args):		
		slider = args.window				
		bradSpeed = slider.getCurrentValue()
	def handleRachelSpeedSlider(self,args):		
		slider = args.window				
		rachelSpeed = slider.getCurrentValue()	
alphaVal = 0.5		
guiHandler = GUIHandler()	
wayPt = [SrVec(0,0,0), SrVec(-9, 0, -9), SrVec(9, 0, 9), SrVec(9, 0, -9), SrVec(-9, 0, 9)]
buttonSize = 80
buttonPosX = [ 80, 10, 140, 140, 10]
buttonPosY = [ 50, 10, 90, 10, 90]
bradBtnXOffset = 10
bradBtnYOffset = 30
for i in range(0,len(wayPt)):
	wayPtPawn = scene.createPawn('wayPt'+str(i))
	wayPtPawn.setPosition(wayPt[i])
gui.createStaticText('bradText',"Brad Goto", bradBtnXOffset, bradBtnYOffset-20)
for i in range(0,len(wayPt)):
	gotoBtn = gui.createButton('bradBtn'+str(i),'wayPt'+str(i),buttonPosX[i]+bradBtnXOffset, buttonPosY[i]+bradBtnYOffset, buttonSize)
	gotoBtn.subscribeEvent(PushButton.EventClicked, guiHandler.handleBradSteerButton)
	gotoBtn.setAlpha(alphaVal)
bradSlider = gui.createSlider('bradSpeed',4.0,0.0, bradBtnXOffset, bradBtnYOffset+160)
bradSlider.subscribeEvent(Slider.EventValueChanged, guiHandler.handleBradSpeedSlider)
bradSlider.setAlpha(alphaVal)
rachelBtnXOffset = 10
rachelBtnYOffset = 280
gui.createStaticText('rachelText',"Rachel Goto", rachelBtnXOffset, rachelBtnYOffset-20)
for i in range(0,len(wayPt)):
	gotoBtn = gui.createButton('rachelBtn'+str(i),'wayPt'+str(i),buttonPosX[i]+rachelBtnXOffset, buttonPosY[i]+rachelBtnYOffset, buttonSize)
	gotoBtn.subscribeEvent(PushButton.EventClicked, guiHandler.handleRachelSteerButton)
	gotoBtn.setAlpha(alphaVal)
	
bradSlider = gui.createSlider('rachelSpeed',4.0,0.0, rachelBtnXOffset, rachelBtnYOffset+160)
bradSlider.subscribeEvent(Slider.EventValueChanged, guiHandler.handleRachelSpeedSlider)
bradSlider.setAlpha(alphaVal)
