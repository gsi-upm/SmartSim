#mirrorMotion = scene.getMotion("ChrUsaMleAdult@Idle01_ToWalkRt01")
#mirrorMotion.mirror("ChrUsaMleAdult@Idle01_ToWalkLf01", "ChrGarza.sk")

stateManager = scene.getStateManager()

# transition1 = stateManager.createTransition("ChrGarzaStartingLeft", "ChrGarzaLocomotion")
# transition1.addCorrespondancePoint("ChrUsaMleAdult@Idle01_ToWalkLf01", "ChrUsaMleAdult@Walk01", 0.54, 0.85, 0.00, 0.2)
# transition2 = stateManager.createTransition("ChrGarzaStartingRight", "ChrGarzaLocomotion")
# transition2.addCorrespondancePoint("ChrUsaMleAdult@Idle01_ToWalkRt01", "ChrUsaMleAdult@Walk01", 0.54, 0.9, 0.74, 0.93)

walkRightTransitionIn = stateManager.createTransition("mocapStartingRight","mocapLocomotion")
walkRightTransitionIn.setEaseInInterval("ChrMarine@Walk01", 1.30,1.56)
#walkRightTransitionIn.addEaseOutInterval("ChrMarine@Idle01_ToWalkLf01",1.34,1.56)
walkRightTransitionIn.addEaseOutInterval("ChrMarine@Idle01_ToWalk01_Turn180Rt01",2.06,2.45)


walkLeftTransitionIn = stateManager.createTransition("mocapStartingLeft","mocapLocomotion")
walkLeftTransitionIn.setEaseInInterval("ChrMarine@Walk01", 0.75,1.08)
walkLeftTransitionIn.addEaseOutInterval("ChrMarine@Idle01_ToWalk01_Turn180Lf01",2.06,2.45)
#walkLeftTransitionIn.addEaseOutInterval("ChrMarine@Idle01_ToWalk01",1.33,1.56)


#jumpTransitionIn = stateManager.createTransition("ChrMarineLocomotion", "ChrMarineRunJumpState")
#jumpTransitionIn.setEaseInInterval("ChrBrad_ChrMarine@Run01_JumpHigh01", 0.00,0.2)
#jumpTransitionIn.addEaseOutInterval("ChrBrad_ChrMarine@Run01",0.47,0.67)

#jumpTransitionOut = stateManager.createTransition( "ChrMarineRunJumpState","ChrMarineLocomotion")
#jumpTransitionOut.setEaseInInterval("ChrBrad_ChrMarine@Run01",0.33,0.5)
#jumpTransitionOut.addEaseOutInterval("ChrBrad_ChrMarine@Run01_JumpHigh01",1.10,1.35);
