//------------------------------------------------------------------------------
//  BeingNode.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/beingnode.h"
#include "kok/model/beingnodeinstance.h"
#include "coregraphics/shaderserver.h"

namespace KOK
{
ImplementClass(KOK::BeingNode, 'BENE', KOK::KokShapeNode);

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
BeingNode::BeingNode()
{
}

//------------------------------------------------------------------------------
/**
*/
BeingNode::~BeingNode()
{
	
}

//------------------------------------------------------------------------------
/**
*/
Resource::State
BeingNode::GetResourceState() const
{
	return Resource::Loaded;
}

//------------------------------------------------------------------------------
/**
*/
void
BeingNode::LoadResources()
{
    // call parent class
    TransformNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
BeingNode::UnloadResources()
{    
    KokShapeNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelNodeInstance> 
BeingNode::CreateNodeInstance()const
{
	Ptr<ModelNodeInstance> instance = (ModelNodeInstance*)BeingNodeInstance::Create();
	return instance;
}

//------------------------------------------------------------------------------
/**
*/
bool
BeingNode::ApplySharedState()
{
	//ShaderServer* shdServer = ShaderServer::Instance();
	//shdServer->SetFeatureBits(shdServer->FeatureStringToMask("Skin"));
	
	return KokShapeNode::ApplySharedState();
}

//------------------------------------------------------------------------------
/**
*/
void
BeingNode::LoadFromStream(const Ptr<Stream>& stream, 
							 int iVersion, 
							 bool bMirrorZ, 
							 bool bCompuiteDuplicateVertexCollection,
							 bool bBlend)
{
	//this->verticesBuffer = (VertexFVF*)n_new_array(VertexSkinnedFVF, this->verticesBufferSize);
	KokShapeNode::LoadFromStream(stream, iVersion, bMirrorZ, bCompuiteDuplicateVertexCollection, bBlend);

	// �������¼��������������Ķ�������
	
	if (this->verticesBufferSize <= 0 || this->skinWeightNum <= 0)
		return;
	
	VertexSkinnedFVF* skinVertices = (VertexSkinnedFVF*)this->verticesBuffer;

	
	for (SizeT i = 0; i < this->skinWeightNum; i++)
	{
		cSkinWeights* skin = &skinWeights[i];
		DWORD*  vertexIndies = skin->getSkinWeightsSkeletonInfluencedMeshVerticesIndex();
		float* vertexWeights = skin->getSkinWeightsSkeletonInfluencedMeshVerticesWeight();
		SizeT vertexNum = skin->getSkinWeightsSkeletonInfluencedMeshVerticesSize();

		for (SizeT v = 0; v < vertexNum; v++)
		{
			// ȡ�ö�������(������������!!)
			int vertex  = vertexIndies[v];

			for (SizeT j = 0; j < 4; j++)
			{
				if (skinVertices[vertex].weights[j] == 0.0)
				{
					skinVertices[vertex].bones[j]   = i;
					skinVertices[vertex].weights[j] = vertexWeights[v];
					break;
				}
			}
		}
	}

	CreateMesh();
}

void
BeingNode::CreateVertexBuffer(const Ptr<Stream>& stream, bool bMirrorZ)
{
	bbox boundingbox;
	boundingbox.pmin = Math::vector(N_MAXREAL, N_MAXREAL, N_MAXREAL);
	boundingbox.pmax = Math::vector(N_MINREAL, N_MINREAL, N_MINREAL);

	VertexSkinnedFVF* vb = n_new_array(VertexSkinnedFVF, this->verticesBufferSize);
	for (SizeT i = 0; i < this->verticesBufferSize; i++)
	{
		stream->Read(&(vb[i].p), sizeof(Math::vector3));
		//this->addSubMeshDuplicateVertexCollections(i, bComputieDuplicateVertexCollection);
		stream->Read(&(vb[i].n), sizeof(Math::vector3));

		if (bMirrorZ)
		{
			vb[i].p.z = -vb[i].p.z;
			vb[i].n.z = -vb[i].n.z;
		}
		stream->Read(&(vb[i].tex), sizeof(float)*2);

		boundingbox.extend(point(vb[i].p.x,
			vb[i].p.y,
			vb[i].p.z));
	}
	this->SetBoundingBox(boundingbox);

	this->verticesBuffer = (VertexFVF*)vb;
}

void
BeingNode::CreateMesh()
{
	Util::Array<CoreGraphics::VertexComponent> vertexComponents;
	if (vertexComponents.Size() == 0)
	{
		vertexComponents.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3));
		vertexComponents.Append(VertexComponent(VertexComponent::Normal, 0, VertexComponent::Float3));
		//vertexComponents.Append(VertexComponent(VertexComponent::Color, 0, VertexComponent::Float4));
		vertexComponents.Append(VertexComponent(VertexComponent::TexCoord, 0, VertexComponent::Float2));
		vertexComponents.Append(VertexComponent(VertexComponent::SkinWeights, 0, VertexComponent::Float4));
		vertexComponents.Append(VertexComponent(VertexComponent::SkinJIndices, 0, VertexComponent::Float4));
	}

	KokShapeNode::CreateMesh(vertexComponents, sizeof(VertexSkinnedFVF));
}

//------------------------------------------------------------------------------
/**
	��װ��ʱ��Ҳ��Ҫ���û����¼�������
	ע�⣬���������
	being: ÿ����ģ�͵������Ŷ�����ͬ�ģ���װ��ʱ����Ҫ�ı���(iNo)
	thing: �����ǹ̶��ģ����Լ��ص�ʱ�����ú��ˣ�����texId==-1������Ҫ�ı�������
*/
void 
BeingNode::LoadTextures(const String& path, int texId)
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

		// �ٷ�����ͼ
		fileName.Format("%s%s%02d_s.dds", path.AsCharPtr(), m_pMaterial[i].m_pszTextName.AsCharPtr(), m_pMaterial[i].iNo);
		m_pMaterial[i].LoadFakeReflectTexture(fileName);
	}
}

}