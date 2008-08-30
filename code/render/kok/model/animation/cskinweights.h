#pragma once
#ifndef KOK_SKINWEIGHTS_H
#define KOK_SKINWEIGHTS_H
//------------------------------------------------------------------------------
/**
    @class KOK::cSkinWeights
	
	cSkinWeights 定义一个如何影响skined mesh's vertices,为每个骨架与skined mesh得点
	资料(mesh点资料)的影响关系.
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
	//  该影响骨架的名称
	char *m_szSkeletonName;       
	//  该影响骨架的矩阵
	D3DXMATRIXA16 m_SkeletonMatrix;
	//  该影响骨架的影响顶点总数
	unsigned int	m_uiNumSkeletonInfluencedMeshVertices; 
	//  该影响骨架的影响顶点列表（不是索引!!）
	DWORD* m_pdwSkeletonInfluencedMeshVerticesIndex;
	//  该影响骨架影响顶点的影响比重 (0 ~ 1 之间)
	float* m_pfSkeletonInfluencedMeshVerticesWeight; 

public:
	cSkinWeights();
	virtual ~cSkinWeights();
	//  取的该影响骨架的名称
	char* getSkinWeightsSkeletonName();
	//  取的该影响骨架的矩阵
	D3DXMATRIXA16* getSkinWeightsSkeletonMatrix();
	//  取的该影响骨架的影响顶点总数
	unsigned int	getSkinWeightsSkeletonInfluencedMeshVerticesSize(); 
	//  取的该影响骨架的某个影响顶点index
	DWORD* getSkinWeightsSkeletonInfluencedMeshVerticesIndex(unsigned int uiSkeletonInfluencedMeshVerticesIndex = 0); 
	//  取的该影响骨架的某个影响顶点的影响权重
	float* getSkinWeightsSkeletonInfluencedMeshVerticesWeight(unsigned int uiSkeletonInfluencedMeshVerticesIndex = 0); 
	//  输出SkinWeights档案到file
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
