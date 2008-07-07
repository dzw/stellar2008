#pragma once
#ifndef WOW_M2CHARACTERNODEINSTANCE_H
#define WOW_M2CHARACTERNODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    @class Models::M2CharacterNodeInstance
  
    The M2CharacterNodeInstance actually renders a skinned shape, and holds
    all the necessary per-instance state to do so.

    It knows the selected visible skins an active textures for
    actual rendering.
    
    (C) 2007 Radon Labs GmbH
*/
#include "models/nodes/characternodeinstance.h"
#include "models/nodes/shapenodeinstance.h"
#include "addons/nebula2/nebula2wrapper.h"
#include "wow/m2/m2character.h"

//------------------------------------------------------------------------------
namespace WOW
{
class M2CharacterNodeInstance : public Models::CharacterNodeInstance
{
    DeclareClass(M2CharacterNodeInstance);
public:
    /// constructor
    M2CharacterNodeInstance();
    /// destructor
    virtual ~M2CharacterNodeInstance();
    
    /// perform per-frame updates
    virtual void Update();    

    /// get character
    const Ptr<M2Character>& GetCharacter() const;
    /// get character set
    //const Ptr<Char::CharacterSet>& GetCharacterSet() const;

protected:
    /// called when attached to ModelInstance
	virtual void OnAttachToModelInstance(const Ptr<Models::ModelInstance>& inst, const Ptr<Models::ModelNode>& node, const Ptr<Models::ModelNodeInstance>& parentNodeInst);
    /// called when removed from ModelInstance
    virtual void OnRemoveFromModelInstance();
    /// render node specific debug shape
    virtual void RenderDebug();
    /// validate character 
    void ValidateCharacter();
    
private:    
    Ptr<M2Character> character;
    //Ptr<Char::CharacterSet> characterSet;
    //Util::Array<Char::SkinInfo> skinInfos;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<M2Character>& 
M2CharacterNodeInstance::GetCharacter() const
{
    return this->character;
}

//------------------------------------------------------------------------------
/**
*/
//inline const Ptr<Char::CharacterSet>& 
//M2CharacterNodeInstance::GetCharacterSet() const
//{
//    return this->characterSet;
//}
} // namespace Models
//------------------------------------------------------------------------------
#endif

