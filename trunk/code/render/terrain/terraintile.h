#pragma once
#ifndef TERRAIN_TERRAINTILE_H
#define TERRAIN_TERRAINTILE_H
//------------------------------------------------------------------------------
/**
	һ��adt�ļ���Դ����������ADT�ļ���
	һ������tile,����tile�ϵ���������
*/
#include "resources/resource.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class TerrainTile : public Resources::Resource
{
    DeclareClass(TerrainTile);
public:
	TerrainTile();
	~TerrainTile();

	virtual void Unload();

	/// 16*16��chunk
	Util::FixedArray<Ptr<TerrainChunkEntity>> terrainChunk;
	/// buliding
	//Util::Array<Ptr<InteriorEntity> interiors;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

    