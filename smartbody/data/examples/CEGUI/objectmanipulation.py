import random
print "|--------------------------------------------|"
print "|            Starting GrabTouchCEGui Demo    |"
print "|--------------------------------------------|"

# Add asset paths
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('motion', 'ChrBrad')
scene.addAssetPath("script", "behaviorsets")
scene.addAssetPath('script', 'scripts')
scene.loadAssets()

# Set scene parameters and camera
print 'Configuring scene parameters and camera'
scene.setScale(1.0)
scene.setBoolAttribute('internalAudio', True)
scene.run('default-viewer.py')
camera = getCamera()

camera.setEye(-3.31565, 6.04572, 8.35714)
camera.setCenter(-2.11867, 5.04335, 7.11799)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(100)
camera.setNearPlane(0.1)
camera.setAspectRatio(1.39286)

scene.getPawn('camera').setPosition(SrVec(0, -2, 0))

# Set joint map for Brad'
print 'Setting up joint map for Brad'
scene.run('zebra2-map.py')
zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
bradSkeleton = scene.getSkeleton('ChrBrad.sk')
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse('ChrMaarten')

# Setting up Brad
print 'Setting up Brad'

baseName = 'ChrBrad%s' % 1
brad = scene.createCharacter(baseName, '')
bradSkeleton = scene.createSkeleton('ChrBrad.sk')
brad.setSkeleton(bradSkeleton)
brad.createStandardControllers()
# Set deformable mesh
brad.setDoubleAttribute('deformableMeshScale', 0.01)
brad.setStringAttribute('deformableMesh', 'ChrBrad.dae')

scene.run('BehaviorSetReaching.py')
setupBehaviorSet()
retargetBehaviorSet(baseName)		

scene.run('BehaviorSetMaleMocapLocomotion.py')
setupBehaviorSet()
retargetBehaviorSet(baseName)

# Play idle animation
bml.execBML(baseName, '<body posture="ChrMarine@Idle01"/>')

# make the lights invisible
scene.getPawn("light0").setBoolAttribute("visible", False)
scene.getPawn("light1").setBoolAttribute("visible", False)


# set gravity 
phyManager = scene.getPhysicsManager()
phyManager.getPhysicsEngine().setDoubleAttribute('gravity',9.80)


# Setting character positions
print 'Setting character positions'
scene.getCharacter('ChrBrad1').setPosition(SrVec(0, 0, 1))

# Turn on GPU deformable geomtery for all
for name in scene.getCharacterNames():
	scene.getCharacter(name).setStringAttribute("displayType", "GPUmesh")

# Set up pawns in scene
print 'Adding pawns to scene'
numPawns = 0

shapeList = ['sphere', 'box', 'capsule']


# add the initial set of objects
for o in range(0, 1):
	baseName = 'object%s' % numPawns
	size = random.randrange(5, 20)
	pawn = scene.createPawn(baseName)
	pawn.setStringAttribute('collisionShape', random.choice(shapeList))
	pawn.getAttribute('collisionShapeScale').setValue(SrVec(0.1, 0.1, 0.1))
	pawn.setPosition(SrVec(-0.4, 6.6, 1.35))
	numPawns += 1
	pawn = scene.getPawn(baseName)
					
	pawn.getAttribute('color').setValue(SrVec( random.random(), random.random(), random.random()))
	# Append all pawn to list
	pawnList = []
	for name in scene.getPawnNames():
		if 'object' in name:
			pawnList.append(scene.getPawn(name))
			print scene.getPawn(name)
			
	# Setup pawn physics	
	print 'Setting up object physics'
	phyManager = scene.getPhysicsManager()
	phyManager.getPhysicsEngine().setBoolAttribute('enable', True)

	for pawn in pawnList:
		pawn.getAttribute('createPhysics').setValue()
		phyManager.getPhysicsPawn(pawn.getName()).setDoubleAttribute('mass', 1)

	for pawn in pawnList:
		pawn.setBoolAttribute('enablePhysics', True)


	
# ---- pawn: shelf pawn
baseName = 'shelf'
pawn = scene.createPawn(baseName)

pawn.setPosition(SrVec( 5, 0, 5))
pawn.setStringAttribute('collisionShape', 'box')
pawn.getAttribute('collisionShapeScale').setValue(SrVec( 0.5, 1.5, 0.2))
pawn.setDoubleAttribute('rotY' , 90)
pawn.setBoolAttribute('visible', False)
	
# ---- pawn: pawnTop
baseName = 'pawnTop'
pawn = scene.createPawn(baseName)
pawn.setStringAttribute('collisionShape', 'box')
pawn.getAttribute('collisionShapeScale').setValue(SrVec( 0.2, 0.02, 0.5))
pawn.setPosition(SrVec( 4.93, 1.22, 4.93))
pawn.getAttribute('createPhysics').setValue()

# ---- pawn: pawnMid
baseName = 'pawnMid'
pawn = scene.createPawn(baseName)
pawn.setStringAttribute('collisionShape', 'box')
pawn.getAttribute('collisionShapeScale').setValue(SrVec( 0.2, 0.02, 0.5))
pawn.setPosition(SrVec( 4.93, 0.62, 4.93))
pawn.getAttribute('createPhysics').setValue()
	
# ---- pawn: pawnBot
baseName = 'pawnBot'
pawn = scene.createPawn(baseName)
pawn.setStringAttribute('collisionShape', 'box')
pawn.getAttribute('collisionShapeScale').setValue(SrVec( 0.2, 0.02, 0.5))
pawn.setPosition(SrVec( 4.93, 0.09, 4.93))
pawn.getAttribute('createPhysics').setValue()



curTime = 0
lastPass = 0
delay = 3
onAction = False


class ReachDemo(SBScript):
	def update(self, time):
		global curTime,lastPass,delay,onAction
		curTime = time
		
		#print onAction
		
		if(not onAction):
			lastPass=curTime
			
		
		if(curTime-lastPass > delay):
			print'finish counting delay'
			lastPass = curTime
			onAction=False
		
	
	

# Run the update script
scene.removeScript('reachdemo')
reachdemo = ReachDemo()
scene.addScript('reachdemo', reachdemo)


currentPawnNum=0;
grabbed = False
released=True
canOtherAction =True
class ReachingHandler(SBEventHandler):
	def executeAction(self, ev):
		params = ev.getParameters()
		global grabbed ,released , canOtherAction  , currentPawnNum
		if 'ChrBrad1' in params:
			if 'pawn-attached' in params and not grabbed :
				grabbed = True;
				released = False
				canOtherAction=True
						
			elif 'pawn-released' in params and grabbed :
				grabbed = False
				canOtherAction =True
		
evtMgr = scene.getEventManager()
reachingHdl = ReachingHandler()
evtMgr.addEventHandler('reachNotifier', reachingHdl)

print 'Setting up GUI'
scene.run('GUIUtil.py')
gui = GUIManager()


class GUIHandler:
	def __init__(self):
		print "init"
		

	def handleTouchButton(self,args):		
		global onAction, delay,canOtherAction ,currentPawnNum
		
		if(not onAction and canOtherAction):
			print 'try touching pawn'
			baseName = 'object%s' % currentPawnNum
			onAction = True
			delay = 2
			bml.execBML('ChrBrad1', '<sbm:reach sbm:action="touch" target="'+baseName+'" sbm:reach-finish="true"/>')
			

		
	def handlePointButton(self,args):
		
		global onAction , delay,canOtherAction,currentPawnNum
		if(not onAction and canOtherAction):
			print 'try pointing pawn'
			baseName = 'object%s' % currentPawnNum
			delay = 3
			bml.execBML('ChrBrad1', '<sbm:reach sbm:action="point-at" sbm:reach-duration="1" target="'+baseName+'"/>')
			bml.execBML('ChrBrad1', '<gaze target="touchPawn" sbm:joint-range="EYES NECK"/>')
			onAction = True
			
		
	def handleGrabButton(self,args):
		global onAction , delay,currentPawnNum,grabbed,canOtherAction
		
		if(not onAction and not grabbed):
			print 'grab'
			canOtherAction=False
			onAction = True
			delay = 2
			baseName = 'object%s' % currentPawnNum
			bml.execBML('ChrBrad1', '<sbm:reach sbm:action="pick-up" sbm:reach-finish="true" sbm:reach-duration="2" target="'+baseName+'" sbm:use-locomotion="true"/>')	
			bml.execBML('ChrBrad1', '<gaze target="'+baseName+'" sbm:joint-range="EYES NECK"/>')		
				
	def handleGrabMoveTopButton(self,args):	
		global pawn1 , onAction , delay,currentPawnNum ,numPawns,grabbed,released,canOtherAction
		if(not onAction and grabbed and not released):
			baseName = 'object%s' % currentPawnNum
			if(currentPawnNum <=numPawns):
				currentPawnNum += 1
			canOtherAction = False
			released = True
			bml.execBML('ChrBrad1', '<sbm:reach sbm:action="put-down" sbm:reach-finish="true" sbm:target-pos="' + '%s %s %s' % (4.9, 1.6, 4.9) + '" sbm:use-locomotion="true"/>')
			bml.execBML('ChrBrad1', '<gaze target="'+baseName+'" sbm:joint-range="EYES NECK"/>')	
			delay = 3
			onAction=True
			
		
	
	def handleGrabMoveMidButton(self,args):
		global pawn1 , onAction , delay,currentPawnNum ,numPawns,grabbed,released,canOtherAction
		if(not onAction and grabbed and not released):
			baseName = 'object%s' % currentPawnNum
			if(currentPawnNum <=numPawns):
				currentPawnNum += 1
			canOtherAction = False
			released = True
			bml.execBML('ChrBrad1', '<sbm:reach sbm:action="put-down" sbm:reach-finish="true" sbm:target-pos="' + '%s %s %s' % (4.9, 0.9, 4.9) + '" sbm:use-locomotion="true"/>')
			bml.execBML('ChrBrad1', '<gaze target="'+baseName+'" sbm:joint-range="EYES NECK"/>')	
			delay = 3
			onAction=True	
	
	
	def handleGrabMoveBotButton(self,args):
		global pawn1 , onAction , delay,currentPawnNum ,numPawns,grabbed,released,canOtherAction
		if(not onAction and grabbed and not released):
			baseName = 'object%s' % currentPawnNum
			if(currentPawnNum <=numPawns):
				currentPawnNum += 1
			canOtherAction = False
			released = True
			bml.execBML('ChrBrad1', '<sbm:reach sbm:action="put-down" sbm:reach-finish="true" sbm:target-pos="' + '%s %s %s' % (4.9, 0.3, 4.9) + '" sbm:use-locomotion="true"/>')
			bml.execBML('ChrBrad1', '<gaze target="'+baseName+'" sbm:joint-range="EYES NECK"/>')
			delay = 3
			onAction=True
	
		
	def addPawnButton(self,args):
		global numPawns
		baseName = 'object%s' % numPawns
		shapeList = ['sphere', 'box', 'capsule']
		size = random.randrange(5, 30)
		
		
		pawn = scene.createPawn(baseName)
		pawn.setPosition(SrVec(0, 6.6, 0))
		pawn.setStringAttribute('collisionShape', random.choice(shapeList))
		pawn.getAttribute('collisionShapeScale').setValue(SrVec(0.1, 0.1, 0.1))
		pawn.getAttribute('color').setValue(SrVec( random.random(), random.random(), random.random()))
	
		
		numPawns += 1
		pawnList.append(scene.getPawn(baseName))
		scene.getPawn(baseName).getAttribute('createPhysics').setValue()
		phyManager.getPhysicsPawn(scene.getPawn(baseName).getName()).setDoubleAttribute('mass', 1)
		
		scene.getPawn(baseName).setBoolAttribute('enablePhysics', True)


guiHandler = GUIHandler()	
touchButton = gui.createButton('touchBtn','Touch object')
touchButton.subscribeEvent(PushButton.EventClicked, guiHandler.handleTouchButton)

pointButton = gui.createButton('pointBtn','Point at Object')
pointButton.subscribeEvent(PushButton.EventClicked, guiHandler.handlePointButton)

grabButton = gui.createButton('grabBtn','Grab Object')
grabButton.subscribeEvent(PushButton.EventClicked, guiHandler.handleGrabButton)

grabMoveTopButton = gui.createButton('grabMoveTopBtn','Place on high shelf')
grabMoveTopButton.subscribeEvent(PushButton.EventClicked, guiHandler.handleGrabMoveTopButton)

grabMoveMidButton = gui.createButton('grabMoveMidBtn','Place on mid shelf')
grabMoveMidButton.subscribeEvent(PushButton.EventClicked, guiHandler.handleGrabMoveMidButton)

grabMoveBotButton = gui.createButton('grabMoveBotBtn','Place on low shelf')
grabMoveBotButton.subscribeEvent(PushButton.EventClicked, guiHandler.handleGrabMoveBotButton)

addPawnButton = gui.createButton('addPawnBtn','Add object')
addPawnButton.subscribeEvent(PushButton.EventClicked, guiHandler.addPawnButton)

