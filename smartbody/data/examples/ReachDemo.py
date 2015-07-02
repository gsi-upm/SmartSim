import random
print "|--------------------------------------------|"
print "|            Starting Reach Demo             |"
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
camera.setEye(0, 3.45, 5.02)
camera.setCenter(0, 2.46, 3.29)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(100)
camera.setNearPlane(0.1)
camera.setAspectRatio(0.966897)
scene.getPawn('camera').setPosition(SrVec(0, -2, 0))

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
#scene.run('motion-retarget.py')
# Animation setup
#scene.run('init-param-animation.py')

# Setting up Brads and Rachels
print 'Setting up Brads'
for i in range(2):
	baseName = 'ChrBrad%s' % i
	brad = scene.createCharacter(baseName, '')
	bradSkeleton = scene.createSkeleton('ChrBrad.sk')
	brad.setSkeleton(bradSkeleton)
	brad.createStandardControllers()
	# Set deformable mesh
	brad.setDoubleAttribute('deformableMeshScale', 0.01)
	brad.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
	# Play idle animation
	bml.execBML(baseName, '<body posture="ChrBrad@Idle01"/>')
	# Retarget character
	#retargetCharacter(baseName, 'ChrBrad.sk')
	if i== 0 : 
		scene.run('BehaviorSetReaching.py')
		setupBehaviorSet()
		
	retargetBehaviorSet(baseName)		
	
print 'Setting up Rachels'
for i in range(2):
	baseName = 'ChrRachel%s' % i
	rachel = scene.createCharacter(baseName, '')
	rachelSkeleton = scene.createSkeleton('ChrRachel.sk')
	rachel.setSkeleton(rachelSkeleton)
	rachel.createStandardControllers()
	# Set deformable mesh
	rachel.setDoubleAttribute('deformableMeshScale', 0.01)
	rachel.setStringAttribute('deformableMesh', 'ChrRachel.dae')
	# Play idle animation
	bml.execBML(baseName, '<body posture="ChrRachel_ChrBrad@Idle01"/>')
	# Retarget character
	#retargetCharacter(baseName, 'ChrRachel.sk')
	retargetBehaviorSet(baseName)

# add locomotion for ChrBrad2
#scene.run('BehaviorSetMaleLocomotion.py')
#setupBehaviorSet()
#retargetBehaviorSet('ChrBrad2', 'ChrBrad.sk')

# Setting character positions
print 'Setting character positions'
scene.getCharacter('ChrBrad0').setPosition(SrVec(-0.35, 0, -1))
scene.getCharacter('ChrBrad1').setPosition(SrVec(-1.35, 0, -1))
#scene.getCharacter('ChrBrad2').setPosition(SrVec(1.35, 0, 0))
scene.getCharacter('ChrRachel0').setPosition(SrVec(-1, 0, 1))
scene.getCharacter('ChrRachel0').setHPR(SrVec(90, 0, 0))
scene.getCharacter('ChrRachel1').setPosition(SrVec(-0.35, 0, 1))
scene.getCharacter('ChrRachel1').setHPR(SrVec(-90, 0, 0))

# Setting up pawns
print 'Setting up pawns'
pawn0 = scene.createPawn('touchPawn')
pawn0.setStringAttribute('collisionShape', 'sphere')
pawn0.getAttribute('collisionShapeScale').setValue(SrVec(0.1, 0.1, 0.1))
pawn0.setPosition(SrVec(0, 0.9, -1.3))
pawn1 = scene.createPawn('pointPawn')
pawn1.setStringAttribute('collisionShape', 'sphere')
pawn1.getAttribute('collisionShapeScale').setValue(SrVec(0.1, 0.1, 0.1))
pawn1.setPosition(SrVec(-1.75, 1.6, -0.35))
pawn2 = scene.createPawn('pickupPawn')
pawn2.setStringAttribute('collisionShape', 'sphere')
pawn2.getAttribute('collisionShapeScale').setValue(SrVec(0.1, 0.1, 0.1))
pawn2.setPosition(SrVec(1.35, 1.6, -2))
pawn3 = scene.createPawn('passPawn')
pawn3.setStringAttribute('collisionShape', 'sphere')
pawn3.getAttribute('collisionShapeScale').setValue(SrVec(0.05, 0.05, 0.05))
pawn3.setPosition(SrVec(-0.65, 1.6 , 1))

# Turn on GPU deformable geomtery for all
for name in scene.getCharacterNames():
	scene.getCharacter(name).setStringAttribute("displayType", "GPUmesh")


# Variables to perform pick up and put down
area1PickUp = False
area1PutDown = False
area2PickUp = False
area2PutDown = False
moving = False

# Updates to repeat reaches
last = 0
canTime = True
delay = 5

lastPass = 0
canTimePass = True
delayPass = 1
class ReachDemo(SBScript):
	def update(self, time):
		global canTime, last
		if canTime:
			last = time
			canTime = False
		diff = time - last
		if diff >= delay:
			diff = 0
			canTime = True
		
		# If time up, do actions
		if canTime:
			# Brad0 actions
			bml.execBML('ChrBrad0', '<sbm:reach sbm:action="touch" target="touchPawn" sbm:reach-finish="true"/>')
			bml.execBML('ChrBrad0', '<gaze target="touchPawn" sbm:joint-range="EYES NECK"/>')
			# Brad1 actions
			bml.execBML('ChrBrad1', '<sbm:reach sbm:action="point-at" sbm:reach-duration="1" target="pointPawn"/>')
			bml.execBML('ChrBrad1', '<gaze target="pointPawn" sbm:joint-range="EYES NECK"/>')
			# Brad2 actions
			#checkBrad2()
		
		global lastPass, canTimePass
		if canTimePass:
			lastPass = time
			canTimePass = False
		diffPass = time - lastPass
		if diffPass >= delayPass:
			diffPass = 0
			canTimePass = True
		if canTimePass:
			checkPass()

# Logic of picking up and putting down

def checkBrad2():
	global moving
	if not area1PickUp and not moving:
		bml.execBML('ChrBrad2', '<sbm:reach sbm:action="pick-up" sbm:reach-duration="0.2" target="pickupPawn" sbm:use-locomotion="true"/>')
		bml.execBML('ChrBrad2', '<gaze target="pickupPawn" sbm:joint-range="EYES NECK"/>')
		moving = True
	if not area1PutDown and area1PickUp and not moving:
		bml.execBML('ChrBrad2', '<sbm:reach sbm:action="put-down" sbm:reach-duration="0.2" sbm:target-pos="1.35 1 1" sbm:use-locomotion="true"/>')
		print 'Put down front'
		moving = True
	if not area2PickUp and area1PutDown and not moving:
		bml.execBML('ChrBrad2', '<sbm:reach sbm:action="pick-up" sbm:reach-duration="0.2" target="pickupPawn" sbm:use-locomotion="true"/>')
		bml.execBML('ChrBrad2', '<gaze target="pickupPawn" sbm:joint-range="EYES NECK"/>')
		moving = True
	if not area2PutDown and area2PickUp and not moving:
		bml.execBML('ChrBrad2', '<sbm:reach sbm:action="put-down" sbm:reach-duration="0.2" sbm:target-pos="1.35 1.6 -1" sbm:use-locomotion="true"/>')
		print 'Put down back'
		moving = True


# Passing variables
currentTurn = 'ChrRachel0'
pawnAttached = False
reaching = False
puttingDown = False
# Passing logic
def checkPass():
	global currentTurn, pawnAttached
	if not pawnAttached and not reaching:
		bml.execBML(currentTurn, '<sbm:reach sbm:action="pick-up" sbm:reach-finish="false" target="passPawn"/>')
	elif pawnAttached and not puttingDown:
		randX = random.uniform(-0.6, -0.7)
		randY = random.uniform(1.0, 1.6)
		randZ = random.uniform(0.8, 1.2)
		bml.execBML(currentTurn, '<sbm:reach sbm:action="put-down" sbm:reach-finish="true" sbm:target-pos="' + '%s %s %s' % (randX, randY, randZ) + '" sbm:use-locomotion="true"/>')
	bml.execBML('ChrRachel0', '<gaze target="passPawn" sbm:joint-range="EYES NECK"/>')
	bml.execBML('ChrRachel1', '<gaze target="passPawn" sbm:joint-range="EYES NECK"/>')
		
# Run the update script
scene.removeScript('ReachDemo')
reachdemo = ReachDemo()
scene.addScript('ReachDemo', reachdemo)

class ReachingHandler(SBEventHandler):
	def executeAction(self, ev):
		params = ev.getParameters()
		global area1PickUp, area1PutDown, area2PickUp, area2PutDown, moving
		if 'ChrBrad2' in params:
			if not area1PickUp and 'pawn-attached' in params:
				area1PickUp = True
				moving = False
			# If area 1 done
			elif area1PutDown and not area2PickUp and 'pawn-attached' in params:
				area2PickUp = True
				moving = False
			if not area1PutDown and 'pawn-released' in params:
				area1PutDown = True
				moving = False
			elif area2PickUp and not area2PutDown and 'pawn-released' in params:
				area2PutDown = True
				# Reset all
				area1PickUp = False
				area1PutDown = False
				area2PickUp = False
				area2PutDown = False
				moving = False
		
		global pawnAttached, currentTurn, reaching, puttingDown
		# Check which doctor's turn it is
		if currentTurn in params:
			if 'pawn-attached' in params:
				pawnAttached = True
				reaching = False
			if 'pawn-released' in params:
				pawnAttached = False
				if currentTurn == 'ChrRachel0':
					currentTurn = 'ChrRachel1'
				elif currentTurn == 'ChrRachel1':
					currentTurn = 'ChrRachel0'
				puttingDown = False
				
		reachingHdl = ReachingHandler()
		
evtMgr = scene.getEventManager()
reachingHdl = ReachingHandler()
evtMgr.addEventHandler('reachNotifier', reachingHdl)
