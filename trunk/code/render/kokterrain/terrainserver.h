#pragma once
#ifndef KOK_TERRAINSERVER_H
#define KOK_TERRAINSERVER_H
//------------------------------------------------------------------------------
/**
    @class Terrain::TerrainServer
	
	º”‘ÿ°¢π‹¿ÌTerrain

    (C) ctuo
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "resources/resourceid.h"
#include "io/uri.h"
#include "graphics/cameraentity.h"
#include "graphics/stage.h"
#include "kokterrain/terrainentity.h"
#include "coregraphics/meshpool.h"
#include "coregraphics/dynamicmeshpool.h"

//------------------------------------------------------------------------------
namespace KOK
{
class TerrainServer : public Core::RefCounted
{
    DeclareClass(TerrainServer);
    DeclareSingleton(TerrainServer);
public:
    /// constructor
    TerrainServer();
    /// destructor
    virtual ~TerrainServer();


    /// open the model server
    void Open();
    /// close the model server
    void Close();
    /// return true if model server is open
    bool IsOpen() const;

	void LoadTerrain(const Resources::ResourceId resId);
	//const Ptr<CoreGraphics::MeshPool>& GetMeshPool()const;
	const Ptr<CoreGraphics::DynamicMeshPool>& GetTerrainMeshPool()const;
private:
	void CreateMeshPool();

	//Ptr<CoreGraphics::MeshPool> distMeshPool;
	Ptr<CoreGraphics::DynamicMeshPool> terrMeshPool;

	bool isOpen;
	Resources::ResourceId resId;
	Ptr<TerrainEntity> terrain;
};

inline const Ptr<CoreGraphics::DynamicMeshPool>&
TerrainServer::GetTerrainMeshPool()const
{
	return this->terrMeshPool;
}

//inline const Ptr<CoreGraphics::MeshPool>& 
//TerrainServer::GetMeshPool()const
//{
//	n_assert(this->distMeshPool.isvalid());
//	return this->distMeshPool;
//}

inline bool
TerrainServer::IsOpen()const
{
	return this->isOpen;
}
} // namespace Models
//------------------------------------------------------------------------------
#endif
