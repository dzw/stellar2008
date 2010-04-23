//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fighter/flevel.h"
#include "physx/physxserver.h"

namespace Fighter
{
ImplementClass(Fighter::FLevel, 'FLEL', Core::RefCounted);

using namespace Graphics;
using namespace Resources;
using namespace PhysX;

//------------------------------------------------------------------------------
/**
*/
FLevel::FLevel()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FLevel::~FLevel()
{

}

void 
FLevel::Init(const Ptr<Stage>& s)
{
	this->stage = s;

	this->model = ModelEntity::Create();
	this->model->SetResourceId(ResourceId("mdl:examples/ground.n2"));
	this->stage->AttachEntity(this->model.upcast<GraphicsEntity>());

#ifdef PHYSX
	PhysXServer::Instance()->CreateMeshFromFile("msh:examples/ground_c_0.nvx2");
#endif
}

void 
FLevel::Clear()
{
	this->model = 0;
	this->stage = 0;
}

} // namespace Tools
