def transitionSetup(preFix, statePrefix):

	stateManager = scene.getStateManager()
	if (stateManager.getTransition(statePrefix+"StartingLeft", statePrefix+"Locomotion") == None):
		transition1 = stateManager.createTransition(statePrefix+"StartingLeft", statePrefix+"Locomotion")
		transition1.setEaseInInterval(preFix+"ChrUtah_Meander01", 0.4, 0.78)
		transition1.addEaseOutInterval(preFix+"ChrUtah_StopToWalkLf01", 0.54, 0.83)
		
	if (stateManager.getTransition(statePrefix+"StartingRight", statePrefix+"Locomotion") == None):
		transition2 = stateManager.createTransition(statePrefix+"StartingRight", statePrefix+"Locomotion")
		transition2.setEaseInInterval(preFix+"ChrUtah_Meander01", 1.1, 1.5)
		transition2.addEaseOutInterval(preFix+"ChrUtah_StopToWalkRt01", 0.54, 0.83)
