import random
import math
import httplib
import json
import time
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

steerManager.setDoubleAttribute('gridDatabaseOptions.gridSizeX', 500)
steerManager.setDoubleAttribute('gridDatabaseOptions.gridSizeZ', 500)

#steerManager.setDoubleAttribute('maxUpdateFrequency', 100) 

# Setting up Brads
print 'Setting up Brads'

site = 'localhost:8080'
amount = 15

conn = httplib.HTTPConnection(site)
conn.request('GET', '/UbikSimMOSI-AGIL-Server/ubiksim?position=people')
data = conn.getresponse()
data = data.read()
jsondata = json.loads(data)
listdata = jsondata.items()

for i in range(amount):
	baseName = 'ChrBrad%s' % i
	brad = scene.createCharacter(baseName, '')
	bradSkeleton = scene.createSkeleton('ChrBrad.sk')
	brad.setSkeleton(bradSkeleton)
	posX = listdata[i][1]["positionX"]
	posZ = listdata[i][1]["positionY"]
	# Set position logic
	bradPos = SrVec(posX, 0, posZ)
	brad.setPosition(bradPos)
	# Set up standard controllers
	brad.createStandardControllers()
	# Set deformable mesh
	brad.setDoubleAttribute('deformableMeshScale', .01)
	brad.setStringAttribute('deformableMesh', 'ChrBrad.dae')
	# Retarget character
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

conn = httplib.HTTPConnection(site)
conn.request('GET', '/UbikSimMOSI-AGIL-Server/ubiksim?position=goals')
data = conn.getresponse()
data = data.read()
jsondata = json.loads(data)
listdata = jsondata.items()

# Step registry
bradStep = []
for i in range (amount):
	bradStep.append(0)

def string2vec (stvec):
	posX = int (stvec[stvec.index('(')+1:stvec.index(',')])
	
	posZ = int (stvec[stvec.index(',')+1:stvec.index(')')])
	
	return SrVec(posX, 0, posZ)
	
def vec2str(vec):
	''' Converts SrVec to string '''
	x = vec.getData(0)
	y = vec.getData(2)
	z = vec.getData(2)
	if -0.0001 < x < 0.0001: x = 0
	if -0.0001 < y < 0.0001: y = 0
	if -0.0001 < z < 0.0001: z = 0
	return "" + str(x) + " " + str(y) + ""

class Crowd(SBScript):
	def update(self, time):
		for i in range (amount):
			brad = scene.getCharacter('ChrBrad%s' % i)
			positionVec = brad.getPosition()
			position = (str(int(positionVec.getData(0))) + ' ' + str(int(positionVec.getData(1))) + ' ' + str(int(positionVec.getData(2))))
			#reachPosition = str(string2vec(str(listdata[i][1]['goalPath'][bradStep[i]])))
			reachPosition = string2vec(str(listdata[i][1]['goalPath'][bradStep[i]]))
			#print reachPosition
			#if (position == reachPosition):
			if ((math.fabs(positionVec.getData(0) - reachPosition.getData(0))<=1) and (math.fabs(positionVec.getData(2) - reachPosition.getData(2))<=1)):
			#if ((positionVec.getData(0) < reachPosition.getData(0)+7) and (positionVec.getData(0) > reachPosition.getData(0)-7) and (positionVec.getData(2) < reachPosition.getData(2)+7) and (positionVec.getData(2) > reachPosition.getData(2)-7)):
				nextPosition = string2vec(str(listdata[i][1]['goalPath'][bradStep[i]+1]))
				print ('current '+position) 				
				print ('next '+str(nextPosition))
				print ('reach '+str(reachPosition))
				if (reachPosition != str(listdata[i][1]['goalPath'][bradStep[i]+1])):
				#if ((positionVec.getData(0)+5 == nextPosition.getData(0)) or (positionVec.getData(2)+5 > reachPosition.getData(2))):
					#print nextPosition
					#print reachPosition
					bml.execBML(brad.getName(), '<locomotion speed="' +  str(random.uniform(1.20, 5.0)) + '" target="' + vec2str(nextPosition) + '"/>')
					print ('bml '+vec2str(nextPosition))
					print ('namebrad '+brad.getName())
				bradStep[i] = bradStep[i]+1
			print 'not moving'
			print reachPosition
			print ('X ' + str(math.fabs(positionVec.getData(0) - reachPosition.getData(0))))
			print ('Y ' + str(math.fabs(positionVec.getData(2) - reachPosition.getData(2))))


# Run the update script
scene.removeScript('crowd')
crowddemo = Crowd()
scene.addScript('crowd', crowddemo)


