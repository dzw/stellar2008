//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fighter/flevel.h"

namespace Fighter
{
ImplementClass(Fighter::FLevel, 'FLEL', Core::RefCounted);

using namespace Graphics;
using namespace Resources;

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
	this->model->SetResourceId(ResourceId("wow:Character\\Bloodelf\\male\\bloodelfmale.m2"));
	this->stage->AttachEntity(this->model.upcast<GraphicsEntity>());
}

void 
FLevel::Clear()
{
	this->model = 0;
	this->stage = 0;
}

} // namespace Tools
