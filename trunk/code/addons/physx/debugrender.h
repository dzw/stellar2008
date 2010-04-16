#ifndef PHYSX_DEBUGRENDER_H
#define PHYSX_DEBUGRENDER_H
//------------------------------------------------------------------------------
/**
	@class PhysX::DebugRender

    (C) 2010
*/

#include "NxPhysics.h"
#include "CoreGraphics/shaderInstance.h"

//------------------------------------------------------------------------------
namespace PhysX
{

class DebugRender
{
public:
	DebugRender();
	~DebugRender();
	void RenderData(const NxDebugRenderable& data);

private:
	Ptr<CoreGraphics::ShaderInstance> shaderInstance;
};

}; // namespace Physics

//------------------------------------------------------------------------------
#endif
