#pragma once
#ifndef KOK_DISTRICT_H
#define KOK_DISTRICT_H
//------------------------------------------------------------------------------
/**
    @class KOK::DistrictNode

    (C) 2008 cTuo
*/    
#include "models/nodes/statenode.h"
#include "models/modelnodeinstance.h"
#include "kokterrain/terraindef.h"
#include "kokterrain/terrainmeshgrid.h"

//------------------------------------------------------------------------------
namespace KOK
{
class DistrictNode : public Models::StateNode
{
    DeclareClass(DistrictNode);
public:
    /// constructor
    DistrictNode();
    /// destructor
    virtual ~DistrictNode();

    /// get overall state of contained resources (Initial, Loaded, Pending, Failed, Cancelled)
    virtual Resources::Resource::State GetResourceState() const;
    /// apply state shared by all my ModelNodeInstances
    virtual bool ApplySharedState();

	void SetTerrainMeshGrid(const Ptr<TerrainMeshGrid>& grid);
	const Ptr<TerrainMeshGrid>& GetTerrainMeshGrid()const;

	void SetPosition(int x, int z);
	int  GetX()const;
	int	 GetZ()const;
	int  GetTileSize()const;
	int  GetMapWide()const;
	float GetTilePosOffset()const;

	void CreateMeshData();
	void UpdateDrawTable(TerrainMeshData** pMeshDatas);

	/// called when resources should be loaded
    virtual void LoadResources();
protected:
	friend class DistrictNodeInstance;

    /// create a model node instance
	virtual Ptr<Models::ModelNodeInstance> CreateNodeInstance() const;
    
    /// called when resources should be unloaded
    virtual void UnloadResources();

	/// district×ø±ê
	int disX;
	int disZ;

	int tileStartX;
	int tileStartY;

	Ptr<TerrainMeshGrid> terrMeshGrid;
	
	/// render
	TileMesh*		vertices;
	int				vertexSize;
	WORD*			indices;
	int				indexSize;
	TileDrawTable*	drawTable;

	int				shadowIndexSize;
};

inline const Ptr<TerrainMeshGrid>& 
DistrictNode::GetTerrainMeshGrid()const
{
	return this->terrMeshGrid;
}

inline int  
DistrictNode::GetX()const
{
	return this->disX;
}

inline int	 
DistrictNode::GetZ()const
{
	return this->disZ;
}

inline void 
DistrictNode::SetTerrainMeshGrid(const Ptr<TerrainMeshGrid>& grid)
{
	this->terrMeshGrid = grid;
}

inline void 
DistrictNode::SetPosition(int x, int z)
{
	this->disX = x;
	this->disZ = z;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
