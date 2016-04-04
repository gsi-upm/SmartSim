print "|--------------------------------------------|"
print "|         Starting Tutorial 3	            |"
print "|--------------------------------------------|"

print 'media path = ' + scene.getMediaPath()
# Add asset paths
assetManager = scene.getAssetManager()
assetManager.addAssetPath('motion', 'ChrBrad')
assetManager.addAssetPath('mesh', 'mesh')
assetManager.addAssetPath('script', 'scripts')
# Load assets based on asset paths
assetManager.loadAssets()
# run a Python script file
scene.run('zebra2-map.py')

motionNames = assetManager.getMotionNames()
skelNames = assetManager.getSkeletonNames()
for i in range(0,len(motionNames)):
	print 'motion ' + str(i) + ' = ' + motionNames[i] 

for i in range(0,len(skelNames)):
	print 'skeleton ' + str(i) + ' = ' + skelNames[i]
	

