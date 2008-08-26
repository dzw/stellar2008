//------------------------------------------------------------------------------
//  .cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/animation/cskinweights.h"

namespace KOK
{
//  cSkinWeights count
unsigned int cSkinWeights::m_uiSkinWeightsCount = 0;


//-----------------------------------------------------------------------------
// Name: cSkinWeights()
// Desc: cSkinWeights 类别的建构元
//-----------------------------------------------------------------------------
cSkinWeights::cSkinWeights()
{
	m_uiNumSkeletonInfluencedMeshVertices = 0;

	m_szSkeletonName = 0;
	m_pdwSkeletonInfluencedMeshVerticesIndex = 0;
	m_pfSkeletonInfluencedMeshVerticesWeight  = 0;

	D3DXMatrixIdentity(&m_SkeletonMatrix) ;
	//  increate cSkinWeights count which news cSkinWeights;
	m_uiSkinWeightsCount++;
}

//-----------------------------------------------------------------------------
// Name: ~cSkinWeights()
// Desc: cSkinWeights 类别的解构元
//-----------------------------------------------------------------------------
cSkinWeights::~cSkinWeights()
{
	n_delete_array( m_szSkeletonName );
	n_delete_array( m_pdwSkeletonInfluencedMeshVerticesIndex );
	n_delete_array( m_pfSkeletonInfluencedMeshVerticesWeight );
	//  decreate cSkinWeights count which deletes 
	cSkinWeights;
	m_uiSkinWeightsCount--;
}
//-----------------------------------------------------------------------------
// Name: getSkinWeightsSkeletonName()
// Desc:   取的该影响骨架的名称
//-----------------------------------------------------------------------------
char* cSkinWeights::getSkinWeightsSkeletonName()
{
	return m_szSkeletonName;
}
//-----------------------------------------------------------------------------
// Name: getSkinWeightsSkeletonMatrix()
// Desc:   取的该影响骨架的矩阵
//-----------------------------------------------------------------------------
D3DXMATRIXA16* cSkinWeights::getSkinWeightsSkeletonMatrix()
{
	return &m_SkeletonMatrix;
}
//-----------------------------------------------------------------------------
// Name: getSkinWeightsSkeletonInfluencedMeshVerticesSize()
// Desc:   取的该影响骨架的影响顶点总数
//-----------------------------------------------------------------------------
unsigned int cSkinWeights::getSkinWeightsSkeletonInfluencedMeshVerticesSize()
{
	return m_uiNumSkeletonInfluencedMeshVertices;
}
//-----------------------------------------------------------------------------
// Name: getSkinWeightsSkeletonInfluencedMeshVerticesSize()
// Desc:   取的该影响骨架的某个影响顶点index
//-----------------------------------------------------------------------------
DWORD* cSkinWeights::getSkinWeightsSkeletonInfluencedMeshVerticesIndex(unsigned 
	int uiSkeletonInfluencedMeshVerticesIndex)
{
	if( uiSkeletonInfluencedMeshVerticesIndex >= 0 && 
		uiSkeletonInfluencedMeshVerticesIndex < m_uiNumSkeletonInfluencedMeshVertices)
	{
		return &m_pdwSkeletonInfluencedMeshVerticesIndex
			[uiSkeletonInfluencedMeshVerticesIndex];
	}else{
		return 0;
	}
}
//-----------------------------------------------------------------------------
// Name: getSkinWeightsSkeletonInfluencedMeshVerticesSize()
// Desc:   取的该影响骨架的某个影响顶点的影响权重
//-----------------------------------------------------------------------------
float* cSkinWeights::getSkinWeightsSkeletonInfluencedMeshVerticesWeight
	(unsigned int uiSkeletonInfluencedMeshVerticesIndex)
{
	if( uiSkeletonInfluencedMeshVerticesIndex >= 0 && 
		uiSkeletonInfluencedMeshVerticesIndex < m_uiNumSkeletonInfluencedMeshVertices)
	{
		return &m_pfSkeletonInfluencedMeshVerticesWeight
			[uiSkeletonInfluencedMeshVerticesIndex];
	}else{
		return 0;
	}
}

//-----------------------------------------------------------------------------
// Name: exportSkinWeightToFile()
// Desc:   输出SkinWeights档案到file
//-----------------------------------------------------------------------------
//int cSkinWeights::exportSkinWeightToFile(FILE *fp)
//{
//	unsigned int i;
//	BYTE *pData;	  //buffer pointer
//	if(!fp) return -1;
//
//	// 写入Skin名称长度
//	int SkinNameLength = lstrlen(m_szSkeletonName);
//	pData =(BYTE *)&SkinNameLength;
//	fwrite(pData,sizeof(int),1,fp);
//
//	// 写入Skeleton名称
//	pData =(BYTE *)m_szSkeletonName;
//	fwrite(pData,sizeof(char)*SkinNameLength,1,fp);
//
//	// 写入Skin Weight 的资料大小
//	pData =(BYTE *)&m_uiNumSkeletonInfluencedMeshVertices;
//	fwrite(pData,sizeof(DWORD),1,fp);
//
//	// 写入Skin Weight
//	for(i=0 ; i < m_uiNumSkeletonInfluencedMeshVertices ; i++){
//		pData =(BYTE *)&m_pdwSkeletonInfluencedMeshVerticesIndex[i];
//		fwrite(pData,sizeof(DWORD),1,fp);
//	}
//
//	// 写入权重
//	for(i=0 ; i < m_uiNumSkeletonInfluencedMeshVertices ; i++){
//		pData =(BYTE *)&m_pfSkeletonInfluencedMeshVerticesWeight[i];
//		fwrite(pData,sizeof(DWORD),1,fp);
//	}
//
//	// matrix Offset 4*4
//	pData =(BYTE *)&m_SkeletonMatrix._11 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&m_SkeletonMatrix._12 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&m_SkeletonMatrix._13 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&m_SkeletonMatrix._14 ;
//	fwrite(pData,sizeof(float),1,fp);
//
//	pData =(BYTE *)&m_SkeletonMatrix._21 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&m_SkeletonMatrix._22 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&m_SkeletonMatrix._23 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&m_SkeletonMatrix._24 ;
//	fwrite(pData,sizeof(float),1,fp);
//
//	pData =(BYTE *)&m_SkeletonMatrix._31 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&m_SkeletonMatrix._32 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&m_SkeletonMatrix._33 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&m_SkeletonMatrix._34 ;
//	fwrite(pData,sizeof(float),1,fp);
//
//	pData =(BYTE *)&m_SkeletonMatrix._41 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&m_SkeletonMatrix._42 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&m_SkeletonMatrix._43 ;
//	fwrite(pData,sizeof(float),1,fp);
//	pData =(BYTE *)&m_SkeletonMatrix._44 ;
//	fwrite(pData,sizeof(float),1,fp);
//
//	return 0;
//}
//-----------------------------------------------------------------------------
// Name: importSkinWeightFromMemory()
// Desc:   import SkinWeights from memory
//-----------------------------------------------------------------------------
void cSkinWeights::importSkinWeightFromMemory(const Ptr<IO::Stream>& stream)
{
	// 读取Skeleton名称长度
	int SkinNameLength;

	stream->Read(&SkinNameLength, sizeof(int));

	// 读取Skeleton名称
	n_delete_array(m_szSkeletonName);
	if(SkinNameLength > 0)
	{
		m_szSkeletonName = n_new_array(char, SkinNameLength+1);
		stream->Read(m_szSkeletonName, sizeof(char)*SkinNameLength);
		m_szSkeletonName[SkinNameLength] = '\0';
	}

	n_delete_array( m_pdwSkeletonInfluencedMeshVerticesIndex );
	n_delete_array( m_pfSkeletonInfluencedMeshVerticesWeight );

	// 读取 Skin Weight 的资料大小
	stream->Read(&m_uiNumSkeletonInfluencedMeshVertices, sizeof(int));

	if(m_uiNumSkeletonInfluencedMeshVertices > 0)
	{
		m_pdwSkeletonInfluencedMeshVerticesIndex = n_new_array(DWORD, m_uiNumSkeletonInfluencedMeshVertices);
		m_pfSkeletonInfluencedMeshVerticesWeight = n_new_array(float, m_uiNumSkeletonInfluencedMeshVertices);

		// 直接复制就好了，为什么要用loop咧??
		stream->Read(m_pdwSkeletonInfluencedMeshVerticesIndex, sizeof(DWORD)*m_uiNumSkeletonInfluencedMeshVertices);
		stream->Read(m_pfSkeletonInfluencedMeshVerticesWeight, sizeof(float)*m_uiNumSkeletonInfluencedMeshVertices);
		// 读取 Skin Weight
		/*for( i = 0 ; i < m_uiNumSkeletonInfluencedMeshVertices ; i++)
		{
		m_pdwSkeletonInfluencedMeshVerticesIndex[i] = *((DWORD*)Buffer);
		Buffer += sizeof(DWORD);
		}

		// 读取权重
		for(i=0 ; i < m_uiNumSkeletonInfluencedMeshVertices ; i++)
		{
		m_pfSkeletonInfluencedMeshVerticesWeight[i] = *((float*)Buffer);
		Buffer += sizeof(float);
		}*/
	}
	// matrix Offset 4*4

	stream->Read(&m_SkeletonMatrix, sizeof(float)*16);
}
//-----------------------------------------------------------------------------
// Name: importSkinWeightToFile()
// Desc:   import SkinWeights from file
//-----------------------------------------------------------------------------
//int cSkinWeights::importSkinWeightToFile(FILE *fp)
//{
//	if(!fp) return -1;
//	unsigned int i;
//
//	// 读取Skin名称长度
//	int SkinNameLength;
//	fread(&SkinNameLength,sizeof(int), 1, fp);
//
//	// 读取Skin名称
//	n_delete_array(m_szSkeletonName);
//	if(SkinNameLength > 0)
//	{
//		m_szSkeletonName = _NEW char[SkinNameLength+1];
//		fread(m_szSkeletonName,sizeof(char)*SkinNameLength, 1, fp);
//		m_szSkeletonName[SkinNameLength] = '\0';
//	}
//
//	n_delete_array( m_pdwSkeletonInfluencedMeshVerticesIndex );
//	n_delete_array( m_pfSkeletonInfluencedMeshVerticesWeight );
//
//	// 读取 Skin Weight 的资料大小
//	fread(&m_uiNumSkeletonInfluencedMeshVertices,sizeof(int), 1, fp);
//
//	if(m_uiNumSkeletonInfluencedMeshVertices > 0)
//	{
//		m_pdwSkeletonInfluencedMeshVerticesIndex = _NEW DWORD
//			[m_uiNumSkeletonInfluencedMeshVertices];
//		m_pfSkeletonInfluencedMeshVerticesWeight = _NEW float
//			[m_uiNumSkeletonInfluencedMeshVertices];
//		// 读取 Skin Weight
//		for(i=0 ; i < m_uiNumSkeletonInfluencedMeshVertices ; i++){
//			fread(&m_pdwSkeletonInfluencedMeshVerticesIndex[i],sizeof(DWORD), 1, fp);
//		}
//		// 读取权重
//		for(i=0 ; i < m_uiNumSkeletonInfluencedMeshVertices ; i++){
//			fread(&m_pfSkeletonInfluencedMeshVerticesWeight[i],sizeof(float), 1, fp);
//		}
//	}
//	// matrix Offset 4*4
//
//	fread(&m_SkeletonMatrix._11 ,sizeof(float), 1, fp);
//	fread(&m_SkeletonMatrix._12 ,sizeof(float), 1, fp);
//	fread(&m_SkeletonMatrix._13 ,sizeof(float), 1, fp);
//	fread(&m_SkeletonMatrix._14 ,sizeof(float), 1, fp);
//
//	fread(&m_SkeletonMatrix._21 ,sizeof(float), 1, fp);
//	fread(&m_SkeletonMatrix._22 ,sizeof(float), 1, fp);
//	fread(&m_SkeletonMatrix._23 ,sizeof(float), 1, fp);
//	fread(&m_SkeletonMatrix._24 ,sizeof(float), 1, fp);
//
//	fread(&m_SkeletonMatrix._31 ,sizeof(float), 1, fp);
//	fread(&m_SkeletonMatrix._32 ,sizeof(float), 1, fp);
//	fread(&m_SkeletonMatrix._33 ,sizeof(float), 1, fp);
//	fread(&m_SkeletonMatrix._34 ,sizeof(float), 1, fp);
//
//	fread(&m_SkeletonMatrix._41 ,sizeof(float), 1, fp);
//	fread(&m_SkeletonMatrix._42 ,sizeof(float), 1, fp);
//	fread(&m_SkeletonMatrix._43 ,sizeof(float), 1, fp);
//	fread(&m_SkeletonMatrix._44 ,sizeof(float), 1, fp);
//
//	return 0;
//}

}