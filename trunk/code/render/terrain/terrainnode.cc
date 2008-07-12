//------------------------------------------------------------------------------
//  modelnode.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "terrain/terrainnode.h"

namespace Terrain
{
ImplementClass(Terrain::TerrainNode, 'TNNE', Models::ModelNode);


//------------------------------------------------------------------------------
/**
*/
TerrainNode::TerrainNode()
{
    //this->shaderInstance = ShaderServer::Instance()->CreateShaderInstance(ResourceId("shd:terrain"));
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
TerrainNode::~TerrainNode()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
TerrainNode::ApplySharedState()
{
	// 提交缓冲中的顶点数据,在instance中使用这些数据渲染(参考ShapeNode::ApplySharedState)
	const Ptr<Mesh>& mesh = this->cache->GetMesh();
	mesh->ApplyPrimitives(this->primGroupIndex);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelNodeInstance> 
TerrainNode::CreateNodeInstance() const
{
	Ptr<ModelNodeInstance> newInst = (ModelNodeInstance*) TerrainNodeInstance::Create();
	return newInst;
}

void
TerrainNode::LoadResource()
{
    StateNode::LoadResource();

    // create a managed mesh resource
	/*ResourceId meshResId = this->GetString(Attr::MeshResourceId);
	this->primGroupIndex = this->GetInt(Attr::MeshGroupIndex);
	this->managedMesh = ResourceManager::Instance()->CreateManagedResource(Mesh::RTTI, meshResId).downcast<ManagedMesh>();
	n_assert(this->managedMesh.isvalid());*/

    // create texture
 //   ResourceId resId;
 //   
 //   // 纹理不能这样直接创建！使用纹理缓冲。
	//if (this->HasAttr(Attr::DiffMap0))
	//{
	//	resId = this->GetString(Attr::DiffMap0);
	//	tex[0] = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, resId).downcast<ManagedTexture>();
	//}
	//if (this->HasAttr(Attr::DiffMap1))
	//{
	//	resId = this->GetString(Attr::DiffMap1);
	//	tex[1] = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, resId).downcast<ManagedTexture>();
	//}
	//if (this->HasAttr(Attr::DiffMap2))
	//{
	//	tex[2] = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, resId).downcast<ManagedTexture>();
	//	resId = this->GetString(Attr::DiffMap2);
	//}
	//if (this->HasAttr(Attr::DiffMap3))
	//{
	//	resId = this->GetString(Attr::DiffMap3);
	//	tex[3] = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, resId).downcast<ManagedTexture>();
	//}

	//if (this->HasAttr(Attr::TexBlend0))
	//{
	//	resId = this->GetString(Attr::TexBlend0);
	//	tex[4] = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, resId).downcast<ManagedTexture>();
	//}
}

void 
TerrainNode::Render()
{
    WorldServer::Instance()->ApplyCache();
	RenderDevice::Instance()->SetPrimitiveGroup(this->primGroup);

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
	

	ShaderServer::Instance()->SetActiveShaderInstance(this->shaderInstance);
}

} // namespace Models