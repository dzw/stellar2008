//------------------------------------------------------------------------------
//  transformnode.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/world/Terrain/chunknode.h"
#include "wow/world/Terrain/chunknodeinstance.h"
#include "models/attributes.h"
#include "coregraphics/shaderserver.h"
#include "resources/resourcemanager.h"

#include "CoreGraphics/debugview.h"

namespace Models
{
ImplementClass(Models::ChunkNode, 'CHKN', Models::TransformNode);

using namespace Attr;
using namespace Math;
using namespace CoreGraphics;
using namespace Resources;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
ChunkNode::ChunkNode():
	primGroupIndex(InvalidIndex)
{
    this->shaderInstance = ShaderServer::Instance()->CreateShaderInstance(ResourceId("shd:terrain"));
    this->diffMap[0] = this->shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("DiffMap0"));
    this->diffMap[1] = this->shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("DiffMap1"));
    this->diffMap[2] = this->shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("DiffMap2"));
    this->diffMap[3] = this->shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("DiffMap3"));
    this->diffMap[4] = this->shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("TexBlend0"));
    for (IndexT i = 0; i < 5; i++)
        this->tex[i] = 0;
}

//------------------------------------------------------------------------------
/**
*/
ChunkNode::~ChunkNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelNodeInstance>
ChunkNode::CreateNodeInstance() const
{
    Ptr<ModelNodeInstance> newInst = (ModelNodeInstance*) ChunkNodeInstance::Create();
    return newInst;
}

//------------------------------------------------------------------------------
/**
*/
void
ChunkNode::SaveToAttrs(AttributeContainer& attrs)
{
    ModelNode::SaveToAttrs(attrs);
}

//------------------------------------------------------------------------------
/**
*/
void
ChunkNode::LoadResources()
{
	n_assert(!this->managedMesh.isvalid());
	//n_assert(InvalidIndex == this->primGroupIndex);
	n_assert(this->HasAttr(Attr::MeshResourceId));
	n_assert(this->HasAttr(Attr::MeshGroupIndex));

	DebugView::Instance()->Start();
	// create a managed mesh resource
	ResourceId meshResId = this->GetString(Attr::MeshResourceId);
	this->primGroupIndex = this->GetInt(Attr::MeshGroupIndex);
	this->managedMesh = ResourceManager::Instance()->CreateManagedResource(Mesh::RTTI, meshResId).downcast<ManagedMesh>();
	n_assert(this->managedMesh.isvalid());
	DebugView::Instance()->Stop("ChunkNode load", true);

    // create texture
    ResourceId resId;
    
	if (this->HasAttr(Attr::DiffMap0))
	{
		resId = this->GetString(Attr::DiffMap0);
		tex[0] = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, resId).downcast<ManagedTexture>();
	}
	if (this->HasAttr(Attr::DiffMap1))
	{
		resId = this->GetString(Attr::DiffMap1);
		tex[1] = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, resId).downcast<ManagedTexture>();
	}
	if (this->HasAttr(Attr::DiffMap2))
	{
		tex[2] = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, resId).downcast<ManagedTexture>();
		resId = this->GetString(Attr::DiffMap2);
	}
	if (this->HasAttr(Attr::DiffMap3))
	{
		resId = this->GetString(Attr::DiffMap3);
		tex[3] = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, resId).downcast<ManagedTexture>();
	}

	if (this->HasAttr(Attr::TexBlend0))
	{
		resId = this->GetString(Attr::TexBlend0);
		tex[4] = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, resId).downcast<ManagedTexture>();
	}
    
	// call parent class
	ModelNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
ChunkNode::UnloadResources()
{
	n_assert(this->managedMesh.isvalid());
	n_assert(this->primGroupIndex != InvalidIndex);

	// discard managed resource
	ResourceManager::Instance()->DiscardManagedResource(this->managedMesh.upcast<ManagedResource>());
	this->managedMesh = 0;
	this->primGroupIndex = InvalidIndex;

    for (IndexT i = 0; i < 5; i++)
	{
        this->diffMap[i] = 0;

		if (this->tex[i].isvalid())
			ResourceManager::Instance()->DiscardManagedResource(this->tex[i].upcast<ManagedResource>());
		this->tex[i] = 0;
	}

    this->shaderInstance->Discard();
    this->shaderInstance = 0;

	// call parent class
	ModelNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
bool
ChunkNode::ApplySharedState()
{
	n_assert(this->managedMesh.isvalid());
	n_assert(this->primGroupIndex != InvalidIndex);

	// first call parent class
	ModelNode::ApplySharedState();

	// setup the render device to render our mesh (maybe placeholder mesh
	// if asynchronous resource loading hasn't finished yet)
	const Ptr<Mesh>& mesh = this->managedMesh->GetMesh();    
	if (this->managedMesh->GetState() == Resource::Loaded)
	{
		mesh->ApplyPrimitives(this->primGroupIndex);

		String feature = "Terrain1";
		if (tex[0].isvalid())
			diffMap[0]->SetTexture(tex[0]->GetTexture());
		if (tex[1].isvalid())
		{
			diffMap[1]->SetTexture(tex[1]->GetTexture());
			feature = "Terrain2";
		}
		if (tex[2].isvalid())
		{
			diffMap[2]->SetTexture(tex[2]->GetTexture());
			feature = "Terrain3";
		}
		if (tex[3].isvalid())
		{
			diffMap[3]->SetTexture(tex[3]->GetTexture());
			feature = "Terrain4";
		}
		if (tex[4].isvalid())
			diffMap[4]->SetTexture(tex[4]->GetTexture());

		ShaderServer* shdServer = ShaderServer::Instance();
		shdServer->SetFeatureBits(shdServer->FeatureStringToMask(feature));
	}
	else
	{
		mesh->ApplyPrimitives(0);
	}

	ShaderServer::Instance()->SetActiveShaderInstance(this->shaderInstance);

	return true;
}

} // namespace Models
