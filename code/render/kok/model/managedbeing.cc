//------------------------------------------------------------------------------
//  ManagedThing.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/managedbeing.h"

namespace KOK
{
ImplementClass(KOK::ManagedBeing, 'MDBE', Models::ManagedModel);

ManagedBeing::ManagedBeing():
	textureId(-1)
{
}

} 