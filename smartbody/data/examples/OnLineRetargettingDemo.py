print "|--------------------------------------------|"
print "|        Starting Retargetting Demo          |"
print "|--------------------------------------------|"

		
def setMotionNameSkeleton(motionName, skelName):
	motion = scene.getMotion(motionName)
	if motion != None:		
		motion.scale(6)
		motion.setMotionSkeletonName(skelName)
	
def createRetargetInstance(srcSkelName, tgtSkelName):
	# replace retarget each animation with just a simple retarget instance
	
	# these joints and their children are not retargeted
	endJoints = StringVec();
	endJoints.append('l_forefoot')
	endJoints.append('l_toe')
	endJoints.append('l_wrist')
	endJoints.append('r_forefoot')	
	endJoints.append('r_toe')	
	endJoints.append('r_wrist')

	# these joints are skipped during skeleton alignment
	relativeJoints = StringVec();
	relativeJoints.append('spine1')
	relativeJoints.append('spine2')
	relativeJoints.append('spine3')
	relativeJoints.append('spine4')
	relativeJoints.append('spine5')
	relativeJoints.append('r_sternoclavicular')
	relativeJoints.append('l_sternoclavicular')
	relativeJoints.append('r_acromioclavicular')
	relativeJoints.append('l_acromioclavicular')	
	
	retargetManager = scene.getRetargetManager()
        retarget = retargetManager.getRetarget(srcSkelName,tgtSkelName)
	if retarget == None:
		retarget = 	retargetManager.createRetarget(srcSkelName,tgtSkelName)
		retarget.initRetarget(endJoints,relativeJoints)
		
# Add asset paths
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('motion', 'ChrMaarten')
scene.addAssetPath('motion', 'Sinbad')
scene.addAssetPath('script', 'behaviorsets')
scene.addAssetPath('script', 'scripts')
scene.loadAssets()

# Runs the default viewer for camera
scene.setScale(1.0)
# Apply zebra joint map for ChrBrad
scene.run("zebra2-map.py")
zebra2Map = scene.getJointMapManager().getJointMap("zebra2")
bradSkeleton = scene.getSkeleton("ChrBrad.sk")
bradSkeleton.rescale(6)
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse("ChrMaarten")

scene.run('ogre-sinbad-map.py')
sinbadSkName = 'Sinbad.skeleton.xml'
jointMapManager = scene.getJointMapManager()
sinbadMap = jointMapManager.getJointMap('Sinbad.skeleton.xml')
ogreSk = scene.getSkeleton(sinbadSkName)
sinbadMap.applySkeleton(ogreSk)

# set skeleton names for source motions, so the on-line retargeting will know how to apply retargeting
setMotionNameSkeleton("ChrBrad@Idle01", "ChrBrad.sk")
setMotionNameSkeleton("ChrBrad@Guitar01", "ChrBrad.sk")
setMotionNameSkeleton("ChrBrad@PushUps01", "ChrBrad.sk")

# Create on-line retarget instance from ChrBrad.sk to common.sk
createRetargetInstance("ChrBrad.sk", sinbadSkName)

# Add Brad ( target character )
# target = scene.createCharacter('target', '')
# targetSkeleton = scene.createSkeleton('common.sk')
# target.setSkeleton(targetSkeleton)
# targetPos = SrVec(-80, 102, 0)
# target.setPosition(targetPos)
# target.createStandardControllers()
# target.setDoubleAttribute('deformableMeshScale', 1)
# target.setStringAttribute('deformableMesh', 'brad')
# bml.execBML('target', '<body posture="ChrBrad@Idle01"/>')

sinbadName = 'target'
sinbad = scene.createCharacter(sinbadName,'')
sinbadSk = scene.createSkeleton(sinbadSkName)
sinbad.setSkeleton(sinbadSk)
sinbadPos = SrVec(-6,5.16, 0)
sinbad.setPosition(sinbadPos)
sinbad.createStandardControllers()
sinbad.setDoubleAttribute('deformableMeshScale', 1)
sinbad.setStringAttribute('deformableMesh', 'Sinbad.mesh.xml')
scene.run('BehaviorSetMaleLocomotion.py')
setupBehaviorSet()
retargetBehaviorSet(sinbadName)
bml.execBML('target', '<body posture="ChrBrad@Idle01"/>')
sinbad.setStringAttribute("displayType", "GPUmesh")

# Add ChrBrad (source character )
source = scene.createCharacter('source', '')
sourceSkeleton = scene.createSkeleton('ChrBrad.sk')
source.setSkeleton(sourceSkeleton)
sourcePos = SrVec(6, 0, 0)
source.setPosition(sourcePos)
source.createStandardControllers()
# Deformable mesh
source.setDoubleAttribute('deformableMeshScale', 0.06)
source.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
bml.execBML('source', '<body posture="ChrBrad@Idle01"/>')
source.setStringAttribute("displayType", "GPUmesh")

print 'Configuring scene parameters and camera'
camera = getCamera()
camera.setEye(0, 7.98, 17.44)
camera.setCenter(1.0, 1.7, -39.5)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(100)
camera.setNearPlane(0.1)
camera.setAspectRatio(1.02)

# Retarget motion
	
last = 0
canTime = True
delay = 10
class RetargettingDemo(SBScript):
	def update(self, time):
		global canTime, last, output
		diff = time - last
		if diff >= delay:
			canTime = True
			diff = 0
		if canTime:
			last = time
			canTime = False
			# Play non retargetted and retargetted add delay
			bml.execBML('target', '<animation name="ChrBrad@Guitar01"/>')
			bml.execBML('source', '<animation name="ChrBrad@Guitar01"/>')
			
scene.removeScript('retargettingdemo')
retargettingdemo = RetargettingDemo()
scene.addScript('retargettingdemo', retargettingdemo)
