
def femaleLocomotionSetup(skeletonName, baseJoint, prefix, statePreFix):
	print "** State: harmonyLocomotion"
	
	stateManager = scene.getStateManager()
	stateName = statePreFix + "Locomotion"
	if (stateManager.getBlend(stateName) != None): # don't create duplicate state
		return		
	state = stateManager.createState3D(stateName)
	state.setBlendSkeleton('ChrHarmony.sk')

	# add motions
	motions = StringVec()
	motions.append(prefix + "ChrHarmony@Walk01")	
	motions.append(prefix + "ChrHarmony@IdleHandOnHip01")
	#motions.append(prefix + "ChrHarmony@Meander01")
	#motions.append(prefix + "ChrHarmony@Shuffle01")
	#motions.append(prefix + "ChrHarmony@Jog01")
	#motions.append(prefix + "ChrHarmony@Run01")
	motions.append(prefix + "ChrHarmony@WalkCircleLf01")
	motions.append(prefix + "ChrHarmony@WalkCircleRt01")
	motions.append(prefix + "ChrHarmony@WalkTightCircleLf01")
	motions.append(prefix + "ChrHarmony@WalkTightCircleRt01")
	#motions.append(prefix + "ChrHarmony@RunCircleLf01")
	#motions.append(prefix + "ChrHarmony@RunCircleRt01")
	#motions.append(prefix + "ChrHarmony@RunTightCircleLf01")
	#motions.append(prefix + "ChrHarmony@RunTightCircleRt01")
	motions.append(prefix + "ChrHarmony@StrafeSlowRt01")
	motions.append(prefix + "ChrHarmony@StrafeSlowLf01")
	#motions.append(prefix + "ChrHarmony@StrafeFastRt01")
	#motions.append(prefix + "ChrHarmony@StrafeFastLf01")
	for i in range(0, len(motions)):
		state.addMotion(motions[i], 0, 0, 0)


	# correspondance points
	#floatarray = ([0, 0.811475, 1.34262, 2.13333, 0, 0.698851, 1.37931, 2.13333, 0, 0.82023, 1.30207, 2.12966, 0, 0.812874, 1.35356, 2.12966, 0, 0.988525, 1.87377, 3.06667, 0, 0.713115, 1.29836, 2.13333, 0, 0.501639, 0.92459, 1.6, 0, 0.422951, 0.772131, 1.33333, 0, 0.840984, 1.39672, 2.13333, 0, 0.840984, 1.29836, 2.13333, 0, 0.845902, 1.30328, 2.13333, 0, 0.880328, 1.33279, 2.13333, 0, 0.437705, 0.811475, 1.33333, 0, 0.452459, 0.791803, 1.33333, 0, 0.462295, 0.757377, 1.33333, 0, 0.452459, 0.796721, 1.33333, 0, 0.90000, 1.41000, 2.13000, 0.72, 1.38, 1.92, 0.72, 0, 0.473684, 0.920079, 1.6, 0.4, 0.893233, 1.28421, 0.4])
	
	#floatarray = ([0, .74, 1.33, 2.13, 0, 0.7, 1.35, 2.13, 0.65, 1.31, 1.78, 2.78, 0, .65, 1.26, 2.13, 0.43, 1.03, 1.33, 2.03, 0, .55, .9, 1.6, 0.65, 1.33, 1.87, 2.78, 0.0, .82, 1.33, 2.13])
	
	floatarray = ([0, .98, 1.65, 2.63, 0, 0.7, 1.35, 2.13, 0.71, 1.62, 2.22, 3.38, 0, 0.89, 1.61, 2.66, 0.66, 1.60, 2.29, 3.33, 0, 0.92, 1.61, 2.67, 0.00, 1.00, 1.6, 2.67, 0.65, 1.7, 2.38, 3.31])
	
	
	
	numCorrespondancePoints = 4
	if (len(floatarray) != 4 * len(motions)):
		print "**Correspondance points input wrong"
		
	for i in range(0, numCorrespondancePoints):
		points = DoubleVec()
		for j in range(0, len(motions)):
			points.append(floatarray[j * numCorrespondancePoints + i])
		state.addCorrespondancePoints(motions, points)
		
	# reset parameters (because it needs context of correspondance points, extract parameters from motion)
	skeleton = scene.getSkeleton(skeletonName)
	joint = skeleton.getJointByName(baseJoint)

	#travelDirection = ([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 90, -90, 90, -90])
	travelDirection = ([0, 0, 0, 0, 0, 0, 90, -90])
	for i in range(0, len(motions)):
		motion = scene.getMotion(motions[i])
		motion.connect(skeleton)
		correspondancePoints = state.getCorrespondancePoints(i)
		lenCorrespondancePoints = len(correspondancePoints)
		speed = motion.getJointSpeed(joint, correspondancePoints[0], correspondancePoints[lenCorrespondancePoints - 1])
		omega = motion.getJointAngularSpeed(joint, correspondancePoints[0], correspondancePoints[lenCorrespondancePoints - 1])
		direction = travelDirection[i]
		state.setParameter(motions[i], speed, omega, direction)
		motion.disconnect()
										 
	# add tetrahedrons (need automatic way to generate)
	#state.addTetrahedron(prefix + "ChrHarmony@IdleHandOnHip01", prefix + "ChrHarmony@Walk01", prefix + "ChrHarmony@WalkCircleLf01", prefix + "ChrHarmony@WalkTightCircleLf01")
	#state.addTetrahedron(prefix + "ChrHarmony@IdleHandOnHip01", prefix + "ChrHarmony@Walk01", prefix + "ChrHarmony@WalkCircleRt01", prefix + "ChrHarmony@WalkTightCircleRt01")
	
	state.addTetrahedron(prefix + "ChrHarmony@Walk01", prefix + "ChrHarmony@StrafeSlowLf01", prefix + "ChrHarmony@WalkCircleLf01", prefix + "ChrHarmony@WalkTightCircleLf01")
	state.addTetrahedron(prefix + "ChrHarmony@Walk01", prefix + "ChrHarmony@StrafeSlowRt01", prefix + "ChrHarmony@WalkCircleRt01", prefix + "ChrHarmony@WalkTightCircleRt01")
	
	state.addTetrahedron(prefix + "ChrHarmony@IdleHandOnHip01", prefix + "ChrHarmony@Walk01", prefix + "ChrHarmony@StrafeSlowLf01", prefix + "ChrHarmony@WalkTightCircleLf01")
	state.addTetrahedron(prefix + "ChrHarmony@IdleHandOnHip01", prefix + "ChrHarmony@Walk01", prefix + "ChrHarmony@StrafeSlowRt01", prefix + "ChrHarmony@WalkTightCircleRt01")
	
	#state.addTetrahedron(prefix + "ChrHarmony@IdleHandOnHip01", prefix + "ChrHarmony@StrafeSlowLf01", prefix + "ChrHarmony@WalkCircleLf01", prefix + "ChrHarmony@WalkTightCircleLf01")
	#state.addTetrahedron(prefix + "ChrHarmony@IdleHandOnHip01", prefix + "ChrHarmony@StrafeSlowRt01", prefix + "ChrHarmony@WalkCircleRt01", prefix + "ChrHarmony@WalkTightCircleRt01")
	
	#state.addTetrahedron(prefix + "ChrHarmony@Walk01", prefix + "ChrHarmony@StrafeSlowLf01", prefix + "ChrHarmony@WalkCircleLf01", prefix + "ChrHarmony@IdleHandOnHip01")
	#state.addTetrahedron(prefix + "ChrHarmony@Walk01", prefix + "ChrHarmony@StrafeSlowRt01", prefix + "ChrHarmony@WalkCircleRt01", prefix + "ChrHarmony@IdleHandOnHip01")
	
	state.addTetrahedron(prefix + "ChrHarmony@Walk01", prefix + "ChrHarmony@StrafeSlowLf01", prefix + "ChrHarmony@WalkCircleRt01", prefix + "ChrHarmony@WalkTightCircleRt01")
	state.addTetrahedron(prefix + "ChrHarmony@Walk01", prefix + "ChrHarmony@StrafeSlowRt01", prefix + "ChrHarmony@WalkCircleLf01", prefix + "ChrHarmony@WalkTightCircleLf01")
	
	state.addTetrahedron(prefix + "ChrHarmony@Walk01", prefix + "ChrHarmony@StrafeSlowLf01", prefix + "ChrHarmony@WalkCircleRt01", prefix + "ChrHarmony@IdleHandOnHip01")
	state.addTetrahedron(prefix + "ChrHarmony@Walk01", prefix + "ChrHarmony@StrafeSlowRt01", prefix + "ChrHarmony@WalkCircleLf01", prefix + "ChrHarmony@IdleHandOnHip01")
	
	#state.addTetrahedron(prefix + "ChrHarmony@Walk01", prefix + "ChrHarmony@StrafeSlowLf01", prefix + "ChrHarmony@WalkTightCircleRt01", prefix + "ChrHarmony@IdleHandOnHip01")
	#state.addTetrahedron(prefix + "ChrHarmony@Walk01", prefix + "ChrHarmony@StrafeSlowRt01", prefix + "ChrHarmony@WalkTightCircleLf01", prefix + "ChrHarmony@IdleHandOnHip01")
	
