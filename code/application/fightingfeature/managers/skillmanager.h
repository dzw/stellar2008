#ifndef MANAGERS_SKILLMANAGER_H
#define MANAGERS_SKILLMANAGER_H
//------------------------------------------------------------------------------
/**
    @class FightingFeature::SkillManager

    (C) 2009 cTuo 
*/
#include "game/manager.h"
#include "game/property.h"
#include "game/entity.h"
#include "addons/db/valuetable.h"
#include "util/hashmap.h"

//------------------------------------------------------------------------------
namespace FightingFeature
{
/// �����б�
enum SkillList
{
	SL_WalkUp		= 1,		// ��
	SL_WalkDown		= 10,		// ��
	SL_WalkLeft		= 100,		// ��
	SL_WalkRight	= 1000,		// ��
	SL_Attack		= 10000,	// ��(A)
	SL_Jump			= 100000,	// ��(J)
	SL_Defend		= 1000000,	// ��(D)

	SL_RunRight		= 2000,		// ������
	SL_RunLeft		= 200,		// ������
	SL_RunDown		= 20,
	SL_RunUp		= 2,

	SL_AssaultRight	= 102000,	// ����

	SL_Num
};

struct SkillInfo 
{
	DWORD keyValue;	// ��ֵ 
	int sp;			// ����������
	int mp;			// ħ����������
	int ap;			// ������
	int vp;			// �ƶ��ٶ�
	int delay;		// �������������ʱ
	Util::String animName;	// ��������

	SkillInfo():keyValue(0),sp(0),mp(0),ap(0),vp(0),delay(0){}
};

class SkillManager : public Game::Manager
{
    DeclareClass(SkillManager);
    DeclareSingleton(SkillManager);
public:
    /// constructor
    SkillManager();
    /// destructor
    virtual ~SkillManager();

	/// 
	bool HasSkillValue(DWORD val)const;
	///
	SkillInfo GetSkillValue(DWORD val)const;

protected:
	bool ParesSkillList();

	Util::HashMap<DWORD, SkillInfo, Util::DwordCompare> skillList;
};

}; // namespace Managers
//------------------------------------------------------------------------------
#endif