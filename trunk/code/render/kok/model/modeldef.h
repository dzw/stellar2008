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
	Math::vector3 p ;			// λ��
	FLOAT		  fBlend;		// The blend weight	(0.0->1.0),  Ϊ�˻�ҡ�����������Ҫ��
	Math::vector3 n ;			// ������
	DWORD	      color;		// ��ɫ
	FLOAT         tu;			// ��ͼ����
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

/// ��ͨ�Ķ����ʽ
struct VertexFVF
{
	Math::vector3 p ;			// λ��
	Math::vector3 n ;			// ������
	//Math::nColor  color;		// ��ɫ
	Math::float2  tex;			// ��ͼ����

	VertexFVF()
	{
		p     = Math::vector3(0.0f,0.0f,0.0f);
		n     = Math::vector3(0.0f,1.0f,0.0f);
		//color = Math::nColor(1.0f, 1.0f, 1.0f, 1.0f);
		tex   = Math::float2(0.0f, 0.0f);
	};
};

/// �������������ʽ
struct VertexSkinnedFVF :public VertexFVF
{
	float weights[4];			// Ȩ��
	float bones[4];				// �任��������

	VertexSkinnedFVF():VertexFVF()
	{
		for (SizeT i = 0; i < 4; i++)
		{
			weights[i] = 0;
			bones[i] = 0;
		}
	}
};

// ��ֱ��ʹ��DX���ͣ����Զ�������ṹ���Ӱ���ȡ����
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



/// �Ǽ����
enum SkeleteonType
{
	sktCommon, // mob��npc�Ǽ� 
	sktMale,   // ���ԹǼ�
	sktFemale, // Ů�ԹǼ�
};

// ����λ
enum HumanPartType
{ // model part type -> mpt
	mptBody = 0, //����model
	mptHair,     //����model
	mptHead,     //ͷ��model	
	mptHand,     //��������
	mptArm,      //����С��
	mptPants,    //�°���model
	mptFoot,     //���ҽ���
	mptLeg,      //����С��
	mptCloak,    //����
	mptCOUNT,    //���岿λ����
};

enum MountPartType
{ // mount model part type 
	mptMountBody  = mptBody,
	mptMountArmor = mptHair,
	mptMountCOUNT,
};

/**
* װ����Ʒ��λ
*/
enum EquipLinkerType
{ // equip part type
	eptWeaponL = 0, // �����������(��ʽ����)
	eptWeaponR,     // �����������(��ʽ����)
	eptForearmL,    // ��ǰ�������(���,ȭ��)
	eptForearmR,    // ��ǰ�������(���,ȭ��)
	eptHandbackL,   // ���ֱ������(����,�ֶ�,�޶�)
	eptHandbackR,   // ���ֱ������(����,�ֶ�,�޶�)
	eptShoulderL,   // ��������(���)
	eptShoulderR,   // �Ҽ������(���)
	eptHelmet,      // ͷ�������
	eptFishingRod,  // ����(ʹ�����������)
	eptCOUNT,       // equip part type ���� 
};

/**
* ������Ʒ����
*/
enum EquipPartType
{ // equip model part MainClassID
	emUNKNOWN              =  0,  // δ֪
	emHoldSH               =  1,  //���ֳ�(S �� single, ����)
	emHoldDH               =  2,  //˫�ֳ�(D �� double, ˫��)
	emFightSH              =  3,  //���ֹ���
	emFightDH              =  4,  //˫�ֹ���
	emSwordSH              =  5,  //���ֽ�
	emSwordDH              =  6,  //˫�ֽ�
	emBladeSH              =  7,  //���ֵ�
	emBladeDH              =  8,  //˫�ֵ�
	emAxeSH                =  9,  //���ָ�
	emAxeDH                = 10,  //˫�ָ�
	emStickSH              = 11,  //���ֹ�
	emStickDH              = 12,  //˫�ֹ�
	emStabSH               = 13,  //���ִ�
	emStabDH               = 14,  //˫�ִ�
	emForkSH               = 15,  //���ֲ�
	emForkDH               = 16,  //˫�ֲ�
	emSpearSH              = 17,  //����ì
	emSpearDH              = 18,  //˫��ì
	emWristShield          = 19,  //���
	emHoldShield           = 20,  //�ֶ�
	emBow                  = 21,  //��
	emCrossbow             = 22,  //��
	emArrow                = 23,  //����
	emCosmetic             = 24,  //��Ʒ(Ŀǰδ��)
	emHelmet               = 25,  //ͷ��
	emShoulderL            = 26,  //����
	emShoulderR            = 27,  //�Ҽ��
	emCOUNT,
};

// ʱװ����
enum  // body fashion class
{
	bfClassChangeHair      = 8,   // Ҫ�����͵�ʱװ
	bfClassNoChangeHair    = 9,   // �������͵�ʱװ
};

//	�����ÿһ����λģ�͵�����Դ·�� ����
enum BeingFilePath
{
	BFP_FRAME      = 0,		//	���   
	BFP_BODY	   = 1,		//	����
	BFP_HAIR	   = 2,		//	ͷ��
	BFP_HEAD	   = 3,		//	ͷ������
	BFP_HAND       = 4,		//	��������(�¹��)
	BFP_ARM        = 5,		//	�����ֱ�(�¹��)
	BFP_PANTS	   = 6,		//	����
	BFP_FOOT       = 7,		//	���ҽ���(�¹��)
	BFP_LEG        = 8,		//	����С��(�¹��)
	BFP_CLOAK      = 9,		//	����(�¹��)
};

enum MountFilePath
{
	MFP_FRAME      = 0,		//	���
	MFP_BODY       = 1,		//	����
	MFP_ARMOR      = 2,		//	����
};

// �������̬
enum BeingType
{
	BEING_TYPE_PLAY = 0,	// ���(���ģ��)
	BEING_TYPE_NPC,			// NPC
	BEING_TYPE_MOB,			// MOB
	BEING_TYPE_MOUNT,		// ����
};

// ����Ķ���!!ԭ��������cSkinnedCharacterModel
enum BeingType1
{
	mtDEFAULT = 0,	// ��ʱ����Ϊ MOB �� NPC
	mtMOB,			// MOB�Ǽ�
	mtNPC,			// NPC�Ǽ�
	mtPLAYER,		// ��ҹǼ�
	mtMOUNT,		// �����Ǽ�
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

}  
//------------------------------------------------------------------------------
#endif

    