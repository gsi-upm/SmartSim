#include "vhcl.h"
#if !defined(__FLASHPLAYER__) && !defined(__ANDROID__) && !defined(SB_IPHONE)
#include "external/glew/glew.h"
#include "external/jpge/jpge.h"
#endif

#if !defined(SB_IPHONE)
#include "sbm/sbm_deformable_mesh.h"
#include "SbmDeformableMeshGPU.h"
#include <sbm/GPU/SbmTexture.h>
#include "external/glm/glm/glm.hpp"
#include "external/glm/glm/gtc/type_ptr.hpp"
#include "external/glm/glm/gtc/matrix_transform.hpp"
#endif


#include <algorithm>
#include "SbmBlendFace.h"

#include <sr/jpge.h>
#include <sb/SBSkeleton.h>
#include <sb/SBScene.h>
#include <sb/SBPawn.h>


#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>

#if !defined(__ANDROID__) && !defined(SB_IPHONE)

SbmBlendFace::SbmBlendFace() : DeformableMesh()
{
//	_VBOPos							= NULL;
	_VBONormal						= NULL;
	_VBOTexCoord					= NULL;
	_initGPUVertexBuffer			= false;

	_faceCounter					= 0;

	_shaderName						= "BlendFace";
}

SbmBlendFace::~SbmBlendFace()
{
	for(int i = 0; i<_VBOPos.size(); i++) {
		if (_VBOPos[i]) {
			delete _VBOPos[i];
		}
	}
	_VBOPos.clear();
	
	if (_VBONormal)		delete _VBONormal;
	if (_VBOTexCoord)	delete _VBOTexCoord;
	if (_VBOTri)		delete _VBOTri;
	
	for (unsigned int i=0;i<subMeshTris.size();i++)
	{
		delete subMeshTris[i];
	}
	subMeshTris.clear();
}

void SbmBlendFace::initShader() {

	SbmShaderProgram* program		= SbmShaderManager::singleton().getShader(_shaderName);

	//	If GLSL program does not exist yet in SbmShaderManager
	if(program) 
	{
		_programID	= program->getShaderProgram();
//		LOG("Program %d exisits already", _programID);
	} 
	else
	{
//		LOG("Program does not exist yet");
		initShaderProgram();
		_programID	= SbmShaderManager::singleton().getShader(_shaderName)->getShaderProgram();
	}
	
	
}


void SbmBlendFace::setDeformableMesh(DeformableMesh* mesh)
{
	_mesh = mesh;
}

DeformableMesh* SbmBlendFace::getDeformableMesh() 
{
	return _mesh;
}

VBOVec3f* SbmBlendFace::getVBOPos(int i)
{
	return _VBOPos[i];
}

VBOVec3f* SbmBlendFace::getVBONormal()
{
	return _VBONormal;
}

VBOVec2f* SbmBlendFace::getVBOTexCoord()
{
	return _VBOTexCoord;
}

VBOVec3i* SbmBlendFace::getVBOTri()
{
	return _VBOTri;
}
	
		
bool SbmBlendFace::buildVertexBufferGPU(int number_of_shapes)
{
	bool hasGLContext = SbmShaderManager::singleton().initOpenGL() && SbmShaderManager::singleton().initGLExtension();
	if (!hasGLContext) 
		return false;

	if (_initGPUVertexBuffer)
		return true;

	_faceCounter	= 1;

	_VBOPos.resize(_faceCounter);
	_VBOPos[_faceCounter-1]	= new VBOVec3f((char*)"RestPos", VERTEX_POSITION, _mesh->posBuf, number_of_shapes);		
	_VBONormal				= new VBOVec3f((char*)"Normal", VERTEX_VBONORMAL, _mesh->normalBuf);
	_VBOTexCoord			= new VBOVec2f((char*)"TexCoord", VERTEX_TEXCOORD, _mesh->texCoordBuf);
	_VBOTri					= new VBOVec3i((char*)"TriIdx", GL_ELEMENT_ARRAY_BUFFER, _mesh->triBuf);
	
	for (unsigned int i=0; i<_mesh->subMeshList.size(); i++)
	{
		SbmSubMesh* subMesh		= _mesh->subMeshList[i];
		VBOVec3i* subMeshTriBuf = new VBOVec3i((char*)"TriIdx",GL_ELEMENT_ARRAY_BUFFER,subMesh->triBuf);
		subMeshTris.push_back(subMeshTriBuf);
	}
	
	_initGPUVertexBuffer = true;

	return true;
}

void SbmBlendFace::addFace(SbmDeformableMeshGPU* newFace) 
{
	_faceCounter++;
	_VBOPos.resize(_faceCounter);
	_VBOPos[_faceCounter-1]	= new VBOVec3f((char*)"RestPos", VERTEX_POSITION, newFace->posBuf );
}


void SbmBlendFace::addFace(SrSnModel* newFace) 
{
	SrArray<SrVec> v = newFace->shape().V;
	
	std::vector<SrVec> vertices;
	for (int i = 0; i < v.size(); i++) {
		//std::cerr << "Adding face "<< i << ": " << v[i].x << ", " << v[i].y << ", " << v[i].z << "\n";
		vertices.push_back( v[i] );
	}
	_faceCounter++;
	_VBOPos.resize(_faceCounter);
	_VBOPos[_faceCounter-1]	= new VBOVec3f((char*)"RestPos", VERTEX_POSITION, vertices );
	
}


void SbmBlendFace::initShaderProgram_Dan() {
	
	const std::string shaderVs	= "../../../../core/smartbody/SmartBody/src/sbm/GPU/shaderFiles/blendFace.vert";
	const std::string shaderFs	= "../../../../core/smartbody/SmartBody/src/sbm/GPU/shaderFiles/blendFace.frag";
	//const std::string shaderName= "Blend_Face";

	GLint success = 0;

		// build the shader after there is an opengl context
	_vsID = -1;
	_fsID = -1;
	
	_vsID = glCreateShader(GL_VERTEX_SHADER);
	char *vs = NULL;		
	vs = SbmShaderProgram::textFileRead(shaderVs.c_str());
	std::string shaderStrVs = vs;
	const GLchar *sourceVs = (const GLchar *)shaderStrVs.c_str();
	delete vs;
	glShaderSource(_vsID, 1, &sourceVs, NULL);	
	glCompileShader(_vsID);
	glGetShaderiv(_vsID, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE) {
		LOG("ERROR in glCompileShader(_vsID);");
	}

	//loadShaderStr(_vsID, vsShaderStr.c_str());
	
	_fsID = glCreateShader(GL_FRAGMENT_SHADER);
	char *fs = NULL;		
	fs = SbmShaderProgram::textFileRead(shaderFs.c_str());
	std::string shaderStrFs = fs;
	const GLchar *sourceFs = (const GLchar *)shaderStrFs.c_str();
	delete fs;
	glShaderSource(_fsID, 1, &sourceFs, NULL);	
	glCompileShader(_fsID);
	//loadShaderStr(fsID,fsShaderStr.c_str());
	
	glGetShaderiv(_fsID, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE) {
		LOG("ERROR in glCompileShader(_fsID);");
	}

	SbmShaderProgram::printShaderInfoLog(_vsID);
    SbmShaderProgram::printShaderInfoLog(_fsID);
    
	_programID = glCreateProgram();
	if (_vsID != -1)
		glAttachShader(_programID, _vsID);
	if (_fsID != -1)
		glAttachShader(_programID, _fsID);

	glLinkProgram(_programID);
	GLint isLinked = 0;
	glGetProgramiv(_programID, GL_LINK_STATUS, (int *)&isLinked);
	if(isLinked == GL_FALSE) {
		LOG("ERROR inglLinkProgram(_programID);;");
		SbmShaderProgram::printProgramInfoLog(_programID);
	} else {
		LOG("ProgramID: %d", _programID);
	}

	
	glDetachShader(_programID, _vsID);
	glDetachShader(_programID, _fsID);
}

void SbmBlendFace::initShaderProgram()
{
	const std::string shaderVs	= "../../../../core/smartbody/SmartBody/src/sbm/GPU/shaderFiles/blendFace.vert";
	const std::string shaderFs	= "../../../../core/smartbody/SmartBody/src/sbm/GPU/shaderFiles/blendFace.frag";
	//const std::string shaderName= "Blend_Face";
	

	SbmShaderManager::singleton().addShader(_shaderName.c_str(), shaderVs.c_str(), shaderFs.c_str(), true);

	SbmShaderManager::singleton().buildShaders();
	/*
	_initShader = true;()

	if (SbmShaderManager::getShaderSupport() == SbmShaderManager::SUPPORT_OPENGL_3_0)
	{
		SbmShaderManager::singleton().addShader(shaderName.c_str(), shaderVs.c_str(), shaderFs.c_str(), true);
	}
	else if (SbmShaderManager::getShaderSupport() == SbmShaderManager::SUPPORT_OPENGL_2_0)
	{
#ifdef __APPLE__
		SbmShaderManager::singleton().addShader(shaderName.c_str(), shaderVs.c_str(),shaderFs.c_str(), true);
#else
		SbmShaderManager::singleton().addShader(shaderName.c_str(), shaderVs.c_str(), shaderFs.c_str(), true);
#endif
		
	}
	else
	{
		_initShader = false;	
	}
	*/
}

#endif


#if !defined(__ANDROID__) && !defined(SB_IPHONE)
SbmBlendTextures::SbmBlendTextures()
{
}

SbmBlendTextures::~SbmBlendTextures()
{
}


GLuint SbmBlendTextures::getShader(const std::string _shaderName)
{
#if defined(__ANDROID__)
	std::string shaderPath = "/sdcard/uscict_virtualhumandata/shaders/";
#else
	std::string shaderPath = "../../../../core/smartbody/SmartBody/src/sbm/GPU/shaderFiles/";
#endif

	if(_shaderName.compare("Blend_Two_Textures") == 0)
	{
		SbmShaderProgram* program		= SbmShaderManager::singleton().getShader(_shaderName);

		//	If GLSL program does not exist yet in SbmShaderManager
		if(program) 
		{
			return program->getShaderProgram();
		}
		//	If the GLSL shader is not in the ShaderManager yet
		else
		{
			LOG("Program does not exist yet");

			const std::string shaderVs	= shaderPath + "blendTextures.vert";
			const std::string shaderFs	= shaderPath + "blendTextures.frag";
	
			SbmShaderManager::singleton().addShader(_shaderName.c_str(), shaderVs.c_str(), shaderFs.c_str(), true);
			SbmShaderManager::singleton().buildShaders();

			std::cout << "Program Blend_Two_Textures #" << SbmShaderManager::singleton().getShader(_shaderName)->getShaderProgram() << "compiled OK\n";
				
			return SbmShaderManager::singleton().getShader(_shaderName)->getShaderProgram();
		}
	}
	else if(_shaderName.compare("Blend_All_Textures") == 0)
	{
		SbmShaderProgram* program		= SbmShaderManager::singleton().getShader(_shaderName);

		//	If GLSL program does not exist yet in SbmShaderManager
		if(program) 
		{
			return program->getShaderProgram();
		}
		//	If the GLSL shader is not in the ShaderManager yet
		else
		{
			LOG("Program does not exist yet");

			const std::string shaderVs	= shaderPath + "blendAllTextures.vert";
			const std::string shaderFs	= shaderPath + "blendAllTextures.frag";
	
			SbmShaderManager::singleton().addShader(_shaderName.c_str(), shaderVs.c_str(), shaderFs.c_str(), true);
			SbmShaderManager::singleton().buildShaders();
				
			return SbmShaderManager::singleton().getShader(_shaderName)->getShaderProgram();
		}
	}
	else if(_shaderName.compare("Blend_All_Textures_Pairwise") == 0)
	{
		SbmShaderProgram* program		= SbmShaderManager::singleton().getShader(_shaderName);

		//	If GLSL program does not exist yet in SbmShaderManager
		if(program) 
		{
			return program->getShaderProgram();
		}
		//	If the GLSL shader is not in the ShaderManager yet
		else
		{
			LOG("Program does not exist yet");

			const std::string shaderVs	= shaderPath + "blendAllTexturesPairwise.vert";
			const std::string shaderFs	= shaderPath + "blendAllTexturesPairwise.frag";
	
			SbmShaderManager::singleton().addShader(_shaderName.c_str(), shaderVs.c_str(), shaderFs.c_str(), true);
			SbmShaderManager::singleton().buildShaders();
				
			return SbmShaderManager::singleton().getShader(_shaderName)->getShaderProgram();
		}
	}
	else if(_shaderName.compare("BlendGeometry") == 0)
	{
		SbmShaderProgram* program		= SbmShaderManager::singleton().getShader(_shaderName);

		//	If GLSL program does not exist yet in SbmShaderManager
		if(program) 
		{
			return program->getShaderProgram();
		}
		//	If the GLSL shader is not in the ShaderManager yet
		else
		{
			LOG("Program does not exist yet");

			const std::string shaderVs	= shaderPath + "blendGeometry.vert";
			const std::string shaderFs	= shaderPath + "blendGeometry.frag";
	
			SbmShaderManager::singleton().addShader(_shaderName.c_str(), shaderVs.c_str(), shaderFs.c_str(), true);
			SbmShaderManager::singleton().buildShaders();
				
			return SbmShaderManager::singleton().getShader(_shaderName)->getShaderProgram();
		}
	}
	else if(_shaderName.compare("ReadMasks") == 0)
	{
		SbmShaderProgram* program		= SbmShaderManager::singleton().getShader(_shaderName);

		//	If GLSL program does not exist yet in SbmShaderManager
		if(program) 
		{
			return program->getShaderProgram();
		}
		//	If the GLSL shader is not in the ShaderManager yet
		else
		{
			LOG("Program does not exist yet");

			const std::string shaderVs	= shaderPath + "readMasks.vert";
			const std::string shaderFs	= shaderPath + "readMasks.frag";
	
			SbmShaderManager::singleton().addShader(_shaderName.c_str(), shaderVs.c_str(), shaderFs.c_str(), true);
			LOG("Before build shader");
			SbmShaderManager::singleton().buildShaders();
			LOG("After build shader");
				
			return SbmShaderManager::singleton().getShader(_shaderName)->getShaderProgram();
		}
	}
	else if(_shaderName.compare("BlendGeometryWithMasks") == 0)
	{
		SbmShaderProgram* program		= SbmShaderManager::singleton().getShader(_shaderName);

		//	If GLSL program does not exist yet in SbmShaderManager
		if(program) 
		{
			return program->getShaderProgram();
		}
		//	If the GLSL shader is not in the ShaderManager yet
		else
		{
			LOG("Program does not exist yet");

			const std::string shaderVs	= shaderPath + "blendGeometryWithMasks.vert";
			const std::string shaderFs	= shaderPath + "blendGeometryWithMasks.frag";
	
			SbmShaderManager::singleton().addShader(_shaderName.c_str(), shaderVs.c_str(), shaderFs.c_str(), true);
			SbmShaderManager::singleton().buildShaders();
				
			return SbmShaderManager::singleton().getShader(_shaderName)->getShaderProgram();
		}
	}
	else if(_shaderName.compare("BlendGeometryWithMasksFeedback") == 0)
	{
		SbmShaderProgram* program		= SbmShaderManager::singleton().getShader(_shaderName);

		//	If GLSL program does not exist yet in SbmShaderManager
		if(program) 
		{
			return program->getShaderProgram();
		}
		//	If the GLSL shader is not in the ShaderManager yet
		else
		{
			LOG("Program does not exist yet");

			const std::string shaderVs	= shaderPath + "blendGeometryWithMasksFeedback.vert";
			const std::string shaderFs	= ""; // no fragment shader//shaderPath + "blendGeometryWithMasks.frag";

			SbmShaderManager::singleton().addShader(_shaderName.c_str(), shaderVs.c_str(), shaderFs.c_str(), true);
			SbmShaderManager::singleton().buildShaders();

			return SbmShaderManager::singleton().getShader(_shaderName)->getShaderProgram();
		}
	}
	else if(_shaderName.compare("RenderGeometryWithMasks") == 0)
	{
		SbmShaderProgram* program		= SbmShaderManager::singleton().getShader(_shaderName);

		//	If GLSL program does not exist yet in SbmShaderManager
		if(program) 
		{
			return program->getShaderProgram();
		}
		//	If the GLSL shader is not in the ShaderManager yet
		else
		{
			LOG("Program does not exist yet");

			const std::string shaderVs	= shaderPath + "renderGeometryWithMask.vert";
			const std::string shaderFs	= shaderPath + "renderGeometryWithMask.frag";

			SbmShaderManager::singleton().addShader(_shaderName.c_str(), shaderVs.c_str(), shaderFs.c_str(), true);
			SbmShaderManager::singleton().buildShaders();

			return SbmShaderManager::singleton().getShader(_shaderName)->getShaderProgram();
		}
	}
	else if(_shaderName.compare("BlendAllTexturesWithMask") == 0)
	{
		SbmShaderProgram* program		= SbmShaderManager::singleton().getShader(_shaderName);

		//	If GLSL program does not exist yet in SbmShaderManager
		if(program) 
		{
			return program->getShaderProgram();
		}
		//	If the GLSL shader is not in the ShaderManager yet
		else
		{
			LOG("Program does not exist yet");

			const std::string shaderVs	= shaderPath + "blendAllTexturesWithMask.vert";
			const std::string shaderFs	= shaderPath + "blendAllTexturesWithMask.frag";

			SbmShaderManager::singleton().addShader(_shaderName.c_str(), shaderVs.c_str(), shaderFs.c_str(), true);
			SbmShaderManager::singleton().buildShaders();

			return SbmShaderManager::singleton().getShader(_shaderName)->getShaderProgram();
		}
	}
	else
	{
		LOG("*** ERROR: Invalid BlendTextures shader");
		return 0;
	}
}


std::string ZeroPadNumber(int num)
{
	std::stringstream ss;
	
	// the number is converted to string with the help of stringstream
	ss << num; 
	std::string ret;
	ss >> ret;
	
	// Append zero chars
	int str_length = ret.length();
	for (int i = 0; i < 5 - str_length; i++)
		ret = "0" + ret;
	return ret;
}



/*

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
				glMultMatrix(woMat);		

				const SrVec& offsetTrans	= pawn->getVec3Attribute("blendShape.parentJointOffsetTrans");
				const SrVec& offsetRot		= pawn->getVec3Attribute("blendShape.parentJointOffsetRot");


			}
		}

		float meshScale = shape->getMeshScale();
	*/



void SbmBlendTextures::ReadMasks(GLuint * FBODst, GLuint * texDst, std::vector<float> weights, std::vector<GLuint> texIDs, std::vector<std::string> texture_names,  GLuint program, int w, int h)
{
	int numTextures		= weights.size();

	for(int i = 0; i< numTextures; i++) 
	{
		boost::filesystem::path path(texture_names[i]);
		std::string extension = path.extension().string();
		std::string mask		= boost::replace_all_copy(texture_names[i], extension, "_mask" + extension);
		SbmTexture* tex_mask;
		LOG("mask name = %s\n",mask.c_str());
		// Checks if mask file exists
		if (!boost::filesystem::exists(mask))
		{
			LOG("WARNING! Can't find mask named %s for texture %s, using a white mask instead.\n", mask.c_str(), texture_names[i].c_str());
			SbmTextureManager::singleton().createWhiteTexture("white");
			tex_mask	= SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_DIFFUSE,"white");
		}
		else
		{
			// Loads the mask using SBmTextureManager
			SbmTextureManager::singleton().loadTexture(SbmTextureManager::TEXTURE_DIFFUSE, mask.c_str(), mask.c_str());
			tex_mask	= SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_DIFFUSE, mask.c_str());
	
			if(tex_mask == NULL)
				LOG("ERROR loading texture %s",mask.c_str());
		
			// Builds mask to generate OpenGL texture ID
			tex_mask->buildTexture();
		}
		SbmShaderProgram::printOglError("SbmBlendTextures::ReadMasks #1");

		glPushMatrix();
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBODst[i]);                                                              // Bind the framebuffer object
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texDst[i], 0);              // Attach texture to FBO

			assert( glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT );

			SbmShaderProgram::printOglError("SbmBlendTextures::ReadMasks #2");

#if !defined(__ANDROID__)
			glPushAttrib(GL_ENABLE_BIT);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_LIGHTING);
				glMatrixMode (GL_PROJECTION);
				glPushMatrix();

				SbmShaderProgram::printOglError("SbmBlendTextures::ReadMasks #3");
					glLoadIdentity ();
					gluOrtho2D(-1, 1, -1, 1);
					glMatrixMode (GL_MODELVIEW);
					glPushAttrib(GL_VIEWPORT_BIT);
					glPushAttrib(GL_TEXTURE_BIT);
					
						glViewport(0, 0, w, h);
						glLoadIdentity ();
						glClearColor(1.0, 1.0, 1.0, 0);
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						
						glUseProgram(program);
						
						glEnable(GL_TEXTURE_2D);

							GLuint uExpressionSampler	= glGetUniformLocation(program, "uExpressionSampler");
							GLuint uMaskSampler			= glGetUniformLocation(program, "uMaskSampler");
							
							SbmShaderProgram::printOglError("SbmBlendTextures::ReadMasks #7_");
						
							glUniform1i(uExpressionSampler, 0);
							glUniform1i(uMaskSampler, 1);

							glActiveTexture(GL_TEXTURE0);
							glBindTexture(GL_TEXTURE_2D, texIDs[i]);
						
							glActiveTexture(GL_TEXTURE1);
							glBindTexture(GL_TEXTURE_2D, tex_mask->getID());
							
							glBegin(GL_QUADS);
								glTexCoord2f(0, 1);
								glVertex3f(-1.0f, 1.0f, -0.5f);

								glTexCoord2f(0, 0);
								glVertex3f(-1.0f, -1.0f, -0.5f);

								glTexCoord2f(1, 0);
								glVertex3f(1.0f, -1.0f, -0.5f);

								glTexCoord2f(1, 1);
								glVertex3f(1.0f, 1.0f, -0.5f);
							glEnd();

						glUseProgram(0);
						SbmShaderProgram::printOglError("SbmBlendTextures::ReadMasks #9");

					glPopAttrib();                          // Pops texture bit
					glDisable(GL_TEXTURE_2D);
					glPopAttrib();							// Pops viewport information
				glMatrixMode (GL_PROJECTION);
				glPopMatrix();                              
				glMatrixMode (GL_MODELVIEW);
			glPopAttrib();								// Pops ENABLE_BIT
#elif defined(__ANDROID__)
			glDisable(GL_DEPTH_TEST);
			//glDisable(GL_LIGHTING);
			glMatrixMode (GL_PROJECTION);
			glPushMatrix();

			SbmShaderProgram::printOglError("SbmBlendTextures::ReadMasks #3");
			glLoadIdentity ();
			gluOrtho2D(-1, 1, -1, 1);
			glMatrixMode (GL_MODELVIEW);
			int viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport);
			glViewport(0, 0, w, h);
			glLoadIdentity ();
			GLfloat clearColors[4];
			glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColors);
			glClearColor(1.0, 1.0, 1.0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(program);

			//glEnable(GL_TEXTURE_2D);

			GLuint uExpressionSampler	= glGetUniformLocation(program, "uExpressionSampler");
			GLuint uMaskSampler			= glGetUniformLocation(program, "uMaskSampler");

			SbmShaderProgram::printOglError("SbmBlendTextures::ReadMasks #7_");

			glUniform1i(uExpressionSampler, 0);
			glUniform1i(uMaskSampler, 1);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texIDs[i]);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, tex_mask->getID());

			SrVec4 quad[4] = { SrVec4(-1.0, 1.0f, -0.5f, 1.f), SrVec4(-1.0f, -1.0f, -0.5f, 1.f), SrVec4(1.0f, -1.0f, -0.5f, 1.f), SrVec4(1.0f, 1.0f, -0.5f, 1.f) };
			SrVec4 quadT[4] = { SrVec4(0.f, 1.f, 0.f, 0.f), SrVec4(0.f, 0.f, 0.f, 0.f), SrVec4(1.f, 0.f, 0.f, 0.f), SrVec4(1.f, 1.f, 0.f, 0.f) };
			unsigned short indices[] = {0,1,2, 0,2,3};

			GLuint pos_loc = glGetAttribLocation(program,"aPosition");
			GLuint texcoord_loc = glGetAttribLocation(program,"aTexCoord0");
			GLuint mvp_loc = glGetUniformLocation(program, "uMVP");
			glEnableVertexAttribArray(pos_loc);
			glVertexAttribPointer(pos_loc,4,GL_FLOAT,0,0,(GLfloat*)&quad[0]);
			glEnableVertexAttribArray(texcoord_loc);
			glVertexAttribPointer(texcoord_loc,4,GL_FLOAT,0,0,(GLfloat*)&quadT[0]);
			glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, m_modelview_proj->data);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

			glDisableVertexAttribArray(pos_loc);
			glDisableVertexAttribArray(texcoord_loc);

			glUseProgram(0);
			SbmShaderProgram::printOglError("SbmBlendTextures::ReadMasks #9");

			glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
			glClearColor(clearColors[0],clearColors[1],clearColors[2],clearColors[3]);
			//glDisable(GL_TEXTURE_2D);						// Pops viewport information
			glMatrixMode (GL_PROJECTION);
			glPopMatrix();                              
			glMatrixMode (GL_MODELVIEW);
			glEnable(GL_DEPTH_TEST);
			//glEnable(GL_LIGHTING);							// Pops ENABLE_BIT

#endif
			SbmShaderProgram::printOglError("SbmBlendTextures::ReadMasks #10");

			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);    // Bind the render buffer
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);         
		
			SbmShaderProgram::printOglError("SbmBlendTextures::ReadMasks #6");

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);																							
		glPopMatrix();
	}
}

void SbmBlendTextures::BlendGeometryWithMasks(GLuint * FBODst, std::vector<float> weights, GLuint * texIDs, std::vector<std::string> texture_names, DeformableMeshInstance* meshInstance, GLuint program, glm::mat4x4 translation, glm::mat4x4 rotation)
{
#if !defined(__ANDROID__)
	DeformableMesh * _mesh		= meshInstance->getDeformableMesh();

	bool showMasks		= false;
	bool useMasks		= false;
	bool overlayMasks	= false;

	//if (meshInstance->isStaticMesh())
	{
		SmartBody::SBSkeleton* skel = meshInstance->getSkeleton();
		SmartBody::SBPawn* pawn		= skel->getPawn();
		SmartBody::SBAttribute* maskAttribute;
		// Checks showMasks attribute: If masks for the blendshape are present, user can enable or disable the visualization for debugging purposes
		maskAttribute = pawn->getAttribute("blendShape.showMasks");
		if (!maskAttribute)
		{
			showMasks = false;
		}
		else
		{
			showMasks	= pawn->getBoolAttribute("blendShape.showMasks");
		}

		// Checks useMasks attribute: If masks for the blendshape are present, user can enable or disable the use of them
		maskAttribute = pawn->getAttribute("blendShape.useMasks");
		if (!maskAttribute)
		{
			useMasks = false;
		}
		else
		{
			useMasks	= pawn->getBoolAttribute("blendShape.useMasks");
		}

		// Checks overlayMasks attribute
		maskAttribute = pawn->getAttribute("blendShape.overlayMasks");
		if (!maskAttribute)
		{
			overlayMasks = false;
		}
		else
		{
			overlayMasks	= pawn->getBoolAttribute("blendShape.overlayMasks");
		}
	}

	SbmShaderProgram::printOglError("SbmBlendTextures::BlendGeometry #0");

	SbmBlendFace * aux = new SbmBlendFace();

	std::vector<int> verticesUsed(_mesh->posBuf.size(), 0);
	std::vector<SrVec2> edges;

	GLuint tbo, verticesUsedBuffer;
	glGenTextures(1, &tbo);
	
	glGenBuffers( 1, &verticesUsedBuffer);
    glBindBuffer( GL_TEXTURE_BUFFER, verticesUsedBuffer);
#if _MSC_VER == 1500
	glBufferData( GL_TEXTURE_BUFFER, verticesUsed.size() * sizeof(int), &verticesUsed.front(), GL_DYNAMIC_DRAW);
#else
	glBufferData( GL_TEXTURE_BUFFER, verticesUsed.size() * sizeof(int), verticesUsed.data(), GL_DYNAMIC_DRAW);
#endif
	
	aux->setDeformableMesh(_mesh);
	aux->buildVertexBufferGPU(weights.size());

	SrSnModel* writeToBaseModel = NULL;
	SrSnModel* baseModel		= NULL;
	bool foundBaseModel			= false;

	std::vector<SrArray<SrPnt>*> shapes;

	SrArray<SrPnt> neutralV;
	SrArray<SrPnt> visemeV;

	// find the base shape and other shapes
	std::map<std::string, std::vector<SrSnModel*> >::iterator mIter;
	for (mIter = _mesh->blendShapeMap.begin(); mIter != _mesh->blendShapeMap.end(); ++mIter)
	{
		for (size_t i = 0; i < mIter->second.size(); ++i)
		{
			if (strcmp(mIter->first.c_str(), (const char*)mIter->second[i]->shape().name) == 0)
			{
				baseModel		= mIter->second[i];
				foundBaseModel	= true;
				break;
			}
		}
		if (baseModel == NULL)
		{
			LOG("original base model cannot be found");
			continue;
		}

		neutralV	= (baseModel->shape().V);

		// Copies reference to the shape vector
		shapes.push_back(&(baseModel->shape().V));

		for (size_t i = 0; i < mIter->second.size(); ++i)
		{
			if ((i == 0) ||(!mIter->second[i]))
			{
				continue;
			}
		
			visemeV = mIter->second[i]->shape().V;
			aux->addFace(mIter->second[i]);

			// Copies reference to the shape vector
			shapes.push_back(&(baseModel->shape().V));
		}
	}
	
	const int MAX_SHAPES = 14;	// I can't enable more than 16 attributes (15 vertex buffer + 1 texture coordinate buffer)
								// NOTE: Also change #define in shader if you change this value

	std::vector<float>	usedWeights;
	std::vector<int>	usedShapeIDs;

	GLfloat modelview_matrix[16];
	GLfloat projection_matrix[16];
	
	glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

	glUseProgram(program);
		GLuint aVertexTexcoord	= glGetAttribLocation(program, "aVertexTexcoord");
		GLuint aVertexPosition	= glGetAttribLocation(program, "aVertexPosition");

		for(int i=0; i<weights.size(); i++)
		{
			// If it is the first weight (netural shape), or wieght is > 0.000, sends this shape to shader
			if(((weights[i] > 0.0001) && (usedWeights.size() < MAX_SHAPES)) || (i == 0))
			{
				GLuint vertexAttribLoc = aVertexPosition + usedWeights.size();
				//LOG("vertexAttribLoc = %d", vertexAttribLoc);
				glEnableVertexAttribArray(vertexAttribLoc);
				VBOVec3f* vbo = aux->getVBOPos(i);
				if (!vbo)
					continue;
				vbo->VBO()->BindBuffer();
				glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0,0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				// Pushes this weight to the vector of used weights
				usedWeights.push_back(weights[i]);
				usedShapeIDs.push_back(i);
			}
		}
	   //SbmShaderProgram::printOglError("BlendGeometry glVertexAttributePointer");
		
		glEnableVertexAttribArray(aVertexTexcoord);
		aux->getVBOTexCoord()->VBO()->BindBuffer();
		glVertexAttribPointer(aVertexTexcoord, 2, GL_FLOAT, GL_FALSE, 0,0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint uMatrixMV		= glGetUniformLocation(program, "uMatrixMV");
		GLuint uMatrixProj		= glGetUniformLocation(program, "uMatrixProj");
		GLuint uWeights			= glGetUniformLocation(program, "uWeights");
		GLuint uBorderVertices	= glGetUniformLocation(program, "uBorderVertices");
		GLuint uNumberOfShapes	= glGetUniformLocation(program, "uNumberOfShapes");
		//GLuint uTranslate		= glGetUniformLocation(program, "uTranslate");
		GLuint uRotate			= glGetUniformLocation(program, "uRotate");
		GLuint uNeutralSampler	= glGetUniformLocation(program, "uNeutralSampler");
		GLuint uShowMasks		= glGetUniformLocation(program, "uShowMasks");
		GLuint uUseMasks		= glGetUniformLocation(program, "uUseMasks");
		GLuint uOverlayMasks	= glGetUniformLocation(program, "uOverlayMasks");
		GLuint uUsedShapeIDs	= glGetUniformLocation(program, "uUsedShapeIDs");

		SbmShaderProgram::printOglError("BlendGeometry GetUniformLocation");
		int * image_array		= new int[MAX_SHAPES];
		float * w				= new float[usedWeights.size()];
		int * usedShapesID_array = new int[usedShapeIDs.size()];

		for(int i=0; i<MAX_SHAPES; i++)
		{
			if(i < usedWeights.size())
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, texIDs[usedShapeIDs[i]]);

				image_array[i]			= i;
				w[i]					= usedWeights[i];
				usedShapesID_array[i]	= usedShapeIDs[i];
			}
			// Textures not used, but we still need to pass 15 textures to the fragment shader for completeness
			else
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, texIDs[0]);
				image_array[i]	= i;
			}
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texIDs[0]);
		glUniform1i(uNeutralSampler, 0);

		glUniformMatrix4fv(uMatrixMV, 1, GL_FALSE, modelview_matrix);
		glUniformMatrix4fv(uMatrixProj, 1, GL_FALSE, projection_matrix);
		//glUniformMatrix4fv(uTranslate, 1, GL_FALSE, glm::value_ptr(translation));
		glUniformMatrix4fv(uRotate, 1, GL_FALSE, glm::value_ptr(rotation));
		glUniform1fv(uWeights, usedWeights.size(), w);
		glUniform1iv(uUsedShapeIDs, usedShapeIDs.size(), usedShapesID_array);
		glUniform1i(uNumberOfShapes, usedWeights.size());
		glUniform1iv(uNeutralSampler, MAX_SHAPES, image_array);
		glUniform1i(uBorderVertices,  14);
		glUniform1i(uShowMasks,  showMasks);
		glUniform1i(uUseMasks,  useMasks);
		glUniform1i(uOverlayMasks,  overlayMasks);

		aux->subMeshTris[0]->VBO()->BindBuffer();
			glDrawElements(GL_TRIANGLES, _mesh->triBuf.size()*3 , GL_UNSIGNED_INT,0);
		aux->subMeshTris[0]->VBO()->UnbindBuffer();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		aux->getVBOPos(0)->VBO()->UnbindBuffer();
		aux->getVBOPos(1)->VBO()->UnbindBuffer();
		aux->getVBOTexCoord()->VBO()->UnbindBuffer();

		for(int i=0; i<usedWeights.size(); i++)
		{
			glDisableVertexAttribArray(aVertexPosition + i);
		}

		glDisableVertexAttribArray(aVertexTexcoord);
		
	glUseProgram(0);

	glDeleteTextures(1, &tbo);
	
	glDeleteBuffers( 1, &verticesUsedBuffer);

	delete aux;

	SbmShaderProgram::printOglError("BlendGeometry FINAL");
#endif
}

void SbmBlendTextures::RenderGeometryWithMasks(GLuint * FBODst, std::vector<float> weights, GLuint * texIDs, std::vector<std::string> texture_names, DeformableMeshInstance* meshInstance, GLuint program, glm::mat4x4 translation, glm::mat4x4 rotation)
{
#if !defined(__ANDROID__)
	DeformableMesh * _mesh		= meshInstance->getDeformableMesh();

	bool showMasks		= false;
	bool useMasks		= false;
	bool overlayMasks	= false;

	//if (meshInstance->isStaticMesh())
	{
		SmartBody::SBSkeleton* skel = meshInstance->getSkeleton();
		SmartBody::SBPawn* pawn		= skel->getPawn();
		SmartBody::SBAttribute* maskAttribute;
		// Checks showMasks attribute: If masks for the blendshape are present, user can enable or disable the visualization for debugging purposes
		maskAttribute = pawn->getAttribute("blendShape.showMasks");
		if (!maskAttribute)
		{
			showMasks = false;
		}
		else
		{
			showMasks	= pawn->getBoolAttribute("blendShape.showMasks");
		}

		// Checks useMasks attribute: If masks for the blendshape are present, user can enable or disable the use of them
		maskAttribute = pawn->getAttribute("blendShape.useMasks");
		if (!maskAttribute)
		{
			useMasks = false;
		}
		else
		{
			useMasks	= pawn->getBoolAttribute("blendShape.useMasks");
		}

		// Checks overlayMasks attribute
		maskAttribute = pawn->getAttribute("blendShape.overlayMasks");
		if (!maskAttribute)
		{
			overlayMasks = false;
		}
		else
		{
			overlayMasks	= pawn->getBoolAttribute("blendShape.overlayMasks");
		}
	}

	SbmShaderProgram::printOglError("SbmBlendTextures::BlendGeometry #0");

	SbmBlendFace * aux = new SbmBlendFace();

	
	SrSnModel* writeToBaseModel = NULL;
	SrSnModel* baseModel		= NULL;
	bool foundBaseModel			= false;

	const int MAX_SHAPES = 14;	// I can't enable more than 16 attributes (15 vertex buffer + 1 texture coordinate buffer)
	// NOTE: Also change #define in shader if you change this value

	std::vector<float>	usedWeights;
	std::vector<int>	usedShapeIDs;

	GLfloat modelview_matrix[16];
	GLfloat projection_matrix[16];

	glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

	glUseProgram(program);
	GLuint aVertexTexcoord	= glGetAttribLocation(program, "aVertexTexcoord");
	GLuint aVertexPosition	= glGetAttribLocation(program, "aVertexPosition");

	for(int i=0; i<weights.size(); i++)
	{
		// If it is the first weight (netural shape), or wieght is > 0.000, sends this shape to shader
		if(((weights[i] > 0.0001) && (usedWeights.size() < MAX_SHAPES)) || (i == 0))
 		{
// 			GLuint vertexAttribLoc = aVertexPosition + usedWeights.size();
// 			//LOG("vertexAttribLoc = %d", vertexAttribLoc);
// 			glEnableVertexAttribArray(vertexAttribLoc);
// 			VBOVec3f* vbo = aux->getVBOPos(i);
// 			if (!vbo)
// 				continue;
// 			vbo->VBO()->BindBuffer();
// 			glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0,0);
// 			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// Pushes this weight to the vector of used weights
			usedWeights.push_back(weights[i]);
			usedShapeIDs.push_back(i);
		}
	}
	glEnableVertexAttribArray(aVertexPosition);
	glVertexAttribPointer(aVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, &meshInstance->_deformPosBuf[0]);
	glEnableVertexAttribArray(aVertexTexcoord);
	glVertexAttribPointer(aVertexTexcoord, 2, GL_FLOAT, GL_FALSE, 0,&_mesh->texCoordBuf[0]);
	
	GLuint uMatrixMV		= glGetUniformLocation(program, "uMatrixMV");
	GLuint uMatrixProj		= glGetUniformLocation(program, "uMatrixProj");
	GLuint uWeights			= glGetUniformLocation(program, "uWeights");
	GLuint uBorderVertices	= glGetUniformLocation(program, "uBorderVertices");
	GLuint uNumberOfShapes	= glGetUniformLocation(program, "uNumberOfShapes");
	//GLuint uTranslate		= glGetUniformLocation(program, "uTranslate");
	GLuint uRotate			= glGetUniformLocation(program, "uRotate");
	GLuint uNeutralSampler	= glGetUniformLocation(program, "uNeutralSampler");
	GLuint uShowMasks		= glGetUniformLocation(program, "uShowMasks");
	GLuint uUseMasks		= glGetUniformLocation(program, "uUseMasks");
	GLuint uOverlayMasks	= glGetUniformLocation(program, "uOverlayMasks");
	GLuint uUsedShapeIDs	= glGetUniformLocation(program, "uUsedShapeIDs");

	SbmShaderProgram::printOglError("BlendGeometry GetUniformLocation");
	int * image_array		= new int[MAX_SHAPES];
	float * w				= new float[usedWeights.size()];
	int * usedShapesID_array = new int[usedShapeIDs.size()];

	for(int i=0; i<MAX_SHAPES; i++)
	{
		if(i < usedWeights.size())
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, texIDs[usedShapeIDs[i]]);

			image_array[i]			= i;
			w[i]					= usedWeights[i];
			usedShapesID_array[i]	= usedShapeIDs[i];
		}
		// Textures not used, but we still need to pass 15 textures to the fragment shader for completeness
		else
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, texIDs[0]);
			image_array[i]	= i;
		}
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texIDs[0]);
	glUniform1i(uNeutralSampler, 0);

	glUniformMatrix4fv(uMatrixMV, 1, GL_FALSE, modelview_matrix);
	glUniformMatrix4fv(uMatrixProj, 1, GL_FALSE, projection_matrix);
	//glUniformMatrix4fv(uTranslate, 1, GL_FALSE, glm::value_ptr(translation));
	glUniformMatrix4fv(uRotate, 1, GL_FALSE, glm::value_ptr(rotation));
	glUniform1fv(uWeights, usedWeights.size(), w);
	glUniform1iv(uUsedShapeIDs, usedShapeIDs.size(), usedShapesID_array);
	glUniform1i(uNumberOfShapes, usedWeights.size());
	glUniform1iv(uNeutralSampler, MAX_SHAPES, image_array);
	glUniform1i(uBorderVertices,  14);
	glUniform1i(uShowMasks,  showMasks);
	glUniform1i(uUseMasks,  useMasks);
	glUniform1i(uOverlayMasks,  overlayMasks);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//aux->subMeshTris[0]->VBO()->BindBuffer();
	glDrawElements(GL_TRIANGLES, _mesh->triBuf.size()*3 , GL_UNSIGNED_INT,&_mesh->triBuf[0]);
	//aux->subMeshTris[0]->VBO()->UnbindBuffer();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableVertexAttribArray(aVertexPosition );
	glDisableVertexAttribArray(aVertexTexcoord);

	glUseProgram(0);
	SbmShaderProgram::printOglError("BlendGeometry FINAL");
#endif
}


void SbmBlendTextures::BlendGeometry(GLuint * FBODst, std::vector<float> weights, std::vector<GLuint> texIDs, std::vector<std::string> texture_names, DeformableMeshInstance* meshInstance/*_mesh*/, GLuint program)
{
#if !defined(__ANDROID__)
	DeformableMesh * _mesh		= meshInstance->getDeformableMesh();


	glm::mat4x4 translation	= glm::mat4x4();
	translation = glm::translate(translation, glm::vec3(40.0, 65.0, 0.0));

	if (meshInstance->isStaticMesh())
	{
		SmartBody::SBSkeleton* skel = meshInstance->getSkeleton();
		SmartBody::SBPawn* pawn		= skel->getPawn();
		const std::string& parentJoint = pawn->getStringAttribute("blendShape.parentJoint");
		if (parentJoint != "")
		{
			SmartBody::SBJoint* joint = skel->getJointByName(parentJoint);
			if (joint)
			{
				const SrVec& offsetTrans	= pawn->getVec3Attribute("blendShape.parentJointOffsetTrans");
				const SrVec& offsetRot		= pawn->getVec3Attribute("blendShape.parentJointOffsetRot");

				translation = glm::translate(translation, glm::vec3(offsetTrans.x,offsetTrans.y,offsetTrans.z ));
			}
		}
	}


	SbmShaderProgram::printOglError("SbmBlendTextures::BlendGeometry #0");

	SbmBlendFace * aux = new SbmBlendFace();

	std::vector<int> verticesUsed(_mesh->posBuf.size(), 0);
	std::vector<SrVec2> edges;
	/*
	// Creates lists of edges
	for(int i=0; i< _mesh->triBuf.size(); i++)
	{
			int id1 = _mesh->triBuf[i][0];
			int id2 = _mesh->triBuf[i][1];
			int id3 = _mesh->triBuf[i][2];
			
			edges.push_back(SrVec2(id1,id2));
			edges.push_back(SrVec2(id1,id3));
			edges.push_back(SrVec2(id3,id2));
	}

	for(int i=0; i<edges.size(); i++)
	{
		SrVec2 edge			= edges[i];
		SrVec2 edge_reverse = SrVec2(edge.y, edge.x);
		int count			= 0;
		
		count			= std::count (edges.begin(), edges.end(), edge);
		count			+= std::count (edges.begin(), edges.end(), edge_reverse);

		if(count == 1)
		{
			verticesUsed[edge.x] = 1;
			verticesUsed[edge.y] = 1;

//			std::cerr << "Vertex " << edge.x << " is in the edge.\n";
//			std::cerr << "Vertex " << edge.y << " is in the edge.\n";
		}
	}
	*/
	GLuint tbo, verticesUsedBuffer;
	glGenTextures(1, &tbo);
	
	glGenBuffers( 1, &verticesUsedBuffer);
    glBindBuffer( GL_TEXTURE_BUFFER, verticesUsedBuffer);
#if _MSC_VER == 1500
	glBufferData( GL_TEXTURE_BUFFER, verticesUsed.size() * sizeof(int), &verticesUsed.front(), GL_DYNAMIC_DRAW);
#else
	glBufferData( GL_TEXTURE_BUFFER, verticesUsed.size() * sizeof(int), verticesUsed.data(), GL_DYNAMIC_DRAW);
#endif
	
	aux->setDeformableMesh(_mesh);
	aux->buildVertexBufferGPU(weights.size());

	SrSnModel* writeToBaseModel = NULL;
	SrSnModel* baseModel		= NULL;
	bool foundBaseModel			= false;

	std::vector<SrArray<SrPnt>*> shapes;

	SrArray<SrPnt> neutralV;
	SrArray<SrPnt> visemeV;
	// find the base shape and other shapes
	std::map<std::string, std::vector<SrSnModel*> >::iterator mIter;
	for (mIter = _mesh->blendShapeMap.begin(); mIter != _mesh->blendShapeMap.end(); ++mIter)
	{
		for (size_t i = 0; i < mIter->second.size(); ++i)
		{
			if (strcmp(mIter->first.c_str(), (const char*)mIter->second[i]->shape().name) == 0)
			{
				baseModel		= mIter->second[i];
				foundBaseModel	= true;
				break;
			}
		}
		if (baseModel == NULL)
		{
			LOG("original base model cannot be found");
			continue;
		}

		neutralV	= (baseModel->shape().V);

		// Copies reference to the shape vector
		shapes.push_back(&(baseModel->shape().V));

		for (size_t i = 0; i < mIter->second.size(); ++i)
		{
			if ((i == 0) ||(!mIter->second[i]))
			{
				continue;
			}
		
			visemeV = mIter->second[i]->shape().V;
			aux->addFace(mIter->second[i]);

			// Copies reference to the shape vector
			shapes.push_back(&(baseModel->shape().V));
		}
	}
	
	const int MAX_SHAPES = 14;	// I can't enable more than 16 attributes (15 vertex buffer + 1 texture coordinate buffer)
								// NOTE: Also change #define in shader if you change this value

	std::vector<float>	usedWeights;
	std::vector<int>	usedShapeIDs;
	std::vector<int>	areas;

	GLfloat modelview_matrix[16];
	GLfloat projection_matrix[16];
	
	glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

	glUseProgram(program);
		GLuint aVertexTexcoord	= glGetAttribLocation(program, "aVertexTexcoord");
		GLuint aVertexPosition	= glGetAttribLocation(program, "aVertexPosition");

		for(int i=0; i<weights.size(); i++)
		{
			// If it is the first weight (netural shape), or wieght is > 0.000, sends this shape to shader
			if(((weights[i] > 0.0001) && (usedWeights.size() < MAX_SHAPES)) || (i == 0))
			{
				glEnableVertexAttribArray(aVertexPosition + usedWeights.size());
				aux->getVBOPos(i)->VBO()->BindBuffer();
				glVertexAttribPointer(aVertexPosition + usedWeights.size(), 3, GL_FLOAT, GL_FALSE, 0,0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				// Pushes this weight to the vector of used weights
				usedWeights.push_back(weights[i]);
				usedShapeIDs.push_back(i);

				// Areas is a vector<int> used to set which areas each shape affect (0 -> all, 1-> upper)
				// If this shape is for eye_blink, sets area to upper
				if(
					//(texture_names[i].find("eye") != std::string::npos) ||
					(texture_names[i].find("brows") != std::string::npos) 
					)
					areas.push_back(1);										
				else if(
						(texture_names[i].find("smile") != std::string::npos)||
						(texture_names[i].find("bmp") != std::string::npos)||
						(texture_names[i].find("fv") != std::string::npos) ||
						(texture_names[i].find("w") != std::string::npos) 
						)
				{
					areas.push_back(2);
				}
				else
				{
					areas.push_back(0);
				}
			}
		}

		glEnableVertexAttribArray(aVertexTexcoord);
		aux->getVBOTexCoord()->VBO()->BindBuffer();
		glVertexAttribPointer(aVertexTexcoord, 2, GL_FLOAT, GL_FALSE, 0,0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint uMatrixMV		= glGetUniformLocation(program, "uMatrixMV");
		GLuint uMatrixProj		= glGetUniformLocation(program, "uMatrixProj");
		GLuint uWeights			= glGetUniformLocation(program, "uWeights");
		GLuint uAreas			= glGetUniformLocation(program, "uAreas");
		GLuint uBorderVertices	= glGetUniformLocation(program, "uBorderVertices");
		GLuint uNumberOfShapes	= glGetUniformLocation(program, "uNumberOfShapes");
		GLuint uTranslate		= glGetUniformLocation(program, "uTranslate");
		GLuint uNeutralSampler	= glGetUniformLocation(program, "uNeutralSampler");

		int * image_array		= new int[MAX_SHAPES];
		float * w				= new float[usedWeights.size()];
		
		for(int i=0; i<MAX_SHAPES; i++)
		{
			if(i < usedWeights.size())
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, texIDs[usedShapeIDs[i]]);
				image_array[i]	= i;
				w[i]			= usedWeights[i];
			}
			// Textures not used, but we still need to pass 15 textures to the fragment shader for completeness
			else
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, texIDs[0]);
				image_array[i]	= i;
			}
		}

		glActiveTexture(GL_TEXTURE14);
		glBindTexture(GL_TEXTURE_BUFFER, tbo);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_R8I, verticesUsedBuffer);


		glUniformMatrix4fv(uMatrixMV, 1, GL_FALSE, modelview_matrix);
		glUniformMatrix4fv(uMatrixProj, 1, GL_FALSE, projection_matrix);
		glUniformMatrix4fv(uTranslate, 1, GL_FALSE, glm::value_ptr(translation));
		glUniform1iv(uAreas, areas.size(), &(areas[0]));
		glUniform1fv(uWeights, usedWeights.size(), w);
		glUniform1i(uNumberOfShapes, usedWeights.size());
		glUniform1iv(uNeutralSampler, MAX_SHAPES, image_array);
		glUniform1i(uBorderVertices,  14);


		aux->subMeshTris[0]->VBO()->BindBuffer();
			glDrawElements(GL_TRIANGLES, _mesh->triBuf.size()*3 , GL_UNSIGNED_INT,0);
		aux->subMeshTris[0]->VBO()->UnbindBuffer();

		glBindTexture(GL_TEXTURE_2D, 0);
		aux->getVBOPos(0)->VBO()->UnbindBuffer();
		aux->getVBOPos(1)->VBO()->UnbindBuffer();
		aux->getVBOTexCoord()->VBO()->UnbindBuffer();

		for(int i=0; i<usedWeights.size(); i++)
		{
			glDisableVertexAttribArray(aVertexPosition + i);
		}

		glDisableVertexAttribArray(aVertexTexcoord);

	glUseProgram(0);

	glDeleteTextures(1, &tbo);
	
	glDeleteBuffers( 1, &verticesUsedBuffer);

	delete aux;

	SbmShaderProgram::printOglError("BlendGeometry FINAL");
#endif
}

void SbmBlendTextures::BlendAllAppearancesPairwise(GLuint * FBODst, GLuint * texDst, std::vector<float> weights, std::vector<GLuint> texIDs, std::vector<std::string> texture_names, GLuint program, int w, int h)
{
	int numTextures		= weights.size();

	float sumOfWeights	= 0;

	float WeightUpToNow = 0;

	for(std::vector<float>::iterator j=weights.begin(); j!=weights.end(); ++j)
		sumOfWeights += *j;
	//LOG("Blend ApperancePairWise");
	for(int i = numTextures-1; i >= 0; i--) 
	{
		//LOG("BlendTexture %d", i);
		WeightUpToNow += weights[i];

		int faceArea;
//		if((texture_names[i].find("eyebrowsUp") != std::string::npos) && (weights[i] > 0.001))
//		{
//			faceArea = 1;
//			//std::cerr << "eye_blink\t" << weights[i] << "\n";
//		} 
//		else if((texture_names[i].find("smile") != std::string::npos) && (weights[i] > 0.001))
//		{
//			faceArea = 2;
//			//std::cerr << texture_names[i] <<"\t" << weights[i] << "\n";
//		}
//		else
		{
			faceArea = 0;
		}

	
		
			glBindFramebuffer(GL_FRAMEBUFFER, FBODst[i]);                                                              // Bind the framebuffer object
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texDst[i], 0);              // Attach texture to FBO
		//LOG("After bind framebuffer");
			assert( glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE );
			
#if !defined(__ANDROID__)
			glPushAttrib(GL_ENABLE_BIT);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_LIGHTING);
				glMatrixMode (GL_PROJECTION);
				glPushMatrix();
					glLoadIdentity ();
					gluOrtho2D(-1, 1, -1, 1);
					glMatrixMode (GL_MODELVIEW);
					glPushMatrix();
					glPushAttrib(GL_VIEWPORT_BIT);
					glPushAttrib(GL_TEXTURE_BIT);
						glViewport(0, 0, w, h);
						glLoadIdentity ();

						glClearColor(1.0f, 1.0f, 1.0f, 0);
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

						GLuint uNumberOfTextures		= glGetUniformLocation(program, "uNumberOfTextures");
						GLuint uIteration				= glGetUniformLocation(program, "uIteration");
						GLuint uWeight					= glGetUniformLocation(program, "uWeight");
						GLuint uWeightUpToNow			= glGetUniformLocation(program, "uWeightUpToNow");
						GLuint uFaceArea				= glGetUniformLocation(program, "uFaceArea");
						GLuint uTotalWeights			= glGetUniformLocation(program, "uTotalWeights");
						GLuint uNeutralSampler			= glGetUniformLocation(program, "uNeutralSampler");
						GLuint uExpressionSampler		= glGetUniformLocation(program, "uExpressionSampler");
						GLuint uPreviousResultSampler	= glGetUniformLocation(program, "uPreviousResultSampler");

						glUseProgram(program);
						glEnable(GL_TEXTURE_2D);

						glUniform1i(uNumberOfTextures, numTextures);
						glUniform1i(uIteration, i);
						glUniform1f(uWeight, weights[i]);
						glUniform1f(uWeightUpToNow, WeightUpToNow);
						glUniform1i(uFaceArea, faceArea);
						glUniform1f(uTotalWeights, sumOfWeights);
						glUniform1i(uNeutralSampler, 0);
						glUniform1i(uExpressionSampler, 1);
						glUniform1i(uPreviousResultSampler, 2);

						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, texIDs[0]);

						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, texIDs[i]);

						//std::cerr << "uNeutral: " << texIDs[0] << "\tuExpression: " << texIDs[i];

						// if first iteration, previous result will not be used, passing a random texture just for completeness  
						if(i == numTextures-1) {
							glActiveTexture(GL_TEXTURE2);
							glBindTexture(GL_TEXTURE_2D, texIDs[1]);
							//std::cerr  << "\tprevious: " << texIDs[1] << "\tDest:" << texDst[i] << "\ti = " << i << "\n";
						} else {
							glActiveTexture(GL_TEXTURE2);
							glBindTexture(GL_TEXTURE_2D, texDst[i+1]);
							//std::cerr << "\tprevious: " << texDst[i+1] << "\tDest:" << texDst[i] <<  "\ti = " << i << "\n";
						}

						glBegin(GL_QUADS);
							glTexCoord2f(0, 1);
							glVertex3f(-1.0f, 1.0f, -0.5f);

							glTexCoord2f(0, 0);
							glVertex3f(-1.0f, -1.0f, -0.5f);

							glTexCoord2f(1, 0);
							glVertex3f(1.0f, -1.0f, -0.5f);

							glTexCoord2f(1, 1);
							glVertex3f(1.0f, 1.0f, -0.5f);
						glEnd();

					glUseProgram(0);

					glPopAttrib();                          // Pops texture bit
					glDisable(GL_TEXTURE_2D);
					glPopAttrib();							// Pops viewport information
				glMatrixMode (GL_PROJECTION);
				glPopMatrix();                              // Pops ENABLE_BIT
				glMatrixMode (GL_MODELVIEW);
				glPopMatrix();
			glPopAttrib();
#elif defined(__ANDROID__)
			GLint frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
// 			if (frameBufferStatus == GL_FRAMEBUFFER_COMPLETE)
// 				LOG("frameBufferStatus = GL_FRAMEBUFFER_COMPLETE");
// 			else if (frameBufferStatus == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
// 				LOG("frameBufferStatus = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
// 			else if (frameBufferStatus == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
// 				LOG("frameBufferStatus = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
// 			else if (frameBufferStatus == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS)
// 				LOG("frameBufferStatus = GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS");
// 			else if (frameBufferStatus == GL_FRAMEBUFFER_UNSUPPORTED)
// 				LOG("frameBufferStatus = GL_FRAMEBUFFER_UNSUPPORTED");
			
			glMatrixMode (GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity ();
			
			glMatrixMode (GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluOrtho2D(-1, 1, -1, 1);
			
			glDisable(GL_DEPTH_TEST);
			//glDisable(GL_LIGHTING);
			int viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport);
			GLfloat clearColors[4];
			glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColors);

			glViewport(0, 0, w, h);
			

			glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			
			//LOG("After clear buffer");
			GLuint uNumberOfTextures		= glGetUniformLocation(program, "uNumberOfTextures");
			GLuint uIteration				= glGetUniformLocation(program, "uIteration");
			GLuint uWeight					= glGetUniformLocation(program, "uWeight");
			GLuint uWeightUpToNow			= glGetUniformLocation(program, "uWeightUpToNow");
			GLuint uFaceArea				= glGetUniformLocation(program, "uFaceArea");
			GLuint uTotalWeights			= glGetUniformLocation(program, "uTotalWeights");
			GLuint uNeutralSampler			= glGetUniformLocation(program, "uNeutralSampler");
			GLuint uExpressionSampler		= glGetUniformLocation(program, "uExpressionSampler");
			GLuint uPreviousResultSampler	= glGetUniformLocation(program, "uPreviousResultSampler");
			
			glUseProgram(program);
			
			//glEnable(GL_TEXTURE_2D);
			//LOG("After use program");
			glUniform1i(uNumberOfTextures, numTextures);
			glUniform1i(uIteration, i);
			glUniform1f(uWeight, weights[i]);
			glUniform1f(uWeightUpToNow, WeightUpToNow);
			glUniform1i(uFaceArea, faceArea);
			glUniform1f(uTotalWeights, sumOfWeights);
			glUniform1i(uNeutralSampler, 0);
			glUniform1i(uExpressionSampler, 1);
			glUniform1i(uPreviousResultSampler, 2);
			//LOG("After glUniform1i");
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texIDs[0]);
			//LOG("after bind tedIDs[0]");
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texIDs[i]);
			//LOG("After bindTexture");
			//std::cerr << "uNeutral: " << texIDs[0] << "\tuExpression: " << texIDs[i];

			// if first iteration, previous result will not be used, passing a random texture just for completeness  
			if(i == numTextures-1) {
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, texIDs[1]);
				//std::cerr  << "\tprevious: " << texIDs[1] << "\tDest:" << texDst[i] << "\ti = " << i << "\n";
			} else {
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, texDst[i+1]);
				//std::cerr << "\tprevious: " << texDst[i+1] << "\tDest:" << texDst[i] <<  "\ti = " << i << "\n";
			}
			
			//LOG("before bindAttrib");
			SrVec4 quad[4] = { SrVec4(-1.0, 1.0f, -0.5f, 1.f), SrVec4(-1.0f, -1.0f, -0.5f, 1.f), SrVec4(1.0f, -1.0f, -0.5f, 1.f), SrVec4(1.0f, 1.0f, -0.5f, 1.f) };
			SrVec4 quadT[4] = { SrVec4(0.f, 1.f, 0.f, 0.f), SrVec4(0.f, 0.f, 0.f, 0.f), SrVec4(1.f, 0.f, 0.f, 0.f), SrVec4(1.f, 1.f, 0.f, 0.f) };
			unsigned short indices[] = {0,1,2, 0,2,3};

			GLuint pos_loc = glGetAttribLocation(program,"aPosition");
			GLuint texcoord_loc = glGetAttribLocation(program,"aTexCoord0");
			GLuint mvp_loc = glGetUniformLocation(program, "uMVP");
			glEnableVertexAttribArray(pos_loc);
			glVertexAttribPointer(pos_loc,4,GL_FLOAT,0,0,(GLfloat*)&quad[0]);
			glEnableVertexAttribArray(texcoord_loc);
			glVertexAttribPointer(texcoord_loc,4,GL_FLOAT,0,0,(GLfloat*)&quadT[0]);
			wes_matrix_mvp();
			glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, m_modelview_proj->data);
			//LOG("Before draw elements");
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
			//LOG("After draw elements");
			
			glDisableVertexAttribArray(pos_loc);
			glDisableVertexAttribArray(texcoord_loc);
			
			glUseProgram(0);
			
			glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
			glClearColor(clearColors[0],clearColors[1],clearColors[2],clearColors[3]);                        
			//glDisable(GL_TEXTURE_2D);
			glEnable(GL_DEPTH_TEST);
			//glEnable(GL_LIGHTING);
			
			glMatrixMode (GL_PROJECTION);
			glPopMatrix(); 
			
			glMatrixMode (GL_MODELVIEW);
			glPopMatrix();
#endif
			glBindRenderbuffer(GL_RENDERBUFFER, 0);                                                                                                          // Bind the render buffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);         
		
			glActiveTexture(GL_TEXTURE0);
																									   // Bind the frame buffer object
		
		//LOG("After bind framebuffer");
		/*
		//	Saves USColorCode for current weight i in EXR32 format
		int channels = 3;
		std::string path =  "C:/tmp/iteration." + ZeroPadNumber(i) + ".jpg";
		GLubyte *image = (GLubyte *) malloc(512 * 512 * sizeof(GLubyte) * channels);
		glBindTexture(GL_TEXTURE_2D, texDst[i]);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		jpge::compress_image_to_jpeg_file(path.c_str(),  512, 512, 3, image);
		glBindTexture(GL_TEXTURE_2D, 0);
		delete(image);
		*/
	}
	//LOG("Finish BlendAllAppearancesPairwise");
}

void SbmBlendTextures::BlendAllAppearances(GLuint FBODst, GLuint texDst, std::vector<float> weights, std::vector<GLuint> texIDs, GLuint program, int w, int h)
{
	const int MAX_SHAPES = 50;

	unsigned int numberOfShapes	= weights.size();

	if(numberOfShapes > MAX_SHAPES) {
		std::cerr << "ERROR: SbmBlendTextures::BlendAllAppearances can't handle more than 50 shapes" << "\n";
	}

	std::vector<float> weights_up_to_50(weights);
	weights_up_to_50.resize(MAX_SHAPES);
	float * weights_array = &(weights_up_to_50[0]);

	GLint texIDs_array[MAX_SHAPES];
	for(int i=0; i<MAX_SHAPES; i++) {
		texIDs_array[i] = i;
	}

	glPushMatrix();
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBODst);                                                              // Bind the framebuffer object
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texDst, 0);              // Attach texture to FBO

		assert( glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT );
#if !defined(__ANDROID__)
		glPushAttrib(GL_ENABLE_BIT);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_LIGHTING);
			glMatrixMode (GL_PROJECTION);
			glPushMatrix();
				glLoadIdentity ();
				gluOrtho2D(-1, 1, -1, 1);
				glMatrixMode (GL_MODELVIEW);
				glPushAttrib(GL_VIEWPORT_BIT);
				glPushAttrib(GL_TEXTURE_BIT);
					glViewport(0, 0, w, h);
					glLoadIdentity ();

					glClearColor(1.0, 1.0, 1.0, 0);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					//GLuint texNeutralLoc	= glGetUniformLocation(program, "texNeutral");
					//GLuint texFvLoc		= glGetUniformLocation(program, "texFv");
					//GLuint texOpenLoc		= glGetUniformLocation(program, "texOpen");
					//GLuint texPBMLoc		= glGetUniformLocation(program, "texPBM");
					//GLuint texShChLoc		= glGetUniformLocation(program, "texShCh");
					//GLuint texWLoc		= glGetUniformLocation(program, "texW");
					//GLuint texWideLoc		= glGetUniformLocation(program, "texWide");
					GLuint uTextures		= glGetUniformLocation(program, "uTextures");

					//GLuint wNeutralLoc	= glGetUniformLocation(program, "wNeutral");
					//GLuint wFvLoc			= glGetUniformLocation(program, "wFv");
					//GLuint wOpenLoc		= glGetUniformLocation(program, "wOpen");
					//GLuint wPBMLoc		= glGetUniformLocation(program, "wPBM");
					//GLuint wShChLoc		= glGetUniformLocation(program, "wShCh");
					//GLuint wWLoc			= glGetUniformLocation(program, "wW");
					//GLuint wWideLoc		= glGetUniformLocation(program, "wWide");
					GLuint uWeights			= glGetUniformLocation(program, "uWeights");

					GLuint uNumberOfShapes	= glGetUniformLocation(program, "uNumberOfShapes");
				
					glUseProgram(program);
					glEnable(GL_TEXTURE_2D);

					//glUniform1f(wNeutralLoc, weights[0]);
					//glUniform1f(wFvLoc, weights[1]);
					//glUniform1f(wOpenLoc, weights[2]);
					//glUniform1f(wPBMLoc, weights[3]);
					//glUniform1f(wShChLoc, weights[4]);
					//glUniform1f(wWLoc, weights[5]);
					//glUniform1f(wWideLoc, weights[6]);
					glUniform1fv(uWeights, MAX_SHAPES, weights_array);

					glUniform1iv(uTextures, MAX_SHAPES, texIDs_array);

					glUniform1i(uNumberOfShapes, numberOfShapes);

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, texIDs[0]);
					//glUniform1i(texNeutralLoc, 0);

					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, texIDs[1]);
					//glUniform1i(texFvLoc, 1);

					glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, texIDs[2]);
					//glUniform1i(texOpenLoc, 2);

					glActiveTexture(GL_TEXTURE3);
					glBindTexture(GL_TEXTURE_2D, texIDs[3]);
					//glUniform1i(texPBMLoc, 3);

					glActiveTexture(GL_TEXTURE4);
					glBindTexture(GL_TEXTURE_2D, texIDs[4]);
					//glUniform1i(texShChLoc, 4);

					glActiveTexture(GL_TEXTURE5);
					glBindTexture(GL_TEXTURE_2D, texIDs[5]);
					//glUniform1i(texWLoc, 5);

					glActiveTexture(GL_TEXTURE6);
					glBindTexture(GL_TEXTURE_2D, texIDs[6]);
					//glUniform1i(texWideLoc, 6);

					glBegin(GL_QUADS);
						glTexCoord2f(0, 1);
						glVertex3f(-1.0f, 1.0f, -0.5f);

						glTexCoord2f(0, 0);
						glVertex3f(-1.0f, -1.0f, -0.5f);

						glTexCoord2f(1, 0);
						glVertex3f(1.0f, -1.0f, -0.5f);

						glTexCoord2f(1, 1);
						glVertex3f(1.0f, 1.0f, -0.5f);
					glEnd();

				glUseProgram(0);

				glPopAttrib();                          // Pops texture bit
				glDisable(GL_TEXTURE_2D);
				glPopAttrib();							// Pops viewport information
			glMatrixMode (GL_PROJECTION);
			glPopMatrix();                              // Pops ENABLE_BIT
			glMatrixMode (GL_MODELVIEW);
		glPopAttrib();
#endif
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);                                                                                                          // Bind the render buffer
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);         
		
		glActiveTexture(GL_TEXTURE0);
                                                                                                   // Bind the frame buffer object
	glPopMatrix();
}

void SbmBlendTextures::BlendTwoFBO(GLuint tex0, GLuint tex1, GLuint FBODst, GLuint texDst, float alpha, GLuint m_blendingProgram, int w, int h)
{
	glPushMatrix();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBODst);                                                              // Bind the framebuffer object
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texDst, 0);              // Attach texture to FBO

	assert( glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT );
#if !defined(__ANDROID__)
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glMatrixMode (GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity ();
		gluOrtho2D(-1, 1, -1, 1);
		glMatrixMode (GL_MODELVIEW);
		glPushAttrib(GL_VIEWPORT_BIT);
		glPushAttrib(GL_TEXTURE_BIT);
				glViewport(0, 0, w, h);
				glLoadIdentity ();

				glClearColor(1.0, 1.0, 1.0, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glColor3f(0.3f, 0.42f, 0.26f);

				GLuint t1Location		= glGetUniformLocation(m_blendingProgram, "tex0");
				GLuint t2Location		= glGetUniformLocation(m_blendingProgram, "tex1");
				GLuint outLocation		= glGetUniformLocation(m_blendingProgram, "out");
				GLuint alphaLocation	= glGetUniformLocation(m_blendingProgram, "alpha");

				glUseProgram(m_blendingProgram);
				glEnable(GL_TEXTURE_2D);

				glUniform1f(alphaLocation, alpha);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex0);
				glUniform1i(t1Location, 0);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, tex1);
				glUniform1i(t2Location, 1);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, texDst);
				glUniform1i(outLocation, 2);

				glBegin(GL_QUADS);
						glTexCoord2f(0, 1);
						glVertex3f(-1.0f, 1.0f, -0.5f);

						glTexCoord2f(0, 0);
						glVertex3f(-1.0f, -1.0f, -0.5f);

						glTexCoord2f(1, 0);
						glVertex3f(1.0f, -1.0f, -0.5f);

						glTexCoord2f(1, 1);
						glVertex3f(1.0f, 1.0f, -0.5f);
				glEnd();

				glUseProgram(0);

				glPopAttrib();                           // Pops texture bit
				glDisable(GL_TEXTURE_2D);

				glPopAttrib();							// Pops viewport information
				glMatrixMode (GL_PROJECTION);
		glPopMatrix();                                   // Pops ENABLE_BIT
		glMatrixMode (GL_MODELVIEW);

		glPopAttrib();
#endif
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);                                                                                                          // Bind the render buffer
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);                                                                                                            // Bind the frame buffer object
	glPopMatrix();
}

void SbmBlendTextures::BlendGeometryWithMasksFeedback( GLuint * FBODst, std::vector<float> weights, GLuint * texIDs, std::vector<std::string> texture_names, DeformableMeshInstance* meshInstance, GLuint program, glm::mat4x4 translation, glm::mat4x4 rotation )
{
#if 1
	DeformableMesh * _mesh		= meshInstance->getDeformableMesh();

	bool showMasks		= false;
	bool useMasks		= false;
	bool overlayMasks	= false;

	//if (meshInstance->isStaticMesh())
	{
		SmartBody::SBSkeleton* skel = meshInstance->getSkeleton();
		SmartBody::SBPawn* pawn		= skel->getPawn();
		SmartBody::SBAttribute* maskAttribute;
		// Checks showMasks attribute: If masks for the blendshape are present, user can enable or disable the visualization for debugging purposes
		maskAttribute = pawn->getAttribute("blendShape.showMasks");
		if (!maskAttribute)
		{
			showMasks = false;
		}
		else
		{
			showMasks	= pawn->getBoolAttribute("blendShape.showMasks");
		}

		// Checks useMasks attribute: If masks for the blendshape are present, user can enable or disable the use of them
		maskAttribute = pawn->getAttribute("blendShape.useMasks");
		if (!maskAttribute)
		{
			useMasks = false;
		}
		else
		{
			useMasks	= pawn->getBoolAttribute("blendShape.useMasks");
		}

		// Checks overlayMasks attribute
		maskAttribute = pawn->getAttribute("blendShape.overlayMasks");
		if (!maskAttribute)
		{
			overlayMasks = false;
		}
		else
		{
			overlayMasks	= pawn->getBoolAttribute("blendShape.overlayMasks");
		}
	}

	SbmBlendFace * aux = new SbmBlendFace();

	SbmShaderProgram::printOglError("Setup Transform Feedback begin");
	std::vector<SrVec> blendedVerties(_mesh->posBuf.size());
	GLuint verticesFeedback;
	const char* attr[1]= {"outPos"};
	glGenBuffers( 1, &verticesFeedback);
	SbmShaderProgram::printOglError("Setup Transform Feedback glGenBuffer");
	glBindBuffer( GL_ARRAY_BUFFER, verticesFeedback);
	SbmShaderProgram::printOglError("Setup Transform Feedback glBindBuffer");
	glBufferData( GL_ARRAY_BUFFER, blendedVerties.size() * sizeof(SrVec), 0,  GL_DYNAMIC_READ);
	SbmShaderProgram::printOglError("Setup Transform Feedback glBufferData");
	glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, verticesFeedback );
	SbmShaderProgram::printOglError("Setup Transform Feedback glBufferBase");
	glTransformFeedbackVaryings( program, 1, attr, GL_INTERLEAVED_ATTRIBS );
	glLinkProgram(program);
	SbmShaderProgram::printOglError("Setup Transform Feedback end");
	aux->setDeformableMesh(_mesh);
	aux->buildVertexBufferGPU(weights.size());
	SbmShaderProgram::printOglError("Build vertex buffer end");
	SrSnModel* writeToBaseModel = NULL;
	SrSnModel* baseModel		= NULL;
	bool foundBaseModel			= false;

	std::vector<SrArray<SrPnt>*> shapes;

	SrArray<SrPnt> neutralV;
	SrArray<SrPnt> visemeV;

	// find the base shape and other shapes
	std::map<std::string, std::vector<SrSnModel*> >::iterator mIter;
	for (mIter = _mesh->blendShapeMap.begin(); mIter != _mesh->blendShapeMap.end(); ++mIter)
	{
		for (size_t i = 0; i < mIter->second.size(); ++i)
		{
			if (strcmp(mIter->first.c_str(), (const char*)mIter->second[i]->shape().name) == 0)
			{
				baseModel		= mIter->second[i];
				foundBaseModel	= true;
				break;
			}
		}
		if (baseModel == NULL)
		{
			LOG("original base model cannot be found");
			continue;
		}

		neutralV	= (baseModel->shape().V);

		// Copies reference to the shape vector
		shapes.push_back(&(baseModel->shape().V));

		for (size_t i = 0; i < mIter->second.size(); ++i)
		{
			if ((i == 0) ||(!mIter->second[i]))
			{
				continue;
			}

			visemeV = mIter->second[i]->shape().V;
			aux->addFace(mIter->second[i]);

			// Copies reference to the shape vector
			shapes.push_back(&(baseModel->shape().V));
		}
	}

	const int MAX_SHAPES = 14;	// I can't enable more than 16 attributes (15 vertex buffer + 1 texture coordinate buffer)
	// NOTE: Also change #define in shader if you change this value

	std::vector<float>	usedWeights;
	std::vector<int>	usedShapeIDs;

	
	glUseProgram(program);
	glEnable(GL_RASTERIZER_DISCARD);

	GLuint aVertexTexcoord	= glGetAttribLocation(program, "aVertexTexcoord");
	GLuint aVertexPosition	= glGetAttribLocation(program, "aVertexPosition");

	for(int i=0; i<weights.size(); i++)
	{
		// If it is the first weight (netural shape), or wieght is > 0.000, sends this shape to shader
		if(((weights[i] > 0.0001) && (usedWeights.size() < MAX_SHAPES)) || (i == 0))
		{
			GLuint vertexAttribLoc = aVertexPosition + usedWeights.size();
			//LOG("vertexAttribLoc = %d", vertexAttribLoc);
			glEnableVertexAttribArray(vertexAttribLoc);
			VBOVec3f* vbo = aux->getVBOPos(i);
			if (!vbo)
				continue;
			vbo->VBO()->BindBuffer();
			glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0,0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// Pushes this weight to the vector of used weights
			usedWeights.push_back(weights[i]);
			usedShapeIDs.push_back(i);
		}
	}
	SbmShaderProgram::printOglError("BlendGeometry glVertexAttributePointer");

	glEnableVertexAttribArray(aVertexTexcoord);
	aux->getVBOTexCoord()->VBO()->BindBuffer();
	glVertexAttribPointer(aVertexTexcoord, 2, GL_FLOAT, GL_FALSE, 0,0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint uWeights			= glGetUniformLocation(program, "uWeights");
	GLuint uBorderVertices	= glGetUniformLocation(program, "uBorderVertices");
	GLuint uNumberOfShapes	= glGetUniformLocation(program, "uNumberOfShapes");
	//GLuint uTranslate		= glGetUniformLocation(program, "uTranslate");
	//GLuint uRotate			= glGetUniformLocation(program, "uRotate");
	GLuint uNeutralSampler	= glGetUniformLocation(program, "uNeutralSampler");
	GLuint uUseMasks		= glGetUniformLocation(program, "uUseMasks");

	SbmShaderProgram::printOglError("BlendGeometry GetUniformLocation");
	int * image_array		= new int[MAX_SHAPES];
	float * w				= new float[usedWeights.size()];
	int * usedShapesID_array = new int[usedShapeIDs.size()];

	for(int i=0; i<MAX_SHAPES; i++)
	{
		if(i < usedWeights.size())
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, texIDs[usedShapeIDs[i]]);

			image_array[i]			= i;
			w[i]					= usedWeights[i];
			usedShapesID_array[i]	= usedShapeIDs[i];
		}
		// Textures not used, but we still need to pass 15 textures to the fragment shader for completeness
		else
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, texIDs[0]);
			image_array[i]	= i;
		}
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texIDs[0]);
	glUniform1i(uNeutralSampler, 0);


	glUniform1fv(uWeights, usedWeights.size(), w);
	glUniform1i(uNumberOfShapes, usedWeights.size());
	glUniform1iv(uNeutralSampler, MAX_SHAPES, image_array);
	glUniform1i(uBorderVertices,  14);
	glUniform1i(uUseMasks,  useMasks);
	
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, verticesFeedback);

	SbmShaderProgram::printOglError("BlendGeometry TransformFeedback Begin");
	glBeginTransformFeedback(GL_POINTS);
	SbmShaderProgram::printOglError("BlendGeometry TransformFeedback glBeginTransformFeedback");
	glDrawArrays(GL_POINTS, 0, blendedVerties.size());
	SbmShaderProgram::printOglError("BlendGeometry TransformFeedback glDrawArrays");
	glEndTransformFeedback();
	SbmShaderProgram::printOglError("BlendGeometry TransformFeedback End");

	//aux->subMeshTris[0]->VBO()->BindBuffer();
	//glDrawElements(GL_TRIANGLES, _mesh->triBuf.size()*3 , GL_UNSIGNED_INT,0);
	//aux->subMeshTris[0]->VBO()->UnbindBuffer();

	std::vector<SrVec>& deformPosBuf = meshInstance->_deformPosBuf;
	std::vector<SrVec> blendRestPos; blendRestPos.resize(deformPosBuf.size());
	// read the data back to buffer
	SbmShaderProgram::printOglError("BlendGeometry Feedback GetBuffer Begin");
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, blendRestPos.size()*sizeof(SrVec),&blendRestPos[0]);
	SbmShaderProgram::printOglError("BlendGeometry Feedback GetBuffer End");

	if (meshInstance->isStaticMesh())
	{
		deformPosBuf = blendRestPos;
	}
	else
		meshInstance->updateSkin(blendRestPos, deformPosBuf);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	aux->getVBOPos(0)->VBO()->UnbindBuffer();
	aux->getVBOPos(1)->VBO()->UnbindBuffer();
	aux->getVBOTexCoord()->VBO()->UnbindBuffer();

	for(int i=0; i<usedWeights.size(); i++)
	{
		glDisableVertexAttribArray(aVertexPosition + i);
	}

	glDisableVertexAttribArray(aVertexTexcoord);
	glDisable(GL_RASTERIZER_DISCARD);
	glUseProgram(0);

	glDeleteBuffers(1, &verticesFeedback);
	delete aux;

	SbmShaderProgram::printOglError("BlendGeometry FINAL");
#endif
}

void SbmBlendTextures::BlendTextureWithMasks(GLuint FBODst, GLuint FBOTex,std::vector<float> weights, GLuint * texIDs, std::vector<std::string> texture_names, DeformableMeshInstance* meshInstance, GLuint program, int width, int height )
{
	DeformableMesh * _mesh		= meshInstance->getDeformableMesh();

	bool showMasks		= false;
	bool useMasks		= false;
	bool overlayMasks	= false;

	//if (meshInstance->isStaticMesh())
	{
		SmartBody::SBSkeleton* skel = meshInstance->getSkeleton();
		SmartBody::SBPawn* pawn		= skel->getPawn();
		SmartBody::SBAttribute* maskAttribute;
		// Checks showMasks attribute: If masks for the blendshape are present, user can enable or disable the visualization for debugging purposes
		maskAttribute = pawn->getAttribute("blendShape.showMasks");
		if (!maskAttribute)
		{
			showMasks = false;
		}
		else
		{
			showMasks	= pawn->getBoolAttribute("blendShape.showMasks");
		}

		// Checks useMasks attribute: If masks for the blendshape are present, user can enable or disable the use of them
		maskAttribute = pawn->getAttribute("blendShape.useMasks");
		if (!maskAttribute)
		{
			useMasks = false;
		}
		else
		{
			useMasks	= pawn->getBoolAttribute("blendShape.useMasks");
		}

		// Checks overlayMasks attribute
		maskAttribute = pawn->getAttribute("blendShape.overlayMasks");
		if (!maskAttribute)
		{
			overlayMasks = false;
		}
		else
		{
			overlayMasks	= pawn->getBoolAttribute("blendShape.overlayMasks");
		}
	}



	const int MAX_SHAPES = 14;	// I can't enable more than 16 attributes (15 vertex buffer + 1 texture coordinate buffer)
	// NOTE: Also change #define in shader if you change this value

	std::vector<float>	usedWeights;
	std::vector<int>	usedShapeIDs;

	for(int i=0; i<weights.size(); i++)
	{
		// If it is the first weight (netural shape), or wieght is > 0.000, sends this shape to shader
		if(((weights[i] > 0.0001) && (usedWeights.size() < MAX_SHAPES)) || (i == 0))
		{
			// Pushes this weight to the vector of used weights
			usedWeights.push_back(weights[i]);
			usedShapeIDs.push_back(i);
		}
	}

	SbmShaderProgram::printOglError("BlendTextureWithMask glUseProgram");

	glUseProgram(program);
	SbmShaderProgram::printOglError("BlendTextureWithMask GetUniformLocation start");

	GLuint uWeights			= glGetUniformLocation(program, "uWeights");
	GLuint uNumberOfShapes	= glGetUniformLocation(program, "uNumberOfShapes");
	//GLuint uTranslate		= glGetUniformLocation(program, "uTranslate");
	GLuint uNeutralSampler	= glGetUniformLocation(program, "uNeutralSampler");
	GLuint uShowMasks		= glGetUniformLocation(program, "uShowMasks");
	GLuint uUseMasks		= glGetUniformLocation(program, "uUseMasks");
	GLuint uOverlayMasks	= glGetUniformLocation(program, "uOverlayMasks");
	GLuint uUsedShapeIDs	= glGetUniformLocation(program, "uUsedShapeIDs");

	SbmShaderProgram::printOglError("BlendTextureWithMask GetUniformLocation end");
	int * image_array		= new int[MAX_SHAPES];
	float * w				= new float[usedWeights.size()];
	int * usedShapesID_array = new int[usedShapeIDs.size()];

	for(int i=0; i<MAX_SHAPES; i++)
	{
		if(i < usedWeights.size())
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, texIDs[usedShapeIDs[i]]);

			image_array[i]			= i;
			w[i]					= usedWeights[i];
			usedShapesID_array[i]	= usedShapeIDs[i];
		}
		// Textures not used, but we still need to pass 15 textures to the fragment shader for completeness
		else
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, texIDs[0]);
			image_array[i]	= i;
		}
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texIDs[0]);
	glUniform1i(uNeutralSampler, 0);

	glUniform1fv(uWeights, usedWeights.size(), w);
	glUniform1iv(uUsedShapeIDs, usedShapeIDs.size(), usedShapesID_array);
	glUniform1i(uNumberOfShapes, usedWeights.size());
	glUniform1iv(uNeutralSampler, MAX_SHAPES, image_array);
	glUniform1i(uShowMasks,  showMasks);
	glUniform1i(uUseMasks,  useMasks);
	glUniform1i(uOverlayMasks,  overlayMasks);


	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBODst);                                                              // Bind the framebuffer object
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, FBOTex, 0);              // Attach texture to FBO
	assert( glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT );

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glPushAttrib(GL_VIEWPORT_BIT);
	glPushAttrib(GL_TEXTURE_BIT);
	glViewport(0, 0, width, height);
	glClearColor(1.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// render to FBO texture
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-1.0f, 1.0f, -0.5f);

	glTexCoord2f(0, 0);
	glVertex3f(-1.0f, -1.0f, -0.5f);

	glTexCoord2f(1, 0);
	glVertex3f(1.0f, -1.0f, -0.5f);

	glTexCoord2f(1, 1);
	glVertex3f(1.0f, 1.0f, -0.5f);
	glEnd();

	glPopAttrib();
	glPopAttrib();
	glPopAttrib();
	
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);                                                                                                          // Bind the render buffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);         
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);
}

#endif
