#pragma once
#ifndef WOW_M2SKINSHAPENODEINSTANCE_H
#define WOW_M2SKINSHAPENODEINSTANCE_H
//------------------------------------------------------------------------------
/**
*/
#include "models/nodes/skinshapenodeinstance.h"
#include "addons/nebula2/nebula2wrapper.h"
#include "wow/m2/m2character.h"

//------------------------------------------------------------------------------
namespace WOW
{
class M2SkinShapeNodeInstance : public Models::SkinShapeNodeInstance
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
    /// render mesh fragment
    void RenderFragment(int primGroupIndex, Char::CharJointPalette& jointPalette);
    /// called when attached to ModelInstance
	virtual void OnAttachToModelInstance(const Ptr<Models::ModelInstance>& inst, const Ptr<Models::ModelNode>& node, const Ptr<Models::ModelNodeInstance>& parentNodeInst);
    /// called when removed from ModelInstance
    virtual void OnRemoveFromModelInstance();
    /// validate character
    void ValidateCharacter();
    
private:    
    Ptr<M2Character> character;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif

