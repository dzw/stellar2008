#pragma once
#ifndef WOW_M2SKINSHAPENODEINSTANCE_H
#define WOW_M2SKINSHAPENODEINSTANCE_H
//------------------------------------------------------------------------------
/**
*/
#include "models/nodes/shapenodeinstance.h"
#include "wow/m2/m2characternodeinstance.h"

//------------------------------------------------------------------------------
namespace WOW
{
class M2SkinShapeNodeInstance : public Models::ShapeNodeInstance
{
    DeclareClass(M2SkinShapeNodeInstance);
public:
    /// constructor
    M2SkinShapeNodeInstance();
    /// destructor
    virtual ~M2SkinShapeNodeInstance();
    
    /// check for valid resources
    virtual void Update();
    /// perform rendering
    virtual void Render();
    /// set visible, used by charactersystem
    virtual void SetVisible(bool b);
	
protected:
    /// notify that we are visible
    virtual void OnNotifyVisible(IndexT frameIndex);
    /// render skinned mesh
    void RenderSkinning();
    /// called when attached to ModelInstance
	virtual void OnAttachToModelInstance(const Ptr<Models::ModelInstance>& inst, const Ptr<Models::ModelNode>& node, const Ptr<Models::ModelNodeInstance>& parentNodeInst);
    /// called when removed from ModelInstance
    virtual void OnRemoveFromModelInstance();
    
private:    
    Ptr<M2CharacterNodeInstance> character;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

