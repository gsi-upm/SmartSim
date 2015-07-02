import math
import random
print "|--------------------------------------------|"
print "|         Starting Locomotion Demo           |"
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
camera.setEye(0, 19, 25)
camera.setCenter(0, 18, 24)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(100)
camera.setNearPlane(0.1)
camera.setAspectRatio(0.966897)
scene.getPawn('camera').setPosition(SrVec(0, -5, 0))

# Set joint map for Brad
print 'Setting up joint map for Brad'
scene.run('zebra2-map.py')
zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
bradSkeleton = scene.getSkeleton('ChrBrad.sk')
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse('ChrBrad')

steerManager = scene.getSteerManager()
# Setting up Brads
print 'Setting up Brads'
amount = 16
row = 0; column = 0;
offsetX = 0; offsetZ = 0;
for i in range(amount):
	baseName = 'ChrBrad%s' % i
	brad = scene.createCharacter(baseName, '')
	bradSkeleton = scene.createSkeleton('ChrBrad.sk')
	brad.setSkeleton(bradSkeleton)
	# Set position logic
	posX = (-100 * (5/2)) + 100 * column
	posZ = ((-100 / math.sqrt(amount)) * (amount/2)) + 100 * row
	column = column + 1
	if column >= 5:
		column = 0
		row = row + 1
	bradPos = SrVec((posX + offsetX)/100, 0, (posZ + offsetZ)/100)
	brad.setPosition(bradPos)
	# Set up standard controllers
	brad.createStandardControllers()
	# Set deformable mesh
	brad.setDoubleAttribute('deformableMeshScale', .01)
	brad.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
	# Retarget character
	if i== 0 : 
		scene.run('BehaviorSetMaleLocomotion.py')
		setupBehaviorSet()
	retargetBehaviorSet(baseName)
	# Set up steering	
	steerManager.setEnable(False)
	brad.setBoolAttribute('steering.pathFollowingMode', False)
	steerManager.setEnable(True)
	# Play default animation
	bml.execBML(baseName, '<body posture="ChrBrad@Idle01"/>')

# Turn on GPU deformable geometry for all
for name in scene.getCharacterNames():
	scene.getCharacter(name).setStringAttribute("displayType", "GPUmesh")

	
steeringGroup = []
pathfindingGroup = []
# Assign groups
print 'Assigning Brads in groups'
for name in scene.getCharacterNames():
	if 'ChrBrad' in name:
		if len(steeringGroup) < amount/2:
			steeringGroup.append(scene.getCharacter(name))
		else:
			# Set pathfinding on
			scene.getCharacter(name).setBoolAttribute('steering.pathFollowingMode', True)
			pathfindingGroup.append(scene.getCharacter(name))

# Adding pawns to scene
print 'Adding pawns to scene'
target0 = scene.createPawn('target0')
target0.setPosition(SrVec(-10, 0, -10))
target1 = scene.createPawn('target1')
target1.setPosition(SrVec(-4, 0, 10))
			
group1Reached = True
group2Reached = True

# Update to repeat paths
last = 0
canTime = True
delay = 30
class LocomotionDemo(SBScript):
	def update(self, time):
		global group1Reached, group2Reached, canTime, last
		if canTime:
			last = time
			canTime = False
			group1Reached = group2Reached = True
		diff = time - last
		if diff >= delay:
			diff = 0
			canTime = True
		# Once group 1 completes path, do again
		if group1Reached:
			for brad in steeringGroup:
				bml.execBML(brad.getName(), '<locomotion manner="run" target="-10 10 -4 -10 target1 target0"/>')
			group1Reached = False
		# Once group 2 completes path, do again
		if group2Reached:
			for brad in pathfindingGroup:
				bml.execBML(brad.getName(), '<locomotion manner="run" target="10 10 4 -10 4 10 10 -10"/>')
			group2Reached = False
			
# Run the update script
scene.removeScript('locomotiondemo')
locomotiondemo = LocomotionDemo()
scene.addScript('locomotiondemo', locomotiondemo)
