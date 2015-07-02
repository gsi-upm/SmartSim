print "|--------------------------------------------|"
print "|             Starting Gaze Demo             |"
print "|--------------------------------------------|"

# Add asset paths
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath("script", "behaviorsets")
scene.addAssetPath('motion', 'ChrBrad')
scene.addAssetPath('motion', 'ChrRachel')
scene.addAssetPath('script','scripts')
scene.addAssetPath('script','examples')
scene.loadAssets()

# Set scene parameters and camera
print 'Configuring scene parameters and camera'
scene.setScale(1.0)
scene.setBoolAttribute('internalAudio', True)
scene.run('default-viewer.py')
camera = getCamera()
camera.setEye(0, 1.68, 2.58)
camera.setCenter(0, 0.89, -0.14)
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


# Setting up Brad and Rachel
print 'Setting up Brad'
brad = scene.createCharacter('ChrBrad', '')
bradSkeleton = scene.createSkeleton('ChrBrad.sk')
brad.setSkeleton(bradSkeleton)
bradPos = SrVec(.35, 0, 0)
brad.setPosition(bradPos)
brad.setHPR(SrVec(-17, 0, 0))
brad.createStandardControllers()
# Deformable mesh
brad.setDoubleAttribute('deformableMeshScale', .01)
brad.setStringAttribute('deformableMesh', 'ChrBrad.dae')

# setup gestures
scene.run('BehaviorSetGestures.py')
setupBehaviorSet()
retargetBehaviorSet('ChrBrad')

bml.execBML('ChrBrad', '<body posture="ChrBrad@Idle01" ready="0" relax="0"/>')


print 'Setting up Rachel'
rachel = scene.createCharacter('ChrRachel', '')
rachelSkeleton = scene.createSkeleton('ChrRachel.sk')
rachel.setSkeleton(rachelSkeleton)
rachelPos = SrVec(-.35, 0, 0)
rachel.setPosition(rachelPos)
rachel.setHPR(SrVec(17, 0, 0))
rachel.createStandardControllers()
# Deformable mesh
rachel.setDoubleAttribute('deformableMeshScale', .01)
rachel.setStringAttribute('deformableMesh', 'ChrRachel.dae')

# setup gestures
scene.run('BehaviorSetFemaleGestures.py')
setupBehaviorSet()
retargetBehaviorSet('ChrRachel')

bml.execBML('ChrRachel', '<body posture="ChrConnor@IdleStand01" ready=".2" relax=".2"/>')

# Add pawns in scene
print 'Adding pawn to scene'
gazeTarget = scene.createPawn('gazeTarget')
gazeTarget.setPosition(SrVec(0.75, 1.54, 0.33))

# Turn on GPU deformable geometry for all
for name in scene.getCharacterNames():
	scene.getCharacter(name).setStringAttribute("displayType", "GPUmesh")


# Make characters gaze at pawn
bml.execBML('ChrRachel', '<gaze sbm:joint-range="EYES CHEST" target="gazeTarget"/>')

# Variables to move pawn
gazeX = -2
gazeZ = 2
dirX = 1
dirZ = 1
speed = 0.005
lastTime = -8
class GazeDemo(SBScript):
	def update(self, time):
		global gazeX, gazeZ, dirX, dirZ, speed, lastTime
		# Change direction when hit border
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
		gazeTarget.setPosition(SrVec(gazeX, 2, gazeZ))
		
		diff = time - lastTime
		if diff > 10:
			diff = 0
			lastTime = time
			#Gaze at joints
			bml.execBMLAt(0, 'ChrBrad', '<gaze target="ChrRachel:base" sbm:joint-speed="800" sbm:joint-smooth="0.2"/>')
			bml.execBMLAt(2, 'ChrBrad', '<gaze target="ChrBrad:l_wrist" sbm:joint-speed="800" sbm:joint-smooth="0.2"/>')
			bml.execBMLAt(4, 'ChrBrad', '<gaze target="ChrBrad:r_ankle" sbm:joint-speed="800" sbm:joint-smooth="0.2"/>')
			bml.execBMLAt(6, 'ChrBrad', '<gaze target="ChrRachel:l_wrist" sbm:joint-speed="800" sbm:joint-smooth="0.2"/>')
			bml.execBMLAt(8, 'ChrBrad', '<gaze target="ChrRachel:spine4" sbm:joint-speed="800" sbm:joint-smooth="0.2"/>')
		
# Run the update script
scene.removeScript('gazedemo')
gazedemo = GazeDemo()
scene.addScript('gazedemo', gazedemo)