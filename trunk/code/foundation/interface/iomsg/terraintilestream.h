#pragma once
#ifndef INTERFACE_TERRAINTILESTREAM
#define INTERFACE_TERRAINTILESTREAM
//------------------------------------------------------------------------------
/**
    @class Interface::TerrainTileStream
    
    加载并解析，生成chunk数据。
        
    (C) 2006 Radon Labs GmbH
*/
#include "interface/iomsg/iomessage.h"
#include "render/terrain/terraintile.h"

//------------------------------------------------------------------------------
namespace Interface
{
class TerrainTileStream : public IOMessage
{
    DeclareClass(TerrainTileStream);
    DeclareMsgId;

	///
	~TerrainTileStream();
	///
	void ParseData(const Ptr<Stream>& stream);
	///
	void TerrainTileStream::ParseChunk();
public:

	Ptr<Terrain::TerrainTile> tile;
};

} // namespace Interface
//------------------------------------------------------------------------------
#endif
