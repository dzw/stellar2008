#pragma once
#ifndef INTERFACE_TERRAINTILESTREAM
#define INTERFACE_TERRAINTILESTREAM
//------------------------------------------------------------------------------
/**
    @class Interface::TerrainTileStream
    
    ���ز�����������chunk���ݡ�
        
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

protected:
    Ptr<ManagedTexture> CreateTextureResource(const String& name);

	Ptr<Terrain::TerrainTile> tile;


    /// ����texCoord��alphaCoord��ִֻ��һ��
	void InitGlobalVBOs();
	void fixnamen(char *name, SizeT len);
	void fixname(Util::String &name);

	SizeT mcnk_offsets[256], mcnk_sizes[256];

	/// mapnode
	Math::vector vmin, vmax, vcenter;
	/// mapchunk
	float xbase, ybase, zbase;
	float r;

	int nTextures;

	//float xbase, ybase, zbase;
	//float r;

	unsigned int areaID;

	bool haswater;
	bool visible;
	bool hasholes;
	float waterlevel;

	//unsigned int textures[4];
	unsigned int alphamaps[3];
	unsigned int shadow, blend;

	int animated[4];

	unsigned int vertices, normals;

	short *strip;
	int striplen;

	//Liquid *lq;

	Util::FixedArray<ChunkData> chunks;
	Util::Array<Util::String> textures;

	static bool coordCreated;
	static Math::float2 texCoord[mapbufsize];
	static Math::float2 alphaCoord[mapbufsize];
};

} // namespace Interface
//------------------------------------------------------------------------------
#endif
