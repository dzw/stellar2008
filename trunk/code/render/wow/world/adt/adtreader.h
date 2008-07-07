#pragma once
#ifndef WOW_MAPTILEREADER_H
#define WOW_MAPTILEREADER_H
//------------------------------------------------------------------------------
/**
    @class Models::MapTileReader
    
    (C) 2008 by ctuo
*/
#include "models/load/modelreader.h"
#include "util/stack.h"
//#include "wow/wmo/modelheaders.h"
//#include "wow/world/adt/adt.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/indexbuffer.h"
#include "coregraphics/primitivegroup.h"
#include "resources/managedmesh.h"
#include "wow/world/adt/maptile.h"
#include "graphics/modelentity.h"
#include "wow/world/terrain/maptile.h"

//------------------------------------------------------------------------------
namespace WOW
{
class MapTileReader : public Models::ModelReader
{
    DeclareClass(MapTileReader);
public:
    /// constructor
    MapTileReader();
    /// destructor
    virtual ~MapTileReader();

    /// begin reading from the stream
    virtual bool Open();
    /// end reading from the stream
    virtual void Close();
	/// parse resource and build model hierarchy
	virtual bool FillModel();

    /// set the read cursor to the first Model in the stream
    virtual bool SetToFirstModel();
    /// set the read cursor to the next Model in the stream
    virtual bool SetToNextModel();

	/// set the read cursor to the first ModelNode in the current Model
	virtual bool SetToFirstModelNode();
	/// set the read cursor to the next ModelNode in the current Model
	virtual bool SetToNextModelNode();

	/// 解析maptile头信息
	void ReadADT();
	/// 角析mapchunk
	void InitMapChunk(int x, int y);
	/// 创建模型(doodadset)
	void CreateModelInstance(const Util::String& m2Name, const Math::vector& vpos, const Math::vector& vdir, const float fscale, const float fw);
	/// 设置顶点
	void SetupVertexBuffer(const Math::vector* vb, const Math::vector* nor, int vertices);
	/// 手动创建纹理
	Util::String CreateTextureManual(const void* srcData, int srcDataSize);
	///
	void ReadPrimitiveGroups();
	///
	void SetupVertexComponents();
	///
	void SetupManagedMesh();
	///
	const Util::Array<ChunkData>& GetChunks();
private:

	/// 
	void fixnamen(char *name, SizeT len);
	///
	void fixname(Util::String &name);

	Util::String meshName;
	Util::String texName;
	/// 建立一张默认的纹理，避免没有ALPHA和SHADOW的时候重复建立
	Util::String defName;

	bool loadFinish;
	//Ptr<MapTile> maptile;

	Math::float2* texCoord, *alphaCoord;
	Util::Array<Util::String> textures;

	Ptr<CoreGraphics::VertexBuffer> vertexBuffer;
	Ptr<CoreGraphics::IndexBuffer> indexBuffer;
	Util::Array<CoreGraphics::PrimitiveGroup> primGroups;
	Util::Array<CoreGraphics::VertexComponent> vertexComponents;


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

	int ii, jj;
	SizeT mcnk_offsets[256], mcnk_sizes[256];

	Util::Array<ChunkData> chunks;
};

inline const Util::Array<ChunkData>& 
MapTileReader::GetChunks()
{
	return chunks;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
    