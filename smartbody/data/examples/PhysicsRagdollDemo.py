import random
print "|--------------------------------------------|"
print "|      Starting Physics Ragdoll Demo         |"
print "|--------------------------------------------|"

# Add asset paths
scene.addAssetPath('script', 'sbm-common/scripts')
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('mesh', 'retarget/mesh')
scene.addAssetPath('motion', 'sbm-common/common-sk')
scene.loadAssets()

# Run required scripts
scene.run('init-param-animation.py')

# Set scene parameters and camera
print 'Configuring scene parameters and camera'
scene.setBoolAttribute('internalAudio', True)

scene.setScale(0.01)
camera = getCamera()
camera.setEye(-20, 187, 443)
camera.setCenter(-20, 136, 151)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(5000.0)
camera.setNearPlane(5.0)
camera.setAspectRatio(1.02)

# Setting up characters
print 'Setting up characters'
brad1 = scene.createCharacter('brad1', '')
brad1Skeleton = scene.createSkeleton('common.sk')
brad1.setSkeleton(brad1Skeleton)
brad1.setPosition(SrVec(0, 200, 0))
brad1.createStandardControllers()
brad1.setStringAttribute('deformableMesh', 'Brad.dae')
bml.execBML('brad1', '<body posture="HandsAtSide_Motex"/>')

brad2 = scene.createCharacter('brad2', '')
brad2Skeleton = scene.createSkeleton('common.sk')
brad2.setSkeleton(brad2Skeleton)
brad2.setPosition(SrVec(0, 150, 200))
brad2.createStandardControllers()
brad2.setStringAttribute('deformableMesh', 'Brad.dae')
bml.execBML('brad2', '<body posture="HandsAtSide_Motex"/>')

bradList = []
for name in scene.getCharacterNames():
	bradList.append(scene.getCharacter(name))
	
# Adding pawns in scene
print 'Adding pawns in the scene'
# Constraint
constraint = scene.createPawn('constraint')
constraint.setStringAttribute('collisionShape', 'sphere')
constraint.getAttribute('collisionShapeScale').setValue(SrVec(10, 10, 10))
constraint.setPosition(SrVec(0, 240, 0))
# Ball moved by force
ball = scene.createPawn('ball')
ball.setStringAttribute('collisionShape', 'sphere')
ball.getAttribute('collisionShapeScale').setValue(SrVec(15, 15, 15))
ball.setPosition(SrVec(75, 150, 0))
# Obstacles for ragdoll to fall on
wall1 = scene.createPawn('wall1')
wall1.setStringAttribute('collisionShape', 'box')
wall1.getAttribute('collisionShapeScale').setValue(SrVec(20, 80, 50))
wall1.setPosition(SrVec(-50, 80, -40))
wall2 = scene.createPawn('wall2')
wall2.setStringAttribute('collisionShape', 'box')
wall2.getAttribute('collisionShapeScale').setValue(SrVec(60, 50, 40))
wall2.setPosition(SrVec(-120, 70, 160))
wall3 = scene.createPawn('wall3')
wall3.setStringAttribute('collisionShape', 'box')
wall3.getAttribute('collisionShapeScale').setValue(SrVec(50, 50, 50))
wall3.setPosition(SrVec(150, 80, -20))
wall4 = scene.createPawn('wall4')
wall4.setStringAttribute('collisionShape', 'box')
wall4.getAttribute('collisionShapeScale').setValue(SrVec(50, 50, 50))
wall4.setPosition(SrVec(90, 100, 150))

# Turn on GPU deformable geometry for all
for name in scene.getCharacterNames():
	scene.getCharacter(name).setStringAttribute("displayType", "GPUmesh")

# Configuring character physics
print 'Configuring character physics'
phyManager = scene.getPhysicsManager()
phyManager.getPhysicsEngine().setBoolAttribute('enable', True)
# Brad1 and Brad2
brad1.getAttribute('createPhysics').setValue()
brad2.getAttribute('createPhysics').setValue()
phyManager.getPhysicsCharacter('brad1').setBoolAttribute('usePD', True)
phyManager.getPhysicsCharacter('brad2').setBoolAttribute('usePD', True)

def constrainChr(chr, joint, object=''):
	''' Name of character, jointname of character, object name to act as constraint '''
	bodyLink = phyManager.getJointObj(chr, joint)
	bodyLink.setBoolAttribute('constraint', True)
	bodyLink.setStringAttribute('constraintTarget', object)
	
constrainChr('brad1', 'l_wrist', 'constraint')

# Set up pawn physics
print 'Setting up pawn physics'
ball.getAttribute('createPhysics').setValue()
wall1.getAttribute('createPhysics').setValue()
wall2.getAttribute('createPhysics').setValue()
wall3.getAttribute('createPhysics').setValue()
wall4.getAttribute('createPhysics').setValue()

# Forces
forceX = forceZ = 150
bradRagdoll = False

'''
# Enable collision
collisionManager = getScene().getCollisionManager()
collisionManager.setStringAttribute('collisionResolutionType', 'default')
collisionManager.setEnable(True)
'''
curZ = 0
curX = 0
amountZ = amountX = -1
speed = 0.05
last = 0
canTime = True
delay = 5
started = False
class PhysicsRagdollDemo(SBScript):
	def update(self, time):
		global canTime, last, started, amountZ, curZ, amountX, curX
		global forceX, forceZ, bradRagdoll
		if canTime:
			last = time
			canTime = False
		diff = time - last
		if diff >= delay:
			diff = 0
			canTime = True
		# Trigger once
		if canTime and not started:
			started = True
			ball.setBoolAttribute('enablePhysics', True)
			phyManager.getPhysicsCharacter('brad1').setBoolAttribute('enable', True)
			bml.execBML('brad1', '<body posture="Walk"/>')
		# If time's up, do action
		if canTime:
			forceX = random.uniform(-200, 200)
			forceZ = random.uniform(-200, 200)
			phyManager.applyForceToPawn('ball', SrVec(forceX, 0, forceZ))
			randX = random.uniform(-50000, 50000)
			randY = random.uniform(-50000, 50000)
			randZ = random.uniform(-50000, 50000)
			phyManager.applyForceToCharacter('brad1', 'spine1', SrVec(randX, randY, randZ))
			if not bradRagdoll:
				bradRagdoll = True
				# Brad 2
				phyManager.getPhysicsCharacter(brad2.getName()).setBoolAttribute('enable', True)
			elif bradRagdoll:
				bradRagdoll = False
				phyManager.getPhysicsCharacter(brad2.getName()).setBoolAttribute('enable', False)
				# Randomize position and rotation
				randX = random.uniform(-150, 150)
				randY = random.uniform(200, 250)
				randZ = random.uniform(0, 150)
				randH = random.uniform(-180, 180)
				randP = random.uniform(-180, 180)
				randR = random.uniform(-180, 180)
				scene.getCharacter(brad2.getName()).setPosition(SrVec(randX, randY, randZ))
				scene.getCharacter(brad2.getName()).setHPR(SrVec(randH, randP, randR))
			
# Run the update script
scene.removeScript('physicsragdolldemo')
physicsragdolldemo = PhysicsRagdollDemo()
scene.addScript('physicsragdolldemo', physicsragdolldemo)
