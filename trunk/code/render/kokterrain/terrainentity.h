#pragma once
#ifndef KOK_TERRAINENTITY_H
#define KOK_TERRAINENTITY_H
//------------------------------------------------------------------------------
/**
    cTuo
*/
#include "core/singleton.h"
#include "graphics/graphicsentity.h"
#include "kokterrain/terraindef.h"
#include "kokterrain/managedterrain.h"
#include "coregraphics/vertexchunkpool.h"
#include "coregraphics/dynamicmeshpool.h"

//------------------------------------------------------------------------------
namespace KOK
{
class TerrainEntity : public Graphics::GraphicsEntity
{
public:
    DeclareClass(TerrainEntity);
	DeclareSingleton(TerrainEntity);

public:
    /// constructor
    TerrainEntity();
    /// destructor
    virtual ~TerrainEntity();
	/// set the model's resource id
	void SetResourceId(const Resources::ResourceId& resId);
	/// get the model's resource id
	const Resources::ResourceId& GetResourceId() const;

    virtual void OnActivate();
    virtual void OnDeactivate();
    virtual void OnUpdate();

	const Ptr<CoreGraphics::VertexChunkPool>& GetVertexChunkPool()const;
	const Ptr<CoreGraphics::DynamicMeshPool>& GetTerrainMeshPool()const;
protected:
	void CreateMeshPool();

	Resources::ResourceId resId;
	Ptr<ManagedTerrain> managedTerrain;
	/// vertex buffer,顶点缓冲大小固定
	Ptr<CoreGraphics::VertexChunkPool> terrVertexPool;
	/// index buffer
	Ptr<CoreGraphics::DynamicMeshPool> terrMeshPool;
};

//------------------------------------------------------------------------------
/**
*/
inline void
TerrainEntity::SetResourceId(const Resources::ResourceId& id)
{
	n_assert(!this->IsActive());
	this->resId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::ResourceId&
TerrainEntity::GetResourceId() const
{
	return this->resId;
}

inline const Ptr<CoreGraphics::VertexChunkPool>& 
TerrainEntity::GetVertexChunkPool()const
{
	return this->terrVertexPool;
}

inline const Ptr<CoreGraphics::DynamicMeshPool>&
TerrainEntity::GetTerrainMeshPool()const
{
	return this->terrMeshPool;
}


} // namespace Graphics
//------------------------------------------------------------------------------
#endif
    