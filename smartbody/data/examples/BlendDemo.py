print "|--------------------------------------------|"
print "|           Starting Blend Demo              |"
print "|--------------------------------------------|"

# Add asset paths
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('motion', 'ChrMaarten')
scene.addAssetPath('motion', 'ChrRachel')
scene.addAssetPath('motion', 'Sinbad')
scene.addAssetPath('script', 'behaviorsets')
scene.addAssetPath('script', 'scripts')
scene.loadAssets()

# Set scene parameters and camera
print 'Configuring scene parameters and camera'
scene.setBoolAttribute('internalAudio', True)

print 'Setting up joint map for Brad and Rachel'
scene.run('zebra2-map.py')
zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
bradSkeleton = scene.getSkeleton('ChrBrad.sk')
bradSkeleton.rescale(6)
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse('ChrBrad')
rachelSkeleton = scene.getSkeleton('ChrRachel.sk')
rachelSkeleton.rescale(6)
zebra2Map.applySkeleton(rachelSkeleton)
zebra2Map.applyMotionRecurse('ChrRachel')

scene.run('ogre-sinbad-map.py')
sinbadSkName = 'Sinbad.skeleton.xml'
jointMapManager = scene.getJointMapManager()
sinbadMap = jointMapManager.getJointMap('Sinbad.skeleton.xml')
ogreSk = scene.getSkeleton(sinbadSkName)
sinbadMap.applySkeleton(ogreSk)

scene.run('BehaviorSetMaleLocomotion.py')
setupBehaviorSet()

# Setting up characters
print 'Setting up characters'
# chr0D
chr0D = scene.createCharacter('chr0D', '')
chr0DSkeleton = scene.createSkeleton('ChrBrad.sk')
chr0D.setSkeleton(chr0DSkeleton)
chr0D.setPosition(SrVec(-14.5, 0, 0))
chr0D.createStandardControllers()
chr0D.setDoubleAttribute('deformableMeshScale', 0.06)
chr0D.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
createRetargetInstance('test_utah.sk','ChrBrad.sk')
bml.execBML('chr0D', '<body posture="ChrUtah_Idle001"/>')
# chr1D
chr1D = scene.createCharacter('chr1D', '')
chr1DSkeleton = scene.createSkeleton(sinbadSkName)
chr1D.setSkeleton(chr1DSkeleton)
chr1D.setPosition(SrVec(-5.5, 5.16, 0))
chr1D.createStandardControllers()
chr1D.setStringAttribute('deformableMesh', 'Sinbad.mesh.xml')
createRetargetInstance('test_utah.sk',sinbadSkName)
bml.execBML('chr1D', '<body posture="ChrUtah_Idle001"/>')
# chr2D
chr2D = scene.createCharacter('chr2D', '')
chr2DSkeleton = scene.createSkeleton('ChrRachel.sk')
chr2D.setSkeleton(chr2DSkeleton)
chr2D.setPosition(SrVec(5.5, 0, 0))
chr2D.createStandardControllers()
chr2D.setDoubleAttribute('deformableMeshScale', 0.06)
chr2D.setStringAttribute('deformableMesh', 'ChrRachel.dae')
createRetargetInstance('test_utah.sk','ChrRachel.sk')
bml.execBML('chr2D', '<body posture="ChrUtah_Idle001"/>')

# Turn on GPU deformable geometry for all
for name in scene.getCharacterNames():
	scene.getCharacter(name).setStringAttribute("displayType", "GPUmesh")

# Set up blends
blendManager = scene.getBlendManager()

# 0D Blend
print 'Setting up 0D blend'
blend0D = blendManager.createBlend0D('blend0D')
blend0D.setBlendSkeleton('test_utah.sk')
motions = StringVec()
motions.append('ChrUtah_WalkInCircleRight001')
blend0D.addMotion(motions[0])

# 1D Blend
print 'Setting up 1D blend'
blend1D = blendManager.createBlend1D('blend1D')
blend1D.setBlendSkeleton('test_utah.sk')
motions = StringVec()
motions.append('ChrUtah_Idle001')
motions.append('ChrUtah_Turn90Lf01')
motions.append('ChrUtah_Turn180Lf01')
motions.append('ChrUtah_Turn90Rt01')
motions.append('ChrUtah_Turn180Rt01')

paramsX = DoubleVec()
paramsX.append(0) # ChrUtah_Idle001 X
paramsX.append(-90) # ChrUtah_Turn90Lf01 X
paramsX.append(-180) # ChrUtah_Turn180Lf01 X
paramsX.append(90) # ChrUtah_Turn90Rt01 X
paramsX.append(180) # ChrUtah_Turn180Rt01 X
for i in range(0, len(motions)):
	blend1D.addMotion(motions[i], paramsX[i])

points0 = DoubleVec()
points0.append(0) # ChrUtah_Idle001 0
points0.append(0) # ChrUtah_Turn90Lf01 0
points0.append(0) # ChrUtah_Turn180Lf01 0
points0.append(0) # ChrUtah_Turn90Rt01 0
points0.append(0) # ChrUtah_Turn180Rt01 0
blend1D.addCorrespondencePoints(motions, points0)
points1 = DoubleVec()
points1.append(0.255738) # ChrUtah_Idle001 1
points1.append(0.762295) # ChrUtah_Turn90Lf01 1
points1.append(0.87541) # ChrUtah_Turn180Lf01 1
points1.append(0.757377) # ChrUtah_Turn90Rt01 1
points1.append(0.821311) # ChrUtah_Turn180Rt01 1
blend1D.addCorrespondencePoints(motions, points1)
points2 = DoubleVec()
points2.append(0.633333) # ChrUtah_Idle001 2
points2.append(1.96667) # ChrUtah_Turn90Lf01 2
points2.append(2.46667) # ChrUtah_Turn180Lf01 2
points2.append(1.96667) # ChrUtah_Turn90Rt01 2
points2.append(2.46667) # ChrUtah_Turn180Rt01 2
blend1D.addCorrespondencePoints(motions, points2)

# 2D Blend
print 'Setting up 2D blend'
blend2D = blendManager.createBlend2D("blend2D")
blend2D.setBlendSkeleton('test_utah.sk')

motions = StringVec()
motions.append("ChrUtah_Idle001")
motions.append("ChrUtah_Idle01_StepBackwardRt01")
motions.append("ChrUtah_Idle01_StepForwardRt01")
motions.append("ChrUtah_Idle01_StepSidewaysRt01")
motions.append("ChrUtah_Idle01_StepBackwardLf01")
motions.append("ChrUtah_Idle01_StepForwardLf01")
motions.append("ChrUtah_Idle01_StepSidewaysLf01")

paramsX = DoubleVec()
paramsY = DoubleVec()
paramsX.append(0) # ChrUtah_Idle001 X
paramsY.append(0) # ChrUtah_Idle001 Y
paramsX.append(-0.0275919) # ChrUtah_Idle01_StepBackwardRt01 X
paramsY.append(-19.5057) # ChrUtah_Idle01_StepBackwardRt01 Y
paramsX.append(0.0240943) # ChrUtah_Idle01_StepForwardRt01 X
paramsY.append(45.4044) # ChrUtah_Idle01_StepForwardRt01 Y
paramsX.append(28.8772) # ChrUtah_Idle01_StepSidewaysRt01 X
paramsY.append(0.00321) # ChrUtah_Idle01_StepSidewaysRt01 Y
paramsX.append(-0.0212764) # ChrUtah_Idle01_StepBackwardLf01 X
paramsY.append(-39.203) # ChrUtah_Idle01_StepBackwardLf01 Y
paramsX.append(0.0480087) # ChrUtah_Idle01_StepForwardLf01 X
paramsY.append(47.8086) # ChrUtah_Idle01_StepForwardLf01 Y
paramsX.append(-31.7367) # ChrUtah_Idle01_StepSidewaysLf01 X
paramsY.append(0) # ChrUtah_Idle01_StepSidewaysLf01 Y
for i in range(0, len(motions)):
	blend2D.addMotion(motions[i], paramsX[i], paramsY[i])

points0 = DoubleVec()
points0.append(0) # ChrUtah_Idle001 0
points0.append(0) # ChrUtah_Idle01_StepBackwardRt01 0
points0.append(0) # ChrUtah_Idle01_StepForwardRt01 0
points0.append(0) # ChrUtah_Idle01_StepSidewaysRt01 0
points0.append(0) # ChrUtah_Idle01_StepBackwardLf01 0
points0.append(0) # ChrUtah_Idle01_StepForwardLf01 0
points0.append(0) # ChrUtah_Idle01_StepSidewaysLf01 0
blend2D.addCorrespondencePoints(motions, points0)
points1 = DoubleVec()
points1.append(0.556322) # ChrUtah_Idle001 1
points1.append(0.556322) # ChrUtah_Idle01_StepBackwardRt01 1
points1.append(0.543678) # ChrUtah_Idle01_StepForwardRt01 1
points1.append(0.482989) # ChrUtah_Idle01_StepSidewaysRt01 1
points1.append(0.395402) # ChrUtah_Idle01_StepBackwardLf01 1
points1.append(0.531034) # ChrUtah_Idle01_StepForwardLf01 1
points1.append(0.473563) # ChrUtah_Idle01_StepSidewaysLf01 1
blend2D.addCorrespondencePoints(motions, points1)
points2 = DoubleVec()
points2.append(1.46414) # ChrUtah_Idle001 2
points2.append(1.46414) # ChrUtah_Idle01_StepBackwardRt01 2
points2.append(1.46414) # ChrUtah_Idle01_StepForwardRt01 2
points2.append(1.46414) # ChrUtah_Idle01_StepSidewaysRt01 2
points2.append(1.33333) # ChrUtah_Idle01_StepBackwardLf01 2
points2.append(1.33333) # ChrUtah_Idle01_StepForwardLf01 2
points2.append(1.33103) # ChrUtah_Idle01_StepSidewaysLf01 2
blend2D.addCorrespondencePoints(motions, points2)

blend2D.addTriangle("ChrUtah_Idle001", "ChrUtah_Idle01_StepBackwardLf01", "ChrUtah_Idle01_StepSidewaysLf01")
blend2D.addTriangle("ChrUtah_Idle001", "ChrUtah_Idle01_StepForwardLf01", "ChrUtah_Idle01_StepSidewaysLf01")
blend2D.addTriangle("ChrUtah_Idle001", "ChrUtah_Idle01_StepBackwardRt01", "ChrUtah_Idle01_StepSidewaysRt01")
blend2D.addTriangle("ChrUtah_Idle001", "ChrUtah_Idle01_StepForwardRt01", "ChrUtah_Idle01_StepSidewaysRt01")

''' MISSING 3D BLEND '''
bml.execBML('chr0D', '<blend name="blend0D"/>')

print 'Configuring scene parameters and camera'
camera = getCamera()
camera.setEye(-6, 7.98, 28.44)
camera.setCenter(-6, 1.7, -39.5)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(100)
camera.setNearPlane(0.01)
camera.setAspectRatio(1.02)

last = 0
canTime = True
delay = 5

last1D = 0
delay1D = 1

blend1DX = -180
amount = 10
blend2DX = 28.88
blend2DY = 47.81
class BlendDemo(SBScript):
	def update(self, time):
		global canTime, last, blend1DX, blend2DX, blend2DY, amount, last1D
		# Update every few seconds
		diff1D = time - last1D
		if diff1D > delay1D:
			# 1D Blend
			bml.execBML('chr1D', '<blend name="blend1D" x="' + str(blend1DX) + '"/>')
			# Slowly increment and decrement
			blend1DX = blend1DX + amount
			if blend1DX <= -180: 
				amount = amount * -1
			elif blend1DX >= 180:
				amount = amount * -1
			last1D = time
		
		diff = time - last
		# Start blend after delay
		if diff > delay:
			# 2D Blend
			bml.execBML('chr2D', '<blend name="blend2D" x="' + str(blend2DX) + '" y="' + str(blend2DY) + '"/>')
			# Alternate between 2D blend
			if blend2DY == 47.81:
				blend2DX = -31.74
				blend2DY = -39.20
			elif blend2DY == -39.20: 
				blend2DX = 28.88
				blend2DY = 47.81
				#28.88 -31.74
			# Reset timer
			last = time
		
# Run the update script
scene.removeScript('blenddemo')
blenddemo = BlendDemo()
scene.addScript('blenddemo', blenddemo)
