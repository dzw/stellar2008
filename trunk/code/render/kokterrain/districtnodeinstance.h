#pragma once
#ifndef KOK_DIRSTRICTNODEINSTANCE_H
#define KOK_DIRSTRICTNODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class KOK::DistrictNodeInstance

    (C) 2008 cTuo
*/
#include "models/nodes/statenodeinstance.h"

//------------------------------------------------------------------------------
namespace KOK
{
class DistrictNodeInstance : public Models::StateNodeInstance
{
    DeclareClass(DistrictNodeInstance);
public:
    /// constructor
    DistrictNodeInstance();
    /// destructor
    virtual ~DistrictNodeInstance();
    
    /// perform rendering
    virtual void Render();

protected:
    /// notify that we are visible
    virtual void OnNotifyVisible(IndexT frameIndex);
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

