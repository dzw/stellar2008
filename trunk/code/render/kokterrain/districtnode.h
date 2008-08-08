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
#include "coregraphics/meshpool.h"
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

	void Update();
	/// ‰÷»æ«∞∑÷≈‰ª∫≥Â
	void AllocMeshPool();
	void FreeMeshPool(IndexT frame);

	void SetTerrainMeshGrid(const Ptr<TerrainMeshGrid>& grid);
	void SetPosition(int x, int z);
	int  GetX()const;
	int	 GetZ()const;
protected:
    /// create a model node instance
	virtual Ptr<Models::ModelNodeInstance> CreateNodeInstance() const;
    /// called when resources should be loaded
    virtual void LoadResources();
    /// called when resources should be unloaded
    virtual void UnloadResources();

	/// district◊¯±Í
	int disX;
	int disZ;

	Ptr<TerrainMeshGrid> terrMeshGrid;
	MeshTest vertices[DISTRICT_VERTS*DISTRICT_VERTS*4];

	int curGroup;
	IndexT frameIndex;
	Ptr<CoreGraphics::MeshPool> meshPool;
};

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
