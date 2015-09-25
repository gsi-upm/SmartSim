#include "vhcl.h"
#include "VoxelizerWindow.h"
#include "external/glew/glew.h"
#include <sr/sr_gl.h>
//#include <imdebug/imdebug.h>
#include <queue>

#ifndef WIN32
#define _strdup strdup
#endif

VoxelizerWindow::VoxelizerWindow( int x, int y, int w, int h, char* name ) : Fl_Gl_Window(x,y,w,h,name)
{
	startBuildVoxels = false;
	finishBuildVoxels = false;
	voxels = NULL;
	voxelMesh = NULL;
}

VoxelizerWindow::~VoxelizerWindow()
{

}

#if 0
void createSphereInVolume(PolyVox::SimpleVolume<uint8_t>& volData, float fRadius)
{
	//This vector hold the position of the center of the volume
	PolyVox::Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);

	//This three-level for loop iterates over every voxel in the volume
	for (int z = 0; z < volData.getDepth(); z++)
	{
		for (int y = 0; y < volData.getHeight(); y++)
		{
			for (int x = 0; x < volData.getWidth(); x++)
			{
				//Store our current position as a vector...
				PolyVox::Vector3DFloat v3dCurrentPos(x,y,z);	
				//And compute how far the current position is from the center of the volume
				float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

				uint8_t uVoxelValue = 0;

				//If the current voxel is less than 'radius' units from the center then we make it solid.
				if(fDistToCenter <= fRadius)
				{
					//Our new voxel value
					uVoxelValue = 1;
				}

				//Wrte the voxel value into the volume	
				volData.setVoxelAt(x, y, z, uVoxelValue);
			}
		}
	}
}
#endif


// void VoxelizerWindow::clearVoxels( PolyVox::SimpleVolume<uint8_t>* vol, uint8_t clearValue )
// {
// 	PolyVox::SimpleVolume<uint8_t>& volData = *vol;
// 	for (int z = 0; z < volData.getDepth(); z++)
// 	{
// 		for (int y = 0; y < volData.getHeight(); y++)
// 		{
// 			for (int x = 0; x < volData.getWidth(); x++)
// 			{
// 				volData.setVoxelAt(x,y,z,clearValue);
// 			}
// 		}
// 	}
// }


void VoxelizerWindow::voxelFindConnectedComponents(PolyVox::SimpleVolume<uint8_t>* vol)
{
	PolyVox::SimpleVolume<uint8_t>& volData = *vol;
	uint8_t curComponentIdx = 1;
	std::vector<int> componentCount;
	int largestComponentCount = -1, largestComponentIdx = -1;
	for (int z = 0; z < volData.getDepth(); z++)
	{
		for (int y = 0; y < volData.getHeight(); y++)
		{
			for (int x = 0; x < volData.getWidth(); x++)
			{
				if (volData.getVoxelAt(x,y,z) != 255)
					continue;
				volData.setVoxelAt(x,y,z,curComponentIdx);
				std::queue<SrVec3i> voxelQueue;
				int voxelCount = 0;
				voxelQueue.push(SrVec3i(x,y,z));
				int volDepth = volData.getDepth();
				while (!voxelQueue.empty())
				{
					SrVec3i cv = voxelQueue.front();
					voxelQueue.pop();
					voxelCount++;
					for (int a=-1;a<=1;a++)
						for (int b=-1;b<=1;b++)
							for (int c=-1;c<=1;c++)
							{
								// the same voxel
								if (a==0 && b==0 && c==0)
									continue;
								if (abs(a) == 1 && abs(b) == 1 && abs(c) == 1)
									continue;

								int ax,ay,az;
								ax = a+cv[0];
								ay = b+cv[1];
								az = c+cv[2];
								// invalid neighbor voxel
								if (ax < 0 || ax >= volDepth || 
									ay < 0 || ay >= volDepth ||
									az < 0 || az >= volDepth)
									continue;
								if (volData.getVoxelAt(ax,ay,az) == 255)
								{
									volData.setVoxelAt(ax,ay,az, curComponentIdx);
									voxelQueue.push(SrVec3i(ax,ay,az));
								}
							}
				}
				if (voxelCount > largestComponentCount)
				{
					largestComponentCount = voxelCount;
					largestComponentIdx = curComponentIdx;
				}
				componentCount.push_back(voxelCount);
				curComponentIdx++;
			}
		}
	}

	// second pass, remove smaller connected components

	for (int z = 0; z < volData.getDepth(); z++)
	{
		for (int y = 0; y < volData.getHeight(); y++)
		{
			for (int x = 0; x < volData.getWidth(); x++)
			{
				if (volData.getVoxelAt(x,y,z) == largestComponentIdx)
					volData.setVoxelAt(x,y,z, 255);
				else
					volData.setVoxelAt(x,y,z,0);
			}
		}
	}
}

void VoxelizerWindow::initVoxelizer( SrModel* inMesh, int voxelRes )
{
	mesh = new SrModel(*inMesh);
	mesh->computeNormals();
	SrBox bbox;
	float normalizeScale = 0.95f; // a bit less than 1 to avoid boundary cases	
	mesh->get_bounding_box(bbox);
	voxelCenter = bbox.getCenter();
	voxelScale = bbox.max_size()/normalizeScale;
	voxelResolution = voxelRes;
	//mesh->translate(-voxelCenter); // center the mesh
	mesh->normalize(normalizeScale*0.5f); // normalize the mesh to fit into voxel volume
	voxels = new PolyVox::SimpleVolume<uint8_t>(PolyVox::Region(PolyVox::Vector3DInt32(0,0,0), PolyVox::Vector3DInt32(voxelRes-1, voxelRes-1, voxelRes-1)));	
	// just for testing voxel output function
	//createSphereInVolume(*voxels,60.f);
	//saveVoxels("testSphere.binvox");

	startBuildVoxels = true;
	finishBuildVoxels = false;
}

void VoxelizerWindow::draw()
{
	if (!startBuildVoxels) return;


	//generateVoxelsParityVote();
	generateVoxelsZBufferCarving();

	voxelFindConnectedComponents(voxels);
	

	//Create a surface extractor. Comment out one of the following two lines to decide which type gets created.
	//PolyVox::CubicSurfaceExtractor< PolyVox::SimpleVolume<uint8_t> > surfaceExtractor(voxels, voxels->getEnclosingRegion(), &mesh);
	voxelMesh = new PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>();
	cubicMesh = new PolyVox::SurfaceMesh<PolyVox::PositionMaterial>();
	PolyVox::MarchingCubesSurfaceExtractor< PolyVox::SimpleVolume<uint8_t> > surfaceExtractorMC(voxels, voxels->getEnclosingRegion(), voxelMesh);
	PolyVox::CubicSurfaceExtractor< PolyVox::SimpleVolume<uint8_t>> surfaceExtractorCubic(voxels, voxels->getEnclosingRegion(), cubicMesh);

	//Execute the surface extractor.
	surfaceExtractorMC.execute();
	surfaceExtractorCubic.execute();

	float meshRescale = voxelScale/voxels->getDepth();
	SrVec mtran = -SrVec(0.5f,0.5f,0.5f)*voxelScale + voxelCenter;
	voxelMesh->scaleVertices(meshRescale);
	voxelMesh->translateVertices(PolyVox::Vector3DFloat(mtran[0],mtran[1],mtran[2]));

	cubicMesh->scaleVertices(meshRescale);
	cubicMesh->translateVertices(PolyVox::Vector3DFloat(mtran[0],mtran[1],mtran[2]));
	
	startBuildVoxels = false;
	finishBuildVoxels = true;
}

bool VoxelizerWindow::isFinishBuildVoxels()
{
	return finishBuildVoxels;
}


void VoxelizerWindow::generateSlice( int dir, int depth, void* sliceA, void* sliceB, bool readZBuffer )
{	
	SrVec axis[] = { SrVec(-0.5,0,0), SrVec(0,-0.5,0), SrVec(0,0,0.5) };
	SrVec upDir[] = { SrVec(0,1,0), SrVec(0,0,1), SrVec(0,1,0) };

	SrVec pos = axis[dir];
	SrVec up  = upDir[dir];

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	// generate slices in positive direction
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(pos[0], pos[1], pos[2],
		0.0, 0.0, 0.0,
		up[0],up[1],up[2]);

	int voxelDepth = voxels->getDepth();
	int voxelWidth = voxels->getWidth();
	int voxelHeight = voxels->getHeight();

	GLdouble clipPlaneDepth = (depth + 0.5f) / (float)voxelDepth;		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.5, 0.5, -0.5, 0.5, clipPlaneDepth, 1.0);
	//glDisable(GL_CULL_FACE);
	// draw back face with white color
	if (readZBuffer)
	{
		glDisable(GL_CULL_FACE);
		glColor3ub(255,255,255);
		drawModel();
	}
	else
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glColor3ub(0, 0, 255);
		drawModel();
		// then draw front face with black color
		glCullFace(GL_BACK);
		glColor3ub(0,0,0);
		drawModel();
	}


	glFlush();
	glFinish();
	if (readZBuffer)
	{
		float* zsliceA = (float*)sliceA;
		glReadPixels(0, 0, voxelWidth, voxelHeight, GL_DEPTH_COMPONENT,
			GL_FLOAT, zsliceA);
	}
	else
	{
		unsigned char* colorSliceA = (unsigned char*)sliceA;
		glReadPixels(0, 0, voxelWidth, voxelHeight, GL_BLUE,
			GL_UNSIGNED_BYTE, colorSliceA);
	}


	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	// generate slice in negative direction
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(-pos[0], -pos[1], -pos[2],
		0.0, 0.0, 0.0,
		up[0],up[1],up[2]);

	if (!readZBuffer)
		clipPlaneDepth = (voxelDepth-depth-1+0.5)/(float)voxelDepth;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.5, 0.5, -0.5, 0.5, clipPlaneDepth, 1.0);

	if (readZBuffer)
	{
		glDisable(GL_CULL_FACE);
		glColor3ub(255,255,255);
		drawModel();
	}
	else
	{
		// draw back face with white color
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glColor3ub(255, 255, 255);
		drawModel();
		// then draw front face with black color
		glCullFace(GL_BACK);
		glColor3ub(0,0,0);
		drawModel();
	}
	

	glFlush();
	glFinish();
	if (readZBuffer)
	{
		float* zsliceB = (float*)sliceB;
		glReadPixels(0, 0, voxelWidth, voxelHeight, GL_DEPTH_COMPONENT,
			GL_FLOAT, zsliceB);
	}
	else
	{
		unsigned char* colorSliceB = (unsigned char*)sliceB;
		glReadPixels(0, 0, voxelWidth, voxelHeight, GL_BLUE,GL_UNSIGNED_BYTE, colorSliceB);
	}
}

void VoxelizerWindow::drawModel()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)&mesh->V[0]);
	glNormalPointer(GL_FLOAT, 0, (GLfloat*)&mesh->N[0]);
	glDrawElements(GL_TRIANGLES, mesh->F.size()*3, GL_UNSIGNED_INT, &mesh->F[0]);
	
}

void VoxelizerWindow::voxelCarving( int dir, float* sliceA, float* sliceB )
{
	int width = voxels->getWidth();
	int height = voxels->getHeight();
	int depth = voxels->getDepth();

	int widthAxis[] = { 2, 0, 0 };
	int heightAxis[] = { 1, 2, 1};

	int widx = widthAxis[dir];
	int hidx = heightAxis[dir];

	SrVec3i voxelIdx;
	for (int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
			float zvalueA = sliceA[i*width+j]*depth;
			float zvalueB = sliceB[i*width + (width-j-1)]*depth; // flip over vertical for slice B

			if (dir == 2)
			{
				// swap value in z-direction
				float ztemp = zvalueA;
				zvalueA = zvalueB;
				zvalueB = ztemp;
			}
			
			int zFront = (int)(floor(zvalueA+0.5f)-1.f);
			int zBack = (int)(depth - floor(zvalueB+0.5f) + 1.f);
			voxelIdx[widx] = j;
			voxelIdx[hidx] = i;			
			for (int k=0;k<zFront;k++)
			{
				voxelIdx[dir] = k;
				voxels->setVoxelAt(voxelIdx[0],voxelIdx[1],voxelIdx[2],0); // carve out the empty voxels				 
			}

			for (int k=zBack;k<depth;k++)
			{
				voxelIdx[dir] = k;
				voxels->setVoxelAt(voxelIdx[0],voxelIdx[1],voxelIdx[2],0); // carve out the empty voxels	
			}
		}
	}


}

void VoxelizerWindow::voxelVoting( int dir, int depth, unsigned char* sliceA, unsigned char* sliceB )
{	
	int width = voxels->getWidth();
	int height = voxels->getHeight();

	int widthAxis[] = { 2, 0, 0 };
	int heightAxis[] = { 1, 2, 1};

	int widx = widthAxis[dir];
	int hidx = heightAxis[dir];
	
	SrVec3i voxelIdx;
	for (int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
			unsigned char valueA = sliceA[i*width+j];
			unsigned char valueB = sliceB[i*width + (width-j-1)]; // flip over vertical for slice B
			voxelIdx[widx] = j;
			voxelIdx[hidx] = i;
			if (dir == 2)
			{
				depth = voxels->getDepth()-depth-1;
			}
			voxelIdx[dir] = depth;
			uint8_t voxelVal = voxels->getVoxelAt(voxelIdx[0],voxelIdx[1],voxelIdx[2]);
			if (valueA == 255 || valueB == 255)
				voxelVal++; // increase vote count
			voxels->setVoxelAt(voxelIdx[0],voxelIdx[1],voxelIdx[2],voxelVal);

		}
	}
	
}

void VoxelizerWindow::saveVoxels( std::string outFilename )
{
	FILE* fp = fopen(outFilename.c_str(),"wb");
	fprintf(fp,"#binvox 1\n");
	fprintf(fp,"dim %d %d %d\n",voxels->getWidth(),voxels->getHeight(),voxels->getDepth());
	fprintf(fp,"translate %f %f %f\n",voxelCenter[0],voxelCenter[1],voxelCenter[2]);
	fprintf(fp,"scale %f\n",voxelScale);
	fprintf(fp,"data\n");

	int totalVoxels = voxels->getWidth()*voxels->getHeight()*voxels->getDepth();
	int count = 0;
	uint8_t voxelVal = voxels->getVoxelAt(0,0,0);
	unsigned char sizeCount = 0;
	int index = 0;
	for (int x=0;x<voxels->getWidth();x++)
		for (int z=0;z<voxels->getDepth();z++)
			for (int y=0;y<voxels->getHeight();y++)
			{
				index++;
				if (index < totalVoxels && sizeCount < 255 && voxelVal == voxels->getVoxelAt(x,y,z))
				{					
					sizeCount++;
				}
				else // write out byte
				{
					fwrite(&voxelVal,sizeof(uint8_t),1,fp);
					fwrite(&sizeCount,sizeof(unsigned char),1,fp);
					sizeCount = 1;
					voxelVal = voxels->getVoxelAt(x,y,z);
				}
			}

	fclose(fp);
}



void VoxelizerWindow::generateVoxelsZBufferCarving()
{
	int sliceSize = voxels->getWidth()*voxels->getHeight();
	std::vector<float> sliceA;
	std::vector<float> sliceB;	
	sliceA.resize(sliceSize);
	sliceB.resize(sliceSize);
	float* sliceBufferA = &sliceA[0];
	float* sliceBufferB = &sliceB[0];
	
	clearVoxels(voxels,(uint8_t)255);
	int voxelDepth = voxels->getDepth();

	glViewport(0,0,voxelDepth,voxelDepth);
	//glDisable(GL_MULTISAMPLE);

	glClearColor(0.0f,0.0f,0.0f,1.0f); // clear background to black
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE);
	glDisable(GL_LIGHTING);
	glDisable(GL_DITHER);
	glColor3ub(255, 255, 255);  // go for a more orangy look	
	//  mesh_view_p->set_use_no_colour(1);
	glEnable(GL_CULL_FACE);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(GL_BACK);
	glDrawBuffer(GL_BACK);

	for (int i=0;i<3;i++)
	//int i=2;
	{
		generateSlice(i,0,sliceBufferA,sliceBufferB, true); 
		voxelCarving(i,sliceBufferA,sliceBufferB);
		//imdebug("lum b=32f w=%d h=%d %p",voxelDepth,voxelDepth,sliceBufferA);
		//imdebug("lum b=32f w=%d h=%d %p",voxelDepth,voxelDepth,sliceBufferB);
	}
}

void VoxelizerWindow::generateVoxelsParityVote()
{
	int sliceSize = voxels->getWidth()*voxels->getHeight();
	std::vector<GLubyte> sliceA;
	std::vector<GLubyte> sliceB;	
	sliceA.resize(sliceSize);
	sliceB.resize(sliceSize);
	GLubyte* sliceBufferA = &sliceA[0];
	GLubyte* sliceBufferB = &sliceB[0];

	int voxelDepth = voxels->getDepth();

	glViewport(0,0,voxelDepth,voxelDepth);
	//glDisable(GL_MULTISAMPLE);

	glClearColor(0.0f,0.0f,0.0f,1.0f); // clear background to black
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE);
	glDisable(GL_LIGHTING);
	glDisable(GL_DITHER);
	glColor3ub(255, 255, 255);  // go for a more orangy look	
	//  mesh_view_p->set_use_no_colour(1);
	glEnable(GL_CULL_FACE);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(GL_BACK);
	glDrawBuffer(GL_BACK);

	// get x-direction slices
	for (int i=0;i<voxelDepth;i++)
	{
		generateSlice(0,i,sliceBufferA,sliceBufferB);
		voxelVoting(0,i,sliceBufferA, sliceBufferB);
		//imdebug("lum b=8 w=%d h=%d %p",voxelDepth,voxelDepth,sliceBufferA);
		//imdebug("lum b=8 w=%d h=%d %p",128,128,sliceBufferB);		
	}

	// get y-direction slices
	for (int i=0;i<voxelDepth;i++)
	{
		generateSlice(1,i,sliceBufferA,sliceBufferB);
		voxelVoting(1,i,sliceBufferA, sliceBufferB);
		//imdebug("lum b=8 w=%d h=%d %p",128,128,sliceBufferA);
		//imdebug("lum b=8 w=%d h=%d %p",128,128,sliceBufferB);		
	}

	// get z-direction slices
	for (int i=0;i<voxelDepth;i++)
	{
		generateSlice(2,i,sliceBufferA,sliceBufferB);
		voxelVoting(2,i,sliceBufferA, sliceBufferB);
	}

	// going through each voxel to keep only voxel with enough parity votes
	int numSolidVoxels = 0;
	for (int i=0;i<voxels->getWidth();i++)
		for (int j=0;j<voxels->getHeight();j++)
			for (int k=0;k<voxels->getDepth();k++)
			{
				uint8_t voxelVal = voxels->getVoxelAt(i,j,k);
				if (voxelVal > 1)
				{
					voxels->setVoxelAt(i,j,k,1);
					numSolidVoxels++;
				}
				else
					voxels->setVoxelAt(i,j,k,0);
			}
}

PolyVox::SimpleVolume<uint8_t>* VoxelizerWindow::getVoxels()
{
	return voxels;
}

PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>* VoxelizerWindow::getNormalizedVoxelMesh()
{
	return voxelMesh;
}

PolyVox::SurfaceMesh<PolyVox::PositionMaterial>* VoxelizerWindow::getNormalizedCubicMesh()
{
	return cubicMesh;
}


SrVec VoxelizerWindow::getVoxelCenterByID( SrVec3i& voxID )
{
	SrVec mtran = -SrVec(0.5f,0.5f,0.5f)*voxelScale + voxelCenter; // min point of voxel volume
	float cellSize = voxelScale/voxels->getDepth();
	SrVec voxIDFloat = SrVec(voxID[0]+0.5f,voxID[1]+0.5f,voxID[2]+0.5f);
	SrVec center = mtran + voxIDFloat*cellSize;
	return center;
}

SrVec3i VoxelizerWindow::getVoxelIDFromPosition( SrVec& pos )
{
	SrVec mtran = -SrVec(0.5f,0.5f,0.5f)*voxelScale + voxelCenter; // min point of voxel volume
	float cellSize = voxelScale/voxels->getDepth();
	SrVec voxelPos = (pos-mtran)/cellSize;
	return SrVec3i((int)voxelPos[0],(int)voxelPos[1],(int)voxelPos[2]); // return the voxel ID
}



std::vector<SrVec3i> VoxelizerWindow::getNeighborCells( SrVec3i& voxID )
{
	std::vector<SrVec3i> neighborCells;
	for (int x=-1;x<=1;x++)
	{
		for (int y=-1;y<=1;y++)
		{
			for (int z=-1;z<=1;z++)
			{
				if (x==0 && y==0 && z==0)
					continue;
				if (abs(x) + abs(y) + abs(z) > 1)
					continue;

				SrVec3i nextID = SrVec3i(x+voxID[0],y+voxID[1],z+voxID[2]);
				if (!isValidVoxelID(nextID))
					continue;
				neighborCells.push_back(nextID);
			}
		}
	}
	return neighborCells;
}

bool VoxelizerWindow::isBorderCell( SrVec3i& voxID )
{	
	uint8_t voxVal = voxels->getVoxelAt(voxID[0],voxID[1],voxID[2]);
	if (voxVal == 0) // outside voxel
		return false;
	// detect all borders
	for (int x=-1;x<=1;x++)
	{
		for (int y=-1;y<=1;y++)
		{
			for (int z=-1;z<=1;z++)
			{
				if (x==0 && y==0 && z==0)
					continue;

				SrVec3i nextID = SrVec3i(x+voxID[0],y+voxID[1],z+voxID[2]);
				if (!isValidVoxelID(nextID))
					continue;
				voxVal = voxels->getVoxelAt(voxID[0],voxID[1],voxID[2]);
				if (voxVal == 0)
					return true;
			}
		}
	}
	return false;
}

bool VoxelizerWindow::isValidVoxelID( SrVec3i& voxID )
{
	for (int i=0;i<3;i++)
	{
		int vid = voxID[i];
		if (vid < 0 || vid >= voxelResolution)
			return false;
	}
	return true;
}


std::vector<SrVec3i> VoxelizerWindow::rasterizeVoxelLine( SrVec3i& voxID1, SrVec3i& voxID2 )
{
	#define MAX(a,b) (((a)>(b))?(a):(b))
	#define ABS(a) (((a)<0) ? -(a) : (a))
	#define ZSGN(a) (((a)<0) ? -1 : (a)>0 ? 1 : 0)	
	int x1, y1, x2, y2, z1, z2;
	x1 = voxID1[0]; y1 = voxID1[1]; z1 = voxID1[2];
	x2 = voxID2[0]; y2 = voxID2[1]; z2 = voxID2[2];

	std::vector<SrVec3i> rasterLine;
	{
		int xd, yd, zd;
		int x, y, z;
		int ax, ay, az;
		int sx, sy, sz;
		int dx, dy, dz;

		dx = x2 - x1;
		dy = y2 - y1;
		dz = z2 - z1;

		ax = ABS(dx) << 1;
		ay = ABS(dy) << 1;
		az = ABS(dz) << 1;

		sx = ZSGN(dx);
		sy = ZSGN(dy);
		sz = ZSGN(dz);

		x = x1;
		y = y1;
		z = z1;

		if (ax >= MAX(ay, az))            /* x dominant */
		{
			yd = ay - (ax >> 1);
			zd = az - (ax >> 1);
			for (;;)
			{
				//point3d(x, y, z);
				rasterLine.push_back(SrVec3i(x,y,z));
				if (x == x2)
				{
					return rasterLine;
				}
				if (yd >= 0)
				{
					y += sy;
					yd -= ax;
				}
				if (zd >= 0)
				{
					z += sz;
					zd -= ax;
				}
				x += sx;
				yd += ay;
				zd += az;
			}
		}
		else if (ay >= MAX(ax, az))            /* y dominant */
		{

			xd = ax - (ay >> 1);
			zd = az - (ay >> 1);
			for (;;)
			{
				rasterLine.push_back(SrVec3i(x,y,z));
				if (y == y2)
				{
					return rasterLine;
				}
				if (xd >= 0)
				{
					x += sx;
					xd -= ay;
				}
				if (zd >= 0)
				{
					z += sz;
					zd -= ay;
				}

				y += sy;
				xd += ax;
				zd += az;
			}
		}
		else if (az >= MAX(ax, ay))            /* z dominant */
		{
			xd = ax - (az >> 1);
			yd = ay - (az >> 1);
			for (;;)
			{
				rasterLine.push_back(SrVec3i(x,y,z));
				if (z == z2)
				{
					return rasterLine;
				}
				if (xd >= 0)
				{
					x += sx;
					xd -= az;
				}
				if (yd >= 0)
				{
					y += sy;
					yd -= az;
				}
				z += sz;
				xd += ax;
				yd += ay;
			}
		}
	}
	return rasterLine;

}

std::vector<SrVec3i> VoxelizerWindow::rasterizeVoxelLine( SrVec& pos1, SrVec& pos2 )
{	
	SrVec3i voxID1 = getVoxelIDFromPosition(pos1);
	SrVec3i voxID2 = getVoxelIDFromPosition(pos2);
	return rasterizeVoxelLine(voxID1,voxID2);
}

bool VoxelizerWindow::checkVoxelLineVisible( SrVec3i& v1, SrVec3i& v2 )
{
	std::vector<SrVec3i> voxelLines = rasterizeVoxelLine(v1,v2);
	for (unsigned int i=0;i<voxelLines.size();i++)
	{
		SrVec3i& cv = voxelLines[i];
		if (voxels->getVoxelAt(cv[0],cv[1],cv[2]) == 0)
			return false;
	}
	return true;
}

float VoxelizerWindow::getVoxelEuclideanDist( SrVec3i& voxID1, SrVec3i& voxID2 )
{
	SrVec3i d = SrVec3i(voxID1[0]-voxID2[0],voxID1[1]-voxID2[1],voxID1[2]-voxID2[2]); 
	float dist = sqrtf((float)d[0]*d[0] + (float)d[1]*d[1] + (float)d[2]*d[2]);
	return dist;
}


bool aStarVoxelInsideValidator(const PolyVox::SimpleVolume<uint8_t>* volData, const PolyVox::Vector3DInt32& v3dPos)
{
	//Voxels are considered valid candidates for the path if they are inside the volume...
	if(volData->getVoxelAt(v3dPos) == 0)
	{
		return false;
	}
	return true;
}


float VoxelizerWindow::getVoxelGeodesicDist( SrVec3i& voxID1, SrVec3i& voxID2 )
{
	PolyVox::Vector3DInt32 v1 = PolyVox::Vector3DInt32(voxID1[0],voxID1[1],voxID1[2]);
	PolyVox::Vector3DInt32 v2 = PolyVox::Vector3DInt32(voxID2[0],voxID2[1],voxID2[2]);
	std::list<PolyVox::Vector3DInt32> geoPath;
	PolyVox::AStarPathfinderParams< PolyVox::SimpleVolume<uint8_t> > params(voxels,v1,v2,&geoPath,0.f,10000,PolyVox::SixConnected,&aStarVoxelInsideValidator,0);	
	PolyVox::AStarPathfinder< PolyVox::SimpleVolume<uint8_t> > pathFinder(params);
	LOG("Before A* path finder");
	pathFinder.execute();
	LOG("After A* path finder");
	
	return (float)params.result->size();	

}
