print "Init Steer Agent"
def setupSteerAgent(charName, prefix):
	steerManager = scene.getSteerManager()
	steerManager.removeSteerAgent(charName)
	steerAgent = steerManager.createSteerAgent(charName)
	steerAgent.setSteerStateNamePrefix(prefix)
	steerAgent.setSteerType("example")
	sbCharacter = scene.getCharacter(charName)
	


