import random
print "|--------------------------------------------|"
print "|          Starting Steering Demo            |"
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
obj = getCamera()
obj.setEye(-0.698968, 3.57125, 5.60726)
obj.setCenter(-0.564823, 2.75929, 3.78579)
obj.setUpVector(SrVec(-0.00316508, 0.992044, 0.12585))
obj.setScale(1)
obj.setFov(0.622106)
obj.setFarPlane(100)
obj.setNearPlane(0.1)
obj.setAspectRatio(1.39286)
scene.getPawn('camera').setPosition(SrVec(0, -5, 0))

# Set joint map for Brad 
print 'Setting up joint map for Brad'
scene.run('zebra2-map.py')
zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
bradSkeleton = scene.getSkeleton('ChrBrad.sk')
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse('ChrMaarten')

# Retarget setup
steerManager = scene.getSteerManager()
# Setting up group of Brads
print 'Setting up Brads'
bradList = []
bradPosX = 0.0
# set up the characters
for i in range(4):
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
	# Retarget character
	if i== 0 : 
		scene.run('BehaviorSetMaleMocapLocomotion.py')
		setupBehaviorSet()
	retargetBehaviorSet(baseName)
	# Add current Brad into list
	bradList.append(brad)

# Turn on GPU deformable geometry for all
for name in scene.getCharacterNames():
	scene.getCharacter(name).setStringAttribute("displayType", "GPUmesh")	
	
# retarget locomotion
for i in range(4):
	baseName = 'ChrBrad%s' % i	
	if i== 0 : 
		scene.run('BehaviorSetMaleMocapLocomotion.py')
		setupBehaviorSet()
	retargetBehaviorSet(baseName)
	
# retarget gestures
for i in range(4):
	baseName = 'ChrBrad%s' % i	
	if i== 0 : 
		scene.run('BehaviorSetGestures.py')
		setupBehaviorSet()
	retargetBehaviorSet(baseName)

	
	


# set to idle pose
for i in range(4):
	# Play idle animation
	bml.execBML('ChrBrad%s' % i, '<body posture="ChrBrad@Idle01"/>')

	
# Paths for Brad

print 'Setting up GUI'
scene.run('GUIUtil.py')
gazeTargetName = 'wayPt0'
gazeTargetMove = False
gui = GUIManager()
class GUIHandler:
	def __init__(self):
		self.speed = 0.0	
		self.turnAngle = 0.0
		self.strafeDir = 0.0	

	def handleBradSteerButton(self,args):
		print 'handleBradSteer'
		btn = args.window
		for brad in bradList:
			bmlCmd = '<locomotion target="' + str(btn.getText()) + '" speed="' + str(random.uniform(0.5, 2)) + '"/>'
			print bmlCmd
			bml.execBML(brad.getName(), bmlCmd)
	def handleBradGazeButton(self,args):
		global gazeTargetName
		print 'handleBradGaze'
		btn = args.window
		gazeTargetName = str(btn.getText())
		for brad in bradList:
			bmlCmd = '<gaze target="' + str(btn.getText()) + '" sbm:joint-speed="15000"/>'
			print bmlCmd
			bml.execBML(brad.getName(), bmlCmd)
	def handleGazeTargetMove(self,args):
		global gazeTargetMove
		chkBox = args.window
		gazeTargetMove = chkBox.isSelected()
			
guiHandler = GUIHandler()
wayPt = [SrVec(0,0,0), SrVec(-5, 0, -5), SrVec(5, 0, 5), SrVec(5, 0, -5), SrVec(-5, 0, 5)]
buttonSize = 80
buttonPosX = [ 80, 10, 140, 140, 10]
buttonPosY = [ 50, 10, 90, 10, 90]
gazeBtnXOffset = 10
gazeBtnYOffset = 30
alphaVal = 0.5
for i in range(0,len(wayPt)):
	wayPtPawn = scene.createPawn('wayPt'+str(i))
	wayPtPawn.setPosition(wayPt[i])
gui.createStaticText('bradText',"Brad Gaze At", gazeBtnXOffset, gazeBtnYOffset-20)
for i in range(0,len(wayPt)):
	gotoBtn = gui.createButton('gazeBtn'+str(i),'wayPt'+str(i),buttonPosX[i]+gazeBtnXOffset, buttonPosY[i]+gazeBtnYOffset, buttonSize)
	gotoBtn.subscribeEvent(PushButton.EventClicked, guiHandler.handleBradGazeButton)
	gotoBtn.setAlpha(alphaVal)

gotoBtnXOffset = 10
gotoBtnYOffset = 200
gui.createStaticText('rachelText',"Brad Goto", gotoBtnXOffset, gotoBtnYOffset-20)
for i in range(0,len(wayPt)):
	gotoBtn = gui.createButton('gotoBtn'+str(i),'wayPt'+str(i),buttonPosX[i]+gotoBtnXOffset, buttonPosY[i]+gotoBtnYOffset, buttonSize)
	gotoBtn.subscribeEvent(PushButton.EventClicked, guiHandler.handleBradSteerButton)
	gotoBtn.setAlpha(alphaVal)
targetMoveBtn = gui.createCheckBox('moveTarget','moveTarget')
targetMoveBtn.subscribeEvent(ToggleButton.EventSelectStateChanged, guiHandler.handleGazeTargetMove)
targetMoveBtn.setAlpha(alphaVal)

gazeX = -3
gazeZ = 3
dirX = 1
dirZ = 1
speed = 0.005	
class GazeDemo(SBScript):
	def update(self, time):
		global gazeX, gazeZ, dirX, dirZ, speed, lastTime
		# Change direction when hit border
		if gazeTargetMove == False:
			return		
		gazeTarget = scene.getPawn(gazeTargetName)
		if gazeTarget == None:				
			return
		wayPtID = int(gazeTargetName[5])	
		curWayPt = wayPt[wayPtID] 
		if gazeX > 2:
			dirX = -1
		elif gazeX < -2:
			dirX = 1
		if gazeZ > 2:
			dirZ = -1
		elif gazeZ < -0:
			dirZ = 1
		gazeX = gazeX + speed * dirX
		gazeZ = gazeZ + speed * dirZ		
		gazeTarget.setPosition(SrVec(curWayPt.getData(0)+gazeX, curWayPt.getData(1), curWayPt.getData(2)+gazeZ))
gazedemo = GazeDemo()
scene.addScript('gazedemo', gazedemo)

