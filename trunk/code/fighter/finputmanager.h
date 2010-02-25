#pragma once
#ifndef FIGHTER_INPUTMANAGER_H
#define FIGHTER_INPUTMANAGER_H
//------------------------------------------------------------------------------
/**
	管理输入处理
*/
#include "core/refcounted.h"
#include "core/types.h"
#include "core/singleton.h"
#include "fhero.h"

//------------------------------------------------------------------------------
namespace Fighter
{
struct InputKey
{
	DWORD val;
	Timing::Time t;

	InputKey()
	{
	}
	InputKey(DWORD v, Timing::Time tt)
	{
		val = v;
		t = tt;
	}
};
class FInputManager : public Core::RefCounted
{
	DeclareClass(FInputManager);
	DeclareSingleton(FInputManager);
public:
    /// constructor
    FInputManager();
    /// destructor
    virtual ~FInputManager();
	/// open the application
    virtual bool Open();
    /// close the application
    virtual void Close();
	/// checking
	bool IsOpen()const;
	///
	void SetHero(const Ptr<FHero>& h);
	void Update();
	/// 检查输入是否有效，并返回有效的值
	DWORD CheckInvalid(const Util::Array<DWORD>& buffer)const;

private:
	bool ParesRule();
	void ProcessKeyBuffer(const Util::Array<DWORD>& keys);
	void UpdateAction(DWORD val);
	void UpdatePosition(DWORD keyValue);
	IndexT CheckInput(IndexT input);

	bool isOpen;
	Util::FixedArray<Util::Array<DWORD> > inputRule;
	Ptr<FHero> hero;
	Util::Array<InputKey> keyBuffer;
	DWORD preSkill;
};

inline bool 
FInputManager::IsOpen()const
{
	return isOpen;
}

inline void 
FInputManager::SetHero(const Ptr<FHero>& h)
{
	this->hero = h;
}

} // namespace Test
//------------------------------------------------------------------------------
#endif
