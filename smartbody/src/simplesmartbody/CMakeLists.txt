
include_directories( /usr/include/python2.7 )
include_directories( ../SmartBody )
include_directories( ../../include/vhcl )
include_directories( ../../include/bonebus )
include_directories( ../../include/steersuite )
include_directories( ../../include/steersuite/external )
include_directories( ../../include/steersuite/external/tinyxml )


set( SIMPLESMARTBODY_SRC

	simplesmartbody.cpp 
)

add_executable( simplesmartbody 

	${SIMPLESMARTBODY_SRC}
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
find_library (GLEW GLEW)
set( MORE_LIBS
	${GLEW}
)
else()
set( MORE_LIBS
	GLEW
)
endif()

target_link_libraries( simplesmartbody 

	SmartBody
	pthread
	${MORE_LIBS}
#	tcmalloc
#	profiler
#	${CMAKE_REQUIRED_LIBRARIES} 
)

set( EXEC_LIST

	simplesmartbody
)

set( APP_LIBRARY_BINARY_PATHS

        ../bin
)
	
set( APP_LIBRARY_SOURCE_PATHS

        ${PROJECT_SOURCE_DIR}
)
	


set_target_properties(

        ${EXEC_LIST}

	PROPERTIES

	## link with install tree:
	INSTALL_RPATH                   .
	BUILD_WITH_INSTALL_RPATH        TRUE
)

install( 
	TARGETS ${EXEC_LIST}
	RUNTIME DESTINATION ./bin
)
