include_directories( ${CMAKE_SOURCE_DIR}/src/SmartBody )
include_directories( ${CMAKE_SOURCE_DIR}/include/vhcl )
include_directories( ${CMAKE_SOURCE_DIR}/include/vhmsg )
include_directories( ${CMAKE_SOURCE_DIR}/include/bonebus )
include_directories( ${CMAKE_SOURCE_DIR}/include/steersuite )
include_directories( ${CMAKE_SOURCE_DIR}/include/steersuite/external )
include_directories( ${CMAKE_SOURCE_DIR}/include/steersuite/external/tinyxml )
include_directories( /usr/include/python2.7 )
include_directories( /usr/include/OGRE )


add_executable( ogresmartbody
	ogresmartbody.cpp
	ogresmartbodylistener.cpp
)

target_link_libraries( ogresmartbody

	SmartBody
	OgreMain
	OIS
	GLEW
)

set( EXEC_LIST

	ogresmartbody
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

## link with install tree:
	INSTALL_RPATH			.:${CMAKE_SOURCE_DIR}/lib	

	BUILD_WITH_INSTALL_RPATH	TRUE
)

install(

  	TARGETS ${EXEC_LIST}
	RUNTIME DESTINATION ./bin
)

