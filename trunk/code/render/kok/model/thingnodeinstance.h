#pragma once
#ifndef KOK_THINGNODEINSTANCE_H
#define KOK_THINGNODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class KOK::ThingNodeInstance

	

    (C) 2008 cTuo
*/
#include "kok/model/kokshapenodeinstance.h"

//------------------------------------------------------------------------------
namespace KOK
{
class ThingNodeInstance : public KokShapeNodeInstance
{
    DeclareClass(ThingNodeInstance);
public:
    /// constructor
    ThingNodeInstance();
    /// destructor
    virtual ~ThingNodeInstance();

    /// perform rendering
    virtual void Render();
	virtual void Update();
	virtual void ApplyState();
	virtual void OnNotifyVisible(IndexT frameIndex);

	/// ´´½¨mesh
	void CreateMesh();
protected:
	void RenderBatch(IndexT index);
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

