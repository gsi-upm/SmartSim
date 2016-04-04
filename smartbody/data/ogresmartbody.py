import math
import random
print "|--------------------------------------------|"
print "|         Starting Ogre Demo           |"
print "|--------------------------------------------|"

scene.setScale(0.1)
# Add asset paths

scene.addAssetPath('motion', 'Sinbad')
scene.addAssetPath('script', 'scripts')
scene.addAssetPath('script', 'behaviorsets')
scene.loadAssets()

# Map Sinbad to the standard SmartBody setup
print 'Setting up joint map for Sinbad'
scene.run('ogre-sinbad-map.py')
sinbadSkName = 'Sinbad.skeleton.xml'
jointMapManager = scene.getJointMapManager()
sinbadMap = jointMapManager.getJointMap('Sinbad.skeleton.xml')
ogreSk = scene.getSkeleton(sinbadSkName)
sinbadMap.applySkeleton(ogreSk)

# load up all the behaviors that can be associated with Sinbad
scene.run('default-behavior-sets.py')

# Set up Sinbad
print 'Setting up Sinbad'


numCharacters = 15
for c in range(0, numCharacters):
	sinbadName = 'sinbad' + str(c)
	sinbad = scene.createCharacter(sinbadName,'sinbad')
	sinbadSk = scene.createSkeleton(sinbadSkName)
	sinbad.setSkeleton(sinbadSk)
	sinbadPos = SrVec(0 + c * 5, 5.16, 0 + c * 5)
	sinbad.setPosition(sinbadPos)
	sinbad.createStandardControllers()

	# only retarget the first character, the others
	# can use the same data
	if c == 0:
		# add behaviors to Sinbad: locomotion
		scene.run('BehaviorSetMaleLocomotion.py')
		setupBehaviorSet()
		retargetBehaviorSet(sinbadName)
	steerManager = scene.getSteerManager()
	steerAgent = steerManager.createSteerAgent(sinbadName)
	steerAgent.setSteerStateNamePrefix("all")
	steerAgent.setSteerType("example")

	#sinbad.setBoolAttribute("steering.pathFollowingMode", True)

steerManager.setEnable(False)
steerManager.setEnable(True)
print 'Configuring scene parameters and camera'
scene.setBoolAttribute('internalAudio', True)

sim.start()
for c in range(0, numCharacters):
	sinbadName = 'sinbad' + str(c)
	bml.execBML(sinbadName, '<body posture="ChrUtah_Idle001"/>')
sim.resume()



# the following is a script that tells Sinbad to run back and forth between two points.
class LocomotionHandler(SBEventHandler):
	stareAt = 0
	def executeAction(self, event):
		# get the name of the character
		tokens = event.getParameters().split()
		# when the character arrives at one location, send it to the other location
		sinbadName = tokens[0]
		pos = scene.getCharacter(sinbadName).getPosition()
		if (pos.getData(0) > 20):
			bml.execBML(sinbadName, '<locomotion target="-30 0" />')
			# make Sinbad nod when he reaches one side
			bml.execBML(sinbadName, '<head type="NOD" repeats="2"/>')
		else:
			bml.execBML(sinbadName, '<locomotion target="30 0" />')
			# make Sinbad shake his head when he reaches the other side
			bml.execBML(sinbadName, '<head type="SHAKE" repeats="2"/>')

		# randomly stare at another Sinbad
		stareAtName = "sinbad" + str(self.stareAt)
		#bml.execBML(sinbadName, '<gaze target="' + stareAtName + '" sbm:joint-range="NECK EYES"/>')
		bml.execBML(sinbadName, '<gaze target="' + stareAtName + '"/>')
		self.stareAt = self.stareAt + 1
		if (self.stareAt >= numCharacters):
			self.startAt = 0

myHandler = LocomotionHandler()
eventManager = scene.getEventManager()
eventManager.addEventHandler("locomotion", myHandler)
					
for c in range(0, numCharacters):
	sinbadName = 'sinbad' + str(c)					
	bml.execBMLAt(2.0, sinbadName, '<locomotion target="-30 0" />')

