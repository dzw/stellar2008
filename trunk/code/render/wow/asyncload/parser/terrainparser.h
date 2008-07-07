#pragma once
#ifndef MODELS_MODELREADER_H
#define MODELS_MODELREADER_H
//------------------------------------------------------------------------------
/** 
    parse adt file

    解析ADT文件并将解析后的数据保存起来，等所有chunk都解析完成了再释放数据。
*/    
#include "io/streamreader.h"
#include "util/string.h"
#include "util/array.h"
#include "math/vector.h"
#include "math/float2.h"
//#include "render/wow/world/Terrain/maptile.h"
#include "render/wow/world/adt/maptile.h"

//------------------------------------------------------------------------------
namespace WOW
{
class TerrainParse : public IO::StreamReader
{
    DeclareClass(TerrainParse);
public:
	TerrainParse();
    /// destructor
    virtual ~TerrainParse();

	/// 解析ADT文件头
	void ParseData();
	/// 解析chunk
	void ParseChunk(int chunkId, ChunkData& chunk);
	void InitMapChunk(IndexT index, ChunkData& chunk);
	void ReadADT();

	///
	Util::FixedArray<ChunkData>& GetChunks(); 
protected:

	/// 创建texCoord，alphaCoord，只执行一次
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

} // namespace Models

//------------------------------------------------------------------------------
#endif
