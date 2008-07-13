#pragma once
#ifndef TERRAIN_TERRAINTILE_H
#define TERRAIN_TERRAINTILE_H
//------------------------------------------------------------------------------
/**
	һ��adt�ļ���Դ����������ADT�ļ���
	һ������tile,����tile�ϵ���������

	�ڿɼ����ж�ʱ��������������һ�������ϴοɼ�chunk���б��������û�иı�ʱ
	����ֱ����Ⱦ�ϴα����chunk�б�
*/
#include "models/model.h"
#include "Terrain/terraindef.h"
#include "Terrain/terrainnode.h"

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
	/// 
	const Ptr<TerrainNode>& GetChunk(int x, int z);
	/// Add a chunk to render
	void AddRenderChunk(int x, int z);
	/// �����õģ���Ⱦ����tile
	void AddAllChunk();
	int GetX()const;
	int GetZ()const;
	void SetXZ(int x, int z);
	Math::float2 GetPos()const;
protected:
	/// 16*16��chunk
	//Ptr<TerrainChunk> chunk[TILECHUNKSIZE*TILECHUNKSIZE];
	/// buliding
	//Util::Array<Ptr<InteriorEntity> interiors;
    //Util::Array<Ptr<ModelEntity> models;
    //Util::Array<String> textureNames;
    //Util::Array<String> interiorNames;
    //Util::Array<String> modelNames;

	int x, z;
};

//------------------------------------------------------------------------------
/**
*/
inline int 
TerrainTile::GetX()const
{
	return this->x;
}

//------------------------------------------------------------------------------
/**
*/
inline int 
TerrainTile::GetZ()const
{
	return this->z;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
TerrainTile::SetXZ(int x, int z)
{
	this->x = x;
	this->z = z;
}

inline Math::float2 
TerrainTile::GetPos()const
{
	return Math::float2(this->x * TILESIZE, this->z * TILESIZE);
}

} // namespace Models
//------------------------------------------------------------------------------
#endif

    