cmake_minimum_required (VERSION 2.8)

project(polyvox)

add_definitions(  -std=c++11 )

include_directories(./library/PolyVoxCore/include)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   add_definitions(-fPIC)
endif()


set ( SRC
   ./library/PolyVoxCore/source/Impl/MarchingCubesTables.cpp
   ./library/PolyVoxCore/source/Impl/RandomUnitVectors.cpp
   ./library/PolyVoxCore/source/Impl/RandomVectors.cpp
   ./library/PolyVoxCore/source/Impl/Utility.cpp
   ./library/PolyVoxCore/source/ArraySizes.cpp
   ./library/PolyVoxCore/source/AStarPathfinder.cpp
   ./library/PolyVoxCore/source/Log.cpp
   ./library/PolyVoxCore/source/MeshDecimator.cpp
   ./library/PolyVoxCore/source/Region.cpp
   ./library/PolyVoxCore/source/SimpleInterface.cpp
   ./library/PolyVoxCore/source/VertexTypes.cpp
)

add_library( polyvoxcore
   STATIC 
   ${SRC}
)

