import random
import math
print "|--------------------------------------------|"
print "|           Starting Crowd Demo              |"
print "|--------------------------------------------|"
	
# Add asset paths
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('motion', 'ChrBrad')
scene.addAssetPath("script", "behaviorsets")
scene.addAssetPath('script', 'scripts')
scene.loadAssets()
scene.setScale(1.0)

# Set joint map for Brad
print 'Setting up joint map for Brad'
scene.run('zebra2-map.py')
zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
bradSkeleton = scene.getSkeleton('ChrBrad.sk')
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse('ChrBrad')

# Runs the default viewer for camera
scene.run('default-viewer.py')
camera = getCamera()
camera.setEye(1, 13.43, 18.63)
camera.setCenter(1, 12, 16.91)
scene.getPawn('camera').setPosition(SrVec(0, -50, 0))

steerManager = scene.getSteerManager()

# Setting up Brads
print 'Setting up Brads'
amount = 20
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
	brad.setStringAttribute('deformableMesh', 'ChrBrad.dae')
	# Retarget character
	#retargetCharacter(baseName, 'ChrBrad.sk', False)
	# setup mocap locomotion
	if i== 0 : 
		scene.run('BehaviorSetMaleLocomotion.py')
		setupBehaviorSet()
		retargetBehaviorSet(baseName)
		scene.run('BehaviorSetGestures.py')
		setupBehaviorSet()
		retargetBehaviorSet(baseName)		
	steerAgent = steerManager.createSteerAgent(baseName)
	steerAgent.setSteerStateNamePrefix("all")
	steerAgent.setSteerType("example")
	# Set up steering
	#setupSteerAgent(baseName, 'ChrBrad.sk')
	brad.setBoolAttribute('steering.pathFollowingMode', False)
	# Play default animation
	bml.execBML(baseName, '<body posture="ChrBrad@Idle01"/>')
	

steerManager.setEnable(False)
steerManager.setEnable(True)

# Turn on GPU deformable geometry for all
for name in scene.getCharacterNames():
	scene.getCharacter(name).setStringAttribute("displayType", "GPUmesh")


# Set up list of Brads
bradList = []
for name in scene.getCharacterNames():
	if 'ChrBrad' in name:
		bradList.append(scene.getCharacter(name))

# Paths for characters
bradPath = [SrVec(-8, -8, 0), SrVec(8, 8, 0), SrVec(8, -8, 0), SrVec(-8, 8, 0)]
bradCur = 0
pathAmt = len(bradPath)
bradReached = True

class CrowdDemo(SBScript):
	def update(self, time):
		global bradReached, bradCur
		# Once utah completes path, do again
		if bradReached:
			for brad in bradList:
				#print "Moving Brad " + brad.getName()
				# Move character
				#print  '<locomotion speed="' +  str(random.uniform(1.20, 5.0)) + '" target="' +\
											#vec2str(bradPath[bradCur]) + '"/>'
				bml.execBML(brad.getName(), '<locomotion speed="' +  str(random.uniform(1.20, 5.0)) + '" target="' +\
											vec2str(bradPath[bradCur]) + '"/>')
			bradCur = bradCur + 1
			# If reaches max path, reset
			if bradCur >= pathAmt:
				bradCur = 0
			bradReached = False
		
# Run the update script
scene.removeScript('crowddemo')
crowddemo = CrowdDemo()
scene.addScript('crowddemo', crowddemo)

reachCount = 0
# Locomotion handler to check if characters have arrived
class LocomotionHandler(SBEventHandler):
	def executeAction(self, ev):
		global bradReached, reachCount
		params = ev.getParameters()
		if 'success' in params:
			if 'ChrBrad' in params:
				reachCount = reachCount + 1
				if reachCount >= 6:
					bradReached = True	
					reachCount = 0

locomotionHdl = LocomotionHandler()
evtMgr = scene.getEventManager()
evtMgr.addEventHandler('locomotion', locomotionHdl)

def vec2str(vec):
	''' Converts SrVec to string '''
	x = vec.getData(0)
	y = vec.getData(1)
	z = vec.getData(2)
	if -0.0001 < x < 0.0001: x = 0
	if -0.0001 < y < 0.0001: y = 0
	if -0.0001 < z < 0.0001: z = 0
	return "" + str(x) + " " + str(y) + ""
