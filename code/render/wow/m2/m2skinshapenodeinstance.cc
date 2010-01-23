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
#include "wow/m2/m2skinshapenode.h"

namespace WOW
{
ImplementClass(WOW::M2SkinShapeNodeInstance, 'M2SI', Models::ShapeNodeInstance);

using namespace CoreGraphics;
using namespace Math;
using namespace Models;
using namespace Util;

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

    int searchLevel = 4;
    Ptr<ModelNodeInstance> parent = this->GetParent();
    while (parent.isvalid() && !parent->IsA(M2CharacterNodeInstance::RTTI) && searchLevel > 0)
    {
        parent = parent->GetParent();
        searchLevel--;
    }
	this->character = parent.downcast<M2CharacterNodeInstance>();

	bool isShow = node.downcast<M2SkinShapeNode>()->GetGeoset().id == 0;
	this->SetVisible(isShow);
}

//------------------------------------------------------------------------------
/**
*/
void
M2SkinShapeNodeInstance::Update()
{
    ShapeNodeInstance::Update();
}

//------------------------------------------------------------------------------
/**
*/
void
M2SkinShapeNodeInstance::OnRemoveFromModelInstance()
{
	this->character = 0;

    ShapeNodeInstance::OnRemoveFromModelInstance();
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
    const Ptr<M2SkinShapeNode>& charNode = this->GetModelNode().downcast<M2SkinShapeNode>();
 
	static const int maxJointPaletteSize = 72;
    static matrix44 jointArray[maxJointPaletteSize];
    
    const Array<int>& joints = charNode->GetJointArray();
    n_assert(joints.Size() <= maxJointPaletteSize);

	if (joints.Size() <= 0)
		return;

    //if (joints.Size() > 0)
    {
		const FixedArray<JointInstance>& jointData = character->GetJointData();
        IndexT index;
        for (index = 0; index < joints.Size(); index++)
        {
			JointInstance& joint = jointData[joints[index]];
			jointArray[index] = joint.mat;	// 还要加上mrot!!!
        }
    }
    /*else
    {
        int paletteIndex;
        for (paletteIndex = 0; paletteIndex < paletteSize; paletteIndex++)
        {            
            jointArray[paletteIndex] = matrix44::identity();
        }
    }*/

    // transfer the joint palette to the current shader        
    ShaderVariable::Semantic paletteSemantic = ShaderVariable::Semantic("JointPalette");
    const Ptr<CoreGraphics::ShaderVariableInstance>& shdVar = this->GetShaderVariableInstance(paletteSemantic);
    n_assert(shdVar.isvalid());
    shdVar->SetMatrixArray(jointArray, joints.Size());    
    // apply shader variable
    shdVar->Apply();

    // commit shader variable changes
    CoreGraphics::ShaderServer* shdServer = CoreGraphics::ShaderServer::Instance();
    shdServer->GetActiveShaderInstance()->Commit();

    // set current vertex and index range and draw mesh
    RenderDevice* renderDevice = RenderDevice::Instance();    
    /*const Ptr<ShapeNode>& charNode = this->GetModelNode().downcast<ShapeNode>();
    const Ptr<Mesh>& mesh = charNode->GetManagedMesh()->GetMesh();
    renderDevice->SetPrimitiveGroup(mesh->GetPrimitiveGroupAtIndex(primGroupIndex));*/
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
