//------------------------------------------------------------------------------
//  managedequip.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/equip/managedequip.h"

namespace KOK
{
ImplementClass(KOK::ManagedEquip, 'MDEQ', Models::ManagedModel);

ManagedEquip::ManagedEquip():
	textureId(-1),
	weaponribbonLerp(false)
{
}

} 