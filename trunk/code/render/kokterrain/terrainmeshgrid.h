#pragma once
#ifndef KOK_TERRAINMESHGRID_H
#define KOK_TERRAINMESHGRID_H
//------------------------------------------------------------------------------
/**
    @class KOK::TerrainMeshGrid
    
    (C) 2008 cTuo
*/
#include "core/refcounted.h"
#include "util/array.h"
#include "math/vector.h"
#include "io/stream.h"

//------------------------------------------------------------------------------
namespace KOK
{

class TerrainMeshGrid : public Core::RefCounted
{
    DeclareClass(TerrainMeshGrid);
public:
    /// constructor
    TerrainMeshGrid();
    /// destructor
    virtual ~TerrainMeshGrid();

	void Init(unsigned int grid_count_x,unsigned int grid_count);

	float CalcHeightWithCliffWater(int iMeshNodeX,int iMeshNodeZ,float fDiffNodeX,float fDiffNodeZ);
	float CalcHeightWithCliff(int iMeshNodeX,int iMeshNodeZ,float fDiffNodeX,float fDiffNodeZ);
	float CalcHeight(int iMeshNodeX,int iMeshNodeZ,float fDiffNodeX,float fDiffNodeZ);

	float CalcTileHeightWithCliffWater(int iMeshNodeX,int iMeshNodeZ);
	float CalcTileHeight(int iMeshNodeX,int iMeshNodeZ);

	void CalcGridNormals();

	Math::vector& GetGridNormal(int iMeshNodeX,int iMeshNodeZ);
	Math::vector& GetGridPosition(int iMeshNodeX,int iMeshNodeZ);
	BYTE GetGridWaterLevel(int iMeshNodeX,int iMeshNodeZ);

	void GetTileGridData( int iX, int iY, BYTE* pCliffType,BYTE* pCliffLevel, BYTE* pWaterLevel,BYTE* pTransLevel);

	void ImportCliffLevelData(const Ptr<IO::Stream>& stream, bool shrink);
	void ImportWaterLevelData(const Ptr<IO::Stream>& stream, bool shrink);
	void ImportTransLevelData(const Ptr<IO::Stream>& stream, bool shrink);
	void ImportMapHeightData(const Ptr<IO::Stream>& stream, bool shrink);
	void ImportCliffIdData(const Ptr<IO::Stream>& stream, bool shrink);

	bool Intersect(int iStartX,int iStartZ, Math::vector& rayDir,Math::vector& rayPos,Math::vector* vInter,float* dist);

private:
	int IX_QT(int x,int z);

	

	struct MeshNode 
	{
		int iHigh;			// ���θ߶�

		BYTE cCliffTableID;	// ���µ�ID
		BYTE cCliffLevel;	// ���µĲ㼶
		BYTE cWaterLevel;	// ˮ�صĲ㼶
		BYTE cTransLevel;	// б�µĲ㼶

		Math::vector vNorm;	// �õ�ķ�����
		Math::vector vPos;

		MeshNode()
		{
			iHigh          = 10;
			vNorm          = Math::vector(0.0f,1.0f,0.0f);
			vPos		   = Math::vector(0.0f,0.0f,0.0f);

			cCliffTableID  = 0x00 ;
			cCliffLevel    = 0x00 ;
			cWaterLevel    = 0x00 ;
			cTransLevel    = 0x00 ;
		};
	};
	/// ���������(257)
	unsigned int m_nGridCountX;
	Util::Array<MeshNode> m_MeshNodes;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
    