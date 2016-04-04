# state ChrMarineStep
# autogenerated by SmartBody

stateManager = scene.getStateManager()

stateChrMarineStep = stateManager.createState2D("mocapStep")
stateChrMarineStep.setBlendSkeleton('ChrBackovic.sk')
motions = StringVec()
motions.append("ChrMarine@Idle01")
motions.append("ChrMarine@Idle01_StepBackwardsLf01")
motions.append("ChrMarine@Idle01_StepBackwardsRt01")
motions.append("ChrMarine@Idle01_StepForwardLf01")
motions.append("ChrMarine@Idle01_StepForwardRt01")
motions.append("ChrMarine@Idle01_StepSidewaysLf01")
motions.append("ChrMarine@Idle01_StepSidewaysRt01")

paramsX = DoubleVec()
paramsY = DoubleVec()
paramsX.append(8.31458e-005) # ChrMarine@Idle01 X
paramsY.append(-6.8546e-005) # ChrMarine@Idle01 Y
paramsX.append(0.0) # ChrMarine@Idle01_StepBackwardsLf01 X
paramsY.append(-0.349858) # ChrMarine@Idle01_StepBackwardsLf01 Y
paramsX.append(0.0) # ChrMarine@Idle01_StepBackwardsRt01 X
paramsY.append(-0.349858) # ChrMarine@Idle01_StepBackwardsRt01 Y
paramsX.append(-0.0225843) # ChrMarine@Idle01_StepForwardLf01 X
paramsY.append(0.535643) # ChrMarine@Idle01_StepForwardLf01 Y
paramsX.append(0.0234374) # ChrMarine@Idle01_StepForwardRt01 X
paramsY.append(0.535643) # ChrMarine@Idle01_StepForwardRt01 Y
paramsX.append(-0.234709) # ChrMarine@Idle01_StepSidewaysLf01 X
paramsY.append(-0.010002) # ChrMarine@Idle01_StepSidewaysLf01 Y
paramsX.append(0.234693) # ChrMarine@Idle01_StepSidewaysRt01 X
paramsY.append(-0.010002) # ChrMarine@Idle01_StepSidewaysRt01 Y
for i in range(0, len(motions)):
	stateChrMarineStep.addMotion(motions[i], paramsX[i], paramsY[i])

points0 = DoubleVec()
points0.append(0) # ChrMarine@Idle01 0
points0.append(0) # ChrMarine@Idle01_StepBackwardsLf01 0
points0.append(0) # ChrMarine@Idle01_StepBackwardsRt01 0
points0.append(0) # ChrMarine@Idle01_StepForwardLf01 0
points0.append(0) # ChrMarine@Idle01_StepForwardRt01 0
points0.append(0) # ChrMarine@Idle01_StepSidewaysLf01 0
points0.append(0) # ChrMarine@Idle01_StepSidewaysRt01 0
stateChrMarineStep.addCorrespondencePoints(motions, points0)
points1 = DoubleVec()
points1.append(1.1153) # ChrMarine@Idle01 1
points1.append(0.68) # ChrMarine@Idle01_StepBackwardsLf01 1
points1.append(0.68) # ChrMarine@Idle01_StepBackwardsRt01 1
points1.append(1.02258) # ChrMarine@Idle01_StepForwardLf01 1
points1.append(1.0303) # ChrMarine@Idle01_StepForwardRt01 1
points1.append(0.821667) # ChrMarine@Idle01_StepSidewaysLf01 1
points1.append(0.824242) # ChrMarine@Idle01_StepSidewaysRt01 1
stateChrMarineStep.addCorrespondencePoints(motions, points1)
points2 = DoubleVec()
points2.append(2.165) # ChrMarine@Idle01 2
points2.append(1.32) # ChrMarine@Idle01_StepBackwardsLf01 2
points2.append(1.32) # ChrMarine@Idle01_StepBackwardsRt01 2
points2.append(1.985) # ChrMarine@Idle01_StepForwardLf01 2
points2.append(2) # ChrMarine@Idle01_StepForwardRt01 2
points2.append(1.595) # ChrMarine@Idle01_StepSidewaysLf01 2
points2.append(1.6) # ChrMarine@Idle01_StepSidewaysRt01 2
stateChrMarineStep.addCorrespondencePoints(motions, points2)

stateChrMarineStep.addTriangle("ChrMarine@Idle01", "ChrMarine@Idle01_StepBackwardsLf01", "ChrMarine@Idle01_StepSidewaysLf01")
stateChrMarineStep.addTriangle("ChrMarine@Idle01", "ChrMarine@Idle01_StepForwardLf01", "ChrMarine@Idle01_StepSidewaysLf01")
stateChrMarineStep.addTriangle("ChrMarine@Idle01", "ChrMarine@Idle01_StepBackwardsRt01", "ChrMarine@Idle01_StepSidewaysRt01")
stateChrMarineStep.addTriangle("ChrMarine@Idle01", "ChrMarine@Idle01_StepForwardRt01", "ChrMarine@Idle01_StepSidewaysRt01")
