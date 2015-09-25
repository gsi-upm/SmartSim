/*  This file is part of the Pinocchio automatic rigging library.
    Copyright (C) 2007 Ilya Baran (ibaran@mit.edu)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "pinocchioApi.h"
#include "PinnocchioCallback.h"
#include <fstream>
#include <ctime>


PinocchioOutput autorig(const Skeleton &given, const Mesh &m, bool computeSkinWeights)
{
    int i;
    PinocchioOutput out;

    Mesh newMesh = prepareMesh(m);

    if(newMesh.vertices.size() == 0)
        return out;

    TreeType *distanceField = constructDistanceField(newMesh);

    //discretization
    vector<Sphere> medialSurface = sampleMedialSurface(distanceField);

    vector<Sphere> spheres = packSpheres(medialSurface);

    PtGraph graph = connectSamples(distanceField, spheres);

    //discrete embedding
    vector<vector<int> > possibilities = computePossibilities(graph, spheres, given);

    //constraints can be set by respecifying possibilities for skeleton joints:
    //to constrain joint i to sphere j, use: possiblities[i] = vector<int>(1, j);

    vector<int> embeddingIndices = discreteEmbed(graph, spheres, given, possibilities);

    if(embeddingIndices.size() == 0) { //failure
        delete distanceField;
        return out;
    }

    vector<Vector3> discreteEmbedding = splitPaths(embeddingIndices, graph, given);

    //continuous refinement
    vector<Vector3> medialCenters(medialSurface.size());
    for(i = 0; i < (int)medialSurface.size(); ++i)
        medialCenters[i] = medialSurface[i].center;

    out.embedding = refineEmbedding(distanceField, medialCenters, discreteEmbedding, given);

	if (computeSkinWeights)
	{
		//attachment
		VisTester<TreeType> *tester = new VisTester<TreeType>(distanceField);
		out.attachment = new Attachment(newMesh, given, out.embedding, tester);
		//cleanup
		delete tester;
	}    

    delete distanceField;
    return out;
}

Vector3 PINOCCHIO_API barycentricCoord(Vector3& pt, Tri3Object& tri)
{
	float w1,w2,w3;
	Vector3 ptToV1 = tri.v1 - pt;
	Vector3 ptToV2 = tri.v2 - pt;
	Vector3 ptToV3 = tri.v2 - pt;
	Vector3 cross1 = ptToV1%ptToV2;
	double area1 = cross1.length();
	Vector3 cross2 = ptToV2%ptToV3;
	double area2 = cross2.length();
	Vector3 cross3 = ptToV3%ptToV1;
	double area3 = cross3.length();
	double totalArea = area1 + area2 + area3;
	if (totalArea == 0)
	{
		w1 = 1.0;
		w2 = 0.0;
		w3 = 0.0;		
	}
	else
	{
		w1 = (float)area2 / (float)totalArea;
		w2 = (float)area3 / (float)totalArea;
		w3 = (float)area1 / (float)totalArea;
	}
	return Vector3(w1,w2,w3);
}


PinocchioOutput PINOCCHIO_API autoRigSkinWeightOnly( const Skeleton &given, Mesh &voxelMesh, Mesh& origMesh, std::vector<Vector3>& inputSkeleton )
{
	int i;
	PinocchioOutput out;

	Mesh newMesh = prepareMesh(voxelMesh);

	if(newMesh.vertices.size() == 0)
		return out;

	TreeType *distanceField = constructDistanceField(newMesh);

	
	out.embedding = inputSkeleton;
	//out.embedding = refineEmbedding(distanceField, medialCenters, discreteEmbedding, given);
	//out.embedding.resize()
	
	
	{
		//attachment for voxel mesh
		VisTester<TreeType> *tester = new VisTester<TreeType>(distanceField);
		Attachment* voxelAttachment = new Attachment(newMesh ,given, out.embedding, tester);
		out.attachment = new Attachment();


		vector<Tri3Object> triobjvec;
		for(int i = 0; i < (int)voxelMesh.edges.size(); i += 3) {
			Vector3 v1 = voxelMesh.vertices[voxelMesh.edges[i].vertex].pos;
			Vector3 v2 = voxelMesh.vertices[voxelMesh.edges[i + 1].vertex].pos;
			Vector3 v3 = voxelMesh.vertices[voxelMesh.edges[i + 2].vertex].pos;
			triobjvec.push_back(Tri3Object(v1, v2, v3,i/3));
		}

		ObjectProjector<3, Tri3Object> proj(triobjvec);
		vector<Vector<double, -1> >& meshWeights = out.attachment->getAllWeights();	
		meshWeights.resize(origMesh.vertices.size());
		printf("mesh weights size = %d\n",meshWeights.size());
		Tri3Object projTri;
		int bones = given.fGraph().verts.size() - 1;
		for (int i=0;i<origMesh.vertices.size();i++)
		{
			MeshVertex& vtx = origMesh.vertices[i];
			Vector<double, -1>& origWeight = meshWeights[i];
			origWeight[bones - 1] = 0.f;
			for (int k=0;k<bones;k++) // set to zero
				origWeight[k] = 0.f;

			Vector3 projPos = proj.projectObj(vtx.pos,projTri);	
			Vector3 baryW = barycentricCoord(projPos, projTri);	
			for (int v=0;v<3;v++)
			{
				int vidx = voxelMesh.edges[projTri.triIdx*3+v].vertex;
				Vector<double, -1> voxelWeight = voxelAttachment->getWeights(vidx);
				for (int k=0;k<origWeight.size();k++)
				{				
					origWeight[k] += voxelWeight[k]*baryW[v];		
					//printf("voxel weight[%d] = %f\n", k,voxelWeight[k]);
				}
			}	
		}
		//cleanup
		delete tester;
		delete voxelAttachment;
	}		
	delete distanceField;
	return out;	
}

PinocchioOutput PINOCCHIO_API autorigVoxelTransfer( const Skeleton &given, Mesh &voxelMesh, Mesh& origMesh, bool computeSkinWeights )
{

	int i;
	PinocchioOutput out;

	PinnocchioCallBackManager& callbackManager = PinnocchioCallBackManager::singleton();
	Mesh newMesh = prepareMesh(voxelMesh);

	if(newMesh.vertices.size() == 0)
		return out;

	TreeType *distanceField = constructDistanceField(newMesh);
	callbackManager.runCallBack();
	//discretization
	vector<Sphere> medialSurface = sampleMedialSurface(distanceField);
	callbackManager.runCallBack();

	vector<Sphere> spheres = packSpheres(medialSurface);
	callbackManager.runCallBack();

	PtGraph graph = connectSamples(distanceField, spheres);
	callbackManager.runCallBack();
	//discrete embedding
	vector<vector<int> > possibilities = computePossibilities(graph, spheres, given);
	callbackManager.runCallBack();
	//constraints can be set by respecifying possibilities for skeleton joints:
	//to constrain joint i to sphere j, use: possiblities[i] = vector<int>(1, j);

	vector<int> embeddingIndices = discreteEmbed(graph, spheres, given, possibilities);
	callbackManager.runCallBack();

	if(embeddingIndices.size() == 0) { //failure
		delete distanceField;
		return out;
	}

	vector<Vector3> discreteEmbedding = splitPaths(embeddingIndices, graph, given);
	callbackManager.runCallBack();
	//continuous refinement
	vector<Vector3> medialCenters(medialSurface.size());
	for(i = 0; i < (int)medialSurface.size(); ++i)
		medialCenters[i] = medialSurface[i].center;

	out.embedding = refineEmbedding(distanceField, medialCenters, discreteEmbedding, given);
	
	PinnocchioCallBack* callBack = callbackManager.getCallBack();
	if (callBack)
	{
		std::vector<Vector3> outSk(out.embedding.size());
		for (unsigned int i=0;i<outSk.size();i++)
		{
			outSk[i] = (out.embedding[i] - voxelMesh.toAdd) / voxelMesh.scale;
		}

		callBack->skeletonCompleteCallBack(outSk);

		std::clock_t start;
		double duration = 0.f;

		start = std::clock();
		while (duration < 0.1f) // delay 1sec to show the skeleton
		{
			duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
			callbackManager.runCallBack();
		}
	}
	callbackManager.runCallBack();

	if (computeSkinWeights)
	{
		//attachment for voxel mesh
		VisTester<TreeType> *tester = new VisTester<TreeType>(distanceField);
		Attachment* voxelAttachment = new Attachment(newMesh, given, out.embedding, tester);
		out.attachment = new Attachment();


		vector<Tri3Object> triobjvec;
		for(int i = 0; i < (int)voxelMesh.edges.size(); i += 3) {
			Vector3 v1 = voxelMesh.vertices[voxelMesh.edges[i].vertex].pos;
			Vector3 v2 = voxelMesh.vertices[voxelMesh.edges[i + 1].vertex].pos;
			Vector3 v3 = voxelMesh.vertices[voxelMesh.edges[i + 2].vertex].pos;
			triobjvec.push_back(Tri3Object(v1, v2, v3,i/3));
		}

		ObjectProjector<3, Tri3Object> proj(triobjvec);
		vector<Vector<double, -1> >& meshWeights = out.attachment->getAllWeights();	
		meshWeights.resize(origMesh.vertices.size());
		printf("mesh weights size = %d\n",meshWeights.size());
		Tri3Object projTri;
		int bones = given.fGraph().verts.size() - 1;
		for (size_t i=0;i<origMesh.vertices.size();i++)
		{
			MeshVertex& vtx = origMesh.vertices[i];
			Vector<double, -1>& origWeight = meshWeights[i];
			origWeight[bones - 1] = 0.f;
			for (int k=0;k<bones;k++) // set to zero
				origWeight[k] = 0.f;

			Vector3 projPos = proj.projectObj(vtx.pos,projTri);	
			Vector3 baryW = barycentricCoord(projPos, projTri);	
			for (int v=0;v<3;v++)
			{
				int vidx = voxelMesh.edges[projTri.triIdx*3+v].vertex;
				Vector<double, -1> voxelWeight = voxelAttachment->getWeights(vidx);
				for (int k=0;k<origWeight.size();k++)
				{				
					origWeight[k] += voxelWeight[k]*baryW[v];		
					//printf("voxel weight[%d] = %f\n", k,voxelWeight[k]);
				}
			}	
		}
		callbackManager.runCallBack();
		//cleanup
		delete tester;
		delete voxelAttachment;
	}		
	callbackManager.runCallBack();
	delete distanceField;
	return out;
}

