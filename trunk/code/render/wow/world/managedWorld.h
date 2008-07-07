#pragma once
#ifndef WOW_MANAGEDWORLD_H
#define WOW_MANAGEDWORLD_H
//------------------------------------------------------------------------------
/**
    @class Models::ManagedModel
    
    A specialized managed resource for Models.
    
    (C) 2007 Radon Labs GmbH
*/
#include "resources/managedresource.h"
#include "wow/world/world.h"

//------------------------------------------------------------------------------
namespace WOW
{
class ManagedWorld : public Resources::ManagedResource
{
    DeclareClass(ManagedWorld);
public:
    /// get contained model resource
    const Ptr<World>& GetModel() const;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<World>&
ManagedWorld::GetModel() const
{
    return this->GetResource().downcast<World>();
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
