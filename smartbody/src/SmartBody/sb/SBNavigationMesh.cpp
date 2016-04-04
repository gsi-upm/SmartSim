#include <sb/SBTypes.h>
#include <sb/SBNavigationMesh.h>
#include <external/recast/Recast.h>
#include <external/recast/DetourNavMeshBuilder.h>
#include <external/recast/DetourNavMeshQuery.h>
#include <math.h>
#include <string.h>
#include <sb/SBAttribute.h>
#include <sb/SBAssetManager.h>
#include <sb/SBScene.h>

const int MaxPolys = 255;

namespace SmartBody {

SBAPI SBNavigationMesh::SBNavigationMesh()
{
	rawMesh = NULL;
	naviMesh = NULL;
	m_navMesh = NULL;
	m_navQuery = NULL;
	
	createStringAttribute("meshSource", "", true, "Navigation Mesh", 400, false, false, false, "Geometry/mesh");

}

SBAPI SBNavigationMesh::~SBNavigationMesh()
{
}


void SBNavigationMesh::cleanUp()
{
	if (rawMesh)
		delete rawMesh;
	if (naviMesh)
		delete naviMesh;
	if (m_navMesh)
		dtFreeNavMesh(m_navMesh);
	if (m_navQuery)
		dtFreeNavMeshQuery(m_navQuery);

	rawMesh = NULL;
	naviMesh = NULL;
	m_navMesh = NULL;
	m_navQuery = NULL;
}


SBAPI bool SBNavigationMesh::buildNavigationMeshFromModel( SrModel& inMesh )
{	
	if (inMesh.V.size() <= 0 || inMesh.F.size() <= 0)
	{
		LOG("buildNavigation: The number of vertices or faces is zero.");
		return false;
	}
	unsigned char* m_triareas;
	rcHeightfield* m_solid;
	rcCompactHeightfield* m_chf;
	rcContourSet* m_cset;
	rcPolyMesh* m_pmesh;
	rcConfig m_cfg;	
	rcPolyMeshDetail* m_dmesh;

	rawMesh = new SrModel(inMesh); // copy orignal mesh data
	
	rcContext* m_ctx = new rcContext();

	// hardcoded default parameters for now
	m_cellSize = 0.1f;
	m_cellHeight = 0.05f;
	m_agentHeight = 1.6f;
	m_agentRadius = 0.4f;
	m_agentMaxClimb = 0.3f;
	m_agentMaxSlope = 60.0f;
	m_regionMinSize = 8;
	m_regionMergeSize = 20;
	m_monotonePartitioning = false;
	m_edgeMaxLen = 12.0f;
	m_edgeMaxError = 1.3f;
	m_vertsPerPoly = 6.0f;
	m_detailSampleDist = 2.0f;
	m_detailSampleMaxError = 0.3f;
	// end of hardcoded default parameters
	
	// convert SrModel to navigation mesh structure
	SrBox bBox;
	inMesh.get_bounding_box(bBox);
	const float* bmin = &bBox.a[0];
	const float* bmax = &bBox.b[0];
	const float* verts = &inMesh.V.get(0)[0];
 	const int nverts = inMesh.V.size();
#if defined(__ANDROID__) || defined(SB_IPHONE)
 	//const int* tris = &inMesh.F.get(0)[0];
        // need to convert to integer array
        int* tris = new int[inMesh.F.size()*3];
	for (int i=0;i<inMesh.F.size();i++)
	{
              tris[i*3+0] = inMesh.F.get(i)[0];
              tris[i*3+1] = inMesh.F.get(i)[1];
              tris[i*3+2] = inMesh.F.get(i)[2];
        }
#else
	const int* tris = &inMesh.F.get(0)[0];
#endif
 	const int ntris = inMesh.F.size();

	bool m_keepInterResults = true;

	//
	// Step 1. Initialize build config.
	//

	// Init build configuration 
	memset(&m_cfg, 0, sizeof(m_cfg));
	m_cfg.cs = m_cellSize;
	m_cfg.ch = m_cellHeight;
	m_cfg.walkableSlopeAngle = m_agentMaxSlope;
	m_cfg.walkableHeight = (int)ceilf(m_agentHeight / m_cfg.ch);
	m_cfg.walkableClimb = (int)floorf(m_agentMaxClimb / m_cfg.ch);
	m_cfg.walkableRadius = (int)ceilf(m_agentRadius / m_cfg.cs);
	m_cfg.maxEdgeLen = (int)(m_edgeMaxLen / m_cellSize);
	m_cfg.maxSimplificationError = m_edgeMaxError;
	m_cfg.minRegionArea = (int)rcSqr(m_regionMinSize);		// Note: area = size*size
	m_cfg.mergeRegionArea = (int)rcSqr(m_regionMergeSize);	// Note: area = size*size
	m_cfg.maxVertsPerPoly = (int)m_vertsPerPoly;
	m_cfg.detailSampleDist = m_detailSampleDist < 0.9f ? 0 : m_cellSize * m_detailSampleDist;
	m_cfg.detailSampleMaxError = m_cellHeight * m_detailSampleMaxError;

	// Set the area where the navigation will be build.
	// Here the bounds of the input mesh are used, but the
	// area could be specified by an user defined box, etc.
	rcVcopy(m_cfg.bmin, bmin);
	rcVcopy(m_cfg.bmax, bmax);
	rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);

	//
	// Step 2. Rasterize input polygon soup.
	//

	// Allocate voxel heightfield where we rasterize our input data to.
	m_solid = rcAllocHeightfield();
	if (!m_solid)
	{
		LOG("buildNavigation: Out of memory 'solid'.");
		return false;
	}
	if (!rcCreateHeightfield(m_ctx, *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
	{
		LOG("buildNavigation: Could not create solid heightfield.");
		return false;
	}

	// Allocate array that can hold triangle area types.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	m_triareas = new unsigned char[ntris];
	if (!m_triareas)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", ntris);
		return false;
	}

	// Find triangles which are walkable based on their slope and rasterize them.
	// If your input data is multiple meshes, you can transform them here, calculate
	// the are type for each of the meshes and rasterize them.
	memset(m_triareas, 0, ntris*sizeof(unsigned char));
	rcMarkWalkableTriangles(m_ctx, m_cfg.walkableSlopeAngle, verts, nverts, tris, ntris, m_triareas);
	rcRasterizeTriangles(m_ctx, verts, nverts, tris, m_triareas, ntris, *m_solid, m_cfg.walkableClimb);
	
	if (!m_keepInterResults)
	{
		delete [] m_triareas;
		m_triareas = 0;
	}

	//
	// Step 3. Filter walkables surfaces.
	//

	// Once all geoemtry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	rcFilterLowHangingWalkableObstacles(m_ctx, m_cfg.walkableClimb, *m_solid);
	rcFilterLedgeSpans(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid);
	rcFilterWalkableLowHeightSpans(m_ctx, m_cfg.walkableHeight, *m_solid);


	//
	// Step 4. Partition walkable surface to simple regions.
	//

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	m_chf = rcAllocCompactHeightfield();
	if (!m_chf)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
		return false;
	}
	if (!rcBuildCompactHeightfield(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
		return false;
	}

	if (!m_keepInterResults)
	{
		rcFreeHeightField(m_solid);
		m_solid = 0;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(m_ctx, m_cfg.walkableRadius, *m_chf))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
		return false;
	}

	// (Optional) Mark areas.
	//const ConvexVolume* vols = m_geom->getConvexVolumes();
	//for (int i  = 0; i < m_geom->getConvexVolumeCount(); ++i)
	//	rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_chf);

	if (m_monotonePartitioning)
	{
		// Partition the walkable surface into simple regions without holes.
		// Monotone partitioning does not need distancefield.
		if (!rcBuildRegionsMonotone(m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build regions.");
			return false;
		}
	}
	else
	{
		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if (!rcBuildDistanceField(m_ctx, *m_chf))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
			return false;
		}

		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build regions.");
			return false;
		}
	}

	//
	// Step 5. Trace and simplify region contours.
	//

	// Create contours.
	m_cset = rcAllocContourSet();
	if (!m_cset)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
		return false;
	}
	if (!rcBuildContours(m_ctx, *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
		return false;
	}

	//
	// Step 6. Build polygons mesh from contours.
	//

	// Build polygon navmesh from the contours.
	m_pmesh = rcAllocPolyMesh();
	if (!m_pmesh)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
		return false;
	}
	if (!rcBuildPolyMesh(m_ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
		return false;
	}

	//
	// Step 7. Create detail mesh which allows to access approximate height on each polygon.
	//

	m_dmesh = rcAllocPolyMeshDetail();
	if (!m_dmesh)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmdtl'.");
		return false;
	}

	if (!rcBuildPolyMeshDetail(m_ctx, *m_pmesh, *m_chf, m_cfg.detailSampleDist, m_cfg.detailSampleMaxError, *m_dmesh))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build detail mesh.");
		return false;
	}

	if (!m_keepInterResults)
	{
		rcFreeCompactHeightfield(m_chf);
		m_chf = 0;
		rcFreeContourSet(m_cset);
		m_cset = 0;
	}	

	// At this point the navigation mesh data is ready, you can access it from m_pmesh.
	// See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.

	if (m_dmesh)
	{
		// copy navigation mesh to SrModel
		naviMesh = new SrModel();
		naviMesh->V.size(m_dmesh->nverts);
		naviMesh->F.size(m_dmesh->ntris);

		for (int i=0;i<m_dmesh->nverts;i++)
		{
			naviMesh->V[i] = SrPnt(m_dmesh->verts[i*3+0],m_dmesh->verts[i*3+1],m_dmesh->verts[i*3+2]);
		}
		//LOG("nverts = %d, ntris = %d",m_dmesh->nverts, m_dmesh->ntris);

		for (int i=0;i<m_dmesh->nmeshes;i++)
		{
			const unsigned int* m = &m_dmesh->meshes[i*4];
			const unsigned int bverts = m[0];
			const unsigned int btris = m[2];
			const int ntris = (int)m[3];	
			//LOG("bverts = %d, btris = %d, ntris = %d",bverts,btris, ntris);
			for (int j=0;j<ntris;j++)
			{
				int a,b,c;
				a = m_dmesh->tris[(j+btris)*4+0] + bverts;
				b = m_dmesh->tris[(j+btris)*4+1] + bverts;
				c = m_dmesh->tris[(j+btris)*4+2] + bverts;
				naviMesh->F[j+btris].set(a,b,c);
				//LOG("face %d, idx = %d %d %d",j+btris, a,b,c);
			}
		}

		naviMesh->computeNormals();		
		naviMesh->remove_redundant_materials();
		//naviMesh->export_iv("../../../../data/test.iv");
	}

	//
	// (Optional) Step 8. Create Detour data from Recast poly mesh.
	//

	// The GUI may allow more max points per polygon than Detour can handle.
	// Only build the detour navmesh if we do not exceed the limit.
	const int DT_VERTS_PER_POLYGON = 6;
	if (m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
	{
		unsigned char* navData = 0;
		int navDataSize = 0;

		// Update poly flags from areas.
		for (int i = 0; i < m_pmesh->npolys; ++i)
		{
			m_pmesh->flags[i] = 1;		
		}


		dtNavMeshCreateParams params;
		memset(&params, 0, sizeof(params));
		params.verts = m_pmesh->verts;
		params.vertCount = m_pmesh->nverts;
		params.polys = m_pmesh->polys;
		params.polyAreas = m_pmesh->areas;
		params.polyFlags = m_pmesh->flags;
		params.polyCount = m_pmesh->npolys;
		params.nvp = m_pmesh->nvp;
		params.detailMeshes = m_dmesh->meshes;
		params.detailVerts = m_dmesh->verts;
		params.detailVertsCount = m_dmesh->nverts;
		params.detailTris = m_dmesh->tris;
		params.detailTriCount = m_dmesh->ntris;

		// no custom data
// 		params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
// 		params.offMeshConRad = m_geom->getOffMeshConnectionRads();
// 		params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
// 		params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
// 		params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
// 		params.offMeshConUserID = m_geom->getOffMeshConnectionId();
// 		params.offMeshConCount = m_geom->getOffMeshConnectionCount();

		params.walkableHeight = m_agentHeight;
		params.walkableRadius = m_agentRadius;
		params.walkableClimb = m_agentMaxClimb;
		rcVcopy(params.bmin, m_pmesh->bmin);
		rcVcopy(params.bmax, m_pmesh->bmax);
		params.cs = m_cfg.cs;
		params.ch = m_cfg.ch;
		params.buildBvTree = false;

		if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
		{
			m_ctx->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
			return false;
		}
		m_navMesh = dtAllocNavMesh();
		if (!m_navMesh)
		{
			dtFree(navData);
			m_ctx->log(RC_LOG_ERROR, "Could not create Detour navmesh");
			return false;
		}

		dtStatus status;

		status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
		if (dtStatusFailed(status))
		{
			dtFree(navData);
			m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh");
			return false;
		}
		m_navQuery = dtAllocNavMeshQuery();
		status = m_navQuery->init(m_navMesh, 2048);
		if (dtStatusFailed(status))
		{
			m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh query");
			return false;
		}		
	}	
	return true;
}

SBAPI bool SBNavigationMesh::buildNavigationMesh( std::string meshName )
{
	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
	DeformableMesh* mesh = assetManager->getDeformableMesh(meshName);
	if (!mesh)
		return false;

	if (mesh->dMeshStatic_p.size() == 0)
		return false;
	SrModel& inMesh = mesh->dMeshStatic_p[0]->shape();
	// this is a quick hack to get the API working, but should have a more robust way 
	// to build the navigation from all meshes inside a deformable model.
	return buildNavigationMeshFromModel(inMesh);
}

SBAPI float SBNavigationMesh::queryFloorHeight( SrVec pos, SrVec searchSize )
{
	float nearest[3];
	static dtQueryFilter filter;

	dtPolyRef polyRef = 0;
	m_navQuery->findNearestPoly(&pos[0], &searchSize[0], &filter, &polyRef, nearest);
	return nearest[1]; // return y-pos
}

SrModel* SBNavigationMesh::getRawMesh()
{
	return rawMesh;

}

SrModel* SBNavigationMesh::getNavigationMesh()
{
	return naviMesh;
}

SrVec SBNavigationMesh::queryMeshPointByRayCast( SrVec& p1, SrVec& p2 )
{
	if (naviMesh)
	{
		SrLine line = SrLine(p1,p2);
		return naviMesh->intersectLine(line);
	}
	return SrVec();
}

std::vector<SrVec> SBNavigationMesh::findPath( SrVec& spos, SrVec& epos )
{
	static SrVec polyPickExt = SrVec(2,4,2);
	static dtQueryFilter filter;
	static dtPolyRef polys[MaxPolys];
	static SrVec m_straightPath[MaxPolys];
	static unsigned char m_straightPathFlags[MaxPolys];
	static dtPolyRef m_straightPathPolys[MaxPolys];
	int nPolys = 0;
	dtPolyRef startPoly = 0, endPoly = 0;

	std::vector<SrVec> pathList;	
	m_navQuery->findNearestPoly(&spos[0], &polyPickExt[0], &filter, &startPoly, 0);
	m_navQuery->findNearestPoly(&epos[0], &polyPickExt[0], &filter, &endPoly, 0);	

	if (startPoly && endPoly)
	{		
		m_navQuery->findPath(startPoly, endPoly, &spos[0], &epos[0], &filter, polys, &nPolys, MaxPolys);
		int m_nstraightPath = 0;
		if (nPolys)
		{
			SrVec epos1 = epos;
			// In case of partial path, make sure the end point is clamped to the last polygon.			
			if (polys[nPolys-1] != endPoly)
				m_navQuery->closestPointOnPoly(polys[nPolys-1], &epos[0], &epos1[0]);

			m_navQuery->findStraightPath(&spos[0], &epos1[0], polys, nPolys,
				&m_straightPath[0][0], m_straightPathFlags,
				m_straightPathPolys, &m_nstraightPath, MaxPolys, DT_STRAIGHTPATH_ALL_CROSSINGS);

			pathList.clear();
			for (int i=0;i<m_nstraightPath;i++)
				pathList.push_back(m_straightPath[i]);
		}
	}
	return pathList;
}

void SBNavigationMesh::notify( SBSubject* subject )
{

	SBAttribute* attribute = dynamic_cast<SBAttribute*>(subject);
	if (attribute)
	{
		if (attribute->getName() == "meshSource")
		{
			std::string meshName = this->getStringAttribute(attribute->getName());
			buildNavigationMesh(meshName);			
		}
	}
}

}
