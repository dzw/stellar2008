//------------------------------------------------------------------------------
//  statenode.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/terrainrender.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/shadervariable.h"
#include "coregraphics/renderdevice.h"
#include "resources/resourcemanager.h"
#include "math/matrix44.h"
#include "coregraphics/transformdevice.h"

namespace KOK
{
ImplementClass(KOK::TerrainRender, 'TNRR', Core::RefCounted);

using namespace Util;
using namespace CoreGraphics;
using namespace Resources;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
TerrainRender::TerrainRender():
	shaderInstance(0)
{
    // empty    
}

//------------------------------------------------------------------------------
/**
*/
TerrainRender::~TerrainRender()
{
	if (this->shaderInstance.isvalid())
	{
		// discard shader instance
		this->shaderInstance->Discard();
		this->shaderInstance = 0;
	}

	this->shaderVariable = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainRender::Init()
{
    n_assert(!this->shaderInstance.isvalid());
    ShaderServer::ShaderParamBindMode paramBindMode = ShaderServer::Instance()->GetShaderParamBindMode();

    // create a new shader instance from the Shader attribute
	this->shaderInstance = ShaderServer::Instance()->CreateShaderInstance(ResourceId("shd:kokterrain"));
	this->shaderVariable = this->shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("DiffMap0"));
}

//------------------------------------------------------------------------------
/**
*/
bool
TerrainRender::ApplySharedState(const Ptr<ManagedTexture>& tex)
{
	if (!tex->GetTexture().isvalid())
		return false;

	this->shaderVariable->SetTexture(tex->GetTexture());
    // set our shader instance as current
    ShaderServer::Instance()->SetActiveShaderInstance(this->shaderInstance);

	
	matrix44 mat = matrix44::identity();
	TransformDevice* transformDevice = TransformDevice::Instance();
	
	transformDevice->SetModelTransform(mat);
	transformDevice->ApplyModelTransforms();    

	return true;
}

} // namespace Models
