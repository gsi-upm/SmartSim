#include "vhcl.h"
#if !defined(__FLASHPLAYER__) && !defined(__ANDROID__)
#include "external/glew/glew.h"
#endif
#include "SbmShader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <direct.h>
#endif
#include <vhcl.h>


#if !defined(ANDROID_BUILD) && !defined(SB_IPHONE)
/************************************************************************/
/* Shader program class                                                 */
/************************************************************************/


SbmShaderManager& SbmShaderManager::singleton() 
{
	if (!_singleton)
		_singleton = new SbmShaderManager();
	return *_singleton;			
}

void SbmShaderManager::destroy_singleton() {
	if( _singleton )
		delete _singleton;
	_singleton = NULL;
}

int SbmShaderManager::getShaderSupport() { return shaderSupport; }

SbmShaderProgram::SbmShaderProgram()
{
	vsID = -1; 
	fsID = -1;
	programID = -1;
	isBuilt = false;	
}


SbmShaderProgram::~SbmShaderProgram()
{
#if !defined(__FLASHPLAYER__)
	if (programID > 0 )
		glDeleteProgram(programID);
	if (vsID > 0)
		glDeleteShader(vsID);
	if (fsID > 0)
		glDeleteShader(fsID);
#endif
	isBuilt = false;
}


void SbmShaderProgram::initShaderProgram( const char* vsName, const char* fsName )
{
	// we can initialize the shader name first, before there is a opengl context
	if (vsName)
	{
		vsFilename = vsName;
		char *vs = NULL;		
		vs = textFileRead(vsName);	
		if (vs)
			vsShaderStr = vs;		
		free(vs);	

	}
	if (fsName)
	{
		fsFilename = fsName;
		char *fs = NULL;		
		fs = textFileRead(fsName);	
		if (fs)
			fsShaderStr = fs;		
		free(fs);	
	}
}

void SbmShaderProgram::initShaderProgramStr( const char* shaderVS, const char* shaderFS )
{
	if (shaderVS)
		vsShaderStr = shaderVS;
	if (shaderFS)
		fsShaderStr = shaderFS;
}

void SbmShaderProgram::buildShader()
{
#if !defined(__FLASHPLAYER__)
	// build the shader after there is an opengl context
	vsID = -1;
	fsID = -1;
	if (vsShaderStr.size() > 0)
	{
		LOG("Create Vertex Shader");
		vsID = glCreateShader(GL_VERTEX_SHADER);
		loadShaderStr(vsID,vsShaderStr.c_str());
		LOG("After Build Vertex Shader");
	}	
	
	if (fsShaderStr.size() > 0)
	{
		LOG("Create Fragment Shader");
		fsID = glCreateShader(GL_FRAGMENT_SHADER);
		loadShaderStr(fsID,fsShaderStr.c_str());
		LOG("After Build Fragment Shader");
	}

	printShaderInfoLog(vsID);
    printShaderInfoLog(fsID);
    
	programID = glCreateProgram();
	if (vsID != -1)
		glAttachShader(programID,vsID);
	if (fsID != -1)
		glAttachShader(programID,fsID);
	glLinkProgram(programID);
	printProgramInfoLog(programID);
	isBuilt = true;
	//printOglError("linkProgram");
#endif
}


void SbmShaderProgram::loadShader(GLuint sID,  const char* shaderFileName )
{
	char *vs = NULL;		
	vs = textFileRead(shaderFileName);	
	if (!vs) return;
	const char* vv = vs;
	loadShaderStr(sID,vv);
	free(vs);	
}


void SbmShaderProgram::loadShaderStr( GLuint sID, const char* shaderStr )
{		
	glShaderSource(sID, 1, &shaderStr,NULL);	
	glCompileShader(sID);
}


char * SbmShaderProgram::textFileRead(const char *fn )
{
	FILE *fp;
	char *content = NULL;
	int count=0;
	if (fn != NULL) {
		fp = fopen(fn,"rt");
		if (fp != NULL) {
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);
			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		} 
		else
		{
			LOG("*** ERROR: Could not open file %s", fn);
		}
	}
	return content;
}

void SbmShaderProgram::printShaderInfoLog( GLuint obj )
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		LOG("%s\n",infoLog);
		free(infoLog);
	}
}

void SbmShaderProgram::printProgramInfoLog( GLuint obj )
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		LOG("%s\n",infoLog);
		free(infoLog);
	}
}

void SbmShaderProgram::printOglError(const char* tag)
{
	GLenum glErr;
	int    retCode = 0;

	glErr = glGetError();
#if 1
	while (glErr != GL_NO_ERROR)
	{
#if !defined(__FLASHPLAYER__)
		LOG("glError %s: %s\n", tag,gluErrorString(glErr));
#endif
		retCode = 1;
		glErr = glGetError();
	}
#endif
}
/************************************************************************/
/* Shader Manager                                                       */
/************************************************************************/
SbmShaderManager* SbmShaderManager::_singleton = NULL;
int SbmShaderManager::shaderSupport = NO_GPU_SUPPORT;

SbmShaderManager::SbmShaderManager(void)
{
	viewer = NULL;
	shaderInit = false;
#ifdef __ANDROID__
	shaderSupport = SUPPORT_OPENGL_2_0;
#else
	shaderSupport = NO_GPU_SUPPORT;
#endif
}

void SbmShaderManager::setViewer( SrViewer* vw )
{
	if (vw == NULL)
	{
		shaderInit = false;		
	}
	viewer = vw;	
}

SbmShaderManager::~SbmShaderManager(void)
{
	std::map<std::string,SbmShaderProgram*>::iterator vi;
	for ( vi  = shaderMap.begin();
		  vi != shaderMap.end();
		  vi++)
	{
		SbmShaderProgram* program = vi->second;
		delete program;
	}
}

bool SbmShaderManager::initOpenGL()
{
	if (!viewer)
		return false;
	viewer->makeGLContext();
	return true;
}

bool SbmShaderManager::initGLExtension()
{	
	if (shaderInit && shaderSupport != NO_GPU_SUPPORT) // already initialize glew
		return true;

    if (shaderInit && shaderSupport == NO_GPU_SUPPORT)
        return false;

#if !defined(__FLASHPLAYER__) && !defined(__ANDROID__)
	if (!viewer)
		return false;
        static int counter = 0;

	//viewer->makeGLContext();

	glewInit();

/*
#if !defined(MAC_BUILD)	
    if (glewIsSupported("GL_VERSION_3_0"))
	{
        LOG("Ready for OpenGL 3.0\n");
		shaderInit = true;
		shaderSupport = SUPPORT_OPENGL_3_0;
		return true;
	}
    else 
#endif
	*/
	if (glewIsSupported("GL_VERSION_2_0") || glewIsSupported("GL_VERSION_3_0"))
	{
		LOG("Ready for OpenGL 2.0.\n");
		shaderInit = true; 
		shaderSupport = SUPPORT_OPENGL_2_0;
		return true;
	}
	else {
		//if (counter == 3)
		LOG("OpenGL 2.0 not supported. GPU Shader will be disabled.\n");
		//exit(1);
        counter++;
        if (counter == 3)
            shaderInit = true;
		return false;
    }
    //return false;
#elif defined(__ANDROID__)
	shaderInit = true;
	return true;
	return false;
#endif
}

void SbmShaderManager::addShader(const char* entryName, const char* vsName, const char* fsName, bool shaderFile )
{
    if (shaderSupport == NO_GPU_SUPPORT)
        return;

	std::string keyName = entryName;
	if (shaderMap.find(keyName) != shaderMap.end())
	{
		SbmShaderProgram* tempS = shaderMap[keyName];
		delete tempS;
	}
	
    SbmShaderProgram* program = new SbmShaderProgram();
	if (shaderFile)
		program->initShaderProgram(vsName,fsName);
	else
	{		
		program->initShaderProgramStr(vsName,fsName);
	}
	shaderMap[keyName] = program;
}

void SbmShaderManager::buildShaders()
{	
	std::map<std::string,SbmShaderProgram*>::iterator vi;
	for ( vi  = shaderMap.begin();
		  vi != shaderMap.end();
		  vi++)
	{
		SbmShaderProgram* program = vi->second;
		if (!program->finishBuild())
			program->buildShader();
	}
}

SbmShaderProgram* SbmShaderManager::getShader( const std::string& entryName )
{
	SbmShaderProgram* program = NULL;
	if (shaderMap.find(entryName) != shaderMap.end())
	{
		program = shaderMap[entryName];		
	}
	return program;	
}
#endif