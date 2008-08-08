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
		int iHigh;			// 地形高度

		BYTE cCliffTableID;	// 悬崖的ID
		BYTE cCliffLevel;	// 悬崖的层级
		BYTE cWaterLevel;	// 水池的层级
		BYTE cTransLevel;	// 斜坡的层级

		Math::vector vNorm;	// 该点的法向量
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
	/// 顶点的数量(257)
	unsigned int m_nGridCountX;
	Util::Array<MeshNode> m_MeshNodes;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
    