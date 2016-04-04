print "|--------------------------------------------|"
print "|         Starting Tutorial 5	            |"
print "|--------------------------------------------|"

print 'media path = ' + scene.getMediaPath()
# Add asset paths
assetManager = scene.getAssetManager()
assetManager.addAssetPath('motion', 'ChrBrad')
assetManager.addAssetPath('mesh', 'mesh')
assetManager.addAssetPath('script', 'scripts')
# Load assets based on asset paths
assetManager.loadAssets()

# set the camera
scene.setScale(1.0)
scene.run('default-viewer.py')

# run a Python script file
scene.run('zebra2-map.py')
zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
bradSkeleton = scene.getSkeleton('ChrBrad.sk')
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse('ChrBrad')

# Set up Brad
brad = scene.createCharacter('ChrBrad', '')
bradSkeleton = scene.createSkeleton('ChrBrad.sk')
brad.setSkeleton(bradSkeleton)
# Set standard controller
brad.createStandardControllers()
# Deformable mesh
brad.setDoubleAttribute('deformableMeshScale', .01)
brad.setStringAttribute('deformableMesh', 'ChrBrad.dae')

# show the character
brad.setStringAttribute('displayType', 'GPUmesh')

# Starting the simulation
sim.start()
# set posture
bml.execBML('ChrBrad', '<body posture="ChrBrad@Idle01"/>')
# set animation
bml.execBML('ChrBrad', '<animation name="ChrBrad@Guitar01"/>')
# set head nod
bml.execBML('ChrBrad', '<head type="NOD"/>')
# set gaze
obj = scene.createPawn('obj1')
obj.setPosition(SrVec(0.5,0.1,0))
bml.execBML('ChrBrad','<gaze target="obj1"/>')
sim.resume()
