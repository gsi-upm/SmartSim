def femaleTransitionSetup(prefix, statePreFix):
	#mirrorMotion = scene.getMotion("ChrHarmony@Idle01_ToWalkRt01")
	#mirrorMotion.mirror("ChrHarmony@Idle01_ToWalkLf01", "ChrHarmony.sk")
	
	stateManager = scene.getStateManager()
	'''
	transition1 = stateManager.createTransition(statePreFix + "StartingLeft", statePreFix + "Locomotion")
	transition1.setEaseInInterval(prefix + "ChrHarmony@Walk01", 0.38, 1.06)
	transition1.addEaseOutInterval(prefix + "ChrHarmony@Idle01_ToWalkLf01", 0.94, 1.65)

	transition2 = stateManager.createTransition(statePreFix + "StartingRight", statePreFix + "Locomotion")
	transition1.setEaseInInterval(prefix + "ChrHarmony@Walk01", 1.06, 1.70)
	transition1.addEaseOutInterval(prefix + "ChrHarmony@Idle01_ToWalkRt01",  0.94, 1.65)
	'''
	transition1 = stateManager.createTransition(statePreFix + "StartingLeft", statePreFix + "Locomotion")
	transition1.setEaseInInterval("ChrHarmony@Walk01", 0.78, 1.40)
	transition1.addEaseOutInterval("ChrHarmony@Idle01_ToWalk01_Turn180Lf01", 2.06, 2.59) 
	transition2 = stateManager.createTransition(statePreFix + "StartingRight", statePreFix + "Locomotion")
	transition2.setEaseInInterval( "ChrHarmony@Walk01", 1.40, 2.06)
	#transition2.addEaseOutInterval("ChrHarmony@Idle01_ToWalkRt01", 1.46, 2.06)
	transition2.addEaseOutInterval("ChrHarmony@Idle01_ToWalk01_Turn180Rt01", 2.06, 2.59) 

