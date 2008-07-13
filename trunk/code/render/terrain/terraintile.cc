//------------------------------------------------------------------------------
//  model.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "Terrain/terraintile.h"

namespace Terrain
{
ImplementClass(Terrain::TerrainTile, 'TNTE', Models::Model);

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<TerrainNode>& 
TerrainTile::GetChunk(int x, int z)
{
	return this->nodes[x<<4+z].downcast<TerrainNode>();
	/*String nodeName;
	nodeName.Format("%d_%d", x, z);
	return LookupNode(nodeName).downcast<TerrainNode>();*/
}

inline void 
TerrainTile::AddRenderChunk(int x, int z)
{
	this->nodes[x<<4+z].downcast<TerrainNode>()->AddToRender();
}

void 
TerrainTile::AddAllChunk()
{
	for (SizeT i = 0; i < this->nodes.Size(); i++)
	{
		if (this->nodes[i].isvalid() && this->nodes[i]->GetResourceState() == Resources::Resource::Loaded)
			this->nodes[i].downcast<TerrainNode>()->AddToRender();
	}
}

} // namespace Models
