#pragma once
#ifndef KOK_PARTICLEEMITTERSERVER_H
#define KOK_PARTICLEEMITTERSERVER_H
//------------------------------------------------------------------------------
/**
	���ӷ���������

	(C) 2008 cTuo
*/

namespace KOK
{
class c3dsMaxParticleManager
{
	public:
	  c3dsMaxParticleManager();
	  ~c3dsMaxParticleManager();
	
	  void InitializeResource( LPDIRECT3DDEVICE9 pD3DDevice );                  // ���ʼ
	  
	  // ������Ч
	  cParticleEffectBase* CreateParticleEffectByEmitData( c3dsMaxEmitDataBase* pEmitData, char* pTexurePath = NULL );  // ����Max�ɤl�S��
	  
	  // ������Ч��
	  cParticleEffectBase* CreateParticleEffectBySubMeshSerializer( cSubMeshSerializer* pSubMeshSerializer, char* pTexurePath = NULL ); // ����Billboard�ɤl�S��
	
	  // ���������
	  cParticleEffectBase* CreateParticleEffectWeaponRibbon( bool bDoLerp, const D3DXVECTOR3& vMinPos, const D3DXVECTOR3& vMaxPos, char* pTexurePath = NULL );  // ���ͤM���S��
	
	  void ReleaseParticleEffect( cParticleEffectBase* pRelease );              // �ͷ�����ϵͳ
	
	  void FrameMove( float fElapsedTime );                                     // Framemove
	  void Draw( LPDIRECT3DDEVICE9 pD3DDevice );                                // Draw
	
	  // ��Ч��Ч
	  void SetEnable( bool bEnable ) { m_bEnable = bEnable; }                   // �趨�Ƿ�������
	
	  // ��Ч
	  void AddForceDrawParticleEffect( cParticleEffectBase* pParticleEffect );  // ������Ƭ��������Ч
	  void ForceDraw( LPDIRECT3DDEVICE9 pD3DDevice );                           // ǿ�ƻ�������Ƭ�ã�
	
	  static void SetCameraParam( const D3DXVECTOR3& vCameraPos, const D3DXMATRIXA16& matCameraView ) // �趨��Ӱ������
	  { 
	    m_vCameraPos = vCameraPos;
	    m_matCameraView = matCameraView;
	  }
	  static const D3DXVECTOR3& GetCameraPos( void ) { return m_vCameraPos; }             // ȡ����Ӱ��λ��
	  static const D3DXMATRIXA16& GetCameraViewMatrix( void ) { return m_matCameraView; } // ȡ����Ӱ��View����
	
	protected:
	  // ��Ч��Ч
	  bool            m_bEnable;                                                // �Ƿ�������
	
	  cRenderBuffer*  m_pRenderBuffer;                                          // Render Buffer
	  cTextureDB*     m_pTextureDB;                                             // ��ͼDB
	  cParticlePool*  m_pParticlePool;                                          // ����Pool
	
	  cRenderBuffer*  m_pDecalIndexBuffer;                                      // Index Buffer(��cParticleZodiac Decal��)
	
	  // 071225 cyhsieh �S��
	  static D3DXVECTOR3 m_vCameraPos;                                          // ��Ӱ��λ��(������Alpha��)
	  static D3DXMATRIXA16 m_matCameraView;                                     // ��Ӱ��View�x�}(��������������)
	
	  BTList*         m_pParticleEffectList;                                    // ����������ЧList
	  BTList*         m_pParticleEffectSortList;                                // ������
	
	  // ��Ч
	  BTList*         m_pForceDrawParticleEffectList;                           // ����Ƭ��������ЧList
};
}