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

//------------------------------------------------------------------------------
/**
*/
TerrainEntity::TerrainEntity()
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
TerrainEntity::CreateTerrainCacha(SizeT num)
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
TerrainEntity::AddChunkToRender(const Ptr<TerrainChunk>& chunk)
{

}

} // namespace Graphics
