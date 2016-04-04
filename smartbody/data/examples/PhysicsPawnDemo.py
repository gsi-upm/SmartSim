import random
print "|--------------------------------------------|"
print "|        Starting Physics Pawn Demo          |"
print "|--------------------------------------------|"

# Add asset paths
scene.addAssetPath('script', 'sbm-common/scripts')
scene.loadAssets()

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

# Set up pawns in scene
print 'Adding pawns to scene'
numPawns = 50
for i in range(numPawns):
	baseName = 'phy%s' % i
	shapeList = ['sphere', 'box', 'capsule']
	size = random.randrange(5, 30)
	pawn = scene.createPawn(baseName)
	pawn.setStringAttribute('collisionShape', random.choice(shapeList))
	pawn.getAttribute('collisionShapeScale').setValue(SrVec(15, 15, 15))

# Append all pawn to list
pawnList = []
for name in scene.getPawnNames():
	if 'phy' in name:
		pawnList.append(scene.getPawn(name))
		
# Setup pawn physics
print 'Setting up pawn physics'
phyManager = scene.getPhysicsManager()
phyManager.getPhysicsEngine().setBoolAttribute('enable', True)
for pawn in pawnList:
	pawn.getAttribute('createPhysics').setValue()
	# Random mass
	mass = random.randrange(1, 11)
	phyManager.getPhysicsPawn(pawn.getName()).setDoubleAttribute('mass', mass)

last = 0
canTime = True
delay = 5
physicsOn = True
class PhysicsPawnDemo(SBScript):
	def update(self, time):
		global canTime, last, physicsOn
		if canTime:
			last = time
			canTime = False
		diff = time - last
		if diff >= delay:
			diff = 0
			canTime = True
		# When time's up, do action
		if canTime:
			for pawn in pawnList:
				pawn.setBoolAttribute('enablePhysics', physicsOn)
			physicsOn = not physicsOn
			randomizePos()

size = 150
# Randomize position and rotation
def randomizePos():
	for pawn in pawnList:
		x = random.uniform(-size, size) + 1
		y = random.uniform(50, size * 3) + 1
		z = random.uniform(-size, size) + 1
		h = random.uniform(-180, 180) + 1
		p = random.uniform(-180, 180) + 1
		r = random.uniform(-180, 180) + 1
		pawn.setPosition(SrVec(x, y, z))
		pawn.setHPR(SrVec(h, p, r))

randomizePos()
			
# Run the update script
scene.removeScript('physicspawndemo')
physicspawndemo = PhysicsPawnDemo()
scene.addScript('physicspawndemo', physicspawndemo)