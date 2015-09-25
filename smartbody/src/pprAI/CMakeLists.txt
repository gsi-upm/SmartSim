include_directories( "." )
include_directories( ../../include/steersuite )
include_directories( ../../include/steersuite/external )
include_directories( ../../include/steersuite/external/tinyxml )


add_library(
        pprAI
        SHARED
	
	./PPRAIModule.cpp
	./PPRAgent.cpp
)


set( EXEC_LIST

       pprAI
)

target_link_libraries( pprAI

	steerlib
)


set_target_properties(
	${EXEC_LIST}
	PROPERTIES
	## link with install tree:
	INSTALL_RPATH                   "${APP_LIBRARY_SOURCE_PATHS}"
	BUILD_WITH_INSTALL_RPATH        TRUE
)

install(
  	TARGETS ${EXEC_LIST}
	DESTINATION ./bin

)

