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