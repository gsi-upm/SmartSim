/*
 *  ParserFBX.cpp - part of Motion Engine and SmartBody-lib
 *  Copyright (C) 2008  University of Southern California
 *
 *  SmartBody-lib is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public License
 *  as published by the Free Software Foundation, version 3 of the
 *  license.
 *
 *  SmartBody-lib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with SmartBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Adam Reilly, USC
 */

#include "vhcl.h"

#include "ParserFBX.h"

#if ENABLE_FBX_PARSER   // defined in the project settings

#include <iostream>

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/convenience.hpp"

#include "sr/sr_euler.h"


bool ParserFBX::parse(SkSkeleton& skeleton, SkMotion& motion, const std::string& fileName, float scale)
{
   KFbxSdkManager* pSdkManager;
   KFbxImporter* pImporter;
   KFbxScene* pScene;
   if (!Init(&pSdkManager, &pImporter, &pScene, fileName))
   {
      return false;
   }

   // save the name of the skeleton/anim
   std::string filebasename = boost::filesystem::basename(fileName);
   motion.setName(filebasename.c_str());
   skeleton.name(filebasename.c_str());
   int order = -1;

   KFbxNode* pRootNode = pScene->GetRootNode();

   if(pRootNode) 
   {
       FBxMetaData metaData;
       for (int i = 0; i < pRootNode->GetChildCount(); i++)
       {
          parseJoints(pRootNode->GetChild(i), skeleton, motion, scale, order, metaData, NULL);
          //parseSkinRecursive(pRootNode->GetChild(i), "", 1.0f, std::string(""), NULL, NULL);
       }

       // go through all the animation data and add it
       AddAnimation(pScene, skeleton, motion, scale, order, metaData);
   }
   else
   {
      LOG("Failed to load fbx: %s\n", filebasename.c_str());
      return false;
   }
          
   Shutdown(pSdkManager, pImporter);
   return true;
}

bool ParserFBX::parseSkin(const std::string& fileName, const char* char_name, float scaleFactor, std::string& jointPrefix, mcuCBHandle* mcu_p)
{
   KFbxSdkManager* pSdkManager;
   KFbxImporter* pImporter;
   KFbxScene* pScene;
   if (!Init(&pSdkManager, &pImporter, &pScene, fileName))
   {
      return false;
   }

   KFbxNode* pRootNode = pScene->GetRootNode();
   if(pRootNode) 
   {
       SbmCharacter* char_p = mcu_p->getCharacter(char_name );
       std::vector<SrModel*> meshModelVec;
       for (int i = 0; i < pRootNode->GetChildCount(); i++)
       {
          parseSkinRecursive(pRootNode->GetChild(i), char_name, scaleFactor, jointPrefix, mcu_p, char_p, meshModelVec);
       }

      // cache the joint names for each skin weight
      /*for (size_t x = 0; x < char_p->dMesh_p->skinWeights.size(); x++)
      {
         SkinWeight* skinWeight = char_p->dMesh_p->skinWeights[x];
         for (size_t j = 0; j < skinWeight->infJointName.size(); j++)
         {
            std::string& jointName = skinWeight->infJointName[j];
            SkJoint* curJoint = char_p->skeleton_p->search_joint(jointName.c_str());
            skinWeight->infJoint.push_back(curJoint); // NOTE: If joints are added/removed during runtime, this list will contain stale data
         }
      }*/

      for (unsigned int i = 0; i < meshModelVec.size(); i++)
      {
         for (int j = 0; j < meshModelVec[i]->V.size(); j++)
         {
            meshModelVec[i]->V[j] *= scaleFactor;
         }
         SrSnModel* srSnModelDynamic = new SrSnModel();
         SrSnModel* srSnModelStatic = new SrSnModel();
         srSnModelDynamic->shape(*meshModelVec[i]);
         srSnModelStatic->shape(*meshModelVec[i]);
         srSnModelDynamic->changed(true);
         srSnModelDynamic->visible(false);
         srSnModelStatic->shape().name = meshModelVec[i]->name;
         srSnModelDynamic->shape().name = meshModelVec[i]->name;
         char_p->dMesh_p->dMeshDynamic_p.push_back(srSnModelDynamic);
         char_p->dMesh_p->dMeshStatic_p.push_back(srSnModelStatic);
         mcu_p->root_group_p->add(srSnModelDynamic);	
      }
   }
   else
   {
      LOG("No root node found in %s", fileName.c_str());
   }

   Shutdown(pSdkManager, pImporter);
   return true;
}

void ParserFBX::parseSkinRecursive(KFbxNode* pNode, const char* char_name, float scaleFactor,
                                   std::string& jointPrefix, mcuCBHandle* mcu_p, SbmCharacter* char_p, std::vector<SrModel*>& meshModelVec)
{
   if (pNode->GetNodeAttribute()->GetAttributeType() == KFbxNodeAttribute::eMESH)
   {
      KFbxMesh* pMesh = (KFbxMesh*)pNode->GetNodeAttribute();
      KFbxSkin* pSkin = (KFbxSkin*)pMesh->GetDeformer(0, KFbxDeformer::eSKIN);

      const char* pMeshName = pMesh->GetName();
      //const char* pSkinName = pSkin->GetName();
      KFbxXMatrix transformMatrix;
      SrPnt pnt;   

      SkinWeight* skinWeight = new SkinWeight();
      skinWeight->sourceMesh = pMeshName;

      int ClusterCount = pSkin->GetClusterCount();
      for (int j = 0; j < ClusterCount; ++j)
      {
         KFbxCluster* pCluster = pSkin->GetCluster(j);
         KFbxNode* pBone = pCluster->GetLink();

         // get the transform
         pCluster->GetTransformMatrix(transformMatrix);

         // convert to smartbody format
         double* matBuffer = new double[16];
         for (int i = 0; i < 4; ++i)
         {
            KFbxVector4 col = transformMatrix.GetColumn(i);
            matBuffer[i * 4] = col.GetAt(0);
            matBuffer[i * 4 + 1] = col.GetAt(1);
            matBuffer[i * 4 + 2] = col.GetAt(2);
            matBuffer[i * 4 + 3] = col.GetAt(3);
         }
         skinWeight->bindShapeMat.set(matBuffer);
         SrMat sbmMatrix(matBuffer);
         skinWeight->bindPoseMat.push_back(sbmMatrix);

         //const char* pClusterName = pCluster->GetName();
         const char* pBoneName = pBone->GetName();

         skinWeight->infJointName.push_back(pBoneName);

         //SrModel* objModel = new SrModel();
         // set up the drawing data that will be used in the viewer
         //objModel->name = pMeshName;

         //SkJoint* joint = char_p->skeleton_p->search_joint(pBoneName);
         //if (joint)
         {
            skinWeight->jointNameIndex.push_back(1/*joint->index()*/);
            skinWeight->weightIndex.push_back(1/*joint->index()*/);
         }
         
         int AffectedVertexCount = pCluster->GetControlPointIndicesCount();
         for (int k = 0; k < AffectedVertexCount; ++k) 
         {         
            int AffectedVertexIndex = pCluster->GetControlPointIndices()[k];
            //skinWeight->numInfJoints.push_back(AffectedVertexIndex);

            float Weight = (float)pCluster->GetControlPointWeights()[k]; 
            skinWeight->bindWeight.push_back(Weight);

            // get the vertex associated with this index
            /*KFbxVector4 vertex = pMesh->GetControlPointAt(AffectedVertexIndex);
            ConvertKFbxVector4ToSrPnt(vertex, pnt);
            objModel->V.push(pnt);

            // get the vertex normal associated with this index
            KFbxVector4 normal = pMesh->GetElementNormal()->GetDirectArray().GetAt(k);
            ConvertKFbxVector4ToSrPnt(normal, pnt);
            objModel->N.push(pnt);*/

            //pMesh->GetElementPolygonGroup()->GetDirectArray().GetAt(k);
            //objModel->F.push();
         }

         //int NumVertices = pMesh->GetControlPointsCount();
         //for (int k = 0; k < NumVertices; ++k)
         //{
         //   
         //}
        
         //for (int k = 0; k < pMesh->GetElementNormal()->GetDirectArray().GetCount(); ++k)
         //{
         //   // get the vertex normal associated with this index
         //   KFbxVector4 normal = pMesh->GetElementNormal()->GetDirectArray().GetAt(k);
         //   ConvertKFbxVector4ToSrPnt(normal, pnt);
         //   objModel->N.push(pnt);
         //}

         //meshModelVec.push_back(objModel);
      } 

                  // set up the drawing data that will be used in the viewer
                  SrModel* objModel = new SrModel();
                  objModel->name = pMeshName;

                  KFbxVector4* pControlPoints = pMesh->GetControlPoints();
                  int nNumControlPoints = pMesh->GetControlPointsCount();
                  for (int i = 0; i < nNumControlPoints; ++i)
                  {
                     ConvertKFbxVector4ToSrPnt(pControlPoints[i], pnt);
                     objModel->V.push(pnt);
                  }

                  KFbxLayerElementArrayTemplate<KFbxVector4>* normals;
                  bool retVal = pMesh->GetNormals(&normals);
                  for (int i = 0; i < normals->GetCount(); ++i)
                  {
                     ConvertKFbxVector4ToSrPnt((*normals)[i], pnt);
                     objModel->N.push(pnt);
                  }

                  int nPolygonCount = pMesh->GetPolygonCount();
                  for (int i = 0; i < nPolygonCount; ++i)
                  {
                     int nPolygonSize = pMesh->GetPolygonSize(i);
                     SrModel::Face face;
                     for (int j = 0; j < nPolygonSize; j++)
                     {
                        int nControlPointIndex = pMesh->GetPolygonVertex(i, j);

                        KFbxVector4 normal;
                        pMesh->GetPolygonVertexNormal(i, nControlPointIndex, normal);
                        ConvertKFbxVector4ToSrPnt(normal, pnt);
                        objModel->N.push(pnt);

                        //KFbxVector2 uvs;
                        //pMesh->GetPolygonVertexUV(i, nControlPointIndex, "", uvs);
                        //SrPnt2 pnt2;
                        //ConvertKFbxVector2ToSrPnt2(uvs, pnt2);
                        //objModel->T.push(pnt2);

                        int nUVIndex = pMesh->GetTextureUVIndex(i, j);


                        face.a = nControlPointIndex; face.b = nUVIndex; face.c = max(nControlPointIndex - 1, 0);
                        //face.set(nControlPointIndex, nControlPointIndex, nControlPointIndex);
                        objModel->F.push(face);
                     }
                  }
      
                  /*KFbxLayerElementArrayTemplate<int>* normalIndices = new KFbxLayerElementArrayTemplate<int>(eINTEGER1);
                  bool succeeded = pMesh->GetNormalsIndices(&normalIndices);
                  if (succeeded)
                  {
                     int count = min(normalIndices->GetCount(), objModel->F.size());
                     for (int i = 0; i < count; i++)
                     {
                        objModel->F[i].c = (*normalIndices)[i];
                     }
                  }*/
                  

                  /*int nNumNormals = pMesh->GetElementNormalCount();
                  for (int i = 0; i < nNumNormals; ++i)
                  {
                     KFbxGeometryElementNormal* pElementNormal = pMesh->GetElementNormal(i);
                     int directArrayCount = pElementNormal->GetDirectArray().GetCount();
                     for (int j = 0; j < pElementNormal->GetDirectArray().GetCount(); ++j)
                     {
                        ConvertKFbxVector4ToSrPnt(pElementNormal->GetDirectArray().GetAt(j), pnt);
                        objModel->N.push(pnt);
                     }
                  }*/

               //pMesh->GetNormalsIndices(
                  

                  meshModelVec.push_back(objModel);

      //int layerCount = pMesh->GetLayerCount();
      //for (int i = 0; i < layerCount; i++)
      //{
      //   KFbxLayer* pLayer = pMesh->GetLayer(i);
      //   pLayer->GetNormals(
      //}

      if (char_p)
      {
         //char_p->dMesh_p->skinWeights.push_back(skinWeight);
      }
   }

   for (int i = 0; i < pNode->GetChildCount(); i++)
   {
        // iterate on the children of this node
        parseSkinRecursive(pNode->GetChild(i), char_name, scaleFactor, jointPrefix, mcu_p, char_p, meshModelVec);
   }
}

// starts up the fbx sdk
bool ParserFBX::Init(KFbxSdkManager** pSdkManager, KFbxImporter** pImporter, KFbxScene** pScene, const std::string& fileName)
{
   // Initialize the sdk manager. This object handles all our memory management.
   *pSdkManager = KFbxSdkManager::Create();

   /*
   To import the contents of an FBX file, a KFbxIOSettings object and a KFbxImporter object must be created.
   A KFbxImporter object is initialized by providing the filename of the file we want to import along with a
   KFbxIOSettings object which has been appropriately configured to suit our importing needs (see I/O Settings).
   */

   // Create the io settings object.
   KFbxIOSettings *ios = KFbxIOSettings::Create(*pSdkManager, IOSROOT);

   (*pSdkManager)->SetIOSettings(ios);

   // Create an importer using our sdk manager.
   *pImporter = KFbxImporter::Create(*pSdkManager,"");
    
   // Use the first argument as the filename for the importer.
   if(!(*pImporter)->Initialize(fileName.c_str(), -1, (*pSdkManager)->GetIOSettings())) 
   {
       LOG("Call to KFbxImporter::Initialize() failed. Error returned: %s\n\n", (*pImporter)->GetLastErrorString());
       return false;
   }

   /*
      The KFbxImporter object populates a provided KFbxScene object with the elements contained in the FBX file.
      Observe that the KFbxScene::Create() function is passed an empty string as its second parameter. Objects
      created in the FBX SDK can be given arbitrary, non-unique names, which allow the user or other programs to
      identify the object after it has been exported. After the KFbxScene is populated, the KFbxImporter can be
      safely destroyed.
   */
   // Create a new scene so it can be populated by the imported file.
   *pScene = KFbxScene::Create(*pSdkManager, "");

   // Import the contents of the file into the scene.
   (*pImporter)->Import(*pScene);

   return true;
}

// shuts down the fbx sdk
void ParserFBX::Shutdown(KFbxSdkManager* pSdkManager, KFbxImporter* pImporter)
{
   // The file has been imported; we can get rid of the importer.
   pImporter->Destroy();

   // Shutdown sdk manager
   pSdkManager->Destroy();
}


void ParserFBX::parseJoints(KFbxNode* pNode, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, FBxMetaData& metaData, SkJoint* parent)
{
   SkJoint* joint = parent;
   if (pNode->GetNodeAttribute() != NULL 
      && pNode->GetNodeAttribute()->GetAttributeType() == KFbxNodeAttribute::eSKELETON)
   {
      // we only create joints for nodes flagged as having the eSKELETON attribute
      joint = createJoint(pNode, skeleton, motion, scale, order, parent);
   }

   // if it has this meta data property, then it has all the others,
   // so parse them out save them
   KFbxProperty prop = pNode->FindProperty("readyTime");
   if (prop.IsValid())
   {
      ParseMetaData(pNode, metaData);
   }

   for (int i = 0; i < pNode->GetChildCount(); i++)
   {
        // iterate on the children of this node
        parseJoints(pNode->GetChild(i), skeleton, motion, scale, order, metaData, joint);
   }
}

SkJoint* ParserFBX::createJoint(KFbxNode* pNode, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, SkJoint* parent)
{
   int index = -1;
   if (parent != NULL)	
   {
      index = parent->index();
   }

   // get joint name
   std::string nameAttr = pNode->GetName();
   SkJoint* joint = skeleton.add_joint(SkJoint::TypeQuat, index);
   joint->quat()->activate();
   joint->name(nameAttr);

   // setup skeleton channels	
   bool bHasChannelProperty = false;
   if (HasSmartbodyChannel(pNode, "SbodyPosX", bHasChannelProperty) && bHasChannelProperty)
   {
      skeleton.channels().add(joint->name(), SkChannel::XPos);
      joint->pos()->limits(SkVecLimits::X, false);
   }

   if (HasSmartbodyChannel(pNode, "SbodyPosY", bHasChannelProperty) && bHasChannelProperty)
   {
      skeleton.channels().add(joint->name(), SkChannel::YPos);
      joint->pos()->limits(SkVecLimits::Y, false);
   }

   if (HasSmartbodyChannel(pNode, "SbodyPosZ", bHasChannelProperty) && bHasChannelProperty)
   {
      skeleton.channels().add(joint->name(), SkChannel::ZPos);
      joint->pos()->limits(SkVecLimits::Z, false);
   }

   if (HasSmartbodyChannel(pNode, "SbodyQuat", bHasChannelProperty) && bHasChannelProperty)
   {
      skeleton.channels().add(joint->name(), SkChannel::Quat);
      joint->quat()->activate();
   }

   SrVec offset, rot, jointRot; 

   if (parent == NULL)
      skeleton.root(joint);

   order = 123;//getRotationOrder(orderVec);
   if (order == -1)
      LOG("ParserFBX::parseJoints ERR: rotation info not correct in the file");

   // get local position
   fbxDouble3 translation = pNode->LclTranslation.Get();
   ConvertfbxDouble3ToSrVec(translation, offset);
   offset *= scale;
   joint->offset(offset);

   // get local rotation
   fbxDouble3 rotation = pNode->LclRotation.Get();
   ConvertfbxDouble3ToSrVec(rotation, rot);
   rot *= float(M_PI) / 180.0f;

   // get prerotation
   fbxDouble3 jointRotation = pNode->PreRotation.Get();
   ConvertfbxDouble3ToSrVec(jointRotation, jointRot);
   jointRot *= float(M_PI) / 180.0f;

   // convert from euler angles to mat
   SrMat rotMat, jorientMat;
   sr_euler_mat(order, rotMat, rot.x, rot.y, rot.z);
   sr_euler_mat(order, jorientMat, jointRot.x, jointRot.y, jointRot.z);
   //SrMat finalRotMat = rotMat;

   // convert from mat to quat
   SrQuat quat = SrQuat(rotMat);
   SrQuat jorientQ = SrQuat(jorientMat);
   SkJointQuat* jointQuat = joint->quat();
   jointQuat->prerot(quat);
   jointQuat->postrot(jorientQ);
   jointQuat->orientation(jorientQ);

   return joint;
}

void ParserFBX::ParseMetaData(KFbxNode* pNode, FBxMetaData& out_metaData)
{
   //KFbxNode* smartbodyNode = pRootNode->FindChild("AnimDef_ChrBillFord_IdleSeated01");
   KFbxProperty prop;

   prop = pNode->FindProperty("readyTime");
   if (prop.IsValid())
   {
      prop.Get(&out_metaData.readyTime, eDOUBLE1);
   }

   prop = pNode->FindProperty("strokeStartTime");
   if (prop.IsValid())
   {
      prop.Get(&out_metaData.strokeStart, eDOUBLE1);
   }

   prop = pNode->FindProperty("emphasisTime");
   if (prop.IsValid())
   {
      prop.Get(&out_metaData.emphasisTime, eDOUBLE1);
   }

   prop = pNode->FindProperty("strokeTime");
   if (prop.IsValid())
   {
      prop.Get(&out_metaData.strokeTime, eDOUBLE1);
   }

   prop = pNode->FindProperty("relaxTime");
   if (prop.IsValid())
   {
      prop.Get(&out_metaData.relaxTime, eDOUBLE1);
   }

   out_metaData -= 1.0f; // subtract 1 frame
   out_metaData *= ONE_OVER_THIRTY;
}

void ParserFBX::AddAnimation(KFbxScene* pScene, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, const FBxMetaData& metaData)
{
   KFbxNode* rootNode = pScene->GetRootNode();

   // need this to store the anim data then give it to the motion struct
   std::vector<FBXAnimData*> fbxAnimData;
   for (int i = 0; i < pScene->GetSrcObjectCount(FBX_TYPE(KFbxAnimStack)); i++)
   {
      // stacks hold animation layers, so go through the stacks first
      KFbxAnimStack* pAnimStack = KFbxCast<KFbxAnimStack>(pScene->GetSrcObject(FBX_TYPE(KFbxAnimStack), i));
      pScene->GetEvaluator()->SetContext(pAnimStack);

      KTimeSpan timeSpan = pAnimStack->GetLocalTimeSpan();

      AddAnimationRecursive(pAnimStack, rootNode, skeleton, motion, scale, order, fbxAnimData);
      ConvertfbxAnimToSBM(fbxAnimData, skeleton, motion, scale, order, metaData);

      // start fresh for new anim
      for (unsigned int i = 0; i < fbxAnimData.size(); i++)
      {
         delete fbxAnimData[i];
      }
      fbxAnimData.clear();
   }
} 

void ParserFBX::AddAnimationRecursive(KFbxAnimStack* pAnimStack, KFbxNode* pNode, SkSkeleton& skeleton, SkMotion& motion,
                                      float scale, int& order, std::vector<FBXAnimData*>& fbxAnimData)
{
   int nbAnimLayers = pAnimStack->GetMemberCount(FBX_TYPE(KFbxAnimLayer));

   std::string takeName = pAnimStack->GetName();  
   for (int layerNo = 0; layerNo < nbAnimLayers; layerNo++)
   {
      // layers hold animcurves so go through each layers set of curves
      KFbxAnimLayer* pAnimLayer = pAnimStack->GetMember(FBX_TYPE(KFbxAnimLayer), layerNo);

      AddAnimationRecursive(pAnimLayer, pNode, takeName, skeleton, motion, scale, order, fbxAnimData);
   }
}

void ParserFBX::ParseKeyData(const KFbxAnimCurve* pCurve, const SkChannel::Type type, const std::string& jointName,
                             SkMotion& motion, std::vector<FBXAnimData*>& fbxAnimData)
{
   if (!pCurve)
   {
      return;
   }

   // create a new FBXAnimData and store it
   FBXAnimData* pNewAnimData = new FBXAnimData();
   pNewAnimData->channelName = jointName;
   pNewAnimData->channelType = type;
  
   int numKeys = pCurve->KeyGetCount();
   for (int i = 1; i < numKeys; i++) // frame 0 holds the skeleton bind pose, so skip over it
   {
      KFbxAnimCurveKey key = pCurve->KeyGet(i);
      
      // the time at which the data takes places
      KTime time = key.GetTime();
      const kLongLong t = time.Get();

      // the key channel transformation data
      float value = key.GetValue();
      pNewAnimData->keyFrameData[t] = value;
      pNewAnimData->keyFrameDataFrame[i - 1] = value;
   }

   fbxAnimData.push_back(pNewAnimData);
}

void ParserFBX::AddAnimationRecursive(KFbxAnimLayer* pAnimLayer, KFbxNode* pNode, std::string &takeName, SkSkeleton& skeleton,
                                      SkMotion& motion, float scale, int& order, std::vector<FBXAnimData*>& fbxAnimData)
{
   // get the joint name
   std::string jointName = pNode->LclTranslation.GetParent().GetName();
   KFbxAnimCurve *pCurve = NULL;
   
   bool bHasChannelProperty = false;

   if (HasSmartbodyChannel(pNode, "SbodyPosX", bHasChannelProperty) && bHasChannelProperty)
   {
      // x pos data
      pCurve = pNode->LclTranslation.GetCurve<KFbxAnimCurve>(pAnimLayer, KFCURVENODE_T_X);
      ParseKeyData(pCurve, SkChannel::XPos, jointName, motion, fbxAnimData);
   }

   if (HasSmartbodyChannel(pNode, "SbodyPosY", bHasChannelProperty) && bHasChannelProperty)
   {
       // y pos data
      pCurve = pNode->LclTranslation.GetCurve<KFbxAnimCurve>(pAnimLayer, KFCURVENODE_T_Y);
      ParseKeyData(pCurve, SkChannel::YPos, jointName, motion, fbxAnimData);
   }

   if (HasSmartbodyChannel(pNode, "SbodyPosZ", bHasChannelProperty) && bHasChannelProperty)
   {
       // z pos data
      pCurve = pNode->LclTranslation.GetCurve<KFbxAnimCurve>(pAnimLayer, KFCURVENODE_T_Z);
      ParseKeyData(pCurve, SkChannel::ZPos, jointName, motion, fbxAnimData);
   }

   if (HasSmartbodyChannel(pNode, "SbodyQuat", bHasChannelProperty) && bHasChannelProperty)
   {
        // x rot data
      pCurve = pNode->LclRotation.GetCurve<KFbxAnimCurve>(pAnimLayer, KFCURVENODE_R_X);
      ParseKeyData(pCurve, SkChannel::XRot, jointName, motion, fbxAnimData);

      // y rot data
      pCurve = pNode->LclRotation.GetCurve<KFbxAnimCurve>(pAnimLayer, KFCURVENODE_R_Y);
      ParseKeyData(pCurve, SkChannel::YRot, jointName, motion, fbxAnimData);

      // z rot data
      pCurve = pNode->LclRotation.GetCurve<KFbxAnimCurve>(pAnimLayer, KFCURVENODE_R_Z);
      ParseKeyData(pCurve, SkChannel::ZRot, jointName, motion, fbxAnimData);
   }
    
   int childCount = pNode->GetChildCount();
   for (int i = 0; i < childCount; i++)
   {
      AddAnimationRecursive(pAnimLayer, pNode->GetChild(i), takeName, skeleton, motion, scale, order, fbxAnimData);
   }
}

void ParserFBX::ConvertfbxDouble3ToSrVec(const fbxDouble3& fbx, SrVec& sbm)
{
   sbm.x = (float)fbx.mData[0];
   sbm.y = (float)fbx.mData[1];
   sbm.z = (float)fbx.mData[2];
}

void ParserFBX::ConvertKFbxVector4ToSrPnt(const KFbxVector4& fbx, SrPnt& sbm)
{
   sbm.x = (float)fbx.GetAt(0);
   sbm.y = (float)fbx.GetAt(1);
   sbm.z = (float)fbx.GetAt(2);
}

void ParserFBX::ConvertKFbxVector2ToSrPnt2(const KFbxVector2& fbx, SrPnt2& sbm)
{
   sbm.x = (float)fbx.GetAt(0);
   sbm.y = (float)fbx.GetAt(1);
}

void ParserFBX::ConvertfbxAnimToSBM(const std::vector<FBXAnimData*>& fbxAnimData, SkSkeleton& skeleton,
                                    SkMotion& motion, float scale, int& order, const FBxMetaData& metaData)
{
   if (fbxAnimData.empty())
   {
      return;
   }

   const int stride = 6;
   const float DEG_TO_RAD = float(M_PI) / 180.0f;
   motion.init(skeleton.channels());

   // create the frames and the times assoicated with them
   int numKeys = fbxAnimData[0]->keyFrameData.size();
   std::map<kLongLong, float>::iterator it = fbxAnimData[0]->keyFrameData.begin();
   int f = 0;
   // need to offset by the first amount since we skipped frame 0. We do this to start at time 0
   float timeOffset = (float)((it->first) / 46186158000.0f);

   for (it = fbxAnimData[0]->keyFrameData.begin(); it != fbxAnimData[0]->keyFrameData.end(); f++, it++)
   {
      // convert the way fbx stores the time into the way sbm does (in seconds)
      float kt = (float)((it->first) / 46186158000.0f);
      motion.insert_frame(f, (float)(kt - timeOffset));

      for (int j = 0; j < motion.posture_size(); j++)
      {
         motion.posture(f)[j] = 0.0f;
      }
   }

   //float tempVec, keyChannelData;
   SrMat mat; SrQuat quat;
   for (int i = 0; i < numKeys; i++)
   {
      int offset = 1;
      unsigned int j = 0;
      for (j = 0; j < fbxAnimData.size(); j+= offset)
      {
         std::string jointName = fbxAnimData[j]->channelName;

         // find the skeleton's joint index based off the name of the joint and its channel type
         int jointIndex = motion.channels().search(fbxAnimData[j]->channelName,
            fbxAnimData[j]->channelType == SkChannel::XRot ? SkChannel::Quat : fbxAnimData[j]->channelType);
         int floatIndex = -1;
         if (jointIndex >= 0)
         {
            floatIndex = motion.channels().float_position(jointIndex);
         }
            
         if (floatIndex < 0)
         {
            // either the joint doesn't exist or the channel's buffer doesn't exist
            offset = 1;
            continue;
         }

         // i need the joint so that I can subtract the starting offset from the anim data
         SkJoint* joint = skeleton.search_joint(fbxAnimData[j]->channelName.c_str());

         if (fbxAnimData[j]->channelType == SkChannel::XRot)
         {
            // rotational data 
            //float xRot = fbxAnimData[j]->keyFrameDataFrame[i];
            //float yRot = fbxAnimData[j+1]->keyFrameDataFrame[i];
            //float zRot = fbxAnimData[j+2]->keyFrameDataFrame[i];
            
            //if (stricmp(joint->name().c_str(), "JtElbowLf") == 0)
            //   LOG("Frame %d %s, x: %.2f, y: %.2f, z: %.2f", i, joint->name().c_str(), xRot, yRot, zRot);

            // convert euler angles to mat
            sr_euler_mat(order, mat, fbxAnimData[j]->keyFrameDataFrame[i] * DEG_TO_RAD,
               fbxAnimData[j+1]->keyFrameDataFrame[i] * DEG_TO_RAD, fbxAnimData[j+2]->keyFrameDataFrame[i] * DEG_TO_RAD);
            
            // convert the mat to a quat
            quat = SrQuat(mat);

            // set the rotation
            motion.posture(i)[floatIndex] = quat.w;
            motion.posture(i)[floatIndex+1] = quat.x;
            motion.posture(i)[floatIndex+2] = quat.y;
            motion.posture(i)[floatIndex+3] = quat.z;

            
         }
         else
         {
            // translational data, x, y, or z
            //tempVec = joint->offset()[fbxAnimData[j]->channelType - SkChannel::XPos];
            //keyChannelData = fbxAnimData[j]->keyFrameDataFrame[i];
            motion.posture(i)[floatIndex] = (fbxAnimData[j]->keyFrameDataFrame[i] * scale) - joint->offset()[fbxAnimData[j]->channelType - SkChannel::XPos];
         }

         // rots are stored as 3 seperate entries in fbxAnimData, x, y, z euler angles.  You need all 3 of them to 
         // make the sbm quat, so if this was rotational data, move to the next joint, otherwise, for translational data,
         // we move by 1 channel.
         offset = fbxAnimData[j]->channelType == SkChannel::XRot ? 3 : 1; 
      }
      //numKeys = fbxAnimData[j - 1]->keyFrameData.size();
   }
  
   if (metaData == (double)0)
   {
      LOG("FBX animation %s has no SBM Metadata", motion.getName().c_str());
   }
   
   // set the meta data
   if (numKeys >= 1)
   {
      motion.synch_points.set_time(motion.keytime(0), metaData.readyTime, metaData.strokeStart, 
         metaData.emphasisTime, metaData.strokeTime, metaData.relaxTime, motion.keytime(numKeys - 1));
      motion.compress();
   }
   else
   {
      LOG("FBX animation %s metadata was not set because it doesn't have any key frames", motion.getName().c_str());
   }
}

bool ParserFBX::HasSmartbodyChannel(KFbxNode* pNode, const char* pChannelName, bool& out_ChannelValue)
{
   KFbxProperty prop = pNode->FindProperty(pChannelName);
   if (prop.IsValid())
   {
      prop.Get(&out_ChannelValue, eBOOL1);
      return true;
   }

   return false;
}

#else

int g_parser_fbx_cpp____unused = 0;  // to prevent warning

#endif  // ENABLE_FBX_PARSER
