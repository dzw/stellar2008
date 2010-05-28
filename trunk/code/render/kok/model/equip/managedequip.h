#pragma once
#ifndef TERRAIN_MANAGEDEQUIP_H
#define TERRAIN_MANAGEDEQUIP_H
//------------------------------------------------------------------------------
/**
	װ��

    (C) 2008 cTuo
*/
#include "models/managedmodel.h"
#include "kok/model/equip/equip.h"
#include "kok/model/modeldef.h"

//------------------------------------------------------------------------------
namespace KOK
{
class ManagedEquip : public Models::ManagedModel
{
    DeclareClass(ManagedEquip);
public:
	ManagedEquip();
    /// get contained model resource
    const Ptr<Equip>& GetEquip() const;

	void SetPartType(EquipLinkerType& link);
	EquipLinkerType GetPartType()const;

	void SetTextureId(int texId);
	int GetTextureId()const;

	void SetWeaponRibbonLerp(bool b);
	bool GetWeaponRibbonLerp()const;
protected:
	EquipLinkerType linkId;
	int textureId;
	bool weaponribbonLerp;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Equip>&
ManagedEquip::GetEquip() const
{
    return this->GetResource().downcast<Equip>();
}

inline void 
ManagedEquip::SetPartType(EquipLinkerType& link)
{
	this->linkId = link;
}

inline EquipLinkerType 
ManagedEquip::GetPartType()const
{
	return this->linkId;
}

inline void 
ManagedEquip::SetTextureId(int texId)
{
	this->textureId = texId;
}

inline int 
ManagedEquip::GetTextureId()const
{
	return this->textureId;
}

inline void 
ManagedEquip::SetWeaponRibbonLerp(bool b)
{
	this->weaponribbonLerp = b;
}

inline bool 
ManagedEquip::GetWeaponRibbonLerp()const
{
	return this->weaponribbonLerp;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif