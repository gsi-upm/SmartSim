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
zebra2Map.applyMotionRecurse('ChrMaarten')

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

sliderList = []

print 'Setting up GUI'
scene.run('GUIUtil.py')
gui = GUIManager()
class GUIHandler:
	def handleFaceAUSlider(self,args):
		slider = args.window
		#print 'handleFaceAUSlider name = ' + slider.getName() + ' value = ' + str(slider.getCurrentValue())	
		auName = str(slider.getText())		
		#print 'handleFaceAUSlider  auName = ' + auName + ' value = ' + str(slider.getCurrentValue())	
		cmd = 'char ChrBrad viseme ' + auName + ' ' + str(slider.getCurrentValue())
		scene.command(cmd)
		
	def handleFaceVisemeSlider(self,args):
		slider = args.window
		#print 'handleFaceAUSlider name = ' + slider.getName() + ' value = ' + str(slider.getCurrentValue())	
		viName = str(slider.getText())		
		#print 'handleFaceVisemeSlider  auName = ' + viName + ' value = ' + str(slider.getCurrentValue())	
		cmd = 'char ChrBrad viseme ' + viName + ' ' + str(slider.getCurrentValue())
		scene.command(cmd)
	def handleFaceResetButton(self,args):
		#print 'handleFaceReset'
		for i in range(0,len(sliderList)):
			slider = sliderList[i]
			slider.setCurrentValue(0.0)
		
alphaVal = 0.5		
guiHandler = GUIHandler()
auNums = bradFace.getAUNumbers()
for i in range(0,len(auNums)):
	auIdx = auNums[i]
	auSide = bradFace.getAUSide(auIdx)
	auNames = []
	if auSide == "BOTH":
		auNames.append('au_'+str(auIdx))
	elif auSide == "LEFTRIGHT":
		auNames.append('au_'+str(auIdx)+'_left')
		auNames.append('au_'+str(auIdx)+'_right')
	elif auSide == "LEFT":
		auNames.append('au_'+str(auIdx)+'_left')
	elif auSide == "RIGHT":
		auNames.append('au_'+str(auIdx)+'_right')
	
	for j in range(0,len(auNames)):
		auSlider = gui.createSlider(auNames[j],1.0,0.0)
		auSlider.setText(auNames[j])		
		auSlider.subscribeEvent(Slider.EventValueChanged, guiHandler.handleFaceAUSlider)
		auSlider.setAlpha(alphaVal)
		sliderList.append(auSlider)
		
visemeNames = bradFace.getVisemeNames()
for i in range(0,len(visemeNames)):
		viName = visemeNames[i]
		visemeSlider = gui.createSlider(viName,1.0,0.0)
		visemeSlider.setText(viName)
		visemeSlider.subscribeEvent(Slider.EventValueChanged, guiHandler.handleFaceVisemeSlider)
		visemeSlider.setAlpha(alphaVal)
		sliderList.append(visemeSlider)
		
faceResetBtn = gui.createButton('resetFace','Reset Face')	
faceResetBtn.subscribeEvent(PushButton.EventClicked, guiHandler.handleFaceResetButton)
faceResetBtn.setAlpha(alphaVal)



