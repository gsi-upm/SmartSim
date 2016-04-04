#pragma once
#include "SbmShader.h"
#include <controllers/me_ct_ublas.hpp>
#include "gfx/vec3.h"
#include "gfx/vec4.h"

using namespace gfx;

class TBOData
{
public:
	GLuint m_iTBO_ID;
	GLuint m_iTex_ID;
	int m_nWidth, m_nHeight;
	int m_nDataSize;
	char m_Name[20];	
	bool m_bUpdate;
	
public:	
	TBOData(char* name, ublas::vector<float>& Data);	
	TBOData(char* name, ublas::vector<Vec4f>& Data);	
	TBOData(char* name, int nSize);
public:
	~TBOData(void);	
	void BindBufferToTexture();	

	void ReadOutData(float* pOutData, int nSize);
	
	void UpdateTBOData(float* pData);
	void UpdateTBOData(float* pData, int iStart, int nSize);
	void CreateTBO(float* pData, int Size);
	void CreateTexture(int iWidth, int iHeight, int nChan);
	void DeleteTBO();

	void Debug(const char* tag = "TBO", int nSize = 9, int iStart = 0);
};



