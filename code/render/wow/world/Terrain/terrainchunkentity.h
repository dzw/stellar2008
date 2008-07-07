#pragma once
#ifndef GRAPHICS_TERRAINCHUNKENTITY_H
#define GRAPHICS_TERRAINCHUNKENTITY_H
//------------------------------------------------------------------------------
/**
    @class Graphics::ModelEntity
    
    Represents a visible graphics object.
    
    (C) 2007 Radon Labs GmbH
*/
#include "graphics/modelentity.h"
#include "resources/resourceid.h"
#include "wow/world/Terrain/terrainchunk.h"
//#include "wow/world/Terrain/managedchunk.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class TerrainChunkEntity : public ModelEntity
{
    DeclareClass(TerrainChunkEntity);
public:
    /// constructor
    TerrainChunkEntity();
    /// destructor
    virtual ~TerrainChunkEntity();

	/// 加载新的地型
	void UpdateChunk(const ChunkData& data);
    ///
    void LoadChunk(const Resources::ResourceId& resId, int chunkId);
    ///
    bool Update();
	///
	bool IsLoaded();
protected:
    /// called when entity is created
    virtual void OnActivate();
    /// called before entity is destroyed
    virtual void OnDeactivate();
    /// called when transform matrix changed
    virtual void OnTransformChanged();
    /// called to update the entity before rendering
    virtual void OnUpdate();
    /// called to render a debug visualization of the entity
    virtual void OnRenderDebug();
    /// validate the ModelInstance
    void ValidateModelInstance();

    //Resources::ResourceId resId;

	//Ptr<Models::ManagedChunk> managedChunk;
	Ptr<Models::TerrainChunk> terrain;
};

//------------------------------------------------------------------------------
/**
*/
inline bool 
TerrainChunkEntity::Update()
{
    if (this->terrain.isvalid() && this->terrain->GetState() == Resources::Resource::Pending)
        if (this->terrain->Load() == Resources::Resource::Loaded)
        {
            this->ValidateModelInstance();
            return true;
        }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
TerrainChunkEntity::IsLoaded()
{
	return this->terrain->GetState() == Resources::Resource::Loaded;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
TerrainChunkEntity::UpdateChunk(const ChunkData& data)
{
    terrain->UpdateChunks(data);
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
    