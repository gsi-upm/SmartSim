import random
print "|--------------------------------------------|"
print "|       Starting Physics Force Demo          |"
print "|--------------------------------------------|"

# Add asset paths
scene.addAssetPath('script', 'sbm-common/scripts')
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('mesh', 'retarget/mesh')
scene.addAssetPath('motion', 'sbm-common/common-sk')
scene.addAssetPath("script", "behaviorsets")
scene.addAssetPath('script', 'scripts')
scene.loadAssets()

scene.setScale(0.01)
camera = getCamera()
camera.setEye(-55.4046, 209.262, -1010.21)
camera.setCenter(199.493, 56.5922, 83.8954)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(5000.0)
camera.setNearPlane(5.0)
camera.setAspectRatio(1.02)
# Run required scripts
scene.run('init-param-animation.py')
#scene.run('init-example-reach.py')

# Set scene parameters and camera
print 'Configuring scene parameters and camera'
scene.setBoolAttribute('internalAudio', True)
#scene.run('default-viewer.py')
#camera = getCamera()
#camera.setEye(-409, 405, -917)
#camera.setCenter(-307, 325, -767)
#scene.getPawn('camera').setPosition(SrVec(0, -20, 0))

# Setting up characters
print 'Setting up characters'
# Brad1
brad1 = scene.createCharacter('brad1', '')
brad1Skeleton = scene.createSkeleton('common.sk')
brad1.setSkeleton(brad1Skeleton)
brad1.setPosition(SrVec(0, 122, 860))
brad1.setHPR(SrVec(180, 0, 0))
brad1.createStandardControllers()
brad1.setStringAttribute('deformableMesh', 'Brad.dae')
scene.run('BehaviorSetReaching.py')
setupBehaviorSet()
retargetBehaviorSet('brad1')
bml.execBML('brad1', '<body posture="HandsAtSide_Motex"/>')
# Brad2
brad2 = scene.createCharacter('brad2', '')
brad2Skeleton = scene.createSkeleton('common.sk')
brad2.setSkeleton(brad2Skeleton)
brad2.setPosition(SrVec(100, 102, -650))
brad2.createStandardControllers()
brad2.setStringAttribute('deformableMesh', 'Brad.dae')
retargetBehaviorSet('brad2')
bml.execBML('brad2', '<body posture="HandsAtSide_Motex"/>')
	
# Ball variables
ballX = 115 # 112 - 118
ballY = 20
ballZ = 45
brad2Z = scene.getCharacter('brad2').getPosition().getData(2)

# Adding pawns in scene
print 'Adding pawns in the scene'
# Ball
pawn2 = scene.createPawn('pawn2')
pawn2.setStringAttribute('collisionShape', 'sphere')
pawn2.getAttribute('collisionShapeScale').setValue(SrVec(15, 15, 15))
pawn2.setPosition(SrVec(ballX, ballY, brad2Z + ballZ))
# Handle
handle = scene.createPawn('handle1')
handle.setStringAttribute('collisionShape', 'box')
handle.getAttribute('collisionShapeScale').setValue(SrVec(5, 5, 5))
handle.setPosition(SrVec(0, 230, 800))
# Goal post
pole1 = scene.createPawn('pole1')
pole1.setStringAttribute('collisionShape', 'box')
pole1.getAttribute('collisionShapeScale').setValue(SrVec(10, 230, 10))
pole1.setPosition(SrVec(-200, 0, 800))
pole2 = scene.createPawn('pole2')
pole2.setStringAttribute('collisionShape', 'box')
pole2.getAttribute('collisionShapeScale').setValue(SrVec(10, 230, 10))
pole2.setPosition(SrVec(200, 0, 800))
pole3 = scene.createPawn('pole3')
pole3.setStringAttribute('collisionShape', 'box')
pole3.getAttribute('collisionShapeScale').setValue(SrVec(200, 10, 10))
pole3.setPosition(SrVec(0, 230, 800))

# Turn on GPU deformable geometry for all
for name in scene.getCharacterNames():
	scene.getCharacter(name).setStringAttribute("displayType", "GPUmesh")


# Configure character physics
print 'Configuring character physics'
phyManager = scene.getPhysicsManager()
phyManager.getPhysicsEngine().setBoolAttribute('enable', True)
# Brad1 and Brad2
brad1.getAttribute('createPhysics').setValue()
brad2.getAttribute('createPhysics').setValue()
phyManager.getPhysicsCharacter('brad1').setBoolAttribute('usePD', True)
phyManager.getPhysicsCharacter('brad2').setBoolAttribute('usePD', True)

def constrainChr(chr, joint, object=''):
	''' Name of character, joint name of character, object name to act as constraint'''
	bodyLink = phyManager.getJointObj(chr, joint)
	bodyLink.setBoolAttribute('constraint', True)
	bodyLink = phyManager.getJointObj(chr, joint)
	bodyLink.setStringAttribute('constraintTarget', object)

constrainChr('brad2', 'spine1')
constrainChr('brad2', 'r_ankle')

# Set up pawn physics
print 'Setting up pawn physics'
pawn2.getAttribute('createPhysics').setValue()
		
# Physics manager and forces
phyManager = scene.getPhysicsManager()

# Reach once
bml.execBML('brad1', '<sbm:reach sbm:action="touch" sbm:reach-finish="false" target="handle1"/>')

reset = False
force = 5
last = 0
canTime = True
delay = 5
resetTime = 0
class PhysicsForceDemo(SBScript):
	def update(self, time):
		global last, canTime, resetTime
		if canTime:
			last = time
			canTime = False
		diff = time - last
		if diff >= delay:
			diff = 0
			canTime = True
		# If can time, do action
		if canTime:
			global reset
			if not reset:
				pawn2.setBoolAttribute('enablePhysics', True)
				phyManager.getPhysicsCharacter('brad2').setBoolAttribute('enable', True)
				# Brad 2
				phyManager.applyForceToCharacter('brad2', 'l_ankle', SrVec(0, 0, 10000 * force))
				reset = True
			elif reset:
				resetKick()
		# When kicked
		if reset:
			diff = time - resetTime
			if diff > 0.3:
				phyManager.applyForceToCharacter('brad2', 'l_ankle', SrVec(0, 0, 0))
			# Brad and ball position
			bradX = scene.getCharacter('brad1').getPosition().getData(0)
			bradY = scene.getCharacter('brad1').getPosition().getData(1)
			tarX = scene.getPawn('pawn2').getPosition().getData(0)
			tarY = scene.getPawn('pawn2').getPosition().getData(1)
			# Amount of force and direction
			magX = (bradX - tarX) / 10
			magY = (bradY - tarY) / 10	
			forceX = random.randrange(500, 1000) * -magX
			forceY = random.randrange(250, 300) * magY
			# Apply force
			phyManager.applyForceToCharacter('brad1', 'spine4', SrVec(forceX, forceY, 0))
			phyManager.applyForceToCharacter('brad1', 'l_ankle', SrVec(forceX, forceY, 0))
			phyManager.applyForceToCharacter('brad1', 'r_ankle', SrVec(forceX, forceY, 0))
			phyManager.applyForceToCharacter('brad1', 'r_wrist', SrVec(forceX, forceY, 0))
		if not reset:
			resetTime = time
			phyManager.applyForceToCharacter('brad1', 'spine4', SrVec(0, 0, 0))
			phyManager.applyForceToCharacter('brad1', 'l_ankle', SrVec(0, 0, 0))
			phyManager.applyForceToCharacter('brad1', 'r_ankle', SrVec(0, 0, 0))
			phyManager.applyForceToCharacter('brad1', 'r_wrist', SrVec(0, 0, 0))
			'''Make character lying down stand up'''
			
def resetKick():
	# Reset and set up for new kick
	global reset
	pawn2.setBoolAttribute('enablePhysics', False)
	phyManager.getPhysicsCharacter('brad2').setBoolAttribute('enable', False)
	ballX = random.uniform(112, 119)
	pawn2.setPosition(SrVec(ballX, ballY, brad2Z + ballZ))
	phyManager.applyForceToCharacter('brad2', 'l_ankle', SrVec(0, 0, 0))
	reset = False
		
# Run the update script
scene.removeScript('physicsforcedemo')
physicsforcedemo = PhysicsForceDemo()
scene.addScript('physicsforcedemo', physicsforcedemo)

class ReachingHandler(SBEventHandler):
	def executeAction(self, ev):
		params = ev.getParameters()
		if 'brad1' in params and 'reach-complete' in params:
			constrainChr('brad1', 'l_wrist', 'handle1')
			phyManager.getPhysicsCharacter('brad1').setBoolAttribute('enable', True)
			
evtMgr = scene.getEventManager()
reachingHdl = ReachingHandler()
evtMgr.addEventHandler('reachNotifier', reachingHdl)
