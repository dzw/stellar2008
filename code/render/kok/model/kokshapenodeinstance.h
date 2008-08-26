#pragma once
#ifndef KOK_KOKSHAPENODEINSTANCE_H
#define KOK_KOKSHAPENODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class KOK::KokShapeNodeInstance

    (C) 2008 cTuo
*/
#include "models/nodes/transformnodeinstance.h"

//------------------------------------------------------------------------------
namespace KOK
{
class KokShapeNodeInstance : public Models::TransformNodeInstance
{
    DeclareClass(KokShapeNodeInstance);
public:
    /// constructor
    KokShapeNodeInstance();
    /// destructor
    virtual ~KokShapeNodeInstance();

    /// perform rendering
    virtual void Render();
	virtual void Update();
	virtual void ApplyState();
	virtual void OnNotifyVisible(IndexT frameIndex);

protected:
	
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

