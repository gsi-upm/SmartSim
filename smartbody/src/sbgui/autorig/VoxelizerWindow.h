#ifndef _VOXELIZERWINDOW_
#define _VOXELIZERWINDOW_

#include <FL/Fl_Gl_Window.H>
#include <sr/sr_model.h>
#include <string>
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/CubicSurfaceExtractorWithNormals.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/MeshDecimator.h"
#include "PolyVoxCore/VolumeResampler.h"
#include "PolyVoxCore/AStarPathfinder.h"

template <class voxType>
void clearVoxels(PolyVox::SimpleVolume<voxType>* vol, voxType clearValue)
{
	PolyVox::SimpleVolume<voxType>& volData = *vol;
	for (int z = 0; z < volData.getDepth(); z++)
	{
		for (int y = 0; y < volData.getHeight(); y++)
		{
			for (int x = 0; x < volData.getWidth(); x++)
			{
				volData.setVoxelAt(x,y,z,clearValue);
			}
		}
	}
}

class VoxelizerWindow : public Fl_Gl_Window
{
public:
	VoxelizerWindow(int x, int y, int w, int h, char* name);
	~VoxelizerWindow();

	void initVoxelizer(SrModel* inMesh, int voxelRes);
	bool isFinishBuildVoxels();
	// just for testing....should be removed later
	void saveVoxels(std::string outFilename);

	bool    isBorderCell(SrVec3i& voxID);
	bool    isValidVoxelID(SrVec3i& voxID);
	SrVec   getVoxelCenterByID(SrVec3i& voxID);
	SrVec3i getVoxelIDFromPosition(SrVec& pos);
	SrVec getVolumeCenter() { return voxelCenter; }
	float getVolumeScale() { return voxelScale; }
	std::vector<SrVec3i> rasterizeVoxelLine(SrVec& pos1, SrVec& pos2);
	std::vector<SrVec3i> rasterizeVoxelLine(SrVec3i& voxID1, SrVec3i& voxID2);
	bool  checkVoxelLineVisible(SrVec3i& v1, SrVec3i& v2);
	std::vector<SrVec3i> getNeighborCells(SrVec3i& voxID);
	float getVoxelEuclideanDist(SrVec3i& voxID1, SrVec3i& voxID2);
	float getVoxelGeodesicDist(SrVec3i& voxID1, SrVec3i& voxID2);
	
	PolyVox::SimpleVolume<uint8_t>* getVoxels();	
	PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>* getNormalizedVoxelMesh();	
	PolyVox::SurfaceMesh<PolyVox::PositionMaterial>* getNormalizedCubicMesh();	
	virtual void draw();	
protected:
	bool finishBuildVoxels;
	bool startBuildVoxels;
	SrModel* mesh;
	PolyVox::SimpleVolume<uint8_t>* voxels; // use uint8 to tell between inside, outside, and shell
	PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>* voxelMesh;
	PolyVox::SurfaceMesh<PolyVox::PositionMaterial>* cubicMesh;
	SrVec voxelCenter;
	float voxelScale;
	int   voxelResolution;

	// generate a slice of volume from render buffer	
	void generateVoxelsParityVote();
	void generateVoxelsZBufferCarving();
	void generateSlice(int dir, int depth, void* sliceA, void* sliceB, bool readZBuffer = false);
	void drawModel();
	// using parity voting to determine the solid voxels
	void voxelVoting(int dir, int depth, unsigned char* sliceA, unsigned char* sliceB); 
	void voxelCarving(int dir, float* sliceA, float* sliceB); 	
	void voxelFindConnectedComponents(PolyVox::SimpleVolume<uint8_t>* vol);
};



#endif
