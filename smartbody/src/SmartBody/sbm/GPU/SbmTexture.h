#pragma once
#include <map>
#include <string>
#include <vector>
#include <sb/SBTypes.h>
#include <sr/sr_gl.h>

class SbmTexture;

typedef std::map<std::string,SbmTexture*> StrTextureMap;



class SbmTextureManager
{
public:
	enum { TEXTURE_DIFFUSE = 0, TEXTURE_NORMALMAP, TEXTURE_SPECULARMAP };
protected:
	StrTextureMap textureMap;
	StrTextureMap normalTexMap;
	StrTextureMap specularTexMap;
private:
	static SbmTextureManager* _singleton;
	SbmTextureManager(void);
	~SbmTextureManager(void);
public:
	SBAPI static SbmTextureManager& singleton() 
	{
		if (!_singleton)
			_singleton = new SbmTextureManager();
		return *_singleton;			
	}

	SBAPI static void destroy_singleton() {
		if( _singleton )
			delete _singleton;
		_singleton = NULL;
	}	
	SBAPI SbmTexture* findTexture(int type, const char* textureName);
	SBAPI void loadTexture(int type, const char* textureName, const char* fileName);
	SBAPI void updateTexture();
	SBAPI void reloadTexture();
	SBAPI std::vector<std::string> getTextureNames(int type);

	// Creates a 1x1 white texture
	SBAPI void createWhiteTexture(const char* textureName);

	void releaseAllTextures();	
protected:
	StrTextureMap& findMap(int type);
};

class SbmTexture // simple place holder for OpenGL texture
{
protected:
	std::string textureName;
	std::string textureFileName;
	int width, height;
	int channels; // num of channels in the image	
	unsigned char* buffer;
	std::vector<unsigned char> imgBuffer;
	bool finishBuild;
	bool transparentTexture;
	GLuint texID;	
	GLuint internal_format, texture_format;		
public:
	SBAPI SbmTexture(const char* texName);
	SBAPI ~SbmTexture(void);
	SBAPI bool hasBuild() { return finishBuild; }
	SBAPI bool isTransparent() { return transparentTexture; }
	SBAPI const std::string& getName() { return textureName; }
	SBAPI const std::string& getFileName() { return textureFileName; }
	SBAPI GLuint getID() { return texID; }
	SBAPI bool loadImage(const char* fileName);	
	SBAPI void buildTexture(bool buildMipMap = true);

	SBAPI unsigned char* getBuffer();
	SBAPI int getBufferSize();
	SBAPI int getWidth() const { return width; }	
	SBAPI int getHeight() const { return height; }
	SBAPI int getNumChannels() const { return channels; }	

	SBAPI void setBuffer(unsigned char* buffer, int size);
	SBAPI void setTextureSize(int w, int h, int numChannels);

	// Creates a 1x1 white texture
	SBAPI void createWhiteTexture();

};
