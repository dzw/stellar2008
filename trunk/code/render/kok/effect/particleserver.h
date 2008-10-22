#pragma once
#ifndef KOK_PARTICLEEMITTERSERVER_H
#define KOK_PARTICLEEMITTERSERVER_H
//------------------------------------------------------------------------------
/**
	粒子发射器管理

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

	void InitializeResource();                  // 初始

	// 产生Max粒子特效
	ParticleEmitter* CreateParticleEffectByEmitData( EmitterData* pEmitData, char* pTexurePath = NULL );  
	// 产生Billboard粒子特效
	ParticleEmitter* CreateParticleEffectBySubMeshSerializer( const Ptr<KokShapeNode>& pSubMeshSerializer, char* pTexurePath = NULL ); 
	// 产生刀光特效
	ParticleEmitter* CreateParticleEffectWeaponRibbon( bool bDoLerp, const D3DXVECTOR3& vMinPos, const D3DXVECTOR3& vMaxPos, char* pTexurePath = NULL );  

	//void ReleaseParticleEffect( ParticleEmitter* pRelease );              // 释放粒子系统

	//void FrameMove( float fElapsedTime );                                     // Framemove
	//void Draw();                                // Draw

	// 长效特效
	//void SetEnable( bool bEnable ) { m_bEnable = bEnable; }                   // 设定是否有作用

	// 特效
	//void AddForceDrawParticleEffect( ParticleEmitter* pParticleEffect );  // 增加照片用粒子特效
	//void ForceDraw( LPDIRECT3DDEVICE9 pD3DDevice );                           // 强制画（给照片用）

	//static void SetCameraParam( const D3DXVECTOR3& vCameraPos, const D3DXMATRIXA16& matCameraView ) // 设定摄影机参数
	//{ 
	//	m_vCameraPos = vCameraPos;
	//	m_matCameraView = matCameraView;
	//}
	//static const D3DXVECTOR3& GetCameraPos( void ) { return m_vCameraPos; }             // 取得摄影机位置
	//static const D3DXMATRIXA16& GetCameraViewMatrix( void ) { return m_matCameraView; } // 取得摄影机View矩阵

	const Ptr<VertexBufferPool>& GetVertexBufferPool()const;
	const Ptr<IndexBuffer>& GetIndexBuffer()const;
protected:
	// 长效特效
	//bool            m_bEnable;                                                // 是否有作用

	//cRenderBuffer*  m_pRenderBuffer;                                          // Render Buffer
	//cTextureDB*     m_pTextureDB;                                             // 贴图DB
	cParticlePool*  m_pParticlePool;                                          // 粒子Pool

	//cRenderBuffer*  m_pDecalIndexBuffer;                                      // Index Buffer(给cParticleZodiac Decal用)

	//static D3DXVECTOR3 m_vCameraPos;                                          // 摄影机位置(计算雾Alpha用)
	//static D3DXMATRIXA16 m_matCameraView;                                     // 摄影机View痻皚(计算粒子面向用)

	//BTList*         m_pParticleEffectList;                                    // 储存粒子特效List
	//BTList*         m_pParticleEffectSortList;                                // 排序用

	// 特效
	//BTList*         m_pForceDrawParticleEffectList;                           // 画照片用粒子特效List

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