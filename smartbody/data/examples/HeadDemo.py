import random
print "|--------------------------------------------|"
print "|            Starting Head Demo              |"
print "|--------------------------------------------|"

# Add asset paths
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('motion', 'ChrMaarten')
scene.addAssetPath('script', 'scripts')
scene.loadAssets()

# Set scene parameters and camera
print 'Configuring scene parameters and camera'
scene.setScale(1.0)
scene.run("default-viewer.py")
camera = getCamera()
camera.setEye(0.0, 1.65, 0.38)
camera.setCenter(0.0, 1.59, -0.02)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(100)
camera.setNearPlane(0.1)
camera.setAspectRatio(0.966897)

# Setting up joint map for Brad
print 'Setting up joint map and configuring Brad\'s skeleton'
scene.run('zebra2-map.py')
zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
bradSkeleton = scene.getSkeleton('ChrBrad.sk')
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse('ChrBrad')

# Setting up Brad
print 'Setting up Brad'
brad = scene.createCharacter('ChrBrad', '')
bradSkeleton = scene.createSkeleton('ChrBrad.sk')
brad.setSkeleton(bradSkeleton)
brad.createStandardControllers()
# Deformable mesh
brad.setDoubleAttribute('deformableMeshScale', .01)
brad.setStringAttribute('deformableMesh', 'ChrMaarten.dae')

# Turn on GPU deformable Geometry
brad.setStringAttribute("displayType", "GPUmesh")


last = 0
canTime = True
delay = 5
class HeadDemo(SBScript):
	def update(self, time):
		global last, canTime
		diff = time - last
		if diff >= delay:
			diff = 0
			canTime = True
		# When time's up do action
		if canTime:
			last = time
			canTime = False
			nextHead()

headList = ['NOD', 'SHAKE', 'TOSS', 'WIGGLE', 'WAGGLE']
curHead = 0
headAmt = len(headList)
def nextHead():
	global curHead
	headChoice = headList[curHead]
	print 'Playing %s' % headChoice
	# Random repeats, velocity and amount
	repeats = random.uniform(0.1, 2)
	amount = random.uniform(0.1, 1) / 2.0
	# Perform BML command
	bml.execBML('*', '<head type="' + headChoice + '" amount="' + str(amount) + '" repeats="' + str(repeats) + '"/>')
	# Increment index, reset when hit max
	curHead = curHead + 1
	if curHead >= headAmt:
		curHead = 0
			
# Run the update script
scene.removeScript('headdemo')
headdemo = HeadDemo()
scene.addScript('headdemo', headdemo)