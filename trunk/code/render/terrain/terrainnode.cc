//------------------------------------------------------------------------------
//  modelnode.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "terrain/terrainnode.h"
#include "terrain/terrainnodeinstance.h"
#include "terrain/worldserver.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/shaderinstance.h"
#include "resources/managedtexture.h"
#include "util/string.h"

namespace Terrain
{
ImplementClass(Terrain::TerrainNode, 'TNNE', Models::ModelNode);

using namespace CoreGraphics;
using namespace Models;
using namespace Util;

bool TerrainNode::coordCreated = false;
Math::float2 TerrainNode::texCoord[mapbufsize];
Math::float2 TerrainNode::alphaCoord[mapbufsize];

//------------------------------------------------------------------------------
/**
*/
TerrainNode::TerrainNode()
{
	if (!TerrainNode::coordCreated)
		InitGlobalVBOs();

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

void 
TerrainNode::SetChunkHeaderData(void* data)
{
	this->headerData = data;
	MapChunkHeader* heard = this->headerData;

	this->x = heard->ix;
	this->z = heard->iy;
	this->posBase = vector(heard->xpos-1.0f + ZEROPOINT, heard->ypos, heard->zpos-1.0f + ZEROPOINT);
	this->areaId = heard->areaid;
	//this->boundingBox = heard->
}

//------------------------------------------------------------------------------
/**
*/
void 
TerrainNode::ApplySharedState()
{
	// 提交缓冲中的顶点数据,在instance中使用这些数据渲染(参考ShapeNode::ApplySharedState)
	//const Ptr<Mesh>& mesh = this->mesh->GetMesh();
	//mesh->ApplyPrimitives(this->primGroupIndex);
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
    StateNode::LoadResources();

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
	WorldServer::Instance()->ApplyCache();
	RenderDevice::Instance()->SetPrimitiveGroup(this->primGroup);
	RenderDevice::Instance()->Draw();
}

void 
TerrainNode::AddToRender()
{
	DWORD offset = WorldServer::Instance()->GetChunkCacha()->AddChunk(dataBuf);
	SetVertexOffsetInCache(offset);
}

//------------------------------------------------------------------------------
/**
*/
void 
TerrainNode::InitGlobalVBOs()
{
	float2 *vt;
	float tx,ty;

	// init texture coordinates for detail map:
	// 一个MapChunk的纹理坐标，细节纹理  9*9+8*8
	vt = texCoord;
	const float detail_half = 0.5f * detail_size / 8.0f;
	for (int j=0; j<17; j++) {
		for (int i=0; i<((j%2)?8:9); i++) {
			tx = detail_size / 8.0f * i;
			ty = detail_size / 8.0f * j * 0.5f;
			if (j%2) {
				// offset by half
				tx += detail_half;
			}
			*vt++ = float2(tx, ty);
		}
	}



	// init texture coordinates for alpha map:
	// alpha纹理
	vt = alphaCoord;
	const float alpha_half = 0.5f * 1.0f / 8.0f;
	for (int j=0; j<17; j++) {
		for (int i=0; i<((j%2)?8:9); i++) {
			tx = 1.0f / 8.0f * i;
			ty = 1.0f / 8.0f * j * 0.5f;
			if (j%2) {
				// offset by half
				tx += alpha_half;
			}
			//*vt++ = float2(tx*0.95f, ty*0.95f);

			/*const int divs = 32;
			const float inv = 1.0f / divs;
			const float mul = (divs-1.0f);
			*vt++ = float2(tx*(mul*inv), ty*(mul*inv));*/

			if (tx == 0.0f)
				tx += 0.02f;
			else
				tx *= 0.98f;

			if (ty == 0.0f)
				ty += 0.02f;
			else
				ty *= 0.98f;
			*vt++ = float2(tx, ty);
		}
	}
}

} // namespace Models