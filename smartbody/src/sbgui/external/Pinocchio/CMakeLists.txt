cmake_minimum_required (VERSION 2.8)

project(Pinocchio)


include_directories( . )

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   add_definitions(-fPIC)
endif()


set ( SRC
	./attachment.cpp
	./discretization.cpp	
	./embedding.cpp	
	./graphutils.cpp	
	./indexer.cpp	
	./intersector.cpp	
	./lsqSolver.cpp	
	./matrix.cpp	
	./mesh.cpp	
	./Pinocchio.cpp	
	./pinocchioApi.cpp	
	./refinement.cpp	
	./skeleton.cpp	
	./PinnocchioCallback.cpp	
)

add_library( pinocchio
   STATIC 
   ${SRC}
)

