#pragma once
#ifndef KOK_KOKSHAPENODE_H
#define KOK_KOKSHAPENODE_H
//------------------------------------------------------------------------------
/**
    @class KOK::KokShapeNode

	子模型，Thing和Being都需要
	统一解析数据的地方，产生基本的顶点格式，根据具体使用情况，继承的子类做一些特别
	处理，如顶点改变（如摇动、骨骼动画等）

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

	virtual void LoadFromStream(const Ptr<IO::Stream>& stream, 
		int iVersion, bool bMirrorZ, bool bCompuiteDuplicateVertexCollection=false, bool bBlend=false);
	EThingSubMeshSpecialType GetThingType(Util::String name);

	//void Render();
	//void RenderBatch(IndexT index);
	void CreateMesh(const Util::Array<CoreGraphics::VertexComponent>& vertexComponents, int vertexSize);
	void CreateMaterial();
	/// 单独加载，纹理没有存放在一起，而是和模型放一起，导致很难确定路径，每次要根据不同情况传不同的值。
	/// fuck!being纹理放在一起，而建筑纹理又分开，太垃圾了！！！
	virtual void LoadTextures(const Util::String& path, int texId = -1);
	/// 创建顶点，格式可能不同，所以用了继承
	virtual void CreateVertexBuffer(const Ptr<IO::Stream>& stream, bool bMirrorZ);

	DWORD GetSkinWeightNum()const;
	cSkinWeights* GetSkinWeights()const;
	
	DWORD GetAttribuateTableSize()const;
	const AttributeRange* GetAttributeTable()const;

	DWORD GetMaterialSize()const;
	const cMaterial* GetMaterial()const;

	const Ptr<CoreGraphics::ShaderInstance>& GetShaderInstance()const;
	const Ptr<CoreGraphics::Mesh>& GetMesh()const;

	int GetEffectType()const;
	EThingSubMeshSpecialType GetModelType()const;
protected:
	friend class Thing;
	friend class BeingNode;
	friend class ThingNode;

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

inline int 
KokShapeNode::GetEffectType()const
{
	return this->effectType;
}

inline cSkinWeights* 
KokShapeNode::GetSkinWeights()const
{
	return this->skinWeights;
}

inline DWORD 
KokShapeNode::GetSkinWeightNum()const
{
	return this->skinWeightNum;
}

inline const Ptr<CoreGraphics::ShaderInstance>& 
KokShapeNode::GetShaderInstance()const
{
	return this->shaderInstance;
}

inline DWORD 
KokShapeNode::GetAttribuateTableSize()const
{
	return this->attributeTableSize;
}

inline const AttributeRange* 
KokShapeNode::GetAttributeTable()const
{
	return this->attributeTable;
}

inline DWORD 
KokShapeNode::GetMaterialSize()const
{
	return this->m_dwNumMaterial;
}

inline const cMaterial* 
KokShapeNode::GetMaterial()const
{
	return this->m_pMaterial;
}

inline const Ptr<CoreGraphics::Mesh>& 
KokShapeNode::GetMesh()const
{
	return this->mesh;
}

inline EThingSubMeshSpecialType 
KokShapeNode::GetModelType()const
{
	return this->modelType;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
