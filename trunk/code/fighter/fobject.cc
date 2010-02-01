//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fighter/FObject.h"

namespace Fighter
{
ImplementClass(Fighter::FObject, 'OBJT', Core::RefCounted);

using namespace Graphics;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
FObject::FObject()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FObject::~FObject()
{

}

void 
FObject::Init()
{
	this->stage = s;

	this->model = ModelEntity::Create();
	this->model->SetResourceId(ResourceId("wow:Character\\Bloodelf\\male\\bloodelfmale.m2"));
	this->stage->AttachEntity(this->model.upcast<GraphicsEntity>());
}

void 
FObject::Clear()
{
	this->model = 0;
	this->stage = 0;
}

} // namespace Tools
