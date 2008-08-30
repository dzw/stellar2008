#ifndef cBeingModelH
#define cBeingModelH
//------------------------------------------------------------------------------
/**
	@class KOK::cSkinnedCharacterModel

	����Ļ���ģ��
	
	Meshes , skeleton , animation of skinned character model is created by 
	createMeshSerializer , createSkeletonSerializer
	and createAnimationAction , that anmation is running and refreshed by 
	tickAnimationAction with frame move , after skinned mesh 
	will be computed by running tickSkinnedCharacterMesh. then it also makes 
	RenderBuffer and Render from them for scene render list .

	(C) 2008 cTuo
*/
#include "cSkeletonSerializer.h"
#include "cSkinnedCharacterEquipPart.h"
#include "../Scene/cBeingRender.h"
#include "../Scene/cEquipRender.h"

namespace KOK
{

class cBeing;

class cSkinnedCharacterModel
{
	friend cBeing;
public:
	// ��Ϊ������������̫��"����", Ŀǰ��ѡ��Ͽ�Ľ������, ʹ�� type ���и�.
	enum 
	{ // model type
		mtDEFAULT = 0, // Default ��ʱ����Ϊ MOB �� NPC
		mtMOB,    // MOB�Ǽ�
		mtNPC,    // NPC�Ǽ�
		mtPLAYER, // ��ҹǼ�
		mtMOUNT,  // �����Ǽ�
	};
	enum AnimationActionOperation
	{ // Animation Action Operation
		aoDefaultKeyframes = 0, // �������һ��Frame. 
		aoIgnoreCountBackwards1ndKeyframe = 1, // �������һ��keyframe. 
		// Animation���ܻ���ֵ�����״��.
		// ��һ��Frame�����һ��Frameһ��.(������·�ܲ�����)
		//   �����༭����.
		//   ��ʽ����Loop��������ʱ�Ϸ���(���ػ�ͷ�õ�һ��Frame�����һFrame���ڲ�)
		// ��һ��Frame�����һ��Frame��ͬ.(������������)
		//   �����༭����
		//   ���β��Ŷ���, ���ܻ�ӵ���һ�������ĵ�һ��, ��ʱֻ��Ҫ�����һ�ź��Լ���.
		aoCountBackwards1ndKeyframe = 2,   // ֱ���������һ��keyframe.
		aoCountBackwards2ndKeyframe = 3    // ֱ������������keyframe.
	};
	enum RenderEntityStyle{ // RenderStyle
		rsNone = 0x80000000,
		rsDefault  = 0, // һ�����
		rsAddColor = 1, // ��ɫ���� 
		rsNoLight  = 2, // �����
		rsFadeIn   = 3, //fade in 
		rsFadeOut  = 4, //fade out
	};
private:
	// Skinned Character Part has detail and lod cMeshSerializer and its' teture id.
	class SkinnedCharacterPart
	{
	public:
		//  Skinned Character LOD
		cMeshSerializer*  m_pMesh[MAX_BEINGLOD_LV];             // ��Lod Level��Mesh

		int m_iTextureId;
		SkinnedCharacterPart();
		virtual ~SkinnedCharacterPart();
		void clsSkinnedCharacterPart();	
	};

	// stActionState record index , animation time length , play speed , 
		//and animation played time of animation action 
		typedef struct 
	{
		int   m_iIndex;
		float m_fAnimTimeLength;
		float m_fAnimTime;
		float m_fPlaySpeed;
		void clear()
		{
			m_iIndex          = 0;
			m_fAnimTimeLength = 0;
			m_fAnimTime       = 0;
			m_fPlaySpeed      = 1.0f;
		}
	} stActionState;	
	// 
	static unsigned int m_uiSkinnedCharacterModelCount; 
	// translation matrix of skinned character.
	D3DXMATRIXA16 m_matTranslation;
	// rotation matrix of skinned character.
	D3DXMATRIXA16 m_matRotation;
	// scaling matrix of skinned character.
	D3DXMATRIXA16 m_matScaling;
	// matrix of skinned character.
	D3DXMATRIXA16 m_matMatrix;
	bool          m_bUpadteWorldMatrix;                               // �����������
	int           m_iDirection;                                       // ����(�Ƕ�)
	stActionState m_CurnAction;                                       // Ŀǰ�Ķ���
	stActionState m_FadeoutAction;                                    // �����Ķ���
	float         m_fFadeoutTimeRemain;                               // ʣ�൭��ʱ��...
	float         m_fFadeoutTime;                                     // �ܵ���ʱ��
	D3DXVECTOR3  *m_pWorldSpaceBoundingBoxMin,*m_pWorldSpaceBoundingBoxMax; 
	// World Space Bounding Box
	D3DXVECTOR3  *m_pLocalSpaceBoundingBoxMin,*m_pLocalSpaceBoundingBoxMax; 
	// Local Space Bounding Box
	//  ʵ��Ӱ��
	sShadowData*  m_pShadowData;                                      // ��ʵ��Ӱ���������� 
		RenderEntityStyle m_iRenderStyle;

	//ͬ���͵� cBeingModel �Ṳ��һ�� cSkeletonSerializer
	cSkeletonSerializer *m_pSkeletonSerializer; 
	// Skinned Character Parts which have detail and lod cMeshSerializer and its' texture id.
	SkinnedCharacterPart* m_pPart;
	// shared render buffer of Skinned Character.
	cRenderBuffer* m_pRenderBuffer;
	// render entity of Skinned Character.
	cBeingRender *m_pRender;
	// texture factory of Skinned Character to product it's texture.
	cTextureDB	 *m_pTextureDB;
	//  ����
	BTList*       m_pEquipPartList;                                   // �����װ��List
	//  ����������ʾ
	bool          m_bHideWeaponEquip;                                 // �Ƿ���������

	//  Thread
	BYTE          m_byWaitResourceCount;                              // �ȴ�Thread��ȡ��ģ����Դ����

	// weapon of None-Combined Skinned Character.
	char* m_szNoneCombinedSkinnedCharacterWeaponName;
	// adornments of None-Combined Skinned Character.
	bool m_bNoneCombinedSkinnedCharacterAdorn01;
	bool m_bNoneCombinedSkinnedCharacterAdorn02;
	bool m_bNoneCombinedSkinnedCharacterAdorn03;

	sComputeAnimParam m_sComputeAnimParam;

	//  Skinned Character LOD
	static float m_fLodDistanceSq[MAX_BEINGLOD_LV];                   // ��Lod Level�ľ���ƽ��

	// check Skinned Character Parts is combined?
	bool checkSkinnedCharacterPartsCombined();
	// ����Mesh(��Ʒװ��)�Ƿ�Ҫ��ʾ
	bool checkEquipMeshVisible( char* pMeshName );

#ifdef _CHECK_NOUSE_ANIMATION
	SkeletonHierarchyUsabilityMap    m_SkeletonHierarchyUnUsedMap;                
	// ������û���õ���frame
	void getUnusedSkeletonHierarchy( cSkeletonHierarchy* pSkeletonHierarchy, 
		SkeletonHierarchyUsabilityMap& SkeletonHierarchyUsedMap, bool& bParentUse );  
	// �ҳ�û�õ���frame
	void getSubMeshUsedSkeletonHierarchy( SkeletonHierarchyUsabilityMap& SkeletonHierarchyUsedMap, 
		cSubMeshSerializer* pSubMeshSerial );// �ҳ�SubMesh���õ���frame
#endif

		bool UpdateRenderMesh( D3DXMATRIXA16* pRootTransform, bool bUpdateRenderMesh );

	//  Skinned Character LOD
	void SetPartMeshSerializer( int iPart, int iLodLevel, cMeshSerializer* pMeshSerializer ); // �趨����λ��Mesh
	void ComputeLod( void );                                          // ����Ŀǰ��Lod Level

		bool CreateBeingRender();
	void CheckLodMeshExisting(bool bHasLodMesh[MAX_BEINGLOD_LV]);
	void ValidateMeshPartsVisiablility(cMeshSerializer*,sModelPartData*,SkeletonHierarchyUsabilityMap* SkeletonHierarchyUsedMap);
	void ChangeSubMeshTexture(cBeing* pOwnerBeing,cMeshSerializer* pMeshSerial,sModelPartData* pModelPartData,SkinnedCharacterPart* pPart,bool* bPartHasFakeReflect);
	void ChangeModelMeshPart(cBeing* pOwnerBeing,cMeshSerializer* pMeshSerial,sModelPartData* pModelPartData,SkinnedCharacterPart* pPart,bool* bPartHasFakeReflect);

	void CalculateMeshTotalVtxIdx(DWORD dwTotalVertex[MAX_BEINGLOD_LV],DWORD dwTotalIndex[MAX_BEINGLOD_LV],bool bHasLodMesh[MAX_BEINGLOD_LV]);
	void CopyVertexToRender(DWORD dwTotalVertex[MAX_BEINGLOD_LV],DWORD dwTotalIndex[MAX_BEINGLOD_LV],bool bHasLodMesh[MAX_BEINGLOD_LV]);

public:
	cSkinnedCharacterModel();
	virtual ~cSkinnedCharacterModel();
	// create a skeleton serializer from cSkeletonSerializerFactory.
		// iModelType ����� enum model type
		cSkeletonSerializer* createSkeletonSerializer(int iModelType, 
		cSkeletonSerializerFactory* pSkeletonSerializerFactory,const char 
		*szSkeletonSerializerName,const char *szPath);
	// get skeleton serializer.
	cSkeletonSerializer* getSkeletonSerializer() { return m_pSkeletonSerializer; }
	// create detail and lod  mesh serializer from cMeshSerializerFactory.
		void createMeshSerializer(cBeing* pOwnerBeing,int iCharacterPartType,cMeshSerializerFactory* pMeshSerializerFactory,const char *MeshSerializeFileName,const char *szPath);  
	// remove a detail and lod  mesh serializer by CHARACTER_PART_TYPE
		void removeMeshSerializer(int iCharacterPartType);

	//  Skinned Character LOD
	cMeshSerializer* GetPartMeshSerializer( int iPart, int iLodLevel = 0 );
	//  ����۲�
	int GetPartMeshTextureID( int iPart );

	//  ���NPC��MOB�Ƿ�ı���ͼ��װ�����
	bool setMeshSerializerTextureId( int iCharacterPartType, int iTextureId );

	//  combine each character part mesh to one character's mesh.
		void combineSkinnedCharacterPartsMesh(cBeing* pOwnerBeing);
	void ReCombineSkinnedCharacterPartsMesh(cBeing* pOwnerBeing);
	// get maxinum local space bounding box of Skinned Character.
		const D3DXVECTOR3& getLocalBoundingBoxMax(){ return *m_pLocalSpaceBoundingBoxMax; }
	// get mininum local space bounding box of Skinned Character.
		const D3DXVECTOR3& getLocalBoundingBoxMin(){ return *m_pLocalSpaceBoundingBoxMin; }
	// get maxinum world space bounding box of Skinned Character.
		const D3DXVECTOR3& getWorldBoundingBoxMax(){ return *m_pWorldSpaceBoundingBoxMax; }
	// get mininum world space bounding box of Skinned Character.
		const D3DXVECTOR3& getWorldBoundingBoxMin(){ return *m_pWorldSpaceBoundingBoxMin; }

	// set shared render buffer of Skinned Character.(it's not well mothed which creating render buffer from external cRenderBufferManage)
		void setSkinnedCharacterRenderBuffer( cRenderBuffer* pRenderBuffer ){ 
			if( m_pRenderBuffer == 0 ) m_pRenderBuffer = pRenderBuffer; }
	// set texture factory of Skinned Character to product it's texture.(it's not well mothed which creating render buffer from external cTextureDB)
		void setSkinnedCharacterTextureFactory( cTextureDB* pTextureDB ){ if( m_pTextureDB == 0 ) m_pTextureDB = pTextureDB; }
	// set render entity style of Skinned Character.
		void setSkinnedCharacterRenderEntity(RenderEntityStyle iStyle, cBeingRender* pRender = 0);
	// get render entity of Skinned Character.
	cBeingRender* getSkinnedCharacterRenderEntity( ){ return m_pRender; }

	// create a new animation action by index with fading out prevoius animation action 
		//and set new aniamtion action operation , play speed, fade out time. 
		//if it creates suceessfully, return it's animation action index, else return -1;
	int createAnimationAction( int iActionIndex, float fPlaySpeed,AnimationActionOperation animationActionOperation = aoDefaultKeyframes,float fFadeoutTime = 0.0f); 
	// create a new animation action by index into blending current animation action and set it's playing speed , fading out time. 
		int createAnimationActionIntoBlending( int iActionIndex, float fPlaySpeed, float fFadeoutTime );
	// get current animation action index.
	int getAniamtionActionIndex(){return m_CurnAction.m_iIndex; }
	// is current animation action finished? 
	bool isCurrentAnimationActionFinished() 
	{ 
		return (m_CurnAction.m_fAnimTime >= m_CurnAction.m_fAnimTimeLength); 
	}
	// get group size of animation action key info .
		int  getAnimationActionInfoCount();
	// get time length of animation action by action index 
		DWORD getAnimationActionTimeLength(int iActionId);
	// get time length of current animation action
		DWORD getCurrentAnimationActionTimeLength();
	//  ��ӡ  get played time of current animation action 
		float getCurrentAnimationActionTime( void ) { return m_CurnAction.m_fAnimTime; }
	// tick this animation action of skinned character by frame time.
		bool tickAnimationAction(float fElapsedTime);

	//  ���Ӱ��
	bool tickSkinnedCharacterMesh( bool bCreateSkinnedCharacterMesh, D3DXMATRIXA16* pMountMat );

	//  ����ʵ��Ӱ�����������
	void createSkinnedCharacterShadowData( LPDIRECT3DDEVICE9 pD3DDDevice );
	// ȡ��ʵ��Ӱ�ӵ�����
	sShadowData* getSkinnedCharacterShadowData( void ) { return m_pShadowData; } 
	void ComputeShadowData( cSkinnedCharacterModel* pMountModel );

	// �趨modelλ����lod�ľ���
	void setSkinnedCharacterPosition( float fPosX,float fPosY,float fPosZ);
	// set Skinned Character's direction
	void setSkinnedCharacterDirection(float fDirection);
	// set Skinned Character's scale
	void setSkinnedCharacterScale(float fScaleX,float fScaleY,float fScaleZ);
	// create Skinned Character's matrix
	void createSkinnedCharacterMatrix( void ); 
	// get Skinned Character's matrix
	const D3DXMATRIXA16& getSkinnedCharacterMatrix();

	//  ����
	void SetEquipPart( BYTE byPartID, cEquipModel* pEquipModel, int iTextureID,
		bool bWeaponRibbonLerp, cBeing* pOwnerBeing ); // �趨װ��ģ��
	cSkinnedCharacterEquipPart* GetEquipPartByPartID( BYTE byPartID );  // ���ݲ�λȡ��װ��
	cSkinnedCharacterEquipPart* GetEquipPartByIndex( int iIndex );    // ��������ȡ��װ��
	int GetEquipPartCount( void )                                     // ȡ��װ������
	{
		if( m_pEquipPartList )
		{
			return m_pEquipPartList->GetCount();
		}
		return 0;
	}

	//  ���NPC��MOB�Ƿ�ı���ͼ��װ�����
	bool setSkinnedCharacterEquip( int iAttackType, int iWeaponID );

	//  ���NPC��MOB�Ƿ�ı���ͼ��װ�����
	bool setSkinnedCharacterEquip( bool bAdorn01, bool bAdorn02, bool bAdorn03 );

	// update skinned character's equips.
	void updateSkinnedCharacterEquips();
	//
	static unsigned int getSkinnedCharacterModelCount() { return m_uiSkinnedCharacterModelCount; }

	// model effect
	void AddEquipEffect( BYTE byPartID, char* pName, BYTE byStatusType ); // ����װ����Ч

	void ForceAlpha(float falpha);

	// ��Ч�����(�Ƴ�����Ч���)
	void GetLinkWorldTransform( D3DXMATRIXA16& matTransform, BYTE byMagicLinkID, const D3DXMATRIXA16* pForceRotation = NULL );

	// �����Model��Ч�����
	void GetCenterWorldTransform( D3DXMATRIXA16& matTransform, const D3DXMATRIXA16* pForceRotation = NULL );

	// װ������������
	void ForceComputeSkinnedCharacterMesh( int iActionIndex, float fAnimTime ); 
	// ǿ�Ʋ�ĳ����(����Ƭ��)
	void ForceDraw( LPDIRECT3DDEVICE9 pD3DDDevice, D3DXMATRIXA16* pTransform ); 
	// ǿ�ƻ�(����Ƭ��)

	// ������ͼ����
	void SetFakeReflect( bool bShow );

	// ��������
	// ������û��charquit���ж�
	void AddToScene( bool bUseFocusLight, bool bCharQuit );           // �ӵ�Scene
	void SetFocusLight( void );                                       // ��Render���focus

	// ���²���ʾ����
	void HideWeaponEquip( bool bHide );                               // ��������

	// ˫���ʱǰ������ʾ����
	void SetCapeVisibility( bool bStatus );

	//  Skinned Character LOD
	static void SetLodDistance( DWORD dwLodLevel, float fDistance )   // �趨Lod	�ľ���
	{
		if( dwLodLevel < MAX_BEINGLOD_LV )
		{
			m_fLodDistanceSq[dwLodLevel] = fDistance * fDistance;
		}
	}

	// Will
	void SetCurrentAnimationActionFinished() 
	{ 
		m_CurnAction.m_fAnimTime = m_CurnAction.m_fAnimTimeLength; 
	}
	void SetCurrentAnimationActionTime(float Time) 
	{ 
		m_CurnAction.m_fAnimTime = Time; 
	}
	// 
	void SetCurrentAnimationActionPlaySpeed( float fPlaySpeed )       // �趨Ŀǰ���������ٶ�
	{
		m_CurnAction.m_fPlaySpeed = fPlaySpeed;
	}

	// Ф��
	bool HasParts( void ) { return ( m_pPart != NULL ); }             // �Ƿ��в�λ��Model
	//  ����
	void SetEquipActionIndex( BYTE byPartID, int iActionIndex );      // �趨����
	// �����¼�
	cAnimationActionInfo* GetAnimationActionInfo( int iActionIndex ); // ȡ�ö�����Ѷ
	// �����Ӱ������
	sMountCameraAdjust* GetSkeletonMountCameraAdjust( void );         // ȡ�������Ӱ������
	void ResetLocalBoundingBox( void );                               // ����Local Bounding Box

	const D3DXMATRIXA16& GetTranslation( void ) { return m_matTranslation; }
	const D3DXMATRIXA16& GetScaling( void ) { return m_matScaling; }

	//  Thread
	bool GetResourceReady( void );                                    // ģ�������Դ�Ƿ��ȡ���
	bool ResourceFinishLoad( BYTE byEquipPartID );                    // ģ�������Դ��ȡ����
};

}

#endif
