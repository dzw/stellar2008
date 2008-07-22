#pragma once
#ifndef TERRAIN_WORLDSERVER_H
#define TERRAIN_WORLDSERVER_H
//------------------------------------------------------------------------------
/**
    @class Terrain::WorldServer
	
	加载、管理TerrainTile

    (C) ctuo
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "resources/resourceid.h"
#include "io/uri.h"
#include "graphics/cameraentity.h"
#include "graphics/stage.h"
#include "terrain/world.h"
#include "terrain/managedWorld.h"
#include "Terrain/terraindef.h"
//#include "terrain/Terrainentity.h"
#include "terrain/managedterraintile.h"
#include "terrain/TerrainChunkCache.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class WorldServer : public Core::RefCounted
{
    DeclareClass(WorldServer);
    DeclareSingleton(WorldServer);
public:
    /// constructor
    WorldServer();
    /// destructor
    virtual ~WorldServer();


    /// open the model server
    void Open();
    /// close the model server
    void Close();
    /// return true if model server is open
    bool IsOpen() const;


	/// set a camera for the world
	void SetCamera(const Ptr<Graphics::CameraEntity>& camera);
	

	/// load a managed World from URI
	void LoadWorld(const Resources::ResourceId& worldName);
	/// discard a managed model
	void UnloadWorld();
	/// get world resource
	const Ptr<World>& GetWorld()const;


	/// check if change current position
	void CheckTile(const Math::vector& pos);
	///
	void EnterTile(int x, int z);
	/// load terrain tile
	Ptr<ManagedTerrainTile>&  LoadTile(int x, int z);
	///
	bool CheckValidTile(int i, int j)const;
	/// 创建一个tile,可能需要为每个tile创建quadtree
	const Ptr<ManagedTerrainTile>& CreateTerrainTile(const Resources::ResourceId& resId, int x, int z);
	///
	void RemoveTerrainTile(const Ptr<ManagedTerrainTile>& tile);


	///
	const Ptr<TerrainChunkCache>& GetChunkCacha()const;
	void ApplyCache();


	void OnFrame();
	void UpdateVisiableChunk();

private:

    bool isOpen;
	bool oob;
	/// 上一帧位置
	Math::vector prePos;

	Ptr<TerrainChunkCache> chunkCache;
	Ptr<ManagedWorld> managedWorld;
	/// 当前地图 3*3
	Ptr<ManagedTerrainTile> curMaptile[3][3];
	/// 地图缓冲 4*4
	Ptr<ManagedTerrainTile> mapTileCache[MAPTILECACHESIZE];

	Ptr<Graphics::CameraEntity> camera;
	Ptr<Graphics::Stage> stage;
	/// 用于渲染所有可见块
	//Ptr<TerrainEntity> terrain;
	int cx, cz;
	Resources::ResourceId worldName;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
WorldServer::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<World>& 
WorldServer::GetWorld()const
{
	if (!isLoadedWorld)
		n_assert(0);
	return this->managedWorld->GetWorld();
}

//------------------------------------------------------------------------------
/**
*/
inline void 
WorldServer::SetCamera(const Ptr<Graphics::CameraEntity>& camera)
{
	this->camera = camera;
}

inline const Ptr<TerrainChunkCache>& 
WorldServer::GetChunkCacha()const
{
	return this->chunkCache;
}

inline void 
WorldServer::ApplyCache()
{
    return this->chunkCache->ApplyCache();
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
