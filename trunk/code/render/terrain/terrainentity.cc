//------------------------------------------------------------------------------
//  modelentity.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "terrain/terrainentity.h"
#include "terrain/terrainnode.h"
#include "terrain/terrainnodeinstance.h"

namespace Terrain
{
ImplementClass(Terrain::TerrainEntity, 'TNTY', Graphics::GraphicsEntity);

using namespace Models;
using namespace Resources;

const Resources::ResourceId nodeName = "TerrainNode";

//------------------------------------------------------------------------------
/**
*/
TerrainEntity::TerrainEntity():
	,curChunkNum(0)
{
	
}

//------------------------------------------------------------------------------
/**
*/
TerrainEntity::~TerrainEntity()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
TerrainEntity::CreateTerrainCacha(SizeT num, const Ptr<Mesh>& mesh)
{
	this->curChunkNum = num;

	this->terrain = Model::Create();
	Ptr<ModelNode> node = TerrainNode::Create();
	node->SetName(nodeName);
	node->SetVertexBuffer(vertex);
	node->SetIndexBuffer(index);
	this->terrain->AttachNode(node);

	this->terrainInstance = this->terrain->CreateInstance();
}

//------------------------------------------------------------------------------
/**
*/
void 
TerrainEntity::AddChunkToRender(const Ptr<TerrainChunk>& chunk)
{
	Ptr<ModelNode> node = this->terrain->LookupNode(nodeName);
}

} // namespace Graphics
