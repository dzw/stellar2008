//------------------------------------------------------------------------------
//  shapenodeinstance.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/m2/m2characternodeinstance.h"
//#include "coregraphics/renderdevice.h"
//#include "models/nodes/characternode.h"
//#include "models/modelinstance.h"
//#include "nebula2/anim/managedanimation.h"
//#include "timing/timer.h"
//#include "coregraphics/shaperenderer.h"
#include "wow/m2/m2characternode.h"
#include "models/modelinstance.h"

namespace WOW
{
ImplementClass(WOW::M2CharacterNodeInstance, 'M2CI', Models::CharacterNodeInstance);

using namespace CoreGraphics;
using namespace Math;
using namespace Models;

//------------------------------------------------------------------------------
/**
*/
M2CharacterNodeInstance::M2CharacterNodeInstance()
{
}

//------------------------------------------------------------------------------
/**
*/
M2CharacterNodeInstance::~M2CharacterNodeInstance()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
M2CharacterNodeInstance::OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
    TransformNodeInstance::OnAttachToModelInstance(inst, node, parentNodeInst);
    n_assert(node->IsA(M2CharacterNode::RTTI));

    this->ValidateCharacter();
}

//------------------------------------------------------------------------------
/**
*/
void
M2CharacterNodeInstance::ValidateCharacter()
{
    const Ptr<M2CharacterNode>& charNode = this->GetModelNode().downcast<M2CharacterNode>();
    if (!this->character.isvalid() )//&& charNode->GetManagedAnimation()->GetState() == Resources::Resource::Loaded)
    {        
        // create character
        //this->character = M2Character::Create();
        // set character variables from char of characternode for use as instance character
        //this->character->Set(charNode->GetCharacter());
		this->character = charNode->GetCharacter();

        // create character set and copy nodes values (variations and skininfos)
        /*this->characterSet = Char::CharacterSet::Create();

        this->character->SetAnimation(charNode->GetManagedAnimation()->GetAnimation());
        this->character->SetupClipScheduler(charNode);    
        this->character->SetAnimEnabled(true);
        this->character->GetClipScheduler()->FadeClipAt(0, this->GetModelInstance()->GetTime(), Anim::AnimClipScheduler::FadeIn, 1.0);
        const Ptr<Char::CharacterSet>& nodeCharacterSet = charNode->GetCharacterSet();
        nodeCharacterSet->CopyTo(this->characterSet.get());*/
    }
}

//------------------------------------------------------------------------------
/**
*/
void
M2CharacterNodeInstance::OnRemoveFromModelInstance()
{
    this->character = 0;
    //this->characterSet = 0;

    TransformNodeInstance::OnRemoveFromModelInstance();
}

//------------------------------------------------------------------------------
/**
*/
void
M2CharacterNodeInstance::Update()
{    
    TransformNodeInstance::Update();

    this->ValidateCharacter();

    const Ptr<M2CharacterNode>& charNode = GetModelNode().downcast<M2CharacterNode>();
    if (charNode.isvalid()/*charNode->GetManagedAnimation()->GetState() == Resources::Resource::Loaded*/)
    {
        // get the sample time from the render context
        Timing::Time curTime = this->GetModelInstance()->GetTime();

        // check if characterset has to be applied
        /*if (characterSet->IsVariationDirty())
        {
            characterSet->ApplyCurrentVariation();
        }*/

        // evaluate the current state of the character skeleton
        this->character->EvaluateSkeleton(curTime);
        // make character dirty
        //this->character->SetFrameDirty();

        // apply characterset if dirty
        //this->characterSet->ApplySkinVisibility(this);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
M2CharacterNodeInstance::RenderDebug()
{
}

} // namespace Models
