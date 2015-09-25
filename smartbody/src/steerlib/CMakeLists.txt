add_definitions( -DENABLE_GUI )

find_package(OpenGL REQUIRED)

include_directories ( "." )
include_directories ( "../../include/steersuite" )
include_directories ( "../../include/steersuite/external" )
include_directories ( "../../include/steersuite/external/tinyxml" )

add_library(
        steerlib
        SHARED

	./AgentMetricsCollector.cpp
	./CompositeTechnique02.cpp
	./PerformanceProfiler.cpp
	./StateMachine.cpp
	./BenchmarkEngine.cpp
	./DrawLib.cpp                     
	./RecFilePlayerModule.cpp         
	./SteeringCommand.cpp
	./BoxObstacle.cpp                 
	./DynamicLibrary.cpp              
	./RecFileReader.cpp               
	./TestCasePlayerModule.cpp
	./Camera.cpp                      
	./GridDatabase2D.cpp              
	./RecFileWriter.cpp               
	./TestCaseReader.cpp
	./CircleObstacle.cpp              
	./HighResCounter.cpp              
	./SimulationEngine.cpp            
	./TestCaseReaderPrivate.cpp
	./Clock.cpp                       
	./MemoryMapper.cpp                
	./SimulationMetricsCollector.cpp  
	./TestCaseWriter.cpp
	./CommandLineParser.cpp           
	./Misc.cpp                        
	./SimulationOptions.cpp           
	./ThreadedTaskManager.cpp
	./CompositeTechnique01.cpp       
	./OrientedBoxObstacle.cpp         
	./SimulationRecorderModule.cpp    
	./XMLParser.cpp

	../external/tinyxml/ticpp.cpp
	../external/tinyxml/tinyxml.cpp
	../external/tinyxml/tinyxmlparser.cpp
	../external/tinyxml/tinystr.cpp
	../external/tinyxml/tinyxmlerror.cpp
)


set( EXEC_LIST

       steerlib
)


target_link_libraries( steerlib
	${OPENGL_LIBRARIES}
	dl
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

