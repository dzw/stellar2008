//------------------------------------------------------------------------------
//  terraininfo.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/terrain/terrainmeshgrid.h"
#include "kok/terrain/terraindef.h"
#include "memory/memory.h"

namespace KOK
{
ImplementClass(KOK::TerrainMeshGrid, 'TMGD', Core::RefCounted);

using namespace IO;
using namespace Util;
using namespace Math;
using namespace Memory;

//------------------------------------------------------------------------------
/**
*/
TerrainMeshGrid::TerrainMeshGrid()
{
}

//------------------------------------------------------------------------------
/**
*/
TerrainMeshGrid::~TerrainMeshGrid()
{
    
}

void 
TerrainMeshGrid::Init(unsigned int grid_count_x,unsigned int grid_count)
{
	m_nGridCountX=grid_count_x;
	m_MeshNodes.Reserve(grid_count);
}

//------------------------------------------------------------------------------
/**
	calc height
*/
float 
TerrainMeshGrid::CalcHeightWithCliffWater(int iMeshNodeX,int iMeshNodeZ,float fDiffNodeX,float fDiffNodeZ)
{
	float fTransHigh = CLIFF_HEIGHT / 2;
	if( fDiffNodeX + fDiffNodeZ <= 1.0f )
	{
		int iMeshNodeID = IX_QT( iMeshNodeX, iMeshNodeZ );
		float fHigh0 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE +
			( m_MeshNodes[iMeshNodeID].cCliffLevel - m_MeshNodes[iMeshNodeID].cWaterLevel ) * CLIFF_HEIGHT;
		if( m_MeshNodes[iMeshNodeID].cTransLevel == 2 )
		{
			// 在斜坡中心点
			fHigh0 += fTransHigh;
		}

		iMeshNodeID = IX_QT( iMeshNodeX, iMeshNodeZ + 1 );
		float fHigh1 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE +
			( m_MeshNodes[iMeshNodeID].cCliffLevel - m_MeshNodes[iMeshNodeID].cWaterLevel ) * CLIFF_HEIGHT;
		if( m_MeshNodes[iMeshNodeID].cTransLevel == 2 )
		{
			// 在斜坡中心点
			fHigh1 += fTransHigh ;
		}

		iMeshNodeID = IX_QT( iMeshNodeX + 1, iMeshNodeZ );
		float fHigh2 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE +
			( m_MeshNodes[iMeshNodeID].cCliffLevel - m_MeshNodes[iMeshNodeID].cWaterLevel ) * CLIFF_HEIGHT;
		if( m_MeshNodes[iMeshNodeID].cTransLevel == 2 )
		{
			// 在斜坡中心点
			fHigh2 += fTransHigh ;
		}

		return (fHigh0 + ( fHigh2 - fHigh0 ) * fDiffNodeX + ( fHigh1 - fHigh0 ) * fDiffNodeZ);
	}
	else
	{
		int iMeshNodeID = IX_QT( iMeshNodeX, iMeshNodeZ + 1 );
		float fHigh1 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE +
			( m_MeshNodes[iMeshNodeID].cCliffLevel - m_MeshNodes[iMeshNodeID].cWaterLevel ) * CLIFF_HEIGHT;
		if( m_MeshNodes[iMeshNodeID].cTransLevel == 2 )
		{
			// 在斜坡中心点
			fHigh1 += fTransHigh ;
		}

		iMeshNodeID = IX_QT( iMeshNodeX + 1, iMeshNodeZ );
		float fHigh2 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE +
			( m_MeshNodes[iMeshNodeID].cCliffLevel - m_MeshNodes[iMeshNodeID].cWaterLevel ) * CLIFF_HEIGHT;
		if( m_MeshNodes[iMeshNodeID].cTransLevel == 2 )
		{
			// 在斜坡中心点
			fHigh2 += fTransHigh ;
		}

		iMeshNodeID = IX_QT( iMeshNodeX + 1, iMeshNodeZ + 1 );
		float fHigh3 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE +
			( m_MeshNodes[iMeshNodeID].cCliffLevel - m_MeshNodes[iMeshNodeID].cWaterLevel ) * CLIFF_HEIGHT;
		if( m_MeshNodes[iMeshNodeID].cTransLevel == 2 )
		{
			// 在斜坡中心点
			fHigh3 += fTransHigh ;
		}

		return (fHigh3 + ( fHigh1 - fHigh3 ) * ( 1.0f - fDiffNodeX ) + ( fHigh2 - fHigh3 ) * ( 1.0f - fDiffNodeZ ));
	}
	return 0.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
TerrainMeshGrid::CalcHeightWithCliff(int iMeshNodeX,int iMeshNodeZ,float fDiffNodeX,float fDiffNodeZ)
{
	if( fDiffNodeX + fDiffNodeZ <= 1.0f )
	{
		int iMeshNodeID = IX_QT( iMeshNodeX, iMeshNodeZ );
		float fHigh0 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE +
			m_MeshNodes[iMeshNodeID].cCliffLevel * CLIFF_HEIGHT;

		iMeshNodeID = IX_QT( iMeshNodeX, iMeshNodeZ + 1 );
		float fHigh1 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE +
			m_MeshNodes[iMeshNodeID].cCliffLevel * CLIFF_HEIGHT;

		iMeshNodeID = IX_QT( iMeshNodeX + 1, iMeshNodeZ );
		float fHigh2 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE +
			m_MeshNodes[iMeshNodeID].cCliffLevel * CLIFF_HEIGHT;

		return (fHigh0 + ( fHigh2 - fHigh0 ) * fDiffNodeX + ( fHigh1 - fHigh0 ) * fDiffNodeZ);
	}
	else
	{
		int iMeshNodeID = IX_QT( iMeshNodeX, iMeshNodeZ + 1 );
		float fHigh1 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE +
			m_MeshNodes[iMeshNodeID].cCliffLevel * CLIFF_HEIGHT;

		iMeshNodeID = IX_QT( iMeshNodeX + 1, iMeshNodeZ );
		float fHigh2 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE +
			m_MeshNodes[iMeshNodeID].cCliffLevel * CLIFF_HEIGHT;

		iMeshNodeID = IX_QT( iMeshNodeX + 1, iMeshNodeZ + 1 );
		float fHigh3 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE +
			m_MeshNodes[iMeshNodeID].cCliffLevel * CLIFF_HEIGHT;

		return (fHigh3 + ( fHigh1 - fHigh3 ) * ( 1.0f - fDiffNodeX ) + ( fHigh2 - fHigh3 ) * ( 1.0f - fDiffNodeZ ));
	}
	return 0.0f;
}

float 
TerrainMeshGrid::CalcHeight(int iMeshNodeX,int iMeshNodeZ,float fDiffNodeX,float fDiffNodeZ)
{
	if( fDiffNodeX + fDiffNodeZ <= 1.0f )
	{
		int iMeshNodeID = IX_QT( iMeshNodeX, iMeshNodeZ );
		float fHigh0 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE;

		iMeshNodeID = IX_QT( iMeshNodeX, iMeshNodeZ + 1 );
		float fHigh1 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE;

		iMeshNodeID = IX_QT( iMeshNodeX + 1, iMeshNodeZ );
		float fHigh2 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE;

		return (fHigh0 + ( fHigh2 - fHigh0 ) * fDiffNodeX + ( fHigh1 - fHigh0 ) * fDiffNodeZ);
	}
	else
	{
		int iMeshNodeID = IX_QT( iMeshNodeX, iMeshNodeZ + 1 );
		float fHigh1 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE;

		iMeshNodeID = IX_QT( iMeshNodeX + 1, iMeshNodeZ );
		float fHigh2 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE;

		iMeshNodeID = IX_QT( iMeshNodeX + 1, iMeshNodeZ + 1 );
		float fHigh3 = m_MeshNodes[iMeshNodeID].iHigh * MESH_HIGH_SIZE;

		return (fHigh3 + ( fHigh1 - fHigh3 ) * ( 1.0f - fDiffNodeX ) + ( fHigh2 - fHigh3 ) * ( 1.0f - fDiffNodeZ ));
	}

	return 0.0f;
}

float 
TerrainMeshGrid::CalcTileHeightWithCliffWater(int iMeshNodeX,int iMeshNodeZ)
{
	int   l_iMapPoint = IX_QT(iMeshNodeX,iMeshNodeZ);
	float l_fHigh;
	float l_fTransHigh = 0.0f;
	if(m_MeshNodes[l_iMapPoint].cTransLevel == 0x02)
		l_fTransHigh = (CLIFF_HEIGHT /2) ;

	l_fHigh = (m_MeshNodes[l_iMapPoint].iHigh* MESH_HIGH_SIZE) +
		((m_MeshNodes[l_iMapPoint].cCliffLevel - m_MeshNodes[l_iMapPoint].cWaterLevel )* CLIFF_HEIGHT)+ l_fTransHigh;
	return l_fHigh;
}

float 
TerrainMeshGrid::CalcTileHeight(int iMeshNodeX,int iMeshNodeZ)
{
	int   l_iMapPoint = IX_QT(iMeshNodeX,iMeshNodeZ);
	float l_fHigh;

	l_fHigh = (m_MeshNodes[l_iMapPoint].iHigh* MESH_HIGH_SIZE);
	return l_fHigh;
}


        
//------------------------------------------------------------------------------
/**
	calc grid normals
*/
void 
TerrainMeshGrid::CalcGridNormals()
{
	float fSpace = (COMP) ;
	unsigned int i,j;

	vector3 *l_pNorm;

	float l_fHigh0,l_fHigh1,l_fHigh2,l_fHigh3,l_fHigh4;

	for(j=0;j<(m_nGridCountX-1);j++)
	{
		for(i=0;i<(m_nGridCountX-1);i++)
		{
			int l_iMapID = IX_QT(i+0,j+0) ;
			//jingjie add 2005.3.25
			l_pNorm = &m_MeshNodes[l_iMapID].vNorm;

			l_fHigh0 = (m_MeshNodes[l_iMapID].iHigh * MESH_HIGH_SIZE)+
				((m_MeshNodes[l_iMapID].cCliffLevel -
				m_MeshNodes[l_iMapID].cWaterLevel ) * CLIFF_HEIGHT);

			if((j+1) <= (int)(m_nGridCountX-1))
			{
				l_iMapID = IX_QT(i+0,j+1) ;
				l_fHigh1 = (m_MeshNodes[l_iMapID].iHigh * MESH_HIGH_SIZE)+
					((m_MeshNodes[l_iMapID].cCliffLevel -
					m_MeshNodes[l_iMapID].cWaterLevel   )* CLIFF_HEIGHT);

			} else l_fHigh1 = l_fHigh0 ;

			if((i+1) <= (int)(m_nGridCountX-1))
			{
				l_iMapID = IX_QT(i+1,j+0) ;
				l_fHigh2 = (m_MeshNodes[l_iMapID].iHigh * MESH_HIGH_SIZE)+
					((m_MeshNodes[l_iMapID].cCliffLevel -
					m_MeshNodes[l_iMapID].cWaterLevel ) * CLIFF_HEIGHT);
			} else l_fHigh2 = l_fHigh0 ;

			if((j-1) >= 0)
			{
				l_iMapID = IX_QT(i+0,j-1) ;
				l_fHigh3 = (m_MeshNodes[l_iMapID].iHigh * MESH_HIGH_SIZE)+
					((m_MeshNodes[l_iMapID].cCliffLevel -
					m_MeshNodes[l_iMapID].cWaterLevel ) * CLIFF_HEIGHT);
			} else l_fHigh3 = l_fHigh0 ;

			if((i-1) >= 0)
			{
				l_iMapID = IX_QT(i-1,j+0) ;
				l_fHigh4 = (m_MeshNodes[l_iMapID].iHigh * MESH_HIGH_SIZE)+
					((m_MeshNodes[l_iMapID].cCliffLevel -
					m_MeshNodes[l_iMapID].cWaterLevel ) * CLIFF_HEIGHT);
			} else l_fHigh4 = l_fHigh0 ;

			l_pNorm->x = l_fHigh4 - l_fHigh2;
			l_pNorm->z = l_fHigh3 - l_fHigh1;
			l_pNorm->y = fSpace;

			l_pNorm->norm();
			//D3DXVec3Normalize(l_pNorm, l_pNorm);
		}
	}
}


        
//------------------------------------------------------------------------------
/**
	get datas
*/
vector3& 
TerrainMeshGrid::GetGridNormal(int iMeshNodeX,int iMeshNodeZ)
{
	int iMeshNodeID=IX_QT(iMeshNodeX,iMeshNodeZ);
	n_assert(iMeshNodeID<(int)m_MeshNodes.Size());
	return m_MeshNodes[iMeshNodeID].vNorm;
}

vector3& 
TerrainMeshGrid::GetGridPosition(int iMeshNodeX,int iMeshNodeZ)
{
	int iMeshNodeID=IX_QT(iMeshNodeX,iMeshNodeZ);
	n_assert(iMeshNodeID<(int)m_MeshNodes.Size());
	return m_MeshNodes[iMeshNodeID].vPos;
}

BYTE 
TerrainMeshGrid::GetGridWaterLevel(int iMeshNodeX,int iMeshNodeZ)
{
	int iMeshNodeID=IX_QT(iMeshNodeX,iMeshNodeZ);
	n_assert(iMeshNodeID<(int)m_MeshNodes.Size());
	return m_MeshNodes[iMeshNodeID].cWaterLevel;
}

void 
TerrainMeshGrid::GetTileGridData(int iX, int iY, BYTE* pCliffType,BYTE* pCliffLevel, BYTE* pWaterLevel,BYTE* pTransLevel)
{
	int iNodeID1=IX_QT(iX,iY);
	int iNodeID2=IX_QT(iX+1,iY);
	int iNodeID3=IX_QT(iX,iY+1);
	int iNodeID4=IX_QT(iX+1,iY+1);

	if (pCliffType)
	{
		pCliffType[0] = m_MeshNodes[iNodeID1].cCliffTableID ;
		pCliffType[1] = m_MeshNodes[iNodeID2].cCliffTableID ;
		pCliffType[2] = m_MeshNodes[iNodeID3].cCliffTableID ;
		pCliffType[3] = m_MeshNodes[iNodeID4].cCliffTableID ;
	}

	if (pCliffLevel)
	{
		pCliffLevel[0] = m_MeshNodes[iNodeID1].cCliffLevel ;
		pCliffLevel[1] = m_MeshNodes[iNodeID2].cCliffLevel ;
		pCliffLevel[2] = m_MeshNodes[iNodeID3].cCliffLevel ;
		pCliffLevel[3] = m_MeshNodes[iNodeID4].cCliffLevel ;
	}

	if (pWaterLevel)
	{
		pWaterLevel[0] = m_MeshNodes[iNodeID1].cWaterLevel ;
		pWaterLevel[1] = m_MeshNodes[iNodeID2].cWaterLevel ;
		pWaterLevel[2] = m_MeshNodes[iNodeID3].cWaterLevel ;
		pWaterLevel[3] = m_MeshNodes[iNodeID4].cWaterLevel ;
	}

	if (pTransLevel)
	{
		pTransLevel[0] = m_MeshNodes[iNodeID1].cTransLevel ;
		pTransLevel[1] = m_MeshNodes[iNodeID2].cTransLevel ;
		pTransLevel[2] = m_MeshNodes[iNodeID3].cTransLevel ;
		pTransLevel[3] = m_MeshNodes[iNodeID4].cTransLevel ;
	}
}

//------------------------------------------------------------------------------
/**
	import data       
*/
void
TerrainMeshGrid::ImportCliffLevelData(const Ptr<Stream>& stream, bool shrink)
{
	MemoryMark mark(Memory::MemStack);

	int iCliffLevelSize = 0 ;
	stream->Read(&iCliffLevelSize, sizeof(int));

	BYTE *m_pTemp = new(Memory::MemStack, iCliffLevelSize)BYTE;
	stream->Read(m_pTemp, iCliffLevelSize*sizeof(BYTE));

	//----------------------------------------------------------------
	// 地编错误的资料读取
	int l_Wide = m_nGridCountX;
	if (shrink) l_Wide--;

	for(int i=0;i<(int)l_Wide;i++)
	{
		for(int j=0;j<(int)l_Wide;j++)
		{
			int l_iBadMapModeID = j+(i*l_Wide);
			int l_iMapModeID    = j+(i*m_nGridCountX);
			m_MeshNodes[l_iMapModeID].cCliffLevel = m_pTemp[l_iBadMapModeID];
		}
	}
}

void 
TerrainMeshGrid::ImportWaterLevelData(const Ptr<Stream>& stream, bool shrink)
{
	MemoryMark mark(Memory::MemStack);

	int iWaterLevelSize = 0 ;
	stream->Read(&iWaterLevelSize, sizeof(int));

	BYTE *m_pTemp = new(Memory::MemStack, iWaterLevelSize)BYTE;
	stream->Read(m_pTemp, iWaterLevelSize*sizeof(BYTE));


	int l_Wide = m_nGridCountX;
	if (shrink) l_Wide--;

	for(int i=0;i<(int)l_Wide;i++)
	{
		for(int j=0;j<(int)l_Wide;j++)
		{
			int l_iBadMapModeID = j+(i*l_Wide);
			int l_iMapModeID    = j+(i*m_nGridCountX);
			m_MeshNodes[l_iMapModeID].cWaterLevel = m_pTemp[l_iBadMapModeID];
		}
	}
}

void
TerrainMeshGrid::ImportTransLevelData(const Ptr<Stream>& stream, bool shrink)
{
	MemoryMark mark(Memory::MemStack);

	int iTransLevelSize = 0 ;
	stream->Read(&iTransLevelSize, sizeof(int));

	BYTE *m_pTemp = new(Memory::MemStack, iTransLevelSize)BYTE;
	stream->Read(m_pTemp, iTransLevelSize*sizeof(BYTE));

	//----------------------------------------------------------------
	//
	int l_Wide = m_nGridCountX;
	if (shrink) l_Wide--;

	for(int i=0;i<(int)l_Wide;i++)
	{
		for(int j=0;j<(int)l_Wide;j++)
		{
			int l_iBadMapModeID = j+(i*l_Wide);
			int l_iMapModeID    = j+(i*m_nGridCountX);
			m_MeshNodes[l_iMapModeID].cTransLevel = m_pTemp[l_iBadMapModeID];
		}
	}
}

void
TerrainMeshGrid::ImportMapHeightData(const Ptr<Stream>& stream, bool shrink)
{
	MemoryMark mark(Memory::MemStack);

	int iHeightSize = 0 ;
	stream->Read(&iHeightSize, sizeof(int));

	BYTE *m_pTemp = new(Memory::MemStack, iHeightSize)BYTE;
	stream->Read(m_pTemp, iHeightSize*sizeof(BYTE));

	//----------------------------------------------------------------
	//
	int l_Wide = m_nGridCountX;
	if (shrink) l_Wide--;

	for(int i=0;i<(int)l_Wide;i++)
	{
		for(int j=0;j<(int)l_Wide;j++)
		{
			int l_iBadMapModeID = j+(i*l_Wide);
			int l_iMapModeID    = j+(i*m_nGridCountX);
			m_MeshNodes[l_iMapModeID].iHigh = m_pTemp[l_iBadMapModeID];
		}
	}
}

void 
TerrainMeshGrid::ImportCliffIdData(const Ptr<Stream>& stream, bool shrink)
{
	MemoryMark mark(Memory::MemStack);

	int iCliffSize = 0 ;
	stream->Read(&iCliffSize, sizeof(int));

	BYTE *m_pTemp = new(Memory::MemStack, iCliffSize)BYTE;
	stream->Read(m_pTemp, iCliffSize*sizeof(BYTE));

	//----------------------------------------------------------------
	//
	int l_Wide = m_nGridCountX;
	if (shrink) l_Wide--;

	for(int i=0;i<(int)l_Wide;i++)
	{
		for(int j=0;j<(int)l_Wide;j++)
		{
			int l_iBadMapModeID = j+(i*l_Wide);
			int l_iMapModeID    = j+(i*m_nGridCountX);
			m_MeshNodes[l_iMapModeID].cCliffTableID = m_pTemp[l_iBadMapModeID];
		}
	}
}

//------------------------------------------------------------------------------
/**
	intersect
*/
bool 
TerrainMeshGrid::Intersect(int iStartX,int iStartZ, vector& rayDir,vector& rayPos,vector* vInter,float* dist)
{
	return false;
	/*int iStartIndex = IX_QT( iStartX, iStartZ );
	int iNodeIndex; 
	float fU, fV,fDist;
	bool res=false;
	for( int j = 0; j < DISTRICT_VERTS; j++ )
	{
		iNodeIndex = iStartIndex;
		for( int i = 0; i < DISTRICT_VERTS; i++ )
		{
			vector* pv0 = &m_MeshNodes[iNodeIndex].vPos;
			vector* pv1 = &m_MeshNodes[iNodeIndex + m_nGridCountX].vPos;
			vector* pv2 = &m_MeshNodes[iNodeIndex + 1].vPos;
			vector* pv3 = &m_MeshNodes[iNodeIndex + 1 + m_nGridCountX].vPos;

			if( D3DXIntersectTri( pv0, pv1, pv2, &rayPos, &rayDir, &fU, &fV, &fDist ) )
			{
				if (vInter) *vInter = rayPos + rayDir * fDist;
				if (dist) *dist=fDist;
				res=true;
				break;
			}
			if( D3DXIntersectTri( pv1, pv3, pv2, &rayPos, &rayDir, &fU, &fV, &fDist ) )
			{
				if (vInter) *vInter = rayPos + rayDir * fDist;
				if (dist) *dist=fDist;
				res=true;
				break;
			}

			iNodeIndex++;
		}
		if (res) break;
		iStartIndex += m_nGridCountX;
	}

	return res;*/
}

int 
TerrainMeshGrid::IX_QT( int x,  int z)
{
	if((unsigned int)x > (m_nGridCountX-1)){
		x = m_nGridCountX-1;
	}else if(x < 0){
		x=0;  
	}


	if((unsigned int)z > (m_nGridCountX-1)){
		z = m_nGridCountX-1;
	}else if(z < 0){
		z=0;  
	}
	return (x + (z * m_nGridCountX)) ;
}

} // namespace Models
