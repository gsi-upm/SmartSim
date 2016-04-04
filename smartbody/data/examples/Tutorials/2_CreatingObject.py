print "|--------------------------------------------|"
print "|         Starting Tutorial 1	            |"
print "|--------------------------------------------|"

print 'media path = ' + scene.getMediaPath()
print 'num of pawns in the scene = ' + str(scene.getNumPawns())
print 'num of characters in the scene = ' + str(scene.getNumCharacters())

obj = scene.createPawn('obj1')
obj.setStringAttribute('collisionShape','box')
obj.setVec3Attribute('collisionShapeScale',5.0,10.0,3.0)
obj.setPosition(SrVec(0,30,0))
obj.setHPR(SrVec(0,0,90))


