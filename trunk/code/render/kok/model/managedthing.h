#pragma once
#ifndef TERRAIN_MANAGEDTHING_H
#define TERRAIN_MANAGEDTHING_H
//------------------------------------------------------------------------------
/**
    (C) 2008 cTuo
*/
#include "models/managedmodel.h"
#include "kok/model/thing.h"

//------------------------------------------------------------------------------
namespace KOK
{
class ManagedThing : public Models::ManagedModel
{
    DeclareClass(ManagedThing);
public:
    /// get contained model resource
    const Ptr<Thing>& GetThing() const;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Thing>&
ManagedThing::GetThing() const
{
    return this->GetResource().downcast<Thing>();
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
