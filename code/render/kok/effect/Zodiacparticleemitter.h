#pragma once
#ifndef KOK_ZODIACPARTICLEEMITTER_H
#define KOK_ZODIACPARTICLEEMITTER_H
//------------------------------------------------------------------------------
/**
	���ӷ�����

	���ڵر���ر�߶������ƽ����Ч

	(C) 2008 cTuo
*/
#include "kok/effect/particleemitter.h"

namespace KOK
{

#define MAX_DECAL_VERTICES    256   // Decal���Vertex����
#define DECAL_EPSILON         0.25f

class ZodiacParticleEmitter : public ParticleEmitter
{
public:
  cParticleZodiac( cParticlePool* pParticlePool, const Ptr<KokShapeNode>& pSubMeshSerializer );

  virtual void FrameMove( float fElapsedTime );

  virtual void ApplyTransformMatrix( D3DXMATRIXA16* pTransform );

  virtual void SetCurAnimTime( float fTime ) { m_fCurAnimTime = fTime; }

protected:
  D3DXVECTOR3 m_vCorner[3];
  float       m_fHalfWidth;
  float       m_fCurAnimTime;

  // ����Ϊ������վ������Decal��(���������ο�Game Programming Gems2 4.8)
  int ClipDecalPolygon( int iVertexCount, D3DXVECTOR3* pVertex, D3DXVECTOR3* pNewVertex );
  int ClipDecalPolygonAgainstPlane( const D3DXPLANE& plane, int iVertexCount, D3DXVECTOR3* pVertex, D3DXVECTOR3* pNewVertex );
  bool AddDecalPolygon( int iVertexCount, D3DXVECTOR3* pVertex );

  static D3DXVECTOR3 m_vDecalVertices[MAX_DECAL_VERTICES];    // Decal Vertices
  static DWORD       m_dwDecalVertexCount;                    // Decal Vertex����
  static WORD        m_wDecalIndices[MAX_DECAL_VERTICES * 3]; // Decal Indices
  static DWORD       m_dwDecalIndexCount;                     // Decal Index����
  static D3DXPLANE   m_planeLeft;                             // Boundary Clip Plane
  static D3DXPLANE   m_planeRight;
  static D3DXPLANE   m_planeBack;
  static D3DXPLANE   m_planeFront;
};

}

#endif