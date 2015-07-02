print "|--------------------------------------------|"
print "|        Starting Perlin Noise Demo          |"
print "|--------------------------------------------|"

# Add asset paths
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('motion', 'ChrMaarten')
scene.addAssetPath('script', 'scripts')
scene.addAssetPath('script', 'behaviorsets')

scene.loadAssets()

# Set scene parameters to fit new Brad and Rachel
print 'Configuring scene parameters and camera'
scene.setScale(1.0)
scene.setBoolAttribute('internalAudio', True)
scene.run('default-viewer.py')
camera = getCamera()
camera.setEye(0.16, 1.44, 1.73)
camera.setCenter(0.16, 0.94, -0.17)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(100)
camera.setNearPlane(0.1)
camera.setAspectRatio(0.966897)
scene.getPawn('camera').setPosition(SrVec(0, -2, 0))

# Set joint map for Brad
print 'Setting up joint map for Brad'
scene.run('zebra2-map.py')
zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
bradSkeleton = scene.getSkeleton('ChrBrad.sk')
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse('ChrBrad')

# Setting up Brad
print 'Setting up Brad'
scene.run('BehaviorSetGestures.py')
setupBehaviorSet()
bradPosX = -45.0
for i in range(2):
	baseName = 'ChrBrad%s' % i
	brad = scene.createCharacter(baseName, '')
	bradSkeleton = scene.createSkeleton('ChrBrad.sk')
	brad.setSkeleton(bradSkeleton)
	# Set position
	bradPos = SrVec((bradPosX + (i * 100))/100, 0, 0)
	brad.setPosition(bradPos)
	brad.createStandardControllers()
	# Gesture map setup
	brad.setStringAttribute('gestureMap', 'ChrBrad')
	brad.setBoolAttribute('gestureRequest.autoGestureTransition', True)
	# Set deformable mesh
	brad.setDoubleAttribute('deformableMeshScale', 0.01)
	brad.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
	# Play idle animation
	bml.execBML(baseName, '<body posture="ChrBrad@Idle01"/>')
	retargetBehaviorSet(baseName)
	brad.setStringAttribute("displayType", "GPUmesh")
	
# Set rotation
scene.getCharacter('ChrBrad0').setHPR(SrVec(17, 0, 0))
scene.getCharacter('ChrBrad1').setHPR(SrVec(-17, 0, 0))
	

lastTime = -5
import random
class PerlinNoiseDemo(SBScript):
	def update(self, time):
		global lastTime
		timeDiff = time - lastTime
		# When time's up, do action
		if timeDiff >= 5:
			lastTime = time
			which = random.randrange(0, 3, 1)
			# Perlin noise can be added by adding sbm:joint-range="l_shoulder" sbm:frequency="0.03" sbm:scale="0.02" at the end 
			# Perlin noise
			if which == 0:
				bml.execBML('ChrBrad0', '<gesture lexeme="DEICTIC" type="YOU" poststroke_hold="2" sbm:joint-range="l_shoulder l_elbow" sbm:frequency="0.05" sbm:scale="0.02"/>')
				bml.execBML('ChrBrad1', '<gesture lexeme="DEICTIC" type="YOU" poststroke_hold="2"/>')
			elif which == 1:
				bml.execBML('ChrBrad0', '<gesture lexeme="METAPHORIC" type="OBLIGATION" mode="LEFT_HAND" poststroke_hold="2" sbm:joint-range="l_shoulder l_elbow" sbm:frequency="0.05" sbm:scale="0.02"/>')
				bml.execBML('ChrBrad1', '<gesture lexeme="METAPHORIC" type="OBLIGATION" mode="LEFT_HAND" poststroke_hold="2"/>')
			elif which == 2:
				bml.execBML('ChrBrad0', '<gesture lexeme="DEICTIC" type="ME" poststroke_hold="2" sbm:joint-range="l_shoulder l_elbow" sbm:frequency="0.05" sbm:scale="0.02"/>')
				bml.execBML('ChrBrad1', '<gesture lexeme="DEICTIC" type="ME" poststroke_hold="2"/>')					
			
# Run the update script
scene.removeScript('perlinnoisedemo')
perlinnoisedemo = PerlinNoiseDemo()
scene.addScript('perlinnoisedemo', perlinnoisedemo)
