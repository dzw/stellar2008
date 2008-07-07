//------------------------------------------------------------------------------
//  shapenodeinstance.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/m2/m2skinshapenodeinstance.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shaderserver.h"
#include "models/nodes/SkinShapeNode.h"
#include "models/modelinstance.h"
#include "models/nodes/characternodeinstance.h"
#include "timing/timer.h"
#include "wow/m2/m2characternodeinstance.h"

namespace WOW
{
ImplementClass(WOW::M2SkinShapeNodeInstance, 'M2SI', Models::SkinShapeNodeInstance);

using namespace CoreGraphics;
using namespace Math;
using namespace Models;

//------------------------------------------------------------------------------
/**
*/
M2SkinShapeNodeInstance::M2SkinShapeNodeInstance()
{
}

//------------------------------------------------------------------------------
/**
*/
M2SkinShapeNodeInstance::~M2SkinShapeNodeInstance()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
M2SkinShapeNodeInstance::OnAttachToModelInstance(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
    //n_assert(node->IsA(M2SkinShapeNodeInstance::RTTI));

    ShapeNodeInstance::OnAttachToModelInstance(inst, node, parentNodeInst);
    
    // create palette shader variable instance
    ShaderVariable::Semantic paletteSemantic = ShaderVariable::Semantic("JointPalette");
    this->CreateShaderVariableInstance(paletteSemantic);

    this->ValidateCharacter();
}

//------------------------------------------------------------------------------
/**
*/
void
M2SkinShapeNodeInstance::Update()
{
    this->ValidateCharacter();
    ShapeNodeInstance::Update();
}

//------------------------------------------------------------------------------
/**
*/
void
M2SkinShapeNodeInstance::ValidateCharacter()
{
    if (!this->character.isvalid())
    {
        // get character from characternodeinstance
        // find characternodeinstance
        int searchLevel = 4;
        Ptr<ModelNodeInstance> parent = this->GetParent();
        /*while (!parent->IsA(M2CharacterNodeInstance::RTTI) && searchLevel > 0)
        {
            parent = parent->GetParent();
            searchLevel--;
        }*/
        n_assert2(parent->IsA(M2CharacterNodeInstance::RTTI), "No CharacterNodeInstance found as parent!");
        this->character = parent.downcast<M2CharacterNodeInstance>()->GetCharacter();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
M2SkinShapeNodeInstance::OnRemoveFromModelInstance()
{
    this->character = 0;

    SkinShapeNodeInstance::OnRemoveFromModelInstance();
}

//------------------------------------------------------------------------------
/**
*/
void
M2SkinShapeNodeInstance::Render()
{
    // is mesh loaded for skinning?
    const Ptr<ShapeNode>& shapeNode = GetModelNode().downcast<ShapeNode>();
    if (shapeNode->GetManagedMesh()->GetState() == Resources::Resource::Loaded)
    {
        // render skinning
        StateNodeInstance::Render();
        this->RenderSkinning();
    }
    else
    {
        // unskinned placeholder has to be rendered
        ShapeNodeInstance::Render();
    }
}    

//------------------------------------------------------------------------------
/**
*/
void
M2SkinShapeNodeInstance::RenderSkinning()
{
    const Ptr<SkinShapeNode>& charNode = this->GetModelNode().downcast<SkinShapeNode>();
    int numFragments = charNode->GetNumFragments();
    int fragIndex;
    for (fragIndex = 0; fragIndex < numFragments; fragIndex++)
    {
        Char::CharFragment& fragment = charNode->GetFragmentArray()[fragIndex];
        RenderFragment(fragment.GetMeshGroupIndex(), fragment.GetJointPalette());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
M2SkinShapeNodeInstance::RenderFragment(int primGroupIndex, Char::CharJointPalette& jointPalette)
{
    static const int maxJointPaletteSize = 72;
    static matrix44 jointArray[maxJointPaletteSize];
    
    // extract the current joint palette from the skeleton in the
    // right format for the skinning shader
    int paletteSize = jointPalette.GetNumJoints();
    n_assert(paletteSize <= maxJointPaletteSize);

	if (paletteSize <= 0)
		return;

    if (this->character.isvalid())
    {
        M2CharSkeleton& skeleton = this->character->GetSkeleton();
        int paletteIndex;
        for (paletteIndex = 0; paletteIndex < paletteSize; paletteIndex++)
        {
			M2CharJoint& joint = skeleton.GetJointAt(jointPalette.GetJointIndexAt(paletteIndex));
			jointArray[paletteIndex] = joint.GetSkinMatrix44();
        }
    }
    else
    {
        int paletteIndex;
        for (paletteIndex = 0; paletteIndex < paletteSize; paletteIndex++)
        {            
            jointArray[paletteIndex] = matrix44::identity();
        }
    }

    // transfer the joint palette to the current shader        
    ShaderVariable::Semantic paletteSemantic = ShaderVariable::Semantic("JointPalette");
    const Ptr<CoreGraphics::ShaderVariableInstance>& shdVar = this->GetShaderVariableInstance(paletteSemantic);
    n_assert(shdVar.isvalid());
    shdVar->SetMatrixArray(jointArray, paletteSize);    
    // apply shader variable
    shdVar->Apply();

    // commit shader variable changes
    CoreGraphics::ShaderServer* shdServer = CoreGraphics::ShaderServer::Instance();
    shdServer->GetActiveShaderInstance()->Commit();

    // set current vertex and index range and draw mesh
    RenderDevice* renderDevice = RenderDevice::Instance();    
    const Ptr<SkinShapeNode>& charNode = this->GetModelNode().downcast<SkinShapeNode>();
    const Ptr<Mesh>& mesh = charNode->GetManagedMesh()->GetMesh();
    renderDevice->SetPrimitiveGroup(mesh->GetPrimitiveGroupAtIndex(primGroupIndex));
    renderDevice->Draw();
}

//------------------------------------------------------------------------------
/**
*/
void
M2SkinShapeNodeInstance::OnNotifyVisible(IndexT frameIndex)
{
    if (this->IsVisible())
    {
        // just tell our model node that we are a visible instance
        this->modelNode->AddVisibleNodeInstance(frameIndex, this);
    }
}

//------------------------------------------------------------------------------
/**
    Set visibility of node and its children, should not be call per frame!
*/
inline void 
M2SkinShapeNodeInstance::SetVisible(bool b)
{
    // TODO
    if (b && !this->IsVisible())
    {
        //this->GetModelNode().downcast<SkinShapeNode>()->RequestLoadResources();
    }
    else if (this->IsVisible())
    {
        //this->GetModelNode().downcast<SkinShapeNode>()->RequestUnloadResources();
    }
    ShapeNodeInstance::SetVisible(b);
}

} // namespace Models
