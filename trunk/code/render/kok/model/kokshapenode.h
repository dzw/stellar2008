#pragma once
#ifndef KOK_KOKSHAPENODE_H
#define KOK_KOKSHAPENODE_H
//------------------------------------------------------------------------------
/**
    @class KOK::KokShapeNode

	��ģ�ͣ�Thing��Being����Ҫ
	ͳһ�������ݵĵط������������Ķ����ʽ�����ݾ���ʹ��������̳е�������һЩ�ر�
	�����綥��ı䣨��ҡ�������������ȣ�

	���ò��ʡ�����
	VB��IB
	������Ⱦ

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
	/// �������أ�����û�д����һ�𣬶��Ǻ�ģ�ͷ�һ�𣬵��º���ȷ��·����ÿ��Ҫ���ݲ�ͬ�������ͬ��ֵ��
	/// fuck!being�������һ�𣬶����������ַֿ���̫�����ˣ�����
	virtual void LoadTextures(const Util::String& path, int texId = -1);
	/// �������㣬��ʽ���ܲ�ͬ���������˼̳�
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

	/// ��������
	DWORD verticesBufferSize;
	VertexFVF* verticesBuffer;				// ҡ���ĵ�������ݣ�

	/// ��������
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

	/// ��ģ������
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
