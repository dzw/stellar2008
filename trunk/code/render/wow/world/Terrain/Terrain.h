#pragma once
#ifndef WOW_TERRAIN_H
#define WOW_TERRAIN_H
//------------------------------------------------------------------------------
/**
    
*/
#include "models/model.h"
#include "wow/world/Terrain/maptile.h"
#include "coregraphics/vertexcomponent.h"
#include "coregraphics/primitivegroup.h"
#include "resources/managedmesh.h"

//------------------------------------------------------------------------------
namespace Models
{

class Terrain : public Model
{
    DeclareClass(Terrain);
public:
    /// constructor
    Terrain();
    /// destructor
    virtual ~Terrain();

	bool UpdateChunks(const Ptr<MapTile>& mapTile);
	bool CreateChunkNode(const Util::Array<ChunkData>& datas);
	void ReadPrimitiveGroups();
	void SetupVertexComponents();
	const Util::String CreateTextureManual(const void* srcData, int srcDataSize);
	const Util::String UpdateVertexBuffer(const Ptr<Resources::ManagedMesh>& managMesh, const Math::vector* vb, const Math::vector* nor, int vertices);
	void CreateMesh(Ptr<CoreGraphics::Mesh>& mesh);
private:
	int curNode;
	Util::Array<CoreGraphics::VertexComponent> vertexComponents;
	Util::Array<CoreGraphics::PrimitiveGroup> primGroups;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

    