print "|--------------------------------------------|"
print "|         Starting Tutorial 4	            |"
print "|--------------------------------------------|"

print 'media path = ' + scene.getMediaPath()
# Add asset paths
assetManager = scene.getAssetManager()
assetManager.addAssetPath('motion', 'ChrBrad')
assetManager.addAssetPath('mesh', 'mesh')
assetManager.addAssetPath('script', 'scripts')
# Load assets based on asset paths
assetManager.loadAssets()

# set scene scale and reset the camera
scene.setScale(1.0)
scene.getActiveCamera().reset()

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
