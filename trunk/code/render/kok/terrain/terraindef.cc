//------------------------------------------------------------------------------
/**
    (C) 2008 cTuo
*/
#include "stdneb.h"
#include "kok/terrain/terraindef.h"

//------------------------------------------------------------------------------
namespace KOK
{
void 
TerrainMeshData::SetTextureData( int iTexture, int iID, int iLayer )
{
	if( iLayer < 0 || iLayer > 17 )
	{
		return;
	}

	float fBasicTU = 64.0f / 512.0f;
	float fBasicTV = 64.0f / 256.0f;

	int iTexU,iTexV;

	if( !iLayer )
	{
		// 为最底层,Id介于0到17
		switch ( iID )
		{
		case 16:
			iTexU = 0;
			iTexV = 0;
			break;
		case 17:
			iTexU = 3;
			iTexV = 3;
			break;
		default:
			iTexU = ( iID % 4 ) + 4;
			iTexV = iID / 4;
			break;
		}
	}
	else
	{
		// 为其他层 ID介于 0 到 15
		iTexU = iID % 4;
		iTexV = iID / 4;
	}

	TileMesh* pMesh1 = &meshData[0];
	TileMesh* pMesh2 = &meshData[1];
	TileMesh* pMesh3 = &meshData[2];
	TileMesh* pMesh4 = &meshData[3];

	// 贴图修整加权
	int iOffsetAdd = 1;
	int iOffsetSub = 2; 

	// 写入指定贴图
	switch ( iLayer )
	{
	case 0:
		pMesh1->t1.set( ( ( ( iTexU + 0 ) * 64 ) + iOffsetAdd ) / 512.0f,
			( ( ( iTexV + 0 ) * 64 ) + iOffsetAdd ) / 256.0f);
		pMesh2->t1.set( ( ( ( iTexU + 1 ) * 64 ) - iOffsetSub ) / 512.0f,
			( ( ( iTexV + 0 ) * 64 ) + iOffsetAdd ) / 256.0f);
		pMesh3->t1.set( ( ( ( iTexU + 0 ) * 64 ) + iOffsetAdd ) / 512.0f,
			( ( ( iTexV + 1 ) * 64 ) - iOffsetSub ) / 256.0f);
		pMesh4->t1.set( ( ( ( iTexU + 1 ) * 64 ) - iOffsetSub ) / 512.0f,
			( ( ( iTexV + 1 ) * 64 ) - iOffsetSub ) / 256.0f);
		break;
	case 1:
		pMesh1->t2.set( ( ( ( iTexU + 0 ) * 64 ) + iOffsetAdd ) / 512.0f,
			( ( ( iTexV + 0 ) * 64 ) + iOffsetAdd ) / 256.0f);
		pMesh2->t2.set( ( ( ( iTexU + 1 ) * 64 ) - iOffsetSub ) / 512.0f,
			( ( ( iTexV + 0 ) * 64 ) + iOffsetAdd ) / 256.0f);
		pMesh3->t2.set( ( ( ( iTexU + 0 ) * 64 ) + iOffsetAdd ) / 512.0f,
			( ( ( iTexV + 1 ) * 64 ) - iOffsetSub ) / 256.0f);
		pMesh4->t2.set( ( ( ( iTexU + 1 ) * 64 ) - iOffsetSub ) / 512.0f,
			( ( ( iTexV + 1 ) * 64 ) - iOffsetSub ) / 256.0f);
		break;
	case 2:
		pMesh1->t3.set( ( ( ( iTexU + 0 ) * 64 ) + iOffsetAdd ) / 512.0f,
			( ( ( iTexV + 0 ) * 64 ) + iOffsetAdd ) / 256.0f);
		pMesh2->t3.set( ( ( ( iTexU + 1 ) * 64 ) - iOffsetSub ) / 512.0f,
			( ( ( iTexV + 0 ) * 64 ) + iOffsetAdd ) / 256.0f);
		pMesh3->t3.set( ( ( ( iTexU + 0 ) * 64 ) + iOffsetAdd ) / 512.0f,
			( ( ( iTexV + 1 ) * 64 ) - iOffsetSub ) / 256.0f);
		pMesh4->t3.set( ( ( ( iTexU + 1 ) * 64 ) - iOffsetSub ) / 512.0f,
			( ( ( iTexV + 1 ) * 64 ) - iOffsetSub ) / 256.0f);
		break;
	case 3:
		pMesh1->t4.set( ( ( ( iTexU + 0 ) * 64 ) + iOffsetAdd ) / 512.0f,
			( ( ( iTexV + 0 ) * 64 ) + iOffsetAdd ) / 256.0f);
		pMesh2->t4.set( ( ( ( iTexU + 1 ) * 64 ) - iOffsetSub ) / 512.0f,
			( ( ( iTexV + 0 ) * 64 ) + iOffsetAdd ) / 256.0f);
		pMesh3->t4.set( ( ( ( iTexU + 0 ) * 64 ) + iOffsetAdd ) / 512.0f,
			( ( ( iTexV + 1 ) * 64 ) - iOffsetSub ) / 256.0f);
		pMesh4->t4.set( ( ( ( iTexU + 1 ) * 64 ) - iOffsetSub ) / 512.0f,
			( ( ( iTexV + 1 ) * 64 ) - iOffsetSub ) / 256.0f);
		break;
	}

	byLayer[iLayer] = iTexture + 1;
}

} // namespace Models
//------------------------------------------------------------------------------

    