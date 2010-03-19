#pragma once
#ifndef WOW_M2ENTITY_H
#define WOW_M2ENTITY_H
//------------------------------------------------------------------------------
/**
    @class Graphics::M2Entity

    (C) 2010 cTuo
*/    
#include "graphics/modelentity.h"
#include "wow/m2/m2characternodeinstance.h"
#include "wow/m2/animmanager.h"

//------------------------------------------------------------------------------
namespace WOW
{
class M2Entity : public Graphics::ModelEntity
{
    DeclareClass(M2Entity);
public:
    /// constructor
    M2Entity();
    /// destructor
    virtual ~M2Entity();
    /// called when attached to game entity
    virtual void OnActivate();
    /// called when removed from game entity
    virtual void OnDeactivate();
    /// called before rendering happens
    virtual void OnUpdate();
    
	void SetAnimation(int id, DWORD fadeout=500);
	void SetAttachAnimation(int id, DWORD fadeout=500);
	void SetSecondaryAnim(int id);
	bool IsAnimFinish()const;
private:
    Ptr<M2CharacterNodeInstance> m2Char;
	int curAnim;
	int secondaryAnim;
};

inline void 
M2Entity::SetAnimation(int id, DWORD fadeout)
{
	if (m2Char.isvalid() /*&& this->curAnim != id*/)
	{
		m2Char->SetCurAnimID(id, fadeout);
		this->curAnim = id;
	}
}

inline void 
M2Entity::SetAttachAnimation(int id, DWORD fadeout)
{
	if (m2Char.isvalid())
	{
		m2Char->SetAttachAnimId(id, fadeout);
	}
}

inline void 
M2Entity::SetSecondaryAnim(int id)
{
	if (m2Char.isvalid() && this->secondaryAnim != id)
	{
		m2Char->SetSecondaryAnimID(id);
		this->secondaryAnim = id;
	}
}

inline bool 
M2Entity::IsAnimFinish()const
{
	if (!m2Char.isvalid())
		return false;

	const AnimManager* anim = m2Char->GetAnimManager();
	if (anim != 0)
		return anim->IsCurAnimFinish();
	return true;
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
