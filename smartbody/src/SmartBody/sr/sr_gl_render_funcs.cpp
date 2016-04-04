/*
 *  sr_gl_render_funcs.cpp - part of Motion Engine and SmartBody-lib
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
 *  License along with SmarBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Marcelo Kallmann, USC (currently UC Merced)
 */

#include "vhcl.h"
#include <sb/SBTypes.h>

#ifdef __ANDROID__
//#include <GLES/gl.h>
#include <GLES2/gl2.h>
//#include <wes_gl.h>
#elif defined(SB_IPHONE)
#include <OpenGLES/ES1/gl.h>
#else
#if !defined(__FLASHPLAYER__) && !defined(ANDROID_BUILD) && !defined(SB_IPHONE)
#include "external/glew/glew.h"
#include "external/SOIL/SOIL.h"
#endif
#endif


#include <sr/sr_vec.h>
#include <sr/sr_mat.h>
#include <sr/sr_model.h>
#include <sr/sr_lines.h>
#include <sr/sr_points.h>
#include <sr/sr_box.h>
#include <sr/sr_sphere.h>
#include <sr/sr_cylinder.h>
#include <sr/sr_polygons.h>
#include <sr/sr_euler.h>


#include <sbm/GPU/SbmTexture.h>
#include <sbm/GPU/SbmBlendFace.h>
#include <sbm/sbm_deformable_mesh.h>

#if !defined(ANDROID_BUILD) && !defined(SB_IPHONE)
#include <sbm/GPU/SbmShader.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>
#endif

#include <sb/SBSkeleton.h>
#include <sb/SBScene.h>
#include <sb/SBPawn.h>
#include <sr/sr_sn.h>
#include <sr/sr_sn_shape.h>
#include <sr/sr_gl_render_funcs.h>

#include <sr/sr_gl.h>

#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>

#include <sr/jpge.h>

#include "external/glm/glm/glm.hpp"
#include "external/glm/glm/gtc/matrix_transform.hpp"
#include "external/glm/glm/gtc/type_ptr.hpp"

//=============================== render_model ====================================



void SrGlRenderFuncs::renderBlendFace(DeformableMeshInstance* shape)
{
#if !defined(ANDROID_BUILD) && !defined(SB_IPHONE)
	bool USE_SHADER_MANAGER = true;

	SrModel* model			= new SrModel();
	std::string model_path	= "Z:\\casas\\gale_expressions\\v1.00\\Gale_Neutral_clean2_UV_diffuse.BlendArtWarped_average.obj";	
	bool loadSuccess		= model->import_obj(model_path.c_str());	
	if (loadSuccess)
	{
		LOG("Model %s loaded OK", model_path.c_str());
	} 
	else
	{
		LOG("*** ERROR: Couldn't load %s", model_path.c_str());
		delete model;
		return;
	}
	
	SbmDeformableMeshGPU* mesh	= new SbmDeformableMeshGPU();

	boost::filesystem::path p(model_path);
	std::string fileName		= boost::filesystem::basename(p);
	std::string extension		= boost::filesystem::extension(p);

	mesh->setName(fileName + extension);
		
	if (model->Fn.size() == 0)
		model->computeNormals();
	
	SrSnModel* srSnModelStatic		= new SrSnModel();
	srSnModelStatic->shape(*model);
	srSnModelStatic->shape().name	= model->name;
	mesh->dMeshStatic_p.push_back(srSnModelStatic);
	srSnModelStatic->ref();
	
	SrSnModel* srSnModelDynamic		= new SrSnModel();
	srSnModelDynamic->shape(*model);
	srSnModelDynamic->changed(true);
	srSnModelDynamic->visible(false);
	srSnModelDynamic->shape().name = model->name;
	mesh->dMeshDynamic_p.push_back(srSnModelDynamic);
	srSnModelDynamic->ref();

	mesh->buildSkinnedVertexBuffer();


	SbmShaderProgram::printOglError("SrGlRenderFuncs::renderDeformableMesh #1");

	SbmBlendFace * blendFace = new SbmBlendFace();
		//	Gets DeformableMesh
		blendFace->setDeformableMesh(shape->getDeformableMesh());
	
		//	Tests DeformableMesh data
		DeformableMesh* testMesh = blendFace->getDeformableMesh();

		//	Builds VBOs (reference)
		blendFace->buildVertexBufferGPU(1);

		//	Build VBO for rest of shapes
		blendFace->addFace(mesh);

		SbmShaderProgram::printOglError("HOLA 1");

		if(USE_SHADER_MANAGER)
			blendFace->initShader();
		else
			blendFace->initShaderProgram_Dan();

		glBindFragDataLocation(blendFace->_programID, 0, "final_color");

		glUseProgram(blendFace->_programID);

		GLuint	modelviewLocation	= glGetUniformLocation(blendFace->_programID, "M");
		GLuint	projectionLocation	= glGetUniformLocation(blendFace->_programID, "P");
		GLuint	tex0Location		= glGetUniformLocation(blendFace->_programID, "tex0");
		GLuint	tex1Location		= glGetUniformLocation(blendFace->_programID, "tex1");
		GLuint	weightLocation		= glGetUniformLocation(blendFace->_programID, "weight");
		
		GLuint	texCoordLocation	= glGetAttribLocation(blendFace->_programID, "texCoord");
		GLuint	pos0Location		= glGetAttribLocation(blendFace->_programID, "pos0");
		GLuint	pos1Location		= glGetAttribLocation(blendFace->_programID, "pos1");

		GLfloat mp[16], mv[16];
		
		glGetFloatv(GL_PROJECTION_MATRIX, mp);
		glGetFloatv(GL_MODELVIEW_MATRIX, mv);

		glUniformMatrix4fv(modelviewLocation, 1, GL_FALSE, mv);
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, mp);
				
		//glEnableClientState(GL_VERTEX_ARRAY);	
		//VBOPos->VBO()->BindBuffer();
		//glVertexPointer(3, GL_FLOAT, 0, 0);
		glEnableVertexAttribArray(pos0Location);
		blendFace->getVBOPos(0)->VBO()->BindBuffer();
		glVertexAttribPointer(
		   pos0Location,        // attribute 0. No particular reason for 0, but must match the layout in the shader.
		   3,                  // size
		   GL_FLOAT,           // type
		   GL_FALSE,           // normalized?
		   0,                  // stride
		   (void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(pos1Location);
		blendFace->getVBOPos(1)->VBO()->BindBuffer();
		glVertexAttribPointer(
		   pos1Location,        // attribute location
		   3,                  // size
		   GL_FLOAT,           // type
		   GL_FALSE,           // normalized?
		   0,                  // stride
		   (void*)0            // array buffer offset
		);
		
		//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		//VBOTexCoord->VBO()->BindBuffer();
		//glTexCoordPointer(2,GL_FLOAT,0,0);
		glEnableVertexAttribArray(texCoordLocation);
		blendFace->getVBOTexCoord()->VBO()->BindBuffer();
		glVertexAttribPointer(
		   texCoordLocation,   // attribute 0. No particular reason for 0, but must match the layout in the shader.
		   2,                  // size
		   GL_FLOAT,           // type
		   GL_FALSE,           // normalized?
		   0,                  // stride
		   (void*)0            // array buffer offset
		);
		SbmShaderProgram::printOglError("HOLA 7:");


		for(size_t i=0; i<testMesh->subMeshList.size(); i++)
		{
			SbmTextureManager::singleton().loadTexture(SbmTextureManager::TEXTURE_DIFFUSE, "Gale_Neutral_clean2_UV_diffuse.ARTUVwarped.png", "Z:\\casas\\gale_expressions\\v1.00\\Gale_Neutral_clean2_UV_diffuse.ARTUVwarped.png");
			
			SbmTexture* tex;
			SbmTexture* tex0	= SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_DIFFUSE, testMesh->subMeshList[i]->texName.c_str());
			SbmTexture* tex1	= SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_DIFFUSE, "Gale_Neutral_clean2_UV_diffuse.ARTUVwarped.png");

			float weight = SmartBody::SBScene::getScene()->getPawn("defaultPawn0")->getBoundingBox().getCenter().x;
			glUniform1f(weightLocation, weight);

			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, tex0->getID());
			glUniform1i(tex0Location, 0);

			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, tex1->getID());
			glUniform1i(tex1Location, 1);

			blendFace->subMeshTris[i]->VBO()->BindBuffer();
			glDrawElements(GL_TRIANGLES, testMesh->triBuf.size()*3 , GL_UNSIGNED_INT,0);
			blendFace->subMeshTris[i]->VBO()->UnbindBuffer();
		}
		
		SbmShaderProgram::printOglError("HOLA 8");
		glUseProgram(0);
		blendFace->getVBOPos(1)->VBO()->UnbindBuffer();
		blendFace->getVBOPos(0)->VBO()->UnbindBuffer();
		blendFace->getVBOTexCoord()->VBO()->UnbindBuffer();

		//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		//glDisableClientState(GL_VERTEX_ARRAY);	
		glDisableVertexAttribArray(pos0Location);
		glDisableVertexAttribArray(texCoordLocation);
		
		glActiveTexture(GL_TEXTURE0);
		
		//	If we dont use shader manager, we delete shader at every iteration
		if(!USE_SHADER_MANAGER) {
			//We don't need the program anymore.
			glDeleteProgram(blendFace->_programID);
			//Don't leak shaders either.
			glDeleteShader(blendFace->_fsID);
			glDeleteShader(blendFace->_vsID);
		}
		
	delete blendFace;

	delete mesh;
#endif
}

// Renders static mesh WITHOUT Ogre3D
void SrGlRenderFuncs::renderDeformableMesh( DeformableMeshInstance* shape, bool showSkinWeight  )
{
#if ANDROID_BUILD
	bool USE_GPU_BLENDSHAPES = false;
#else
	bool USE_GPU_BLENDSHAPES = false; // set to false for no masks, true for masks
#endif
#if USE_GL_FIXED_PIPELINE
	//LOG("Render Deformable Model");
	DeformableMesh* mesh = shape->getDeformableMesh();
    if (!mesh)
    {
        //LOG("SrGlRenderFuncs::renderDeformableMesh ERR: no deformable mesh found!");
        return; // no deformable mesh
    }
#if !defined(__ANDROID__)
	//LOG("Shape visibility = %d", shape->getVisibility());
	if (SmartBody::SBScene::getScene()->getBoolAttribute("drawMeshWireframe"))
	{
		//LOG("Render in Wireframe mode\n");
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
#endif

	bool useGPUBlendShapes = SmartBody::SBScene::getScene()->getBoolAttribute("useGPUBlendshapes");
	if(useGPUBlendShapes)
	{
			SrVec offsetTrans_;
			SrVec offsetRot_;

			glm::mat4x4 translation;
			glm::mat4x4 rotation = glm::mat4x4(1);
			GLfloat translation_m[16];

			if (shape->isStaticMesh())
			{
				SmartBody::SBSkeleton* skel = shape->getSkeleton();
				SmartBody::SBPawn* pawn		= skel->getPawn();

				const std::string& parentJoint = pawn->getStringAttribute("blendShape.parentJoint");
				if (parentJoint != "")
				{
					SmartBody::SBJoint* joint = skel->getJointByName(parentJoint);
					if (joint)
					{
						const SrMat& woMat = joint->gmat();
						const SrVec & offsetTrans	 		= (pawn->getVec3Attribute("blendShape.parentJointOffsetTrans"));
						const SrVec & offsetRotoffsetRot	= (pawn->getVec3Attribute("blendShape.parentJointOffsetRot"));

						SrQuat quat;
						quat.set(offsetRotoffsetRot.x * M_PI / 180.0f, offsetRotoffsetRot.y * M_PI / 180.0f, offsetRotoffsetRot.z * M_PI / 180.0f);
						SrMat mat;
						quat.get_mat(mat);
						mat.set_translation(offsetTrans);

						SrMat finalMat = mat * woMat ;

						rotation = glm::make_mat4((float*)&finalMat);
					}
				}
				else
				{
					const SrMat& woMat = skel->root()->gmat();
					rotation = glm::make_mat4((float*)&woMat);
				}

				SrVec meshScale = shape->getMeshScale();
				//glScalef(meshScale[0], meshScale[1], meshScale[2]);
			}

		shape->GPUblendShapes(translation, rotation);
    }
	// no USE_GPU_BLENDSHAPES
	else
	{
		
			//LOG("No GPU BlendShapes");
			if (shape->isStaticMesh())
			{
				SmartBody::SBSkeleton* skel = shape->getSkeleton();
				SmartBody::SBPawn* pawn		= skel->getPawn();

				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();

				const std::string& parentJoint = pawn->getStringAttribute("blendShape.parentJoint");
				if (parentJoint != "")
				{
					SmartBody::SBJoint* joint = skel->getJointByName(parentJoint);
					if (joint)
					{
						const SrMat& woMat = joint->gmat();
						glMultMatrix(woMat);		

						const SrVec& offsetTrans	= pawn->getVec3Attribute("blendShape.parentJointOffsetTrans");
						const SrVec& offsetRot		= pawn->getVec3Attribute("blendShape.parentJointOffsetRot");

						SrQuat quat;
						quat.set(offsetRot.x * M_PI / 180.0f, offsetRot.y * M_PI / 180.0f, offsetRot.z * M_PI / 180.0f);
						SrMat mat;
						quat.get_mat(mat);
						mat.set_translation(offsetTrans);
						glMultMatrix(mat);	
					}
				}
				else
				{
					const SrMat& woMat = skel->root()->gmat();

					glMultMatrix(woMat);
				}

				SrVec meshScale = shape->getMeshScale();
				glScalef(meshScale[0], meshScale[1], meshScale[2]);
			}

			std::vector<SbmSubMesh*>& subMeshList = mesh->subMeshList;
			myGLEnable(GL_LIGHTING);
			myGLEnable(GL_TEXTURE_2D);	
			myGLEnable ( GL_ALPHA_TEST );
			myGLEnable (GL_BLEND);
			#if !defined (__ANDROID__) && !defined(SB_IPHONE)
			myGLDisable ( GL_POLYGON_SMOOTH );
			#endif
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glAlphaFunc ( GL_GREATER, 0.0f ) ;
			myGLEnable(GL_CULL_FACE);
	
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
			//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	

			//SrVec tempPos = shape->_deformPosBuf[150];
			//LOG("deformPos = %f %f %f",tempPos[0],tempPos[1],tempPos[2]);
			if (shape->_deformPosBuf.size() > 0)
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)&shape->_deformPosBuf[0]);  
			}
			if (mesh->normalBuf.size() > 0)
			{
				glEnableClientState(GL_NORMAL_ARRAY);
				glNormalPointer(GL_FLOAT, 0, (GLfloat*)&mesh->normalBuf[0]);
			}

			if (showSkinWeight)
			{
				glDepthMask(GL_FALSE);
				glEnableClientState(GL_COLOR_ARRAY);
				glColorPointer(3,GL_FLOAT, 0,  (GLfloat*)&mesh->skinColorBuf[0]);		
				//glColorPointer(3,GL_FLOAT, 0,  (GLfloat*)&mesh->meshColorBuf[0]);
				myGLDisable(GL_LIGHTING);
			}
			else if (mesh->hasVertexColor)
			{
				glEnableClientState(GL_COLOR_ARRAY);
				glColorPointer(3,GL_FLOAT, 0,  (GLfloat*)&mesh->meshColorBuf[0]);		
				//glColorPointer(3,GL_FLOAT, 0,  (GLfloat*)&mesh->meshColorBuf[0]);
				myGLDisable(GL_LIGHTING);

#if defined(__ANDROID__)
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);	
#endif
			}
			else
			{
				glDisableClientState(GL_COLOR_ARRAY);
				myGLEnable(GL_LIGHTING);
			}
		
			if (mesh->texCoordBuf.size() > 0)
			{
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);  	
				glTexCoordPointer(2, GL_FLOAT, 0, (GLfloat*)&mesh->texCoordBuf[0]);   
			}
			
			//LOG("subMeshList size = %d", subMeshList.size());
			for (unsigned int i=0;i<subMeshList.size();i++)
			{	
				SbmSubMesh* subMesh = subMeshList[i];
				glMaterial(subMesh->material);	
				if (subMesh->material.useAlphaBlend)
				{
					myGLEnable(GL_ALPHA_TEST);
					myGLEnable(GL_BLEND);
			
				}
				else
				{
					myGLDisable(GL_ALPHA_TEST);
					myGLDisable(GL_BLEND);
				}

				std::string texturesType = "static";
				if (shape->getCharacter())
					texturesType = shape->getCharacter()->getStringAttribute("texturesType");	

				SmartBody::SBSkeleton* skel = shape->getSkeleton();
				SmartBody::SBPawn* pawn		= skel->getPawn();
				bool useTexBlend = pawn->getBoolAttribute("blendTexturesWithLighting");
				//LOG("textureType = %s", texturesType.c_str());
				if( texturesType == "static" || texturesType == "dynamic")
				{
					SbmTexture* tex = SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_DIFFUSE, subMesh->texName.c_str());		
					//LOG("tex = %d", tex);
					if (tex && !showSkinWeight)
					{
						GLint activeTexture = -1;
						glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
			
						if (activeTexture != GL_TEXTURE0)
							glActiveTexture(GL_TEXTURE0);
			
						//	If we are using blended textures
						myGLEnable(GL_TEXTURE_2D);	
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR); 

						if(!shape->getCharacter())
						{
							glBindTexture(GL_TEXTURE_2D, tex->getID());					
						} 
						else if (texturesType == "dynamic")
						{
							if(shape->_tempTexPairs != NULL)
							{
								glBindTexture(GL_TEXTURE_2D, shape->_tempTexPairs[0]);
								//std::cerr << "Using tex: " << shape->_tempTexPairs[0] << "\n";
								//LOG("Use Blended texture");
							}
							else 
							{
								//LOG("*** WARNING: Blended texture shape->_tempTex not initialized. Using tex->getID() instead.");
								glBindTexture(GL_TEXTURE_2D, tex->getID());
							}
					
						}
						else 		//	If blended textures not used, use neutral appearance				
						{
							glBindTexture(GL_TEXTURE_2D, tex->getID());
							//LOG("Use original texture, texID = %d", tex->getID());
						}				

						if (useTexBlend)
						{
							glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
						}
						else
						{
							glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
						}
						//glColor4f(0.0f, 0.0f, 0.0f, 1.0);				
					}
				}
			#if GLES_RENDER && defined(__ANDROID__)
				//glDrawElements(GL_TRIANGLES, subMesh->triBuf.size()*3, GL_UNSIGNED_SHORT, &subMesh->triBuf[0]);
				glDrawElements_wes(GL_TRIANGLES, subMesh->triBuf.size()*3, GL_UNSIGNED_SHORT, &subMesh->triBuf[0]);
            #elif defined(SB_IPHONE)
                glDrawElements(GL_TRIANGLES, subMesh->triBuf.size()*3, GL_UNSIGNED_SHORT, &subMesh->triBuf[0]);
			#else
				glDrawElements(GL_TRIANGLES, subMesh->triBuf.size()*3, GL_UNSIGNED_INT, &subMesh->triBuf[0]);
			#endif
				glBindTexture(GL_TEXTURE_2D,0);
			}	
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			myGLDisable(GL_TEXTURE_2D);
			myGLDisable(GL_BLEND);	
			if (shape->isStaticMesh())
			{
				glPopMatrix();
			}
			glDepthMask(GL_TRUE);
	}
#endif
	//SbmShaderProgram::printOglError("SrGlRenderFuncs::renderDeformableMesh FINAL");
	//LOG("Finish render deformable model");
}

void SrGlRenderFuncs::render_model ( SrSnShapeBase* shape )
 {	
#if USE_GL_FIXED_PIPELINE
   SrModel& model = ((SrSnModel*)shape)->shape();

   //SR_TRACE1 ( "Render Model faces="<<model.F.size() );
   //initTex();

   SrArray<SrModel::Face>& F = model.F;
   SrArray<SrModel::Face>& Fn = model.Fn;
   SrArray<SrModel::Face>& Ft = model.Ft;
   SrArray<int>&           Fm = model.Fm;
   SrArray<SrVec>&         V = model.V;
   SrArray<SrVec>&         N = model.N;
   SrArray<SrPnt2>&        T = model.T;
   SrArray<SrMaterial>&    M = model.M;

   //LOG("F = %d, V = %d",F.size(), V.size());
   int fsize = F.size();
   int fmsize = Fm.size();
   if ( fsize==0 ) return;

   if ( shape->material_is_overriden() ) fmsize=0; // model materials are ignored
  
//   myGLEnable ( GL_LIGHTING );
   glShadeModel ( GL_SMOOTH );

   if ( model.culling )
    myGLEnable ( GL_CULL_FACE );
   else
    myGLDisable ( GL_CULL_FACE );

   bool flat = true;   
   SrVec fn(SrVec::i);
   int fmIndex = Fm.size() > 0? Fm[0] : -1;
   SrMaterial curmtl = fmsize>0 && fmIndex >= 0? M[Fm[0]]:shape->material();
   glMaterial ( curmtl ); 

#if GLES_RENDER  

   // disable fixed rendering pipeline from OpenGL 1.x
   std::string mtlName = "defaultMaterial";
   if (model.mtlnames.size() > 0)
   {
      mtlName = model.mtlnames[0]; 
   }
   std::string texName = "none";
   if (model.mtlTextureNameMap.find(mtlName) != model.mtlTextureNameMap.end())
       texName = model.mtlTextureNameMap[mtlName];
   SbmTexture* tex = SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_DIFFUSE,texName.c_str());   

   if (tex && T.size() != 0) // apply textures
   {
      myGLEnable ( GL_ALPHA_TEST );
      myGLEnable (GL_BLEND);
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glAlphaFunc ( GL_GREATER, 0.3f ) ;
   
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);   

      myGLDisable(GL_COLOR_MATERIAL);	   
      glActiveTexture(GL_TEXTURE0);
      myGLEnable(GL_TEXTURE_2D);	 	
      glBindTexture(GL_TEXTURE_2D,tex->getID());	   	      
      //LOG("mtlName = %s, has texture = %s, texID = %d", mtlName.c_str(), texName.c_str(), tex->getID());
#if !defined (__FLASHPLAYER__)
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
#else
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
#endif
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	
      
      glTexCoordPointer(2, GL_FLOAT, 0, (GLfloat*)&T[0]);          
   }
   if (N.size() != 0) // has normal array
   {
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, 0, (GLfloat*)&N[0]);
   }
   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)&V[0]);  

   glDrawElements(GL_TRIANGLES, F.size()*3, GL_UNSIGNED_SHORT, &F[0]);
   
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#else
   switch ( shape->render_mode() )
    { case srRenderModeDefault :
      case srRenderModeSmooth :
           glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
           flat = false;
           break;
      case srRenderModeFlat :
           glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
           break;
      case srRenderModeLines :
           glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
           break;
      case srRenderModePoints :
           glPolygonMode ( GL_FRONT_AND_BACK, GL_POINT );
           break;
    }    

   //LOG("(model.mtlnames.size() == 0 #1\n");

   if (model.mtlnames.size() == 0)
   {
	   //LOG("(model.mtlnames.size() == 0\n");
	   glBegin ( GL_TRIANGLES ); // some cards do require begin/end for each triangle!
	   for (int k=0; k<F.size(); k++ )
	   {	
		   int f = k;
		   SrVec n1,n2,n3;
		   n1 = N[Fn[f].a]; n2 = N[Fn[f].b]; n3 = N[Fn[f].c];
		   glNormal ( n1 ); glVertex ( V[F[f].a] );
		   glNormal ( n2 ); glVertex ( V[F[f].b] );
		   glNormal ( n3 ); glVertex ( V[F[f].c] );		   
	   }
	   glEnd ();
   }
   else
   {	   
	   //LOG("(model.mtlnames.size() != 0\n");
	   for (int i=0;i<model.mtlnames.size();i++)
	   {
		   std::string mtlName = model.mtlnames[i];
		   if (model.mtlFaceIndices.find(mtlName) == model.mtlFaceIndices.end())
			   continue;
		   std::vector<int>& mtlFaces = model.mtlFaceIndices[mtlName];	   
		   std::string texName = "none";
		   if (model.mtlTextureNameMap.find(mtlName) != model.mtlTextureNameMap.end())
			   texName = model.mtlTextureNameMap[mtlName];

		   SbmTexture* tex = SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_DIFFUSE,texName.c_str());
		   if ( fsize > Fn.size() || flat ) // no normal
		   {
			   //LOG("No normal\n");
			   glBegin ( GL_TRIANGLES ); // some cards do require begin/end for each triangle!
			   for (unsigned int k=0; k<mtlFaces.size(); k++ )
			   {  
				   int f = mtlFaces[k];			   
				   fn = model.face_normal(f);
				   glNormal ( fn );		   
				   glVertex ( V[F[f].a] );
				   glVertex ( V[F[f].b] );
				   glVertex ( V[F[f].c] );		   
			   }
			   glEnd ();
		   }
		   else if ( fsize > Ft.size() || !tex ) // no texture
		   {
			   //LOG("No texture\n");
			   glBegin ( GL_TRIANGLES );
			   for (unsigned int k=0; k<mtlFaces.size(); k++ )
			   {	
				   int f = mtlFaces[k];
				   glNormal ( N[Fn[f].a] ); glVertex ( V[F[f].a] );
				   glNormal ( N[Fn[f].b] ); glVertex ( V[F[f].b] );
				   glNormal ( N[Fn[f].c] ); glVertex ( V[F[f].c] );		   
			   }
			   glEnd (); 
		   }
		   else // has normal and texture
		   {   // to-do : figure out why texture does not work in the fixed-pipeline ?	  
			   //myGLDisable(GL_LIGHTING);	
			   //LOG("Normal and texture\n");
			   myGLEnable ( GL_ALPHA_TEST );
			   myGLEnable (GL_BLEND);
			   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			   glAlphaFunc ( GL_GREATER, 0.3f ) ;
			   
			   myGLDisable(GL_COLOR_MATERIAL);	   
			   glActiveTexture(GL_TEXTURE0);
			   myGLEnable(GL_TEXTURE_2D);	 	
			   glBindTexture(GL_TEXTURE_2D,tex->getID());	   	   
			   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
			   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
#if !defined (__FLASHPLAYER__)
			   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
#else
			   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
#endif
			   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
			   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	  
			   //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);	
			   //printf("Texture bound[%d], start drawing %s\n", tex->getID(), mtlName.c_str());
			   glBegin ( GL_TRIANGLES );
			   //glColor3f(1.f,0.f,1.f);
			   for (unsigned int k=0; k<mtlFaces.size(); k++ )
			   {	
				   int f = mtlFaces[k];
				   int ft_a = Ft[f].a;
				   int ft_b = Ft[f].b;
				   int ft_c = Ft[f].c;
				   if (ft_a >= T.size() || ft_b >= T.size() || ft_c >= T.size())
				   {
					   LOG("(%s): ft %d %d %d is bigger than T size %d\n", mtlName.c_str(), ft_a, ft_b, ft_c, T.size());
					   continue;
				   }				   

				   glNormal ( N[Fn[f].a] ); 
				   //glColor3f(T[Ft[f].a].x, T[Ft[f].a].y, 0.f);
				   glTexCoord2f(T[Ft[f].a].x, T[Ft[f].a].y); 
				   glVertex ( V[F[f].a] );

				   glNormal ( N[Fn[f].b] ); 
				   //glColor3f(T[Ft[f].b].x, T[Ft[f].b].y, 0.f);
				   glTexCoord2f(T[Ft[f].b].x, T[Ft[f].b].y); 
				   glVertex ( V[F[f].b] ); 

				   glNormal ( N[Fn[f].c] ); 
				   //glColor3f(T[Ft[f].b].x, T[Ft[f].b].y, 0.f);
				   glTexCoord2f(T[Ft[f].c].x, T[Ft[f].c].y); 
				   glVertex ( V[F[f].c] ); 		   
			   }
			   glEnd (); 	   
			   glBindTexture(GL_TEXTURE_2D, 0);	   
			   myGLDisable(GL_BLEND);
		   }

	   }
   }
   
   //if (tex)
	//	printf("texture name = %s, tex id = %d\n",textureName.c_str(), tex->getID());
   

//    for ( f=0; f<fsize; f++ )
//    {    
//       if ( flat || f>=Fn.size() )
//        {
//          fn = model.face_normal(f);
//          glNormal ( fn );
//          glBegin ( GL_TRIANGLES ); // some cards do require begin/end for each triangle!
//          glVertex ( V[F[f].a] );
//          glVertex ( V[F[f].b] );
//          glVertex ( V[F[f].c] );
//          glEnd ();
//        }
//       else
//        {
//          glBegin ( GL_TRIANGLES );
//          glNormal ( N[Fn[f].a] ); glVertex ( V[F[f].a] );
//          glNormal ( N[Fn[f].b] ); glVertex ( V[F[f].b] );
//          glNormal ( N[Fn[f].c] ); glVertex ( V[F[f].c] );
//          glEnd (); 
//        }
//     }

   //SR_TRACE1 ( "End Render Model." );
#endif
#endif
 }

//============================= render_lines ====================================

void SrGlRenderFuncs::render_lines ( SrSnShapeBase* shape )
 {
#if !GLES_RENDER
   //SR_TRACE1 ( "Render lines" );

   SrLines& l = ((SrSnLines*)shape)->shape();
   SrArray<SrPnt>&   V = l.V;
   SrArray<SrColor>& C = l.C;
   SrArray<int>&     I = l.I;

   if ( V.size()<2 ) return;

   myGLDisable ( GL_LIGHTING );
   glColor ( shape->color() );

/*
   if ( shape->render_mode()==srRenderModeSmooth )
    { // render cylinders, with resolution as radius?
    }*/

   glLineWidth ( shape->resolution() ); // default is 1.0

   int v=0;               // current V index
   int i;                 // current I index
   int imax = I.size()-1; // max value for i
   int i1=-1, i2;         // pair I[i],I[i+1]

   if ( I.size()>1 ) { i=0; i1=I[i]; i2=I[i+1]; }

   while ( v<V.size() )
   {
	   if ( v==i1 )
	   {if ( i2<0 ) // new color
	   { /*if ( 0 )//shape->render_mode()==srRenderModeSmooth )
		 { SrMaterial mtl = shape->material();
		 mtl.diffuse = C[-i2-1];
		 glMaterial ( mtl );
		 }
		 else*/
		   { glColor ( C[-i2-1] );
		   }
	   }
	   else // new polyline
	   { glBegin ( GL_LINE_STRIP );
	   while ( v<V.size() && v<=i2 ) glVertex(V[v++]);
	   glEnd ();
	   }
	   i+=2; // update next I information
	   if ( i<imax ) { i1=I[i]; i2=I[i+1]; } else i1=-1;
	   }
	   else
	   { glBegin ( GL_LINES );
	   while ( v<V.size() && v!=i1 ) glVertex(V[v++]); 
	   glEnd ();
	   }
   }

   glLineWidth(1.0f);
#endif
}

//============================= render_points ====================================

void SrGlRenderFuncs::render_points ( SrSnShapeBase* shape )
{
#if !GLES_RENDER
	//SR_TRACE1 ( "Render points" );

	SrPoints& p = ((SrSnPoints*)shape)->shape();

	SrArray<SrPoints::Atrib>* A = p.A;
	SrArray<SrPnt>& P = p.P;

	if ( P.size()==0 ) return;

	myGLDisable ( GL_LIGHTING );
	glColor ( shape->material().diffuse );

	if ( shape->render_mode()==srRenderModeSmooth )
	{ // render shperes, with resolution as radius?
	}

	glPointSize ( shape->resolution() ); // default is 1.0

	int i;

	if ( A )
	{
		for ( i=0; i<P.size(); i++ )
		{ if ( i<A->size() )
		{ glPointSize ( A->get(i).s );
		glColor ( A->get(i).c );
		}

		glBegin ( GL_POINTS );
		glVertex ( P[i] );
		glEnd ();
		}
	}
	else
	{
		glBegin ( GL_POINTS );
		for ( i=0; i<P.size(); i++ ) glVertex ( P[i] );
		glEnd ();
	}

	glPointSize(1.0f);
#endif
}

//=============================== render_box ====================================

void SrGlRenderFuncs::render_box ( SrSnShapeBase* shape )
 {
#if !GLES_RENDER
   //SR_TRACE1 ( "Render box" );

   SrBox& b = ((SrSnBox*)shape)->shape();
   glColor ( shape->material().diffuse );
   glMaterial ( shape->material() );
   glShadeModel ( GL_SMOOTH );
//   myGLEnable ( GL_LIGHTING );
   myGLEnable ( GL_CULL_FACE );

   switch ( shape->render_mode() )
    { case srRenderModeDefault :
      case srRenderModeSmooth :
      case srRenderModeFlat :
           glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
           glDrawBox ( b.a, b.b );
           break;
      case srRenderModeLines :
           glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
           glBegin ( GL_LINE_STRIP );
           glVertex ( b.a.x, b.a.y, b.a.z );
           glVertex ( b.a.x, b.a.y, b.b.z );
           glVertex ( b.a.x, b.b.y, b.b.z );
           glVertex ( b.a.x, b.b.y, b.a.z );
           glVertex ( b.a.x, b.a.y, b.a.z );
           glEnd ();
           glBegin ( GL_LINE_STRIP );
           glVertex ( b.b.x, b.b.y, b.b.z );
           glVertex ( b.b.x, b.a.y, b.b.z );
           glVertex ( b.b.x, b.a.y, b.a.z );
           glVertex ( b.b.x, b.b.y, b.a.z );
           glVertex ( b.b.x, b.b.y, b.b.z );
           glEnd ();
           glBegin ( GL_LINES );
           glVertex ( b.a.x, b.a.y, b.a.z, b.b.x, b.a.y, b.a.z );
           glVertex ( b.a.x, b.a.y, b.b.z, b.b.x, b.a.y, b.b.z );
           glVertex ( b.a.x, b.b.y, b.a.z, b.b.x, b.b.y, b.a.z );
           glVertex ( b.a.x, b.b.y, b.b.z, b.b.x, b.b.y, b.b.z );
           glEnd ();
           break;
      case srRenderModePoints :
           glBegin ( GL_POINTS );
           glVertex ( b.a.x, b.a.y, b.a.z );
           glVertex ( b.a.x, b.a.y, b.b.z );
           glVertex ( b.a.x, b.b.y, b.b.z );
           glVertex ( b.a.x, b.b.y, b.a.z );
           glVertex ( b.a.x, b.a.y, b.a.z );
           glVertex ( b.b.x, b.b.y, b.b.z );
           glVertex ( b.b.x, b.a.y, b.b.z );
           glVertex ( b.b.x, b.a.y, b.a.z );
           glVertex ( b.b.x, b.b.y, b.a.z );
           glVertex ( b.b.x, b.b.y, b.b.z );
           glEnd ();
           break;
    }
#endif
 }

//=============================== render_sphere ====================================

void SrGlRenderFuncs::render_sphere ( SrSnShapeBase* shape )
 {
#if !GLES_RENDER
   //return;
   //SR_TRACE1 ( "Render sphere" );

   SrSphere& sphere = ((SrSnSphere*)shape)->shape();

   srRenderMode rm = shape->render_mode();
   myGLEnable ( GL_CULL_FACE );
//   myGLEnable ( GL_LIGHTING );
   glMaterial ( shape->material() );

   switch ( rm )
    { case srRenderModeFlat :
           glShadeModel ( GL_FLAT );
           glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
           break;
      case srRenderModeLines :
           glShadeModel ( GL_SMOOTH );
           glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
           break;
      case srRenderModePoints :
           glShadeModel ( GL_SMOOTH );
           glPolygonMode ( GL_FRONT_AND_BACK, GL_POINTS );
           break;
      default:
           glShadeModel ( GL_SMOOTH );
           glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
           break;
    }

   int i, j, k, s_x, s_y, s_z, order, octant;
   float botWidth, topWidth, yTop, yBot, tmp;
   SrVec vec;

   tmp = shape->resolution()*4.0f;
   int depth = SR_ROUND(tmp);
   if ( depth<1 ) depth=1;
   SrArray<SrPnt> array(0,depth*2);

   float rad = sphere.radius;

   glPushMatrix ();
   glTranslate ( sphere.center );

   for ( octant=0; octant<8; octant++ )
    { s_x = -(((octant & 01) << 1) - 1);
      s_y = -( (octant & 02)       - 1);
      s_z = -(((octant & 04) >> 1) - 1);
      //sr_out<<"Octant:"<<octant<<": "<<s_x<<srspc<<s_y<<srspc<<s_z<<srnl;
	  order = s_x * s_y * s_z;

	  for ( i=0; i<depth-1; i++ )
       { yBot = (float) i      / depth;
	     yTop = (float)(i + 1) / depth;
         botWidth = 1 - yBot;
         topWidth = 1 - yTop;

         array.size(0);
         for ( j=0; j<depth-i; j++ )
          { // First vertex
            k = order > 0 ? depth - i - j : j;
		    tmp = (botWidth * k) / (depth - i);
		    array.push().set(s_x * tmp, s_y * yBot, s_z * (botWidth - tmp));
            array.top().normalize();

            // Second vertex
		    k = order > 0 ? (depth - i - 1) - j : j;
		    tmp = (topWidth * k) / (depth - i - 1);
		    array.push().set(s_x * tmp, s_y * yTop, s_z * (topWidth - tmp));
            array.top().normalize();
          }

         // Last vertex
	     k = order > 0 ? depth - i - j : j;
	     tmp = (botWidth * k) / (depth - i);
	     array.push().set(s_x * tmp, s_y * yBot, s_z * (botWidth - tmp));
         array.top().normalize();

         if ( rm==srRenderModePoints )
          { glBegin ( GL_POINTS );
            for ( j=0; j<array.size(); j++ ) glVertex(array[j]*rad);
          }
         else if ( rm==srRenderModeFlat )
          { glBegin ( GL_TRIANGLES );
            for ( j=2; j<array.size(); j++ )
             { if ( j%2 ) swap(array[j-2],array[j-1]);
               glNormal ( triangle_normal(array[j-2],array[j-1],array[j]) );
               glVertex(array[j-2]*rad);
               glVertex(array[j-1]*rad);
               glVertex(array[j]*rad);
               if ( j%2 ) swap(array[j-2],array[j-1]);
             }
          }
         else
          { glBegin ( GL_TRIANGLE_STRIP );
            for ( j=0; j<array.size(); j++ ) { glNormal(array[j]); glVertex(array[j]*rad); }
          }
         glEnd ();
       }

      // Handle the top/bottom polygons specially, to avoid divide by zero

      array.size(3);
      yBot = (float) i / depth;
	  yTop = 1.0;
	  botWidth = 1 - yBot;

	  // First cap vertex
	  if (order > 0)
	    array[0].set(0.0, s_y * yBot, s_z * botWidth);
	  else
	    array[0].set(s_x * botWidth, s_y * yBot, 0);
	  array[0].normalize();

       // Second cap vertex
      if (order > 0)
       array[1].set(s_x * botWidth, s_y * yBot, 0);
      else
       array[1].set(0, s_y * yBot, s_z * botWidth);
	  array[1].normalize();

       // Third cap vertex
	  array[2].set(0, float(s_y), 0);

      if ( rm==srRenderModePoints )
       { glBegin ( GL_POINTS );
         for ( j=0; j<3; j++ ) glVertex(array[j]*rad);
       }
      else if ( rm==srRenderModeFlat )
       { glBegin ( GL_TRIANGLES );
         glNormal ( triangle_normal(array[0],array[1],array[2]) );
         glVertex(array[0]*rad);
         glVertex(array[1]*rad);
         glVertex(array[2]*rad);
       }
      else
       { glBegin ( GL_TRIANGLE_STRIP );
         for ( j=0; j<3; j++ ) { glNormal(array[j]); glVertex(array[j]*rad); }
       }
      glEnd ();
    }
   glPopMatrix ();
#endif
 }

//=============================== render_cylinder ====================================

void SrGlRenderFuncs::render_cylinder ( SrSnShapeBase* shape )
 {
#if !GLES_RENDER
   //SR_TRACE1 ( "Render cylinder" );
   //return;

   SrCylinder& cyl = ((SrSnCylinder*)shape)->shape();

   srRenderMode rm = shape->render_mode();
   myGLEnable ( GL_CULL_FACE );
//   myGLEnable ( GL_LIGHTING );

   glMaterial ( shape->material() );

   switch ( rm )
    { case srRenderModeFlat :
           glShadeModel ( GL_FLAT );
           glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
           break;
      case srRenderModeLines :
           glShadeModel ( GL_SMOOTH );
           glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
           break;
      case srRenderModePoints :
           glShadeModel ( GL_SMOOTH );
           break;
      default:
           glShadeModel ( GL_SMOOTH );
           glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
           break;
    }

   //int nfaces = int(shape->resolution()*10.0f);
   //if ( nfaces<3 ) nfaces = 3;
   int nfaces = 12;
   
   float dang = sr2pi/float(nfaces);
   SrVec va = cyl.b-cyl.a; 
   va.normalize(); // axial vector
   SrVec minus_va = va * -1.0f;

   SrVec vr;
   float deg = SR_TODEG ( angle(SrVec::i,va) );

   if ( deg<10 || deg>170 )
     vr = cross ( SrVec::j, va );
   else
     vr = cross ( SrVec::i, va );
   
   vr.len ( cyl.radius ); // radial vector

   SrMat rot;
   rot.rot ( va, dang );

   SrPnt a1 = cyl.a+vr;
   SrPnt b1 = cyl.b+vr;
   SrPnt a2 = a1 * rot;
   SrPnt b2 = b1 * rot;

   if ( rm==srRenderModePoints )
     glBegin ( GL_POINTS );
   else
     glBegin ( GL_QUADS );

   int i=1;
   SrArray<SrPnt> vlist(0,2*(nfaces+1));
   SrVec n1, n2; // normals
   do { n1=(a1-cyl.a);  n2=(a2-cyl.a);
        n1/=cyl.radius; n2/=cyl.radius; // normalize
        vlist.push()=a1; vlist.push()=b1;
        glNormal (n1); glVertex (b1,a1);
        glNormal (n2); glVertex (a2,b2);
        if ( i==nfaces ) break;
        a1=a2; b1=b2; a2=a1*rot; b2=b1*rot;
        i++;
      } while ( true );

   glEnd ();

   if ( rm!=srRenderModePoints )
    { glBegin ( GL_POLYGON );
      glNormal ( minus_va );
      for ( i=vlist.size()-2; i>=0; i-=2 ) glVertex ( vlist[i] );
      glEnd ();
      glNormal ( va );
      glBegin ( GL_POLYGON );
      for ( i=1; i<vlist.size(); i+=2 ) glVertex ( vlist[i] );
      glEnd ();
    }
#endif
 }

//============================= render_polygon ====================================

static void render_polygon ( SrPolygon& p, srRenderMode rm, float res )
 {
#if !GLES_RENDER
   //SR_TRACE1 ( "Render polygon" );

   int i;

   if ( p.open() )
    { glBegin ( GL_LINE_STRIP );
      for ( i=0; i<p.size(); i++ ) glVertex ( p[i] );
      glEnd ();
    }
   else
    { if ( rm==srRenderModeSmooth || rm==srRenderModeFlat || rm==srRenderModeDefault )
       { SrArray<SrPnt2> tris;
         p.ear_triangulation ( tris );
         glBegin ( GL_TRIANGLES );
         for ( i=0; i<tris.size(); i++ ) glVertex ( tris[i] );
         glEnd ();
       }
      else
       { glBegin ( GL_LINE_STRIP );
         for ( i=0; i<p.size(); i++ ) glVertex ( p[i] );
         if ( !p.open() ) glVertex ( p[0] );
         glEnd ();
       }
    }

   if ( rm==srRenderModePoints || rm==srRenderModeFlat )
    { glPointSize ( res*4 );
      glBegin ( GL_POINTS );
      for ( i=0; i<p.size(); i++ ) glVertex ( p[i] );
      glEnd ();
    }
#endif
 }

void SrGlRenderFuncs::render_polygon ( SrSnShapeBase* shape )
 {
#if !GLES_RENDER
   SrPolygon& p = ((SrSnPolygon*)shape)->shape();
   if ( p.size()==0 ) return;

   float resolution = shape->resolution();

   myGLDisable ( GL_LIGHTING );
   glColor ( shape->material().diffuse );
   glLineWidth ( resolution ); // default is 1.0

   ::render_polygon ( p, shape->render_mode(), resolution );
#endif
 }

//============================= render_polygons ====================================

void SrGlRenderFuncs::render_polygons ( SrSnShapeBase* shape )
 {
#if !GLES_RENDER
   SrPolygons& p = ((SrSnPolygons*)shape)->shape();

   if ( p.size()==0 ) return;

   float resolution = shape->resolution();
   myGLDisable ( GL_LIGHTING );
   glColor ( shape->material().diffuse );
   glLineWidth ( resolution ); // default is 1.0

   int i;
   for ( i=0; i<p.size(); i++ )
    ::render_polygon ( p[i], shape->render_mode(), resolution );
#endif
 }

//======================================= EOF ====================================
