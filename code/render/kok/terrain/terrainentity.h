#pragma once
#ifndef KOK_TERRAINENTITY_H
#define KOK_TERRAINENTITY_H
//------------------------------------------------------------------------------
/**
    cTuo
*/
#include "core/singleton.h"
#include "graphics/modelentity.h"
#include "kok/terrain/terraindef.h"
#include "kok/terrain/managedterrain.h"
#include "coregraphics/vertexchunkpool.h"
#include "coregraphics/dynamicmeshpool.h"

//------------------------------------------------------------------------------
namespace KOK
{
class TerrainEntity : public Graphics::ModelEntity
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
	const Ptr<CoreGraphics::IndexBufferPool>& GetIndexBufferPool()const;
	
	Ptr<CoreGraphics::DynamicMeshPool>	terrMeshPool;
protected:
	void CreateMeshPool();

	Resources::ResourceId				resId;
	Ptr<Terrain>						terrain;

	/// vertex buffer,顶点缓冲大小固定
	Ptr<CoreGraphics::VertexChunkPool>	terrVertexPool;
	/// index buffer
	Ptr<CoreGraphics::IndexBufferPool>	indexPool;

	
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

inline const Ptr<CoreGraphics::IndexBufferPool>&
TerrainEntity::GetIndexBufferPool()const
{
	return this->indexPool;
}


} // namespace Graphics
//------------------------------------------------------------------------------
#endif
    