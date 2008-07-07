#pragma once
#ifndef WOW_MANAGEDWMO_H
#define WOW_MANAGEDWMO_H
//------------------------------------------------------------------------------
/**
    @class Models::ManagedModel
    
    A specialized managed resource for Models.
    
    (C) 2007 Radon Labs GmbH
*/
#include "resources/managedresource.h"
#include "wow/wmo/wmo.h"

//------------------------------------------------------------------------------
namespace WOW
{
class ManagedWMO : public Resources::ManagedResource
{
    DeclareClass(ManagedWMO);
public:
    /// get contained model resource
    const Ptr<WMO>& GetModel() const;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<WMO>&
ManagedWMO::GetModel() const
{
    return this->GetResource().downcast<WMO>();
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
