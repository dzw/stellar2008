#pragma once
#ifndef TERRAIN_TERRAINDEF_H
#define TERRAIN_TERRAINDEF_H
//------------------------------------------------------------------------------
/**
    @class 
      
    (C) 2008 by cTuo
*/

#include "math/bbox.h"
#include "math/vector.h"
#include "math/float2.h"
#include "util/string.h"

//------------------------------------------------------------------------------
namespace Terrain
{
	#define TILESIZE (533.33333f)
	#define CHUNKSIZE ((TILESIZE) / 16.0f)
	#define UNITSIZE (CHUNKSIZE / 8.0f)
	#define ZEROPOINT (32.0f * (TILESIZE))

	const int mapbufsize = 9*9 + 8*8;
	const int stripsize = 8*18 + 7*2;
	const float detail_size = 8.0f;
	const int samplerstripsize = 8*8*4*3;
	const int MAPTILECACHESIZE = 16;


	struct MapChunkHeader {
		uint32 flags;
		uint32 ix;
		uint32 iy;
		uint32 nLayers;
		uint32 nDoodadRefs;
		uint32 ofsHeight;
		uint32 ofsNormal;
		uint32 ofsLayer;
		uint32 ofsRefs;
		uint32 ofsAlpha;
		uint32 sizeAlpha;
		uint32 ofsShadow;
		uint32 sizeShadow;
		uint32 areaid;
		uint32 nMapObjRefs;
		uint32 holes;
		uint16 s1;
		uint16 s2;
		uint32 d1;
		uint32 d2;
		uint32 d3;
		uint32 predTex;
		uint32 nEffectDoodad;
		uint32 ofsSndEmitters;
		uint32 nSndEmitters;
		uint32 ofsLiquid;
		uint32 sizeLiquid;
		float  zpos;
		float  xpos;
		float  ypos;
		uint32 textureId;
		uint32 props;
		uint32 effectId;
	};

	/// 地形顶点格式（组织数据）
	struct TerrainChunkFVF
	{
		float x,y,z;
		float nx,ny,nz;
		Math::float2 tex;
		Math::float2 texblend;
	};

	// 保存每个maptile中16*16个chunk的信息，在terrainentity中加载这些chunk到指定的node
	struct ChunkData
	{
		TerrainChunkFVF* dataBuf;			// size=9*9+8*8
		int dataSize;

		unsigned char* blendbuf;	// TexBlend0	size=64*64*4
		int bufSize;
		bool isEmpty;				// 有时候没有混合纹理，需要用一张空纹理代替，避免shader出错，这个标志共享同一张空纹理，不要重复建立

		Util::String tex0;
		Util::String tex1;
		Util::String tex2;
		Util::String tex3;
		Util::String shaderName;	
		//Util::String meshName;
		//int groupIndex;
		Math::bbox box;
		Math::vector pos;
	};

	
} // namespace Models
//------------------------------------------------------------------------------
#endif
