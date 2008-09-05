//------------------------------------------------------------------------------
//  EquipNode.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/equip/equipnode.h"
#include "kok/model/equip/equipnodeinstance.h"
#include "coregraphics/shaderserver.h"
#include "kok/model/equip/equip.h"

namespace KOK
{
ImplementClass(KOK::EquipNode, 'EQNE', KOK::KokShapeNode);

using namespace IO;
using namespace Util;
using namespace Models;
using namespace Resources;
using namespace Math;
using namespace Memory;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
EquipNode::EquipNode():
	skeleton(0)
{
}

//------------------------------------------------------------------------------
/**
*/
EquipNode::~EquipNode()
{
	
}

//------------------------------------------------------------------------------
/**
*/
Resource::State
EquipNode::GetResourceState() const
{
	return Resource::Loaded;
}

//------------------------------------------------------------------------------
/**
*/
void
EquipNode::LoadResources()
{
    // call parent class
    TransformNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
EquipNode::UnloadResources()
{    
    KokShapeNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelNodeInstance> 
EquipNode::CreateNodeInstance()const
{
	/*if (skeleton == NULL)
	{
		const Ptr<Equip> equip = this->model.downcast<Equip>();
		skeleton = equip->GetSkeletonHierarchyRoot()->findSkeletonHierarchy((char*)this->GetName().Value().AsCharPtr());
	}*/

	Ptr<ModelNodeInstance> instance = (ModelNodeInstance*)EquipNodeInstance::Create();
	return instance;
}

//------------------------------------------------------------------------------
/**
*/
bool
EquipNode::ApplySharedState()
{
	//ShaderServer* shdServer = ShaderServer::Instance();
	//shdServer->SetFeatureBits(shdServer->FeatureStringToMask("Skin"));
	
	return KokShapeNode::ApplySharedState();
}

//------------------------------------------------------------------------------
/**
*/
void
EquipNode::LoadFromStream(const Ptr<Stream>& stream, 
							 int iVersion, 
							 bool bMirrorZ, 
							 bool bCompuiteDuplicateVertexCollection,
							 bool bBlend)
{
	//this->verticesBuffer = (VertexFVF*)n_new_array(VertexSkinnedFVF, this->verticesBufferSize);
	KokShapeNode::LoadFromStream(stream, iVersion, bMirrorZ, bCompuiteDuplicateVertexCollection, bBlend);

	CreateMesh();
}

void
EquipNode::CreateMesh()
{
	Util::Array<CoreGraphics::VertexComponent> vertexComponents;
	if (vertexComponents.Size() == 0)
	{
		vertexComponents.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3));
		vertexComponents.Append(VertexComponent(VertexComponent::Normal, 0, VertexComponent::Float3));
		//vertexComponents.Append(VertexComponent(VertexComponent::Color, 0, VertexComponent::Float4));
		vertexComponents.Append(VertexComponent(VertexComponent::TexCoord, 0, VertexComponent::Float2));
	}

	KokShapeNode::CreateMesh(vertexComponents, sizeof(VertexFVF));
}

//------------------------------------------------------------------------------
/**
	
*/
void 
EquipNode::LoadTextures(const String& path, int texId)
{
	if (path.Length() == 0)
		return;

	String fileName;
	for (int i = 0; i < m_dwNumMaterial; i++)
	{
		if (texId != -1)
			m_pMaterial[i].iNo = texId;
		fileName.Format("%s%s%02d.dds", path.AsCharPtr(), m_pMaterial[i].m_pszTextName.AsCharPtr(), m_pMaterial[i].iNo);
		m_pMaterial[i].LoadTexture(fileName);

		// ¼Ù·´¹âÌùÍ¼
		fileName.Format("%s%s%02d_s.dds", path.AsCharPtr(), m_pMaterial[i].m_pszTextName.AsCharPtr(), m_pMaterial[i].iNo);
		m_pMaterial[i].LoadFakeReflectTexture(fileName);
	}
}

matrix44 
EquipNode::GetTransform()
{
	matrix44 mat;
	
	if (skeleton)
	{
		D3DXMATRIXA16* m = skeleton->getSkeletonHierarchyMatrix();
		
		mat.set(float4(m->_11, m->_12, m->_13, m->_14),
			    float4(m->_21, m->_22, m->_23, m->_24),
				float4(m->_31, m->_32, m->_33, m->_34),
				float4(m->_41, m->_42, m->_43, m->_44));
	}

	mat = matrix44::identity();
	return mat;
}

}