import random
print "|--------------------------------------------|"
print "|          Starting Physics Demo             |"
print "|--------------------------------------------|"

# Add asset paths
scene.addAssetPath('script', 'sbm-common/scripts')
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('motion', 'sbm-common/common-sk/motion')
scene.addAssetPath("script", "behaviorsets")
scene.addAssetPath('script', 'scripts')
scene.loadAssets()

scene.loadAssetsFromPath('sbm-common/common-sk/common.sk')

scene.setScale(0.01)
# Set simulation fps
scene.getSimulationManager().setSimFps(60)

#scene.run('default-viewer.py')
camera = getCamera()
camera.setEye(-20, 187, 443)
camera.setCenter(-20, 136, 151)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(5000.0)
camera.setNearPlane(5.0)
camera.setAspectRatio(1.02)


#scene.getPawn('camera').setPosition(SrVec(0, -20, 0))


# Set scene parameters and camera
print 'Configuring scene parameters and camera'
scene.setBoolAttribute('internalAudio', True)

# Setting up characters
print 'Setting up characters'
# Brad
brad = scene.createCharacter('brad', '')
bradSkeleton = scene.createSkeleton('common.sk')
brad.setSkeleton(bradSkeleton)
brad.setPosition(SrVec(-150, 200, 20))
brad.createStandardControllers()
brad.setStringAttribute('deformableMesh', 'Brad.dae')
bml.execBML('brad', '<body posture="HandsAtSide_Motex"/>')
# Elder
elder = scene.createCharacter('elder', '')
elderSkeleton = scene.createSkeleton('common.sk')
elder.setSkeleton(elderSkeleton)
elder.setPosition(SrVec(-75, 102, 10))
elder.createStandardControllers()
elder.setStringAttribute('deformableMesh', 'Elder.dae')
bml.execBML('elder', '<body posture="LHandOnHip_Motex"/>')
# Doctor
doctor = scene.createCharacter('doctor', '')
doctorSkeleton = scene.createSkeleton('common.sk')
doctor.setSkeleton(doctorSkeleton)
doctor.setPosition(SrVec(75, 102, 0))
doctor.setHPR(SrVec(90, 0, 0))
doctor.createStandardControllers()
doctor.setStringAttribute('deformableMesh', 'Doctor.dae')

bml.execBML('doctor', '<body posture="LHandOnHip_Motex"/>')
# Brad2
brad2 = scene.createCharacter('brad2', '')
brad2Skeleton = scene.createSkeleton('common.sk')
brad2.setSkeleton(brad2Skeleton)
brad2.setPosition(SrVec(135, 102, 0))
brad2.setHPR(SrVec(-90, 0, 0))
brad2.createStandardControllers()
brad2.setStringAttribute('deformableMesh', 'Brad.dae')

# setup reach 
scene.run('BehaviorSetReaching.py')
setupBehaviorSet()
retargetBehaviorSet('doctor')
retargetBehaviorSet('brad2')




bml.execBML('brad2', '<body posture="HandsAtSide_Motex"/>')

# Turn on GPU deformable geometry for all
for name in scene.getCharacterNames():
	scene.getCharacter(name).setStringAttribute("displayType", "GPUmesh")


# Setting up pawns
print 'Adding pawns to scene'
for i in range(6):
	baseName = 'pawn%s' % i
	pawn = scene.createPawn(baseName)
	pawn.setStringAttribute('collisionShape', 'sphere')
	collisionShapeScale = SrVec(0, 0, 0)
	pawn.getAttribute('collisionShapeScale').setValue(collisionShapeScale)
	
# Set pawn positions
print 'Setting pawn positions'
pawn0 = scene.getPawn('pawn0')
pawn0.setPosition(SrVec(-150, 240, 20))
pawn0.getAttribute('collisionShapeScale').setValue(SrVec(5, 5, 5))
pawn1 = scene.getPawn('pawn1')
pawn1.setPosition(SrVec(-75, 150, 20))
pawn1.getAttribute('collisionShapeScale').setValue(SrVec(5, 5, 5))
pawn2 = scene.getPawn('pawn2')
pawn2.setPosition(SrVec(75, 150, 10))
pawn3 = scene.getPawn('pawn3')
pawn3.setPosition(SrVec(75, 150, -10))
pawn4 = scene.getPawn('pawn4')
pawn4.setPosition(SrVec(135, 150, 10))
pawn5 = scene.getPawn('pawn5')
pawn5.setPosition(SrVec(135, 150, -10))

# Configure character physics
print 'Configuring character physics'
phyManager = scene.getPhysicsManager()
phyManager.getPhysicsEngine().setBoolAttribute('enable', True)

for name in scene.getCharacterNames():
	char = scene.getCharacter(name)
	char.getAttribute('createPhysics').setValue()
	phyManager.getPhysicsCharacter(name).setBoolAttribute('usePD', True)

def constrainChr(chr, joint, object=''):
	#Name of character, joint name of character, object name to act as constraint
	bodyLink = phyManager.getJointObj(chr, joint)
	bodyLink.setBoolAttribute('constraint', True)
	bodyLink = phyManager.getJointObj(chr, joint)
	bodyLink.setStringAttribute('constraintTarget', object)
	
# Set up constraints
constrainChr('brad', 'l_wrist', 'pawn0')
constrainChr('doctor', 'spine1')
constrainChr('doctor', 'r_wrist')
constrainChr('doctor', 'l_wrist')
constrainChr('doctor', 'r_ankle')
constrainChr('doctor', 'l_ankle')
constrainChr('brad2', 'spine1')
constrainChr('brad2', 'r_wrist')
constrainChr('brad2', 'l_wrist')
constrainChr('brad2', 'r_ankle')
constrainChr('brad2', 'l_ankle')
constrainChr('elder', 'spine1')
constrainChr('elder', 'l_wrist')
constrainChr('elder', 'r_wrist')
constrainChr('elder', 'l_ankle')
constrainChr('elder', 'r_ankle')

# Set up pawn physics
pawn1.getAttribute('createPhysics').setValue()

bradX = -150
bradCur = -1
curZ = 20
curX = -75
amountZ = amountX = -1
speed = 0.2
last = 0
canTime = True
delay = 6
started = False
class PhysicsDemo(SBScript):
	def update(self, time):
		global canTime, last, started
		global amountZ, curZ, amountX, curX, bradX, bradCur
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
			phyManager.getPhysicsCharacter('brad').setBoolAttribute('enable', True)
			phyManager.getPhysicsCharacter('elder').setBoolAttribute('enable', True)
			phyManager.getPhysicsCharacter('doctor').setBoolAttribute('enable', True)
			pawn1.setBoolAttribute('enablePhysics', True)
			pawn1.setBoolAttribute('enablePhysics', False)
			
			pawn1.setPosition(SrVec(-75, 150, 20))
			bml.execBML('brad', '<body posture="Walk"/>')
		# If time's up, do action
		if canTime:
			bml.execBML('brad', '<head repeats="5" velocity="0.75" type="SHAKE"/>')
			boxingLogic()
		# Elder pawn
		pawn1.setPosition(SrVec(curX, 150, curZ))
		curX = curX + speed * amountX
		curZ = curZ + speed * amountZ
		if curX < -90: amountX = 1
		if curX > -60: amountX = -1
		if curZ < 9: amountZ = 1
		if curZ > 20: amountZ = -1
		# Brad pawn
		pawn0.setPosition(SrVec(bradX, 240, 20))
		bradX = bradX + speed * bradCur
		if bradX < -170: bradCur = 1
		if bradX > -130: bradCur = -1
	
# Current turn
currentTurn = 'brad2'		
def boxingLogic():
	global currentTurn
	# Brad's turn, toggle physics and play reach
	if currentTurn == 'brad2':
		phyManager.getPhysicsCharacter('brad2').setBoolAttribute('enable', False)
		phyManager.getPhysicsCharacter('doctor').setBoolAttribute('enable', True)

		randNum = random.randrange(0, 2)
		randDodge = random.randrange(0, 3)
		if randNum == 0:
			bml.execBML('brad2', '<sbm:reach sbm:action="touch" sbm:reach-finish="true" sbm:reach-type="left" target="pawn2"/>')
		elif randNum == 1:
			bml.execBML('brad2', '<sbm:reach sbm:action="touch" sbm:reach-finish="true" sbm:reach-type="right" target="pawn3"/>')
		if randDodge == 2:
			phyManager.getPhysicsCharacter('doctor').setBoolAttribute('enable', False)
		currentTurn = 'doctor'
	# Doctor's turn, toggle physics and play reach
	elif currentTurn == 'doctor':
		phyManager.getPhysicsCharacter('doctor').setBoolAttribute('enable', False)
		phyManager.getPhysicsCharacter('brad2').setBoolAttribute('enable', True)
		
		randNum = random.randrange(0, 2)
		randDodge = random.randrange(0, 3)
		if randNum == 0:
			bml.execBML('doctor', '<sbm:reach sbm:action="touch" sbm:reach-finish="true" sbm:reach-type="right" target="pawn4"/>')
		elif randNum == 1:
			bml.execBML('doctor', '<sbm:reach sbm:action="touch" sbm:reach-finish="true" sbm:reach-type="left" target="pawn5"/>')
		if randDodge == 2:
			phyManager.getPhysicsCharacter('brad2').setBoolAttribute('enable', False)
		currentTurn = 'brad2'
		
class CollisionHandler(SBEventHandler):
	def executeAction(self, ev):
		params = ev.getParameters()
		list = params.split()
		if len(list) > 2:
			# Elder collision
			if list[1] == 'elder':
				if list[2] == 'pawn1':
					#print '%s collided with %s' % (list[1], list[2])
					bml.execBML('elder', '<gaze target="pawn1"/>')
					bml.execBML('elder', '<gaze target="elder" start="2"/>')
			# Doctor doesn't get collision events for some reason
			if list[1] == 'brad2':
				# Brad hits doctor
				if 'wrist' in list[2] and currentTurn == 'doctor':
					target = list[3] + ' ' + list[4] + ' ' + list[5]
					bml.execBML('doctor', '<gaze sbm:target-pos="' + target + '"/>')
					bml.execBML('doctor', '<gaze target="doctor" start="2"/>')
				# Doctor hits brad
				elif '_' in list[2] and currentTurn == 'brad2':
					target = list[3] + ' ' + list[4] + ' ' + list[5]
					bml.execBML('brad2', '<gaze sbm:target-pos="' + target + '"/>')
					bml.execBML('brad2', '<gaze target="brad2" start="2"/>')
		
		
collisionHdl = CollisionHandler()
evtMgr = scene.getEventManager()
evtMgr.addEventHandler('collision', collisionHdl)
			
# Run the update script
scene.removeScript('physicsdemo')
physicsdemo = PhysicsDemo()
scene.addScript('physicsdemo', physicsdemo)
