#pragma once
#ifndef KOK_PARTICLEEMITTERSERVER_H
#define KOK_PARTICLEEMITTERSERVER_H
//------------------------------------------------------------------------------
/**
	���ӷ���������

	(C) 2008 cTuo
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "kok/model/kokshapenode.h"
#include "kok/effect/particleemitter.h"
#include "coregraphics/vertexbufferpool.h"
#include "coregraphics/indexbuffer.h"

namespace KOK
{
class ParticleServer : public Core::RefCounted
{
public:
	DeclareClass(ParticleServer);
	DeclareSingleton(ParticleServer);

	ParticleServer();
	~ParticleServer();

	void InitializeResource();                  // ���ʼ

	// ����Max������Ч
	ParticleEmitter* CreateParticleEffectByEmitData( EmitterData* pEmitData, char* pTexurePath = NULL );  
	// ����Billboard������Ч
	ParticleEmitter* CreateParticleEffectBySubMeshSerializer( const Ptr<KokShapeNode>& pSubMeshSerializer, char* pTexurePath = NULL ); 
	// ����������Ч
	ParticleEmitter* CreateParticleEffectWeaponRibbon( bool bDoLerp, const D3DXVECTOR3& vMinPos, const D3DXVECTOR3& vMaxPos, char* pTexurePath = NULL );  

	//void ReleaseParticleEffect( ParticleEmitter* pRelease );              // �ͷ�����ϵͳ

	//void FrameMove( float fElapsedTime );                                     // Framemove
	//void Draw();                                // Draw

	// ��Ч��Ч
	//void SetEnable( bool bEnable ) { m_bEnable = bEnable; }                   // �趨�Ƿ�������

	// ��Ч
	//void AddForceDrawParticleEffect( ParticleEmitter* pParticleEffect );  // ������Ƭ��������Ч
	//void ForceDraw( LPDIRECT3DDEVICE9 pD3DDevice );                           // ǿ�ƻ�������Ƭ�ã�

	//static void SetCameraParam( const D3DXVECTOR3& vCameraPos, const D3DXMATRIXA16& matCameraView ) // �趨��Ӱ������
	//{ 
	//	m_vCameraPos = vCameraPos;
	//	m_matCameraView = matCameraView;
	//}
	//static const D3DXVECTOR3& GetCameraPos( void ) { return m_vCameraPos; }             // ȡ����Ӱ��λ��
	//static const D3DXMATRIXA16& GetCameraViewMatrix( void ) { return m_matCameraView; } // ȡ����Ӱ��View����

	const Ptr<VertexBufferPool>& GetVertexBufferPool()const;
	const Ptr<IndexBuffer>& GetIndexBuffer()const;
protected:
	// ��Ч��Ч
	//bool            m_bEnable;                                                // �Ƿ�������

	//cRenderBuffer*  m_pRenderBuffer;                                          // Render Buffer
	//cTextureDB*     m_pTextureDB;                                             // ��ͼDB
	cParticlePool*  m_pParticlePool;                                          // ����Pool

	//cRenderBuffer*  m_pDecalIndexBuffer;                                      // Index Buffer(��cParticleZodiac Decal��)

	//static D3DXVECTOR3 m_vCameraPos;                                          // ��Ӱ��λ��(������Alpha��)
	//static D3DXMATRIXA16 m_matCameraView;                                     // ��Ӱ��View�x�}(��������������)

	//BTList*         m_pParticleEffectList;                                    // ����������ЧList
	//BTList*         m_pParticleEffectSortList;                                // ������

	// ��Ч
	//BTList*         m_pForceDrawParticleEffectList;                           // ����Ƭ��������ЧList

	Ptr<CoreGraphics::VertexBufferPool> vertexPool;
	Ptr<CoreGraphics::IndexBuffer>	  indexBuffer;
};

const Ptr<VertexBufferPool>& 
ParticleServer::GetVertexBufferPool()const
{
	return this->vertexPool;
}

const Ptr<IndexBuffer>& 
ParticleServer::GetIndexBuffer()const
{
	return this->indexBuffer;
}

}

#endif