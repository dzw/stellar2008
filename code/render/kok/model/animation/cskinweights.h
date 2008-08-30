#pragma once
#ifndef KOK_SKINWEIGHTS_H
#define KOK_SKINWEIGHTS_H
//------------------------------------------------------------------------------
/**
    @class KOK::cSkinWeights
	
	cSkinWeights ����һ�����Ӱ��skined mesh's vertices,Ϊÿ���Ǽ���skined mesh�õ�
	����(mesh������)��Ӱ���ϵ.
	*cSkinWeights Contains an array of weight values mapped to a mesh's 
	vertices.

    (C) 2008 cTuo
*/    
#include "io/stream.h"

//------------------------------------------------------------------------------
namespace KOK
{

class cSkinWeights
{
private:
	// cSkinWeights count
	static unsigned int m_uiSkinWeightsCount;
	//  ��Ӱ��Ǽܵ�����
	char *m_szSkeletonName;       
	//  ��Ӱ��Ǽܵľ���
	D3DXMATRIXA16 m_SkeletonMatrix;
	//  ��Ӱ��Ǽܵ�Ӱ�춥������
	unsigned int	m_uiNumSkeletonInfluencedMeshVertices; 
	//  ��Ӱ��Ǽܵ�Ӱ�춥���б���������!!��
	DWORD* m_pdwSkeletonInfluencedMeshVerticesIndex;
	//  ��Ӱ��Ǽ�Ӱ�춥���Ӱ����� (0 ~ 1 ֮��)
	float* m_pfSkeletonInfluencedMeshVerticesWeight; 

public:
	cSkinWeights();
	virtual ~cSkinWeights();
	//  ȡ�ĸ�Ӱ��Ǽܵ�����
	char* getSkinWeightsSkeletonName();
	//  ȡ�ĸ�Ӱ��Ǽܵľ���
	D3DXMATRIXA16* getSkinWeightsSkeletonMatrix();
	//  ȡ�ĸ�Ӱ��Ǽܵ�Ӱ�춥������
	unsigned int	getSkinWeightsSkeletonInfluencedMeshVerticesSize(); 
	//  ȡ�ĸ�Ӱ��Ǽܵ�ĳ��Ӱ�춥��index
	DWORD* getSkinWeightsSkeletonInfluencedMeshVerticesIndex(unsigned int uiSkeletonInfluencedMeshVerticesIndex = 0); 
	//  ȡ�ĸ�Ӱ��Ǽܵ�ĳ��Ӱ�춥���Ӱ��Ȩ��
	float* getSkinWeightsSkeletonInfluencedMeshVerticesWeight(unsigned int uiSkeletonInfluencedMeshVerticesIndex = 0); 
	//  ���SkinWeights������file
	//int exportSkinWeightToFile(FILE *fp);
	//  import SkinWeights from file
	//int importSkinWeightToFile(FILE *fp);
	//  import SkinWeights from memory
	void importSkinWeightFromMemory(const Ptr<IO::Stream>& stream);
	// cSkinWeights count
	static unsigned int getSkinWeightsCount(){ return m_uiSkinWeightsCount; 
	}
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
