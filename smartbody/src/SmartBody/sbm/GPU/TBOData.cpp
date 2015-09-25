#include "vhcl.h"
#if !defined(__FLASHPLAYER__)
#include "external/glew/glew.h"
#endif
#include "TBOData.h"
#include "SbmShader.h"
#include <cstring>
#include <cstdio>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))


TBOData::TBOData( char* name, ublas::vector<float>& Data )
{
	strcpy(m_Name,name);
	
	m_bUpdate = true;
	m_iTBO_ID = 0;
	CreateTBO(getPtr(Data),Data.size());
	CreateTexture(Data.size(),1,1);
	m_bUpdate = false;
}


TBOData::TBOData( char* name, int nSize )
{
	ublas::vector<float> tempData; tempData.resize(nSize);

	m_bUpdate = true;
	m_iTBO_ID = 0;
	CreateTBO(getPtr(tempData),tempData.size());
	CreateTexture(tempData.size(),1,1);
	m_bUpdate = false;

}

TBOData::TBOData( char* name, ublas::vector<Vec4f>& Data )
{
	strcpy(m_Name,name);
	
	m_bUpdate = true;
	m_iTBO_ID = 0;
	CreateTBO((float*)getPtr(Data),Data.size()*4);
	CreateTexture(Data.size(),1,4);
	m_bUpdate = false;
}

TBOData::~TBOData(void)
{
	if (m_iTBO_ID)
		glDeleteBuffers(1,&m_iTBO_ID);
	//printf("%10s (%d) is now destroied",m_Name,m_iTBO_ID);
}

void TBOData::CreateTBO( float* pData, int Size )
{	
	int size_tex_data = sizeof(GLfloat) * Size;	
	m_nDataSize = Size;
	glGenBuffers( 1, &m_iTBO_ID);
	glBindBuffer( GL_TEXTURE_BUFFER_EXT, m_iTBO_ID);	
	glBufferData( GL_TEXTURE_BUFFER_EXT, size_tex_data, pData, GL_DYNAMIC_DRAW);
	glBindBuffer( GL_TEXTURE_BUFFER_EXT, 0);	
}


void TBOData::CreateTexture( int iWidth, int iHeight, int nChan )
{	
	glGenTextures(1,&m_iTex_ID);
	glBindTexture(GL_TEXTURE_BUFFER_EXT, m_iTex_ID);	

	if (nChan == 1)
		glTexBufferEXT(GL_TEXTURE_BUFFER_EXT, GL_LUMINANCE32F_ARB , m_iTBO_ID); 	
	else if (nChan == 4)
		glTexBufferEXT(GL_TEXTURE_BUFFER_EXT, GL_RGBA32F_ARB, m_iTBO_ID);	

	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	m_nWidth = iWidth;
	m_nHeight = iHeight;
}

void TBOData::BindBufferToTexture()
{	
	glBindTexture(GL_TEXTURE_BUFFER_EXT, m_iTex_ID);	
}

void TBOData::DeleteTBO()
{

}

void TBOData::Debug( const char* tag /*= "TBO"*/, int nSize /*= 9*/, int iStart /*= 0*/ )
{
	float* testData;
	testData = new float[nSize];
	glBindBuffer(GL_TEXTURE_BUFFER_EXT, m_iTBO_ID);	
	float* pData = (float*) glMapBuffer(GL_TEXTURE_BUFFER_EXT,GL_READ_ONLY);
	for (int i=0;i<nSize;i++)
		testData[i] = pData[i+iStart];
	glUnmapBuffer(GL_TEXTURE_BUFFER_EXT);
	//glBindBuffer(GL_TEXTURE_BUFFER_EXT,0);

	printf("%s : ",tag);
	for (int i=0;i<nSize;i++)
		printf("%f ",testData[i]);
	printf("\n");
	delete [] testData;	
}

void TBOData::UpdateTBOData( float* pData )
{
	glBindBuffer(GL_TEXTURE_BUFFER_EXT, m_iTBO_ID);	
	float* pPBOData = (float*) glMapBuffer(GL_TEXTURE_BUFFER_EXT,GL_WRITE_ONLY);
	if (pPBOData)
		memcpy(pPBOData,pData,sizeof(float)*m_nDataSize);
	glUnmapBuffer(GL_TEXTURE_BUFFER_EXT);
}

void TBOData::UpdateTBOData( float* pData, int iStart, int nSize )
{
	glBindBuffer(GL_TEXTURE_BUFFER_EXT, m_iTBO_ID);	
	float* pPBOData = (float*) glMapBuffer(GL_TEXTURE_BUFFER_EXT,GL_WRITE_ONLY);
	memcpy(&pPBOData[iStart],pData,sizeof(float)*nSize);
	glUnmapBuffer(GL_TEXTURE_BUFFER_EXT);	
}

void TBOData::ReadOutData( float* pOutData, int nSize )
{
	glBindBuffer(GL_TEXTURE_BUFFER_EXT, m_iTBO_ID);	
	float* pData = (float*) glMapBuffer(GL_TEXTURE_BUFFER_EXT,GL_READ_ONLY);	
	memcpy(pOutData,pData,sizeof(float)*nSize);
	glUnmapBuffer(GL_TEXTURE_BUFFER_EXT);
}
