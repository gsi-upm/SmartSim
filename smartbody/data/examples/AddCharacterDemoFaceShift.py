print "|--------------------------------------------|"
print "|         Starting Character Demo            |"
print "|--------------------------------------------|"

# Add asset paths
scene.addAssetPath('mesh', 'mesh')
#scene.addAssetPath('motion', 'ChrBrad')
scene.addAssetPath('motion', 'ChrMaarten')
scene.addAssetPath("script", "behaviorsets")
scene.addAssetPath('script', 'scripts')
scene.loadAssets()

# Set scene parameters and camera
print 'Configuring scene parameters and camera'
scene.setScale(1.0)
scene.setBoolAttribute('internalAudio', True)
scene.run('default-viewer.py')
camera = getCamera()
camera.setEye(0, 1.71, 1.86)
camera.setCenter(0, 1, 0.01)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(100)
camera.setNearPlane(0.1)
camera.setAspectRatio(0.966897)
cameraPos = SrVec(0, 1.6, 10)
scene.getPawn('camera').setPosition(cameraPos)

# Set up joint map for Brad
print 'Setting up joint map and configuring Brad\'s skeleton'
scene.run('zebra2-map.py')
zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
bradSkeleton = scene.getSkeleton('ChrBrad.sk')
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse('ChrBrad')

# Establish lip syncing data set
print 'Establishing lip syncing data set'
scene.run('init-diphoneDefault.py')

# Set up face definition
print 'Setting up face definition'
# Brad's face definition
bradFace = scene.createFaceDefinition('ChrBrad')
bradFace.setFaceNeutral('ChrBrad@face_neutral')

bradFace.setAU(1,  "left",  "ChrBrad@001_inner_brow_raiser_lf")
bradFace.setAU(1,  "right", "ChrBrad@001_inner_brow_raiser_rt")
bradFace.setAU(2,  "left",  "ChrBrad@002_outer_brow_raiser_lf")
bradFace.setAU(2,  "right", "ChrBrad@002_outer_brow_raiser_rt")
bradFace.setAU(4,  "left",  "ChrBrad@004_brow_lowerer_lf")
bradFace.setAU(4,  "right", "ChrBrad@004_brow_lowerer_rt")
bradFace.setAU(5,  "both",  "ChrBrad@005_upper_lid_raiser")
bradFace.setAU(6,  "both",  "ChrBrad@006_cheek_raiser")
bradFace.setAU(7,  "both",  "ChrBrad@007_lid_tightener")
bradFace.setAU(10, "both",  "ChrBrad@010_upper_lip_raiser")
bradFace.setAU(12, "left",  "ChrBrad@012_lip_corner_puller_lf")
bradFace.setAU(12, "right", "ChrBrad@012_lip_corner_puller_rt")
bradFace.setAU(25, "both",  "ChrBrad@025_lips_part")
bradFace.setAU(26, "both",  "ChrBrad@026_jaw_drop")
bradFace.setAU(45, "left",  "ChrBrad@045_blink_lf")
bradFace.setAU(45, "right", "ChrBrad@045_blink_rt")

bradFace.setViseme("open",    "ChrBrad@open")
bradFace.setViseme("W",       "ChrBrad@W")
bradFace.setViseme("ShCh",    "ChrBrad@ShCh")
bradFace.setViseme("PBM",     "ChrBrad@PBM")
bradFace.setViseme("FV",      "ChrBrad@FV")
bradFace.setViseme("wide",    "ChrBrad@wide")
bradFace.setViseme("tBack",   "ChrBrad@tBack")
bradFace.setViseme("tRoof",   "ChrBrad@tRoof")
bradFace.setViseme("tTeeth",  "ChrBrad@tTeeth")

'''
bradFace.setAU(1,  "left",  "")
bradFace.setAU(1,  "right", "")
bradFace.setAU(2,  "left",  "")
bradFace.setAU(2,  "right", "")
bradFace.setAU(4,  "left",  "")
bradFace.setAU(4,  "right", "")
bradFace.setAU(5,  "both",  "")
bradFace.setAU(6,  "both",  "")
bradFace.setAU(7,  "both",  "")
bradFace.setAU(10, "both",  "")
bradFace.setAU(12, "left",  "")
bradFace.setAU(12, "right", "")
bradFace.setAU(25, "both",  "")
bradFace.setAU(26, "both",  "")
bradFace.setAU(45, "left",  "")
bradFace.setAU(45, "right", "")

bradFace.setViseme("open",    "")
bradFace.setViseme("W",       "")
bradFace.setViseme("ShCh",    "")
bradFace.setViseme("PBM",     "")
bradFace.setViseme("FV",      "")
bradFace.setViseme("wide",    "")
bradFace.setViseme("tBack",   "")
bradFace.setViseme("tRoof",   "")
bradFace.setViseme("tTeeth",  "")
'''

'''
bradFace.setViseme("1",  "ChrBrad@face_neutral") # eye blink l
bradFace.setViseme("2",  "ChrBrad@face_neutral") # eye blink r
bradFace.setViseme("9",  "ChrBrad@face_neutral") # eye open l
bradFace.setViseme("10",  "ChrBrad@face_neutral") # eye open r
bradFace.setViseme("15",  "ChrBrad@face_neutral") # brow down l
bradFace.setViseme("16",  "ChrBrad@face_neutral") # brow down r
bradFace.setViseme("17",  "ChrBrad@face_neutral") # inner brows up 
bradFace.setViseme("18",  "ChrBrad@face_neutral") # upper brow l
bradFace.setViseme("19",  "ChrBrad@face_neutral") # upper brow r
bradFace.setViseme("20",  "ChrBrad@face_neutral") # jaw open
bradFace.setViseme("25",  "ChrBrad@face_neutral") # lips upper l
bradFace.setViseme("26",  "ChrBrad@face_neutral") # lips upper r
bradFace.setViseme("31",  "ChrBrad@face_neutral") # smile l
bradFace.setViseme("32",  "ChrBrad@face_neutral") # smile r
'''


print 'Adding character into scene'
# Set up Brad
brad = scene.createCharacter('ChrBrad', '')
bradSkeleton = scene.createSkeleton('ChrBrad.sk')
brad.setSkeleton(bradSkeleton)
# Set position
bradPos = SrVec(0, 0, 0)
brad.setPosition(bradPos)
# Set facing direction
bradFacing = SrVec(0, 0, 0)
brad.setHPR(bradFacing)
# Set face definition
brad.setFaceDefinition(bradFace)
# Set standard controller
brad.createStandardControllers()
# Deformable mesh
brad.setDoubleAttribute('deformableMeshScale', .01)
brad.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
#brad.setStringAttribute('deformableMesh', 'ChrBrad.dae')

# Lip syncing diphone setup
brad.setStringAttribute('lipSyncSetName', 'default')
brad.setBoolAttribute('usePhoneBigram', True)
brad.setVoice('remote')
brad.setVoiceCode('Microsoft|Anna')

# setup locomotion
scene.run('BehaviorSetMaleMocapLocomotion.py')
setupBehaviorSet()
retargetBehaviorSet('ChrBrad')

# setup gestures
scene.run('BehaviorSetGestures.py')
setupBehaviorSet()
retargetBehaviorSet('ChrBrad')
'''
# setup reach 
scene.run('BehaviorSetReaching.py')
setupBehaviorSet()
retargetBehaviorSet('ChrBrad')
'''
# Turn on GPU deformable geometry
brad.setStringAttribute("displayType", "GPUmesh")

# Set up steering
print 'Setting up steering'
steerManager = scene.getSteerManager()
steerManager.setEnable(False)
brad.setBoolAttribute('steering.pathFollowingMode', False) # disable path following mode so that obstacles will be respected
steerManager.setEnable(True)
faceShiftManager = scene.getFaceShiftManager()
faceShiftManager.setStringAttribute('targetCharacter','ChrBrad')
# Start the simulation
print 'Starting the simulation'
sim.start()

bml.execBML('ChrBrad', '<body posture="ChrMarine@Idle01"/>')
bml.execBML('ChrBrad', '<saccade mode="listen"/>')
#bml.execBML('ChrBrad', '<gaze sbm:handle="brad" target="camera"/>')

# create a controller
class MyController (PythonController):
	def init(self, pawn):
		# indicate which channels are going to be affected by this controller
		self.addChannel("au_1_left", "XPos")
		self.addChannel("au_1_right", "XPos")
		self.addChannel("au_2_left", "XPos")
		self.addChannel("au_2_right", "XPos")
		self.addChannel("au_4_left", "XPos")
		self.addChannel("au_4_right", "XPos")
		self.addChannel("au_5", "XPos")
		self.addChannel("au_10", "XPos")
		self.addChannel("au_45_left", "XPos")
		self.addChannel("au_45_right", "XPos")
		self.addChannel("open", "XPos")
		self.addChannel("W", "XPos")
		self.addChannel("ShCh", "XPos")
		self.addChannel("wide", "XPos")
		self.addChannel("PBM", "XPos")
		self.addChannel("FV", "XPos")
		self.addChannel("skullbase", "Quat")
		'''
		self.addChannel("1", "XPos")
		self.addChannel("2", "XPos")
		self.addChannel("9", "XPos")
		self.addChannel("10", "XPos")
		self.addChannel("15", "XPos")
		self.addChannel("16", "XPos")
		self.addChannel("17", "XPos")
		self.addChannel("18", "XPos")
		self.addChannel("19", "XPos")		
		self.addChannel("20", "XPos")	
		'''
		
	def evaluate(self):
		# every time step, get the value from one channel and then modify another
		faceShiftManager = scene.getFaceShiftManager()
		# blink
		val = faceShiftManager.getCoeffValue("1")
		self.setChannelValue("au_45_left", val)
		val = faceShiftManager.getCoeffValue("2")
		self.setChannelValue("au_45_right", val)
		
		#print "BLINK " + str(val)
		
		# eye opener
		val = faceShiftManager.getCoeffValue("9")
		self.setChannelValue("au_5", val)
		
		# eyebrows down opener
		val = faceShiftManager.getCoeffValue("15")
		self.setChannelValue("au_4_left", val)
		val = faceShiftManager.getCoeffValue("16")
		self.setChannelValue("au_4_right", val)	
		
		# eyebrows up
		val = faceShiftManager.getCoeffValue("17")
		self.setChannelValue("au_1_left", val)
		self.setChannelValue("au_1_right", val)			
		
		# outer eyebrows up
		val = faceShiftManager.getCoeffValue("18")
		self.setChannelValue("au_2_left", val)
		val = faceShiftManager.getCoeffValue("19")
		self.setChannelValue("au_2_right", val)			
	
		# LipsPucker
		val = faceShiftManager.getCoeffValue("41")
		self.setChannelValue("W", val)
		#print 'W = ' + str(val)
		
		# LipsFunnel
		val = faceShiftManager.getCoeffValue("42")
		self.setChannelValue("ShCh", val)
		#print 'ShCh = ' + str(val)
		
			
		# jaw open
		val = faceShiftManager.getCoeffValue("20")
		self.setChannelValue("open", val)
		#print 'open = ' + str(val)		
		
		# LipsUpperClose
		val = faceShiftManager.getCoeffValue("29")
		self.setChannelValue("PBM", val)
		#print 'PBM = ' + str(val)
		
		# LipsUpperClose
		val = faceShiftManager.getCoeffValue("46")
		self.setChannelValue("FV", val)
		#print 'FV = ' + str(val)
		
		# MouthDimple
		val = faceShiftManager.getCoeffValue("33")
		self.setChannelValue("wide", val)
		#print 'wide = ' + str(val)
		#val = faceShiftManager.getCoeffValue("34")
		
		rot = faceShiftManager.getHeadRotation()
		#print 'rot = ' + str(rot.getData(0)) + ', ' + str(rot.getData(1)) + ', ' + str(rot.getData(2)) + ', ' + str(rot.getData(3))
		self.setChannelQuatGlobal('skullbase',rot)
		

# instantiate this controller once for each character
myc = MyController()
# get the character
character = scene.getCharacter("ChrBrad")
# run this controller in position 15 (I can explain more about this)
character.addController(15, myc) 
# you can enable/disable the controller like this:
#myc.setIgnore(True)




sim.resume()
