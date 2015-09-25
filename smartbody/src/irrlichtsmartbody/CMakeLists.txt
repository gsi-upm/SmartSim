include_directories( ../SmartBody )
include_directories( ../../include/vhcl )
include_directories( ../../include/vhmsg )
include_directories( ../../include/bonebus )
include_directories( ../../include/steersuite )
include_directories( ../../include/steersuite/external )
include_directories( ../../include/steersuite/external/tinyxml )
include_directories( /usr/include/python2.7 )
include_directories( /usr/local/include/irrlicht)


add_executable( irrlichtsmartbody
	main.cpp
	irrlichtsmartbodylistener.cpp
)

target_link_libraries( irrlichtsmartbody

	SmartBody
	Irrlicht
	GLEW
)

set( EXEC_LIST

	irrlichtsmartbody
)

set( APP_LIBRARY_BINARY_PATHS

	./bin
)

set( APP_LIBRARY_SOURCE_PATHS

	./bin
)

set_target_properties( 

	${EXEC_LIST} 

	PROPERTIES 

## link with binary tree:
#	INSTALL_RPATH				.

## link with install tree:
	INSTALL_RPATH			.	

	BUILD_WITH_INSTALL_RPATH	TRUE
)

install(

  	TARGETS ${EXEC_LIST}
	RUNTIME DESTINATION ./bin
)


