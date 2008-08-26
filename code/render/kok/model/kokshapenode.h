#pragma once
#ifndef KOK_KOKSHAPENODE_H
#define KOK_KOKSHAPENODE_H
//------------------------------------------------------------------------------
/**
    @class KOK::KokShapeNode

	子模型，Thing和Being都需要

	设置材质、纹理
	VB、IB
	批次渲染

    (C) 2008 cTuo
*/    
#include "models/nodes/transformnode.h"
#include "models/modelnodeinstance.h"
#include "io/stream.h"
#include "kok/model/modeldef.h"
#include "kok/model/animation/cskinweights.h"
#include "kok/material/material.h"
#include "kok/material/cMaterialAnimator.h"
#include "coregraphics/mesh.h"
#include "coregraphics/shaderinstance.h"

//------------------------------------------------------------------------------
namespace KOK
{
class KokShapeNode : public Models::TransformNode
{
    DeclareClass(KokShapeNode);
public:
    /// constructor
    KokShapeNode();
    /// destructor
    virtual ~KokShapeNode();

    /// get overall state of contained resources (Initial, Loaded, Pending, Failed, Cancelled)
    virtual Resources::Resource::State GetResourceState() const;
    /// apply state shared by all my ModelNodeInstances
    virtual bool ApplySharedState();
	/// called when resources should be loaded
    virtual void LoadResources();

	void LoadFromStream(const Ptr<IO::Stream>& stream, 
		int iVersion, bool bMirrorZ, bool bCompuiteDuplicateVertexCollection=false, bool bBlend=false);
	EThingSubMeshSpecialType GetThingType(Util::String name);

	void Render();
	void RenderBatch(IndexT index);
	void CreateMesh();
	void CreateMaterial();
protected:
	friend class Thing;
	
	virtual Ptr<Models::ModelNodeInstance> CreateNodeInstance()const;
	virtual void UnloadResources();
	
	void ImportMaterialFromMemory( const Ptr<IO::Stream>& stream, int iIndex, int iVersion );
	void ImportMaterialAnimationFromMemory( const Ptr<IO::Stream>& stream, int iIndex );

	/// 顶点数据
	DWORD verticesBufferSize;
	VertexFVF* verticesBuffer;				// 摇动的地物（树，草）

	/// 索引数据
	DWORD indexBufferSize;
	WORD* indicesBuffer;

	/// skinweight
	DWORD skinWeightNum;
	cSkinWeights* skinWeights;

	/// extended mesh property
	DWORD effectType;
	float ribbonWidth;
	float ribbonLength;

	/// batch
	DWORD attributeTableSize;
	AttributeRange* attributeTable;

	/// material
	DWORD m_dwNumMaterial;
	cMaterial* m_pMaterial;
	cMaterialAnimator* m_pMaterialAnimator;

	Ptr<CoreGraphics::Mesh> mesh;

	/// shader & material
	Ptr<CoreGraphics::ShaderInstance> shaderInstance;
	Ptr<CoreGraphics::ShaderVariable> diffuseColor;
	Ptr<CoreGraphics::ShaderVariable> ambientColor;
	Ptr<CoreGraphics::ShaderVariable> specularColor;
	Ptr<CoreGraphics::ShaderVariable> emissiveColor;
	Ptr<CoreGraphics::ShaderVariable> diffMap;

	/// 子模型类型
	EThingSubMeshSpecialType modelType;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
