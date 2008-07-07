#pragma once
#ifndef GRAPHICS_TERRAINENTITY_H
#define GRAPHICS_TERRAINENTITY_H
//------------------------------------------------------------------------------
/**
    @class Graphics::ModelEntity
    
    Represents a visible graphics object.
    
    (C) 2007 Radon Labs GmbH
*/
#include "graphics/modelentity.h"
#include "resources/resourceid.h"
//#include "wow/world/Terrain/managedterrain.h"
#include "models/modelinstance.h"
#include "wow/world/Terrain/managedmaptile.h"
#include "wow/world/Terrain/terrainchunkentity.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class TerrainEntity : public GraphicsEntity
{
    DeclareClass(TerrainEntity);
public:
    /// constructor
    TerrainEntity();
    /// destructor
    virtual ~TerrainEntity();

	/// 加载新的地型
	void ChangeTerrain(const Resources::ResourceId& resId, int x, int z);
	///
	int Update(int limitNum);
	///
	void SetPos(int x, int z);
	Math::float2 GetPos()const;
	int GetRow()const;
	int GetCol()const;
protected:
    /// called when entity is created
    virtual void OnActivate();
    /// called before entity is destroyed
    virtual void OnDeactivate();
    /// called to update the entity before rendering
    virtual void OnUpdate();
	/// validate the maptile
	void ValidateMapTile();

    Resources::ResourceId resId;

	Ptr<Models::ManagedMapTile> managedMapTile;
	/// 16*16个chunk
	Util::FixedArray<Ptr<TerrainChunkEntity>> terrainChunk;
	int x,z;
};

//------------------------------------------------------------------------------
/**
*/
inline void 
TerrainEntity::SetPos(int x, int z)
{
	this->x = x;
	this->z = z;
}

//------------------------------------------------------------------------------
/**
*/
inline Math::float2 
TerrainEntity::GetPos()const
{
	return Math::float2(this->x * TILESIZE, this->z * TILESIZE);
}

//------------------------------------------------------------------------------
/**
*/
inline int 
TerrainEntity::GetRow()const
{
	return this->x;
}

//------------------------------------------------------------------------------
/**
*/
inline int 
TerrainEntity::GetCol()const
{
	return this->z;
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
    