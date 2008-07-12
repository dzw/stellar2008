#pragma once
#ifndef TERRAIN_TERRAINTILE_H
#define TERRAIN_TERRAINTILE_H
//------------------------------------------------------------------------------
/**
	һ��adt�ļ���Դ����������ADT�ļ���
	һ������tile,����tile�ϵ���������
*/
#include "resources/resource.h"
#include "terrain/terrainchunk.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class TerrainTile : public Models::Model
{
    DeclareClass(TerrainTile);
public:
	TerrainTile();
	~TerrainTile();

	virtual void Unload();

protected:
	/// 16*16��chunk
	//Ptr<TerrainChunk> chunk[TILECHUNKSIZE*TILECHUNKSIZE];
	/// buliding
	//Util::Array<Ptr<InteriorEntity> interiors;
    //Util::Array<Ptr<ModelEntity> models;
    //Util::Array<String> textureNames;
    //Util::Array<String> interiorNames;
    //Util::Array<String> modelNames;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

    