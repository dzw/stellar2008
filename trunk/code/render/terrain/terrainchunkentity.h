#pragma once
#ifndef TERRAIN_TERRAINCHUNKENTITY_H
#define TERRAIN_TERRAINCHUNKENTITY_H
//------------------------------------------------------------------------------
/**
    @class Graphics::TerrainChunkEntity
    
    һ�����ο�ʵ�������ڲü�����Ⱦ
    
    (C) 2007 Radon Labs GmbH
*/
#include "graphics/modelentity.h"
#include "resources/resourceid.h"
#include "Terrain/managedchunkcacha.h"
#include "terrain/worldserver.h"
//#include "wow/world/Terrain/managedchunk.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class TerrainChunkEntity : public Graphics::GraphicsEntity
{
    DeclareClass(TerrainChunkEntity);
public:
    /// constructor
    TerrainChunkEntity();
    /// destructor
    virtual ~TerrainChunkEntity();

	/// �����µĵ���
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

	//Ptr<ManagedChunkCacha> cacha;
	Ptr<ModelInstance> modelInstance;
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
    