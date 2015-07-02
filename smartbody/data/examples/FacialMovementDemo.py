import random

print "|--------------------------------------------|"
print "|       Starting Facial Movement Demo        |"
print "|--------------------------------------------|"

# Add asset paths
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('motion', 'ChrMaarten')
scene.addAssetPath('script', 'scripts')
scene.loadAssets()

# Runs the default viewer for camera
scene.setScale(1.0)
scene.run('default-viewer.py')
camera = getCamera()
camera.setEye(0.0, 1.61, 0.33)
camera.setCenter(0.0, 1.61, -0.12)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(100)
camera.setNearPlane(0.1)
camera.setAspectRatio(0.966897)

print 'Setting up joint map for Brad and Rachel'
scene.run('zebra2-map.py')
zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
bradSkeleton = scene.getSkeleton('ChrBrad.sk')
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse('ChrBrad')

# Setting up face definition
print 'Setting up Brad\'s face definition'
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

# Setting up Brad
print 'Setting up Brad'
brad = scene.createCharacter('ChrBrad', '')
bradSkeleton = scene.createSkeleton('ChrBrad.sk')
brad.setSkeleton(bradSkeleton)
brad.setFaceDefinition(bradFace)
brad.createStandardControllers()
# DeformableMesh
brad.setDoubleAttribute('deformableMeshScale', .01)
brad.setStringAttribute('deformableMesh', 'ChrMaarten.dae')

# Turn on GPU deformable geometry
brad.setStringAttribute("displayType", "GPUmesh")


# Update to repeat reaches
last = 0
canTime = True
delay = 2
class FacialMovementDemo(SBScript):
	def update(self, time):
		global canTime, last
		if canTime:
			last = time
			canTime = False
		diff = time - last
		if diff >= delay:
			diff = 0
			canTime = True
		# If time's up, do action
		if canTime:
			nextFace()

# List of expressions
expressionList = ['sad', 'shock', 'angry', 'happy', 'fear']
chrName = 'ChrBrad'
curFace = 0
faceAmt = len(expressionList)
# List of expressions, choice list, get and call function		
def nextFace():
	global curFace
	expression = expressionList[curFace]
	if expression == 'sad': 
		print 'Playing sad'
		bml.execBML(chrName, '<face type="facs" au="1_left" amount="1"/><face type="facs" au="1_right" amount="1"/> + \
							  <face type="facs" au="4_left" amount="1"/><face type="facs" au="4_right" amount="1"/> + \
							  <face type="facs" au="6" amount="0.58"/>')
	if expression == 'happy':
		print 'Playing happy'
		bml.execBML(chrName, '<face type="facs" au="6" amount="1"/><face type="facs" au="12" amount="1"/>')
	if expression == 'angry':
		print 'Playing angry'
		bml.execBML(chrName, '<face type="facs" au="2_left" amount="1"/><face type="facs" au="2_right" amount="1"/> + \
							  <face type="facs" au="4_left" amount="1"/><face type="facs" au="4_right" amount="1"/> + \
							  <face type="facs" au="5" amount="0.68"/><face type="facs" au="7" amount="0.5"/> + \
							  <face type="facs" au="10" amount="1"/><face type="facs" au="26" amount="0.22"/>')
	if expression == 'shock':
		print 'Playing shock'
		bml.execBML(chrName, '<face type="facs" au="1_left" amount="1"/><face type="facs" au="1_right" amount="1"/> + \
							  <face type="facs" au="5" amount="0.86"/><face type="facs" au="26" amount="0.73"/>')
	if expression == 'fear':
		print 'Playing fear'
		bml.execBML(chrName, '<face type="facs" au="1_left" amount="0.6"/><face type="facs" au="1_right" amount="0.6"/> + \
							  <face type="facs" au="5" amount="0.7"/><face type="facs" au="26" amount="0.25"/> + \
							  <face type="facs" au="38" amount="1"/>')
	# Increment index, reset when hit max
	curFace = curFace + 1
	if curFace >= faceAmt:
		curFace = 0

# Run the update script
scene.removeScript('facialmovementdemo')
facialmovementdemo = FacialMovementDemo()
scene.addScript('facialmovementdemo', facialmovementdemo)