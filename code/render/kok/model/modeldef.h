#pragma once
#ifndef KOK_MODELDEF_H
#define KOK_MODELDEF_H
//------------------------------------------------------------------------------
/**
    @class 

    (C) 2008 cTuo
*/
#include "math/vector3.h"
#include "math/float2.h"
#include "math/ncolor.h"
#include "memory/memory.h"
#include "io/stream.h"
#include "util/string.h"

//------------------------------------------------------------------------------
namespace KOK
{

#define D3DFVF_MDLVTX (D3DFVF_XYZB1|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)//

struct MDLVTX
{
	Math::vector3 p ;			// 位置
	FLOAT		  fBlend;		// The blend weight	(0.0->1.0),  为了会摇动的树与草所要的
	Math::vector3 n ;			// 法向量
	DWORD	      color;		// 颜色
	FLOAT         tu;			// 贴图坐标
	FLOAT         tv;

	MDLVTX()
	{
		p     = Math::vector3(0.0f,0.0f,0.0f);
		fBlend = 1.0;
		n     = Math::vector3(0.0f,1.0f,0.0f);
		color = N_XRGB(255,255,255);
		tu    = 0.0f ;
		tu    = 0.0f ;
	};
};

/// 普通的顶点格式
struct VertexFVF
{
	Math::vector3 p ;			// 位置
	Math::vector3 n ;			// 法向量
	//Math::nColor  color;		// 颜色
	Math::float2  tex;			// 贴图坐标

	VertexFVF()
	{
		p     = Math::vector3(0.0f,0.0f,0.0f);
		n     = Math::vector3(0.0f,1.0f,0.0f);
		//color = Math::nColor(1.0f, 1.0f, 1.0f, 1.0f);
		tex   = Math::float2(0.0f, 0.0f);
	};
};

/// 骨骼动画顶点格式
struct VertexSkinnedFVF :public VertexFVF
{
	float weights[4];			// 权重
	float bones[4];				// 变换矩阵索引

	VertexSkinnedFVF():VertexFVF()
	{
		for (SizeT i = 0; i < 4; i++)
		{
			weights[i] = 0;
			bones[i] = 0;
		}
	}
};

// 不直接使用DX类型，所以定义这个结构，从包中取数据
struct AttributeRange 
{
	DWORD AttribId;
	DWORD FaceStart;
	DWORD FaceCount;
	DWORD VertexStart;
	DWORD VertexCount;
};

enum EThingSubMeshSpecialType
{
	TYPE_NORMAL = 0,
	TYPE_HD,            // "m_hd"
	TYPE_LIGHT,         // "m_light"
	TYPE_REPEAT,        // "m_repeat"
	TYPE_SELECT,        // "m_select"
	TYPE_WATER,         // "m_water"
	TYPE_EFF,           // "e_ff"
	TYPE_EFLAME,        // "e_flame"
}; 

inline void 
ReadString(const Ptr<IO::Stream>& stream, Util::String& str, int len)
{
	Memory::MemoryMark mm(Memory::MemStack);

	char* buf = new(Memory::MemStack, len+1)char;
	stream->Read(buf, len);
	buf[len] = 0;
	str = buf;
}



/// 骨架类别
enum SkeleteonType
{
	sktCommon, // mob或npc骨架 
	sktMale,   // 男性骨架
	sktFemale, // 女性骨架
};

// 各部位
enum HumanPartType
{ // model part type -> mpt
	mptBody = 0, //身体model
	mptHair,     //发型model
	mptHead,     //头部model	
	mptHand,     //左右手掌
	mptArm,      //左右小臂
	mptPants,    //下半身model
	mptFoot,     //左右脚掌
	mptLeg,      //左右小腿
	mptCloak,    //披风
	mptCOUNT,    //身体部位总数
};

enum MountPartType
{ // mount model part type 
	mptMountBody  = mptBody,
	mptMountArmor = mptHair,
	mptMountCOUNT,
};

/**
* 装备饰品部位
*/
enum EquipLinkerType
{ // equip part type
	eptWeaponL = 0, // 左手心连结点(各式武器)
	eptWeaponR,     // 右手心连结点(各式武器)
	eptForearmL,    // 左前臂连结点(腕盾,拳刃)
	eptForearmR,    // 右前臂连结点(腕盾,拳刃)
	eptHandbackL,   // 左手背连结点(塔盾,持盾,巨盾)
	eptHandbackR,   // 右手背连结点(塔盾,持盾,巨盾)
	eptShoulderL,   // 左肩连结点(肩甲)
	eptShoulderR,   // 右肩连结点(肩甲)
	eptHelmet,      // 头盔连结点
	eptFishingRod,  // 钓竿(使用右手连结点)
	eptCOUNT,       // equip part type 总数 
};

/**
* 武器饰品分类
*/
enum EquipPartType
{ // equip model part MainClassID
	emUNKNOWN              =  0,  // 未知
	emHoldSH               =  1,  //单手持(S 是 single, 单手)
	emHoldDH               =  2,  //双手持(D 是 double, 双手)
	emFightSH              =  3,  //单手攻击
	emFightDH              =  4,  //双手攻击
	emSwordSH              =  5,  //单手剑
	emSwordDH              =  6,  //双手剑
	emBladeSH              =  7,  //单手刀
	emBladeDH              =  8,  //双手刀
	emAxeSH                =  9,  //单手斧
	emAxeDH                = 10,  //双手斧
	emStickSH              = 11,  //单手棍
	emStickDH              = 12,  //双手棍
	emStabSH               = 13,  //单手刺
	emStabDH               = 14,  //双手刺
	emForkSH               = 15,  //单手叉
	emForkDH               = 16,  //双手叉
	emSpearSH              = 17,  //单手矛
	emSpearDH              = 18,  //双手矛
	emWristShield          = 19,  //腕盾
	emHoldShield           = 20,  //持盾
	emBow                  = 21,  //弓
	emCrossbow             = 22,  //弩
	emArrow                = 23,  //箭袋
	emCosmetic             = 24,  //饰品(目前未用)
	emHelmet               = 25,  //头盔
	emShoulderL            = 26,  //左肩甲
	emShoulderR            = 27,  //右肩甲
	emCOUNT,
};

// 时装发型
enum  // body fashion class
{
	bfClassChangeHair      = 8,   // 要换发型的时装
	bfClassNoChangeHair    = 9,   // 不换发型的时装
};

//	生物的每一个部位模型档案来源路径 明德
enum BeingFilePath
{
	BFP_FRAME      = 0,		//	框架   
	BFP_BODY	   = 1,		//	上衣
	BFP_HAIR	   = 2,		//	头发
	BFP_HEAD	   = 3,		//	头及脸型
	BFP_HAND       = 4,		//	左右手掌(新规格)
	BFP_ARM        = 5,		//	左右手臂(新规格)
	BFP_PANTS	   = 6,		//	裤子
	BFP_FOOT       = 7,		//	左右脚掌(新规格)
	BFP_LEG        = 8,		//	左右小腿(新规格)
	BFP_CLOAK      = 9,		//	披风(新规格)
};

enum MountFilePath
{
	MFP_FRAME      = 0,		//	框架
	MFP_BODY       = 1,		//	身体
	MFP_ARMOR      = 2,		//	护甲
};

// 生物的型态
enum BeingType
{
	BEING_TYPE_PLAY = 0,	// 玩家(组合模型)
	BEING_TYPE_NPC,			// NPC
	BEING_TYPE_MOB,			// MOB
	BEING_TYPE_MOUNT,		// 座骑
};

// 多余的定义!!原来定义在cSkinnedCharacterModel
enum BeingType1
{
	mtDEFAULT = 0,	// 暂时归类为 MOB 和 NPC
	mtMOB,			// MOB骨架
	mtNPC,			// NPC骨架
	mtPLAYER,		// 玩家骨架
	mtMOUNT,		// 骑乘物骨架
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

}  
//------------------------------------------------------------------------------
#endif

    