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
for i in range(10):
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
	brad.setVec3Attribute('deformableMeshScale', .01, .01, .01)
	brad.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
	# Play idle animation
	bml.execBML(baseName, '<body posture="ChrBrad@Idle01"/>')
	# Retarget character
	#retargetCharacter(baseName, 'ChrBrad.sk', False)
	if i== 0 : 
		scene.run('BehaviorSetMaleMocapLocomotion.py')
		setupBehaviorSet()
		retargetBehaviorSet(baseName)
	else:
		steerAgent = steerManager.createSteerAgent(baseName)
		steerAgent.setSteerStateNamePrefix("mocap")
		steerAgent.setSteerType("example")		
	retargetBehaviorSet(baseName)
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
	rachel.setVec3Attribute('deformableMeshScale', .01, .01, .01)
	rachel.setStringAttribute('deformableMesh', 'ChrRachel.dae')
	# Play idle animation
	bml.execBML(baseName, '<body posture="ChrRachel_ChrBrad@Idle01"/>')
	# Retarget character
	if i == 0:
		scene.run('BehaviorSetMaleMocapLocomotion.py')
		setupBehaviorSet()	
		retargetBehaviorSet(baseName)
	else:
		steerAgent = steerManager.createSteerAgent(baseName)
		steerAgent.setSteerStateNamePrefix("mocap")
		steerAgent.setSteerType("example")		
	# Add Rachel into list
	rachelList.append(rachel)

steerManager.setEnable(False)
steerManager.setIntAttribute("gridDatabaseOptions.gridSizeX", 200)
steerManager.setIntAttribute("gridDatabaseOptions.gridSizeY", 200)
steerManager.setIntAttribute("gridDatabaseOptions.numGridCellsX", 200)
steerManager.setIntAttribute("gridDatabaseOptions.numGridCellsY", 200)
steerManager.setEnable(True)

# Turn on GPU deformable geometry for all
for name in scene.getCharacterNames():
	scene.getCharacter(name).setStringAttribute("displayType", "GPUmesh")


# Paths for Brad and Rachel
print 'Setting up paths for Brad and Rachel'
bradPath = [SrVec(-9, -9, 0), SrVec(9, 9, 0)]
rachelPath = [SrVec(9, 9, 0), SrVec(-9, -9, 0)]
pathAmt = len(bradPath)

bradCur = 0
rachelCur = 0
canTime = True
last = 0
delay = 10
class LocomotionDemo(SBScript):
	def update(self, time):
		global bradCur, rachelCur, canTime, last
		diff = time - last
		if diff >= delay:
			diff = 0
			canTime = True
		# When time's up, do action
		if canTime:
			last = time
			canTime = False
			# Move all Rachel
			for rachel in rachelList:
				bml.execBML(rachel.getName(), '<locomotion target="' + vec2str(rachelPath[rachelCur]) + '" type="basic" speed="' + str(random.uniform(1, 5)) + '"/>')
			rachelCur = rachelCur + 1
			# If raches max path, reset
			if rachelCur >= pathAmt:
				rachelCur = 0
			# Move all brad 
			for brad in bradList:
				bml.execBML(brad.getName(), '<locomotion target="' + vec2str(bradPath[bradCur]) + '" type="basic" speed="' + str(random.uniform(1, 5)) + '"/>')
			bradCur = bradCur + 1
			# If reaches max path, reset
			if bradCur >= pathAmt:
				bradCur = 0
			
# Run the update script
scene.removeScript('locomotiondemo')
locomotiondemo = LocomotionDemo()
scene.addScript('locomotiondemo', locomotiondemo)

def vec2str(vec):
	''' Converts SrVec to string '''
	x = vec.getData(0)
	y = vec.getData(1)
	z = vec.getData(2)
	if -0.0001 < x < 0.0001: x = 0
	if -0.0001 < y < 0.0001: y = 0
	if -0.0001 < z < 0.0001: z = 0
	return "" + str(x) + " " + str(y) + " " + str(z) + ""
