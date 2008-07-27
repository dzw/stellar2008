#pragma once
#ifndef TERRAIN_TERRAINTILESTREAM
#define TERRAIN_TERRAINTILESTREAM
//------------------------------------------------------------------------------
/**
    @class Terrain::TerrainTileStream
    
    加载并解析，生成chunk数据。
        
    (C) 2006 Radon Labs GmbH
*/
#include "interface/iomsg/parsestream.h"
#include "terrain/terraintile.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class TerrainTileStream : public Interface::ParseStream
{
    DeclareClass(TerrainTileStream);
    //DeclareMsgId;

    TerrainTileStream();
	///
	~TerrainTileStream();
	///
	virtual void ParseData(const Ptr<IO::Stream>& stream);
	///
    void ParseChunk(const Ptr<IO::Stream>& stream, Ptr<TerrainNode>& node);
    //Ptr<ManagedTexture> CreateTextureResource(const String& name);

public:
	//Ptr<Resources::ManagedTexture> CreateTextureResource(const String& name);

	Ptr<TerrainTile> tile;

	SizeT mcnk_offsets[256], mcnk_sizes[256];

	/// mapnode
	//Math::vector vmin, vmax, vcenter;
	///// mapchunk
	//float xbase, ybase, zbase;
	//float r;

	//int nTextures;

	////float xbase, ybase, zbase;
	////float r;

	//unsigned int areaID;

	//bool haswater;
	//bool visible;
	//bool hasholes;
	//float waterlevel;

	////unsigned int textures[4];
	//unsigned int alphamaps[3];
	//unsigned int shadow, blend;

	//int animated[4];

	//unsigned int vertices, normals;

	//short *strip;
	//int striplen;

	////Liquid *lq;

	//Util::FixedArray<ChunkData> chunks;
	//Util::Array<Util::String> textures;

	
};

} // namespace Interface
//------------------------------------------------------------------------------
#endif
