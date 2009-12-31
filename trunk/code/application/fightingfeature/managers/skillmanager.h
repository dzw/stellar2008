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
/// 技能列表
enum SkillList
{
	SL_WalkUp		= 1,		// 上
	SL_WalkDown		= 10,		// 下
	SL_WalkLeft		= 100,		// 左
	SL_WalkRight	= 1000,		// 右
	SL_Attack		= 10000,	// 攻(A)
	SL_Jump			= 100000,	// 跳(J)
	SL_Defend		= 1000000,	// 防(D)

	SL_RunRight		= 2000,		// 奔跑右
	SL_RunLeft		= 200,		// 奔跑左
	SL_RunDown		= 20,
	SL_RunUp		= 2,

	SL_AssaultRight	= 102000,	// 冲跳

	SL_Num
};

struct SkillInfo 
{
	DWORD keyValue;	// 键值 
	int sp;			// 体力消耗量
	int mp;			// 魔法力消耗量
	int ap;			// 攻击力
	int vp;			// 移动速度
	int delay;		// 动作结束后的延时
	Util::String animName;	// 动画名称

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