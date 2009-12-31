#ifndef MANAGERS_INPUTRULEMANAGER_H
#define MANAGERS_INPUTRULEMANAGER_H
//------------------------------------------------------------------------------
/**
    @class FightingFeature::InputRuleManager

    (C) 2009 cTuo 
*/
#include "game/manager.h"
#include "game/property.h"
#include "game/entity.h"
#include "addons/db/valuetable.h"

//------------------------------------------------------------------------------
namespace FightingFeature
{
// 键值
enum KeyValue
{
	UpValue		= 1,
	DownValue	= 10,
	LeftValue	= 100,
	RightValue	= 1000,
	AttactValue = 10000,
	JumpValue	= 100000,
	DefendValue= 1000000,
};

class InputRuleManager : public Game::Manager
{
    DeclareClass(InputRuleManager);
    DeclareSingleton(InputRuleManager);
public:
    /// constructor
    InputRuleManager();
    /// destructor
    virtual ~InputRuleManager();
	/// 检查输入是否有效，并返回有效的值
	DWORD CheckInvalid(const Util::Array<DWORD>& buffer)const;

protected:
	bool ParesRule();

	Util::FixedArray<Util::Array<DWORD> > inputRule;
};

}; // namespace Managers
//------------------------------------------------------------------------------
#endif