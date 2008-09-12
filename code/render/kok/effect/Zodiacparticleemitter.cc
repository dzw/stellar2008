//------------------------------------------------------------------------------
//  particleemitter.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/effect/particledata.h"
#include "io/stream.h"
#include "memory/memory.h"

namespace KOK
{
using namespace IO;
using namespace Memory;

ZodiacParticleEmitter::ZodiacParticleEmitter( cParticlePool* pParticlePool, cSubMeshSerializer* pSubMeshSerializer )
:ParticleEmitter( pParticlePool ), m_fHalfWidth(0.0f), m_fCurAnimTime(0.0f)
{
  if( pSubMeshSerializer == NULL )
  {
    return;
  }
  
  SetLinkName( pSubMeshSerializer->getSubMeshName() );

  MDLVTX* pVertex = pSubMeshSerializer->getSubMeshVerticesBuffer();
  for( int i = 0; i < 3; i++ )
  {
    m_vCorner[i] = pVertex[i].p;
  }

  DWORD dwRenderAlphaBlendType = 0;
  c3dsMaxParticleRender* pRender = (c3dsMaxParticleRender*)g_mScene->RenderCreate( RENDER_C_3DSMAXPARTICLE );
  if( pRender == NULL )
  {
    return;
  }

  m_pRender = (c3dsMaxParticleRender*)pRender->Create();
  m_pRender->iMaterialSize = pSubMeshSerializer->getSubMeshMaterialSize();
  if( m_pRender->iMaterialSize > 0 )
  {
    cMaterial* pMaterial;
    cMaterialAnimator* pMaterialAnimator;
    m_pRender->m_pMaterial = _NEW cMaterial[m_pRender->iMaterialSize];
    for( int i = 0; i < m_pRender->iMaterialSize; i++ )
    {
      pMaterial = pSubMeshSerializer->getSubMeshMaterial( i );
      if( pMaterial )
      {
        m_pRender->m_pMaterial[i].m_iNameSize = pMaterial->m_iNameSize;
        if( pMaterial->m_iNameSize > 0 )
        {
          m_pRender->m_pMaterial[i].m_pszTextName = _NEW char[pMaterial->m_iNameSize + 1];
          if( m_pRender->m_pMaterial[i].m_pszTextName )
          {
            strcpy( m_pRender->m_pMaterial[i].m_pszTextName, pMaterial->m_pszTextName );
          }
        }
        m_pRender->m_pMaterial[i].SetAlphaBlendType( pMaterial->GetAlphaBlendType() );
        m_pRender->m_pMaterial[i].SetTextureAniRows( pMaterial->GetTextureAniRows() );
        m_pRender->m_pMaterial[i].SetTextureAniCols( pMaterial->GetTextureAniCols() );
        m_pRender->m_pMaterial[i].SetTextureAniRepeat( pMaterial->GetTextureAniRepeat() );

        if( i == 0 )
        {
          dwRenderAlphaBlendType = pMaterial->GetAlphaBlendType();
        }
      }

      pMaterialAnimator = pSubMeshSerializer->GetMaterialAnimator( i );
      if( pMaterialAnimator )
      {
        m_pRender->m_pMaterial[i].SetMaterialAnimator( pMaterialAnimator );
      }
      
      // Force Clamp
      m_pRender->m_dwAddressU = D3DTADDRESS_CLAMP;
      m_pRender->m_dwAddressV = D3DTADDRESS_CLAMP;
    }
  }

  switch( dwRenderAlphaBlendType )
  {
  case 0:   // Opaque
    m_pRender->m_bAlphaBlendenable = false;
    m_pRender->m_bZWriteenable = true;
    break;
  case 1:   // Additive
    m_pRender->m_bAlphaBlendenable = true;
    m_pRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
    m_pRender->m_dwDestBlend = D3DBLEND_ONE;
    m_pRender->m_bZWriteenable = false;
    break;
  case 2:   // Alpha
    m_pRender->m_bAlphaBlendenable = true;
    m_pRender->m_dwSrcBlend = D3DBLEND_SRCALPHA;
    m_pRender->m_dwDestBlend = D3DBLEND_INVSRCALPHA;
    m_pRender->m_bZWriteenable = false;
    break;
  case 3:   // Modulate
    m_pRender->m_bAlphaBlendenable = true;
    m_pRender->m_dwSrcBlend = D3DBLEND_ZERO;
    m_pRender->m_dwDestBlend = D3DBLEND_SRCCOLOR;
    m_pRender->m_bZWriteenable = false;
    break;
  case 4:   // Modulate Inv
    m_pRender->m_bAlphaBlendenable = true;
    m_pRender->m_dwSrcBlend = D3DBLEND_ZERO;
    m_pRender->m_dwDestBlend = D3DBLEND_INVSRCCOLOR;
    m_pRender->m_bZWriteenable = false;
    break;
  }
}

//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: 
//-----------------------------------------------------------------------------
void ZodiacParticleEmitter::FrameMove( float fElapsedTime )
{
  // !!!To do...Optimize

  if( m_pRender == NULL )
  {
    return;
  }

  float fFogAlphaFactor = ComputeFogAlphaFactor();
  if( fFogAlphaFactor <= 0.0f )
  {
    return;
  }

  if( g_pGameTerrain == NULL ) return;

  DWORD dwVertexSize = 0;

  cThing* pFloorThing = NULL;

  float fStartX = m_vCurPosition.x - m_fHalfWidth;
  float fStartZ = m_vCurPosition.z - m_fHalfWidth;
  float fEndX = m_vCurPosition.x + m_fHalfWidth;
  float fEndZ = m_vCurPosition.z + m_fHalfWidth;

  float fOffset = g_pGameTerrain->GetTilePosOffset();

  int iTileX = (int)( ( m_vCurPosition.x + fOffset ) / COMP );
  int iTileZ = (int)( ( m_vCurPosition.z + fOffset ) / COMP );
  cTile* pTile = g_pGameTerrain->GetTile( iTileX, iTileZ );
  if( pTile && pTile->m_pThing && pTile->m_pThing->getFloorMesh() )
  {
    pFloorThing = pTile->m_pThing;
  }

  float fOneOverDifferenceX = 1.0f / ( fEndX - fStartX );
  float fOneOverDifferenceZ = 1.0f / ( fEndZ - fStartZ );
  float fTU, fTV;

  cMaterial* pMaterial = NULL;
  if( m_pRender->iMaterialSize > 0 )
  {
    pMaterial = &m_pRender->m_pMaterial[0];
  }

  D3DCOLOR color = 0xFFFFFF | ( ( (DWORD)( fFogAlphaFactor * 255.0f ) ) << 24 );
  if( pMaterial )
  {
    pMaterial->SetAniTime( m_fCurAnimTime );
    const D3DXCOLOR* pColor = pMaterial->GetDiffuseOpacity();
    if( pColor )
    {
      color = D3DCOLOR_COLORVALUE( pColor->r, pColor->g, pColor->b, pColor->a * fFogAlphaFactor );
    }
  }

  if(( pFloorThing )&&(pFloorThing->getFloorMesh()->GetFaceCount()>0)) // 位置在可站地上物上面，计算地上物Mesh的Decal(目前只计算正站在上面的地上物)
  {
    D3DXVECTOR3 vNewVertex[7];  // Clipping a convex polygon having n vertices to a plane
                                // results in a new convex plygon having at most n + 1 vertices

    // Calculate boundary planes
    m_planeLeft.d = m_fHalfWidth - m_vCurPosition.x;
    m_planeRight.d = m_fHalfWidth + m_vCurPosition.x;
    m_planeBack.d = m_fHalfWidth - m_vCurPosition.z;
    m_planeFront.d = m_fHalfWidth + m_vCurPosition.z;

    m_dwDecalVertexCount = 0;
    m_dwDecalIndexCount = 0;

    // Clip one triangle at a time
    D3DXVECTOR3 vCross;
		D3DXVECTOR3 v0,v1,v2,v01,v02;
		DWORD iFaceCount=pFloorThing->getFloorMesh()->GetFaceCount();
    for( DWORD i = 0; i < iFaceCount; i++ )
    {
			pFloorThing->getFloorMesh()->GetFaceTriangle(i,&v0,&v1,&v2);
			v01=v1-v0;
			v02=v2-v0;

      D3DXVec3Cross( &vCross, &v01, &v02 );

      // vCross dot vDecalNormal(0,1,0) = vCross.y
      if( vCross.y > ( DECAL_EPSILON * D3DXVec3Length( &vCross ) ) )
      {
        vNewVertex[0] = v0;
        vNewVertex[1] = v1;
        vNewVertex[2] = v2;

        int iCount = ClipDecalPolygon( 3, vNewVertex, vNewVertex );
        if( ( iCount != 0 ) && ( AddDecalPolygon( iCount, vNewVertex ) == false ) )
        {
          break;
        }
      }
    }

    if( m_pRender->m_dwRenderMeshSize < m_dwDecalVertexCount )
    {
      SAFE_DELETE( m_pRender->m_pRenderMesh );
      m_pRender->m_pRenderMesh = _NEW EFFECT_MDLVTX[m_dwDecalVertexCount];
      if( m_pRender->m_pRenderMesh == NULL )
      {
        m_pRender->m_dwVertexSize = 0;
        m_pRender->m_dwIndexSize = 0;
        m_pRender->m_dwRenderMeshSize = 0;
        return;
      }
      m_pRender->m_dwRenderMeshSize = m_dwDecalVertexCount;
    }
    if( m_pRender->m_dwRenderMeshIndexSize < m_dwDecalIndexCount )
    {
      SAFE_DELETE( m_pRender->m_pRenderMeshIndex );
      m_pRender->m_pRenderMeshIndex = _NEW WORD[m_dwDecalIndexCount];
      if( m_pRender->m_pRenderMeshIndex == NULL )
      {
        m_pRender->m_dwVertexSize = 0;
        m_pRender->m_dwIndexSize = 0;
        m_pRender->m_dwRenderMeshIndexSize = 0;
        return;
      }
      m_pRender->m_dwRenderMeshIndexSize = m_dwDecalIndexCount;
    }

    EFFECT_MDLVTX* pRenderMesh = m_pRender->m_pRenderMesh;

    // Copy Decal Vertices
    dwVertexSize = 0;
    for( unsigned int i = 0; i < m_dwDecalVertexCount; i++ )
    {
      pRenderMesh[dwVertexSize].p = m_vDecalVertices[i];
      pRenderMesh[dwVertexSize].p.y += 0.1f;
      fTU = ( ( pRenderMesh[dwVertexSize].p.x - fStartX ) * fOneOverDifferenceX );
      fTV = ( ( pRenderMesh[dwVertexSize].p.z - fStartZ ) * fOneOverDifferenceZ );
      pRenderMesh[dwVertexSize].tu = fTU;
      pRenderMesh[dwVertexSize].tv = 1.0f - fTV;
      pRenderMesh[dwVertexSize].color = color;
      dwVertexSize++;
    }

    // Copy Decal Indices
    memcpy( m_pRender->m_pRenderMeshIndex, m_wDecalIndices, sizeof(WORD) * m_dwDecalIndexCount );
    m_pRender->m_dwIndexSize = m_dwDecalIndexCount;

    m_pRender->iRenderType = RENDER_TYPE_5; // Special Render Type
  }
  else  // 位置在地表上面
  {
    int iScale = g_pGameTerrain->GetMeshScale();
    float fSize = (float)( COMP * iScale );
    float fNodeX = ( fStartX + fOffset ) / fSize;
    float fNodeZ = ( fStartZ + fOffset ) / fSize;
    int iNodeStartX = (int)( floor( fNodeX ) );
    int iNodeStartZ = (int)( floor( fNodeZ ) );

    fNodeX = ( fEndX + fOffset ) / fSize;
    fNodeZ = ( fEndZ + fOffset ) / fSize;
    int iNodeEndX = (int)( ceil( fNodeX ) );
    int iNodeEndZ = (int)( ceil( fNodeZ ) );

    int iNodeSizeX  = iNodeEndX - iNodeStartX;
    int iNodeSizeZ  = iNodeEndZ - iNodeStartZ;

    dwVertexSize = iNodeSizeX * iNodeSizeZ * 4;
    if( m_pRender->m_dwRenderMeshSize < dwVertexSize )
    {
      SAFE_DELETE( m_pRender->m_pRenderMesh );
      m_pRender->m_pRenderMesh = _NEW EFFECT_MDLVTX[dwVertexSize];
      if( m_pRender->m_pRenderMesh == NULL )
      {
        m_pRender->m_dwVertexSize = 0;
        m_pRender->m_dwRenderMeshSize = 0;
        return;
      }
      m_pRender->m_dwRenderMeshSize = dwVertexSize;
    }

    EFFECT_MDLVTX* pRenderMesh = m_pRender->m_pRenderMesh;

    dwVertexSize = 0;
    for( int j = 0; j < iNodeSizeZ; j++ )
    {
      for( int i = 0; i < iNodeSizeX; i++ )
      {
        pRenderMesh[dwVertexSize + 0].p = g_pGameTerrain->GetMeshNode( iNodeStartX + i, iNodeStartZ + j );
        pRenderMesh[dwVertexSize + 0].p.y += 0.1f;
        fTU = ( ( pRenderMesh[dwVertexSize + 0].p.x - fStartX ) * fOneOverDifferenceX );
        fTV = ( ( pRenderMesh[dwVertexSize + 0].p.z - fStartZ ) * fOneOverDifferenceZ );
        pRenderMesh[dwVertexSize + 0].tu = fTU;
        pRenderMesh[dwVertexSize + 0].tv = 1.0f - fTV;
        pRenderMesh[dwVertexSize + 0].color = color;

        pRenderMesh[dwVertexSize + 1].p = g_pGameTerrain->GetMeshNode( iNodeStartX + i, iNodeStartZ + j + 1 );
        pRenderMesh[dwVertexSize + 1].p.y += 0.1f;
        fTU = ( ( pRenderMesh[dwVertexSize + 1].p.x - fStartX ) * fOneOverDifferenceX );
        fTV = ( ( pRenderMesh[dwVertexSize + 1].p.z - fStartZ ) * fOneOverDifferenceZ );
        pRenderMesh[dwVertexSize + 1].tu = fTU;
        pRenderMesh[dwVertexSize + 1].tv = 1.0f - fTV;
        pRenderMesh[dwVertexSize + 1].color = color;

        pRenderMesh[dwVertexSize + 2].p = g_pGameTerrain->GetMeshNode( iNodeStartX + i + 1, iNodeStartZ + j );
        pRenderMesh[dwVertexSize + 2].p.y += 0.1f;
        fTU = ( ( pRenderMesh[dwVertexSize + 2].p.x - fStartX ) * fOneOverDifferenceX );
        fTV = ( ( pRenderMesh[dwVertexSize + 2].p.z - fStartZ ) * fOneOverDifferenceZ );
        pRenderMesh[dwVertexSize + 2].tu = fTU;
        pRenderMesh[dwVertexSize + 2].tv = 1.0f - fTV;
        pRenderMesh[dwVertexSize + 2].color = color;

        pRenderMesh[dwVertexSize + 3].p = g_pGameTerrain->GetMeshNode( iNodeStartX + i + 1, iNodeStartZ + j + 1 );
        pRenderMesh[dwVertexSize + 3].p.y += 0.1f;
        fTU = ( ( pRenderMesh[dwVertexSize + 3].p.x - fStartX ) * fOneOverDifferenceX );
        fTV = ( ( pRenderMesh[dwVertexSize + 3].p.z - fStartZ ) * fOneOverDifferenceZ );
        pRenderMesh[dwVertexSize + 3].tu = fTU;
        pRenderMesh[dwVertexSize + 3].tv = 1.0f - fTV;
        pRenderMesh[dwVertexSize + 3].color = color;

        dwVertexSize += 4;
      }
    }

    m_pRender->iRenderType = RENDER_TYPE_0;
    m_pRender->m_dwIndexSize = 0;
  }

  m_pRender->m_dwVertexSize = dwVertexSize;
  m_pRender->bEnabled = true;
}

//-----------------------------------------------------------------------------
// Name: ApplyTransformMatrix
// Desc: 
//-----------------------------------------------------------------------------
void ZodiacParticleEmitter::ApplyTransformMatrix( D3DXMATRIXA16* pTransform )
{
  D3DXVECTOR3 vTransformed[3];

  for( int i = 0; i < 3; i++ )
  {
    D3DXVec3TransformCoord( &vTransformed[i], &m_vCorner[i], pTransform );
  }

  D3DXVECTOR3 vDiff10 = vTransformed[1] - vTransformed[0];
  D3DXVECTOR3 vDiff20 = vTransformed[2] - vTransformed[0];

  m_vCurPosition = ( vTransformed[1] + vTransformed[2] ) * 0.5f;

  m_fHalfWidth = D3DXVec3Length( &( vTransformed[1] - vTransformed[0] ) ) * 0.5f;
}

//-----------------------------------------------------------------------------
// Name: ClipDecalPolygon
// Desc: 计算Plane切割结果
//-----------------------------------------------------------------------------
int ZodiacParticleEmitter::ClipDecalPolygon( int iVertexCount, D3DXVECTOR3* pVertex, D3DXVECTOR3* pNewVertex )
{
  D3DXVECTOR3 vTempVertex[7];
	
	// Clip against planes
	int iCount = ClipDecalPolygonAgainstPlane( m_planeLeft, iVertexCount, pVertex, vTempVertex );
	if( iCount != 0 )
	{
		iCount = ClipDecalPolygonAgainstPlane( m_planeRight, iCount, vTempVertex, pNewVertex );
		if( iCount != 0 )
		{
			iCount = ClipDecalPolygonAgainstPlane( m_planeBack, iCount, pNewVertex, vTempVertex );
			if( iCount != 0 )
			{
				iCount = ClipDecalPolygonAgainstPlane( m_planeFront, iCount, vTempVertex, pNewVertex );
			}
		}
	}
	
	return iCount;
}

//-----------------------------------------------------------------------------
// Name: ClipDecalPolygonAgainstPlane
// Desc: 计算Plane切出来的Decal Polygon
//-----------------------------------------------------------------------------
int ZodiacParticleEmitter::ClipDecalPolygonAgainstPlane( const D3DXPLANE& plane, int iVertexCount, D3DXVECTOR3* pVertex, D3DXVECTOR3* pNewVertex )
{
  bool bNegative[7];
	
	// Classify vertices
	int iNegativeCount = 0;
	for( int i = 0; i < iVertexCount; i++ )
	{
		bool bNeg = D3DXPlaneDotCoord( &plane, &pVertex[i] ) < 0.0f;
		bNegative[i] = bNeg;
		iNegativeCount += bNeg;
	}
	
	// Discard this polygon if it's completely culled
	if( iNegativeCount == iVertexCount )
  {
    return 0;
  }

	int iCount = 0;
	for( int i = 0; i < iVertexCount; i++ )
	{
		// the index of the previous vertex
		int iPre = ( i != 0 ) ? i - 1 : iVertexCount - 1;
		
		if( bNegative[i] )
		{
			if( !bNegative[iPre] )
			{
				// Current vertex is on negative side of plane,
				// but previous vertex is on positive side.
				const D3DXVECTOR3& v1 = pVertex[iPre];
				const D3DXVECTOR3& v2 = pVertex[i];
				float fT = D3DXPlaneDotCoord( &plane, &v1 ) / ( plane.a * ( v1.x - v2.x ) + plane.b * ( v1.y - v2.y ) + plane.c * ( v1.z - v2.z ) );
				pNewVertex[iCount] = v1 * ( 1.0f - fT ) + v2 * fT;
				iCount++;
			}
		}
		else
		{
			if( bNegative[iPre] )
			{
				// Current vertex is on positive side of plane,
				// but previous vertex is on negative side.
				const D3DXVECTOR3& v1 = pVertex[i];
				const D3DXVECTOR3& v2 = pVertex[iPre];
				float fT = D3DXPlaneDotCoord( &plane, &v1 ) / ( plane.a * ( v1.x - v2.x ) + plane.b * ( v1.y - v2.y ) + plane.c * ( v1.z - v2.z ) );
				pNewVertex[iCount] = v1 * ( 1.0f - fT ) + v2 * fT;
				iCount++;
			}
			
			// Include current vertex
			pNewVertex[iCount] = pVertex[i];
			iCount++;
		}
	}
	
	// Return number of vertices in clipped polygon
	return iCount;
}

//-----------------------------------------------------------------------------
// Name: AddDecalPolygon
// Desc: 增加Decal Polygon
//-----------------------------------------------------------------------------
bool ZodiacParticleEmitter::AddDecalPolygon( int iVertexCount, D3DXVECTOR3* pVertex )
{
  int iCount = m_dwDecalVertexCount;
	if( iCount + iVertexCount >= MAX_DECAL_VERTICES )
  {
    return false;
  }
	
	// Add polygon as a triangle fan
  int iIndex = m_dwDecalIndexCount;
  m_dwDecalIndexCount += ( iVertexCount - 2 ) * 3;
  for( int i = 2; i < iVertexCount; i++ )
  {
    m_wDecalIndices[iIndex++] = iCount;
    m_wDecalIndices[iIndex++] = iCount + i - 1;
    m_wDecalIndices[iIndex++] = iCount + i;
  }
	
	for( int i = 0; i < iVertexCount; i++ )
	{
		m_vDecalVertices[iCount] = pVertex[i];
		iCount++;
	}
	
	m_dwDecalVertexCount = iCount;

  return true;
}
}