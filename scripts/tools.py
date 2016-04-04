class Tools():

	def string2vec (self, stvec):

		'''Converts String to SrVec'''

		posX = int (stvec[stvec.index('(')+1:stvec.index(',')])
		posZ = int (stvec[stvec.index(',')+1:stvec.index(')')])
	
		return SrVec(posX, 0, posZ)
	
	def vec2str(self, vec):

		''' Converts SrVec to string '''

		x = vec.getData(0)
		y = vec.getData(2)
		z = vec.getData(2)
		if -0.0001 < x < 0.0001: x = 0
		if -0.0001 < y < 0.0001: y = 0
		if -0.0001 < z < 0.0001: z = 0

		return "" + str(x) + " " + str(y) + ""

	def distance (self, p1, p2):

		'''Calculates the distance between two points'''

		x = (p1.getData(0) - p2.getData(0))
		x = x*x
		y = (p1.getData(2) - p2.getData(2))
		y = y*y

		return math.sqrt(x+y)

	def xFromVec (self, vec):

		return int (vec[vec.index('(')+1:vec.index(',')])

	def yFromVec (self, vec):
	
		return int (vec[vec.index(',')+1:vec.index(')')])
