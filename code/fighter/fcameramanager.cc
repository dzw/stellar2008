//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fighter/FCameraManager.h"

namespace Fighter
{
ImplementClass(Fighter::FCameraManager, 'CARM', Core::RefCounted);
ImplementSingleton(Fighter::FCameraManager);

using namespace Graphics;
using namespace Resources;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
FCameraManager::FCameraManager():
isOpen(false)
{
    ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
FCameraManager::~FCameraManager()
{
if (this->IsOpen())
    {
        this->Close();
    }
	DestructSingleton;
}

bool 
FCameraManager::Open(const Ptr<View>& view)
{
	n_assert(!this->IsOpen());

	const Ptr<Stage>& s = view->GetStage();

	this->camera = CameraEntity::Create();
	s->AttachEntity(this->camera.upcast<GraphicsEntity>());
	view->SetCameraEntity(this->camera);

	this->isOpen = true;

	return true;
}

void 
FCameraManager::Close()
{
	n_assert(this->IsOpen());

	this->camera = 0;
	this->isOpen = false;
}

void 
FCameraManager::Update()
{
	
}

Math::vector
FCameraManager::TransformDirection(const Math::vector& dir)
{
	n_assert(this->camera.isvalid());
	matrix44 camTransform = this->camera->GetTransform();
	camTransform.setpos_component(float4(0.0f, 0.0f, 0.0f, 1.0f));
	return vector::transform(dir, camTransform);
}


} // namespace Tools
