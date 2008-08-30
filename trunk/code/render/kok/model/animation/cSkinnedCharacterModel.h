#ifndef cBeingModelH
#define cBeingModelH
//------------------------------------------------------------------------------
/**
	@class KOK::cSkinnedCharacterModel

	生物的基本模型
	
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
	// 因为动作档的特例太过"独特", 目前先选择较快的解决方案, 使用 type 做切割.
	enum 
	{ // model type
		mtDEFAULT = 0, // Default 暂时归类为 MOB 和 NPC
		mtMOB,    // MOB骨架
		mtNPC,    // NPC骨架
		mtPLAYER, // 玩家骨架
		mtMOUNT,  // 骑乘物骨架
	};
	enum AnimationActionOperation
	{ // Animation Action Operation
		aoDefaultKeyframes = 0, // 忽略最后一个Frame. 
		aoIgnoreCountBackwards1ndKeyframe = 1, // 忽略最后一个keyframe. 
		// Animation可能会出现的两种状况.
		// 第一张Frame与最后一张Frame一样.(例如走路跑步动作)
		//   美术编辑方便.
		//   程式进行Loop动作播放时较方便(不必回头拿第一个Frame和最后一Frame做内插)
		// 第一张Frame与最后一张Frame不同.(例如死亡动作)
		//   美术编辑方便
		//   单次播放动作, 可能会接到下一个动作的第一张, 这时只需要将最后一张忽略即可.
		aoCountBackwards1ndKeyframe = 2,   // 直接跳至最后一个keyframe.
		aoCountBackwards2ndKeyframe = 3    // 直接跳至最后二个keyframe.
	};
	enum RenderEntityStyle{ // RenderStyle
		rsNone = 0x80000000,
		rsDefault  = 0, // 一般绘制
		rsAddColor = 1, // 加色绘制 
		rsNoLight  = 2, // 不打光
		rsFadeIn   = 3, //fade in 
		rsFadeOut  = 4, //fade out
	};
private:
	// Skinned Character Part has detail and lod cMeshSerializer and its' teture id.
	class SkinnedCharacterPart
	{
	public:
		//  Skinned Character LOD
		cMeshSerializer*  m_pMesh[MAX_BEINGLOD_LV];             // 各Lod Level的Mesh

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
	bool          m_bUpadteWorldMatrix;                               // 更新世界矩阵
	int           m_iDirection;                                       // 面向(角度)
	stActionState m_CurnAction;                                       // 目前的动作
	stActionState m_FadeoutAction;                                    // 淡出的动作
	float         m_fFadeoutTimeRemain;                               // 剩余淡出时间...
	float         m_fFadeoutTime;                                     // 总淡出时间
	D3DXVECTOR3  *m_pWorldSpaceBoundingBoxMin,*m_pWorldSpaceBoundingBoxMax; 
	// World Space Bounding Box
	D3DXVECTOR3  *m_pLocalSpaceBoundingBoxMin,*m_pLocalSpaceBoundingBoxMax; 
	// Local Space Bounding Box
	//  实体影子
	sShadowData*  m_pShadowData;                                      // 画实体影子所需资料 
		RenderEntityStyle m_iRenderStyle;

	//同类型的 cBeingModel 会共用一个 cSkeletonSerializer
	cSkeletonSerializer *m_pSkeletonSerializer; 
	// Skinned Character Parts which have detail and lod cMeshSerializer and its' texture id.
	SkinnedCharacterPart* m_pPart;
	// shared render buffer of Skinned Character.
	cRenderBuffer* m_pRenderBuffer;
	// render entity of Skinned Character.
	cBeingRender *m_pRender;
	// texture factory of Skinned Character to product it's texture.
	cTextureDB	 *m_pTextureDB;
	//  钓竿
	BTList*       m_pEquipPartList;                                   // 储存各装备List
	//  动作武器显示
	bool          m_bHideWeaponEquip;                                 // 是否隐藏武器

	//  Thread
	BYTE          m_byWaitResourceCount;                              // 等待Thread读取完模型资源数量

	// weapon of None-Combined Skinned Character.
	char* m_szNoneCombinedSkinnedCharacterWeaponName;
	// adornments of None-Combined Skinned Character.
	bool m_bNoneCombinedSkinnedCharacterAdorn01;
	bool m_bNoneCombinedSkinnedCharacterAdorn02;
	bool m_bNoneCombinedSkinnedCharacterAdorn03;

	sComputeAnimParam m_sComputeAnimParam;

	//  Skinned Character LOD
	static float m_fLodDistanceSq[MAX_BEINGLOD_LV];                   // 各Lod Level的距离平方

	// check Skinned Character Parts is combined?
	bool checkSkinnedCharacterPartsCombined();
	// 检查此Mesh(饰品装备)是否要显示
	bool checkEquipMeshVisible( char* pMeshName );

#ifdef _CHECK_NOUSE_ANIMATION
	SkeletonHierarchyUsabilityMap    m_SkeletonHierarchyUnUsedMap;                
	// 用来存没有用到的frame
	void getUnusedSkeletonHierarchy( cSkeletonHierarchy* pSkeletonHierarchy, 
		SkeletonHierarchyUsabilityMap& SkeletonHierarchyUsedMap, bool& bParentUse );  
	// 找出没用到的frame
	void getSubMeshUsedSkeletonHierarchy( SkeletonHierarchyUsabilityMap& SkeletonHierarchyUsedMap, 
		cSubMeshSerializer* pSubMeshSerial );// 找出SubMesh有用到的frame
#endif

		bool UpdateRenderMesh( D3DXMATRIXA16* pRootTransform, bool bUpdateRenderMesh );

	//  Skinned Character LOD
	void SetPartMeshSerializer( int iPart, int iLodLevel, cMeshSerializer* pMeshSerializer ); // 设定各部位的Mesh
	void ComputeLod( void );                                          // 计算目前的Lod Level

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
		// iModelType 请参照 enum model type
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
	//  人物观察
	int GetPartMeshTextureID( int iPart );

	//  检查NPC或MOB是否改变贴图或装备配件
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
	//  脚印  get played time of current animation action 
		float getCurrentAnimationActionTime( void ) { return m_CurnAction.m_fAnimTime; }
	// tick this animation action of skinned character by frame time.
		bool tickAnimationAction(float fElapsedTime);

	//  马的影子
	bool tickSkinnedCharacterMesh( bool bCreateSkinnedCharacterMesh, D3DXMATRIXA16* pMountMat );

	//  产生实体影子所需的资料
	void createSkinnedCharacterShadowData( LPDIRECT3DDEVICE9 pD3DDDevice );
	// 取得实体影子的资料
	sShadowData* getSkinnedCharacterShadowData( void ) { return m_pShadowData; } 
	void ComputeShadowData( cSkinnedCharacterModel* pMountModel );

	// 设定model位置与lod的距离
	void setSkinnedCharacterPosition( float fPosX,float fPosY,float fPosZ);
	// set Skinned Character's direction
	void setSkinnedCharacterDirection(float fDirection);
	// set Skinned Character's scale
	void setSkinnedCharacterScale(float fScaleX,float fScaleY,float fScaleZ);
	// create Skinned Character's matrix
	void createSkinnedCharacterMatrix( void ); 
	// get Skinned Character's matrix
	const D3DXMATRIXA16& getSkinnedCharacterMatrix();

	//  钓竿
	void SetEquipPart( BYTE byPartID, cEquipModel* pEquipModel, int iTextureID,
		bool bWeaponRibbonLerp, cBeing* pOwnerBeing ); // 设定装备模型
	cSkinnedCharacterEquipPart* GetEquipPartByPartID( BYTE byPartID );  // 根据部位取得装备
	cSkinnedCharacterEquipPart* GetEquipPartByIndex( int iIndex );    // 根据索引取得装备
	int GetEquipPartCount( void )                                     // 取得装备总数
	{
		if( m_pEquipPartList )
		{
			return m_pEquipPartList->GetCount();
		}
		return 0;
	}

	//  检查NPC或MOB是否改变贴图或装备配件
	bool setSkinnedCharacterEquip( int iAttackType, int iWeaponID );

	//  检查NPC或MOB是否改变贴图或装备配件
	bool setSkinnedCharacterEquip( bool bAdorn01, bool bAdorn02, bool bAdorn03 );

	// update skinned character's equips.
	void updateSkinnedCharacterEquips();
	//
	static unsigned int getSkinnedCharacterModelCount() { return m_uiSkinnedCharacterModelCount; }

	// model effect
	void AddEquipEffect( BYTE byPartID, char* pName, BYTE byStatusType ); // 增加装备特效

	void ForceAlpha(float falpha);

	// 特效连结点(移除旧特效规格)
	void GetLinkWorldTransform( D3DXMATRIXA16& matTransform, BYTE byMagicLinkID, const D3DXMATRIXA16* pForceRotation = NULL );

	// 非玩家Model特效连结点
	void GetCenterWorldTransform( D3DXMATRIXA16& matTransform, const D3DXMATRIXA16* pForceRotation = NULL );

	// 装备介面人物照
	void ForceComputeSkinnedCharacterMesh( int iActionIndex, float fAnimTime ); 
	// 强制播某动画(给照片用)
	void ForceDraw( LPDIRECT3DDEVICE9 pD3DDDevice, D3DXMATRIXA16* pTransform ); 
	// 强制画(给照片用)

	// 反光贴图开关
	void SetFakeReflect( bool bShow );

	// 反光整理
	// 增加有没有charquit的判断
	void AddToScene( bool bUseFocusLight, bool bCharQuit );           // 加到Scene
	void SetFocusLight( void );                                       // 将Render设成focus

	// 坐下不显示武器
	void HideWeaponEquip( bool bHide );                               // 隐藏武器

	// 双骑乘时前座不显示披风
	void SetCapeVisibility( bool bStatus );

	//  Skinned Character LOD
	static void SetLodDistance( DWORD dwLodLevel, float fDistance )   // 设定Lod	的距离
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
	void SetCurrentAnimationActionPlaySpeed( float fPlaySpeed )       // 设定目前动作播放速度
	{
		m_CurnAction.m_fPlaySpeed = fPlaySpeed;
	}

	// 肖像
	bool HasParts( void ) { return ( m_pPart != NULL ); }             // 是否有部位的Model
	//  钓竿
	void SetEquipActionIndex( BYTE byPartID, int iActionIndex );      // 设定动画
	// 击中事件
	cAnimationActionInfo* GetAnimationActionInfo( int iActionIndex ); // 取得动作资讯
	// 骑乘摄影机调整
	sMountCameraAdjust* GetSkeletonMountCameraAdjust( void );         // 取得骑乘摄影机调整
	void ResetLocalBoundingBox( void );                               // 重置Local Bounding Box

	const D3DXMATRIXA16& GetTranslation( void ) { return m_matTranslation; }
	const D3DXMATRIXA16& GetScaling( void ) { return m_matScaling; }

	//  Thread
	bool GetResourceReady( void );                                    // 模型相关资源是否读取完毕
	bool ResourceFinishLoad( BYTE byEquipPartID );                    // 模型相关资源读取结束
};

}

#endif
