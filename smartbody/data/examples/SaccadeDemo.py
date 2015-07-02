print "|--------------------------------------------|"
print "|        Starting Speech/Face Demo           |"
print "|--------------------------------------------|"

# Add asset paths
scene.addAssetPath('script', 'scripts')
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('motion', 'ChrMaarten')
scene.loadAssets()

# Set scene parameters and camera
print 'Configuring scene parameters and camera'
scene.setScale(1.0)
scene.setBoolAttribute('internalAudio', True)
scene.run('default-viewer.py')
camera = getCamera()
camera.setEye(0.08, 1.55, 0.79)
camera.setCenter(0.08, 1.45, 0.0)
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

# Set up 3 Brads
print 'Adding characters into scene'
posX = 0.0
for i in range(1):
	baseName = 'ChrBrad%s' % i
	brad = scene.createCharacter(baseName, '')
	bradSkeleton = scene.createSkeleton('ChrBrad.sk')
	brad.setSkeleton(bradSkeleton)
	# Set position
	bradPos = SrVec((posX + (i * 55))/100, 0, 0)
	brad.setPosition(bradPos)
	# Set up standard controllers
	brad.createStandardControllers()
	# Set deformable mesh
	brad.setDoubleAttribute('deformableMeshScale', .01)
	brad.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
	# Play idle animation
	bml.execBML(baseName, '<body posture="ChrBrad@Idle01"/>')
	
# Set proper facing angle
#scene.getCharacter('ChrBrad0').setHPR(SrVec(25, 0, 0))
#scene.getCharacter('ChrBrad2').setHPR(SrVec(-25, 0, 0))

# Turn on GPU deformable geometry for all
for name in scene.getCharacterNames():
	scene.getCharacter(name).setStringAttribute("displayType", "GPUmesh")

# Talk
bml.execBML('ChrBrad0', '<saccade mode="talk"/>')
# Listen
#bml.execBML('ChrBrad1', '<saccade mode="listen"/>')
# Think
#bml.execBML('ChrBrad2', '<saccade mode="think"/>')