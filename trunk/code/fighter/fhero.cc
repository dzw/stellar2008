//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fighter/fhero.h"

namespace Fighter
{
ImplementClass(Fighter::FHero, 'HERO', Fighter::FPlayer);

using namespace Graphics;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
FHero::FHero()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FHero::~FHero()
{

}

void 
FHero::Init()
{
	const Ptr<Stage>& stage = GraphicsServer::Instance()->GetDefaultView()->GetStage();
	this->model = ModelEntity::Create();
	this->model->SetResourceId(ResourceId("wow:Character\\Bloodelf\\male\\bloodelfmale.m2"));
	stage->AttachEntity(this->model.upcast<GraphicsEntity>());

	this->camera = FCameraManager::Instance()->GetCamera();
	n_assert(this->camera.isvalid());

}

void 
FHero::Clear()
{
	this->model = 0;
}

void
FHero::Walk(const Math::vector& dir, const Util::String& animName)
{
	Math::vector direction = dir;
	direction = this->TransformDirection(dir);
	direction.y() = 0.0f;
	direction = vector::normalize(direction);

	matrix44 entityMatrix = this->GetEntity()->GetMatrix44(Attr::Transform);
	//matrix44 mat = this->graphicsEntities[0]->GetTransform(); 
	//mat.translate(dir);
	this->smoothedPosition.SetGoal(entityMatrix.getpos_component()+direction);
	//this->graphicsEntities[0]->SetTransform(mat);

	this->lookatDirection = direction;
	matrix44 fixedTransform = matrix44::lookatlh(entityMatrix.getpos_component(), entityMatrix.getpos_component() + this->lookatDirection, vector::upvec());
	polar headingAngles(fixedTransform.getz_component());
	this->smoothedHeading.SetGoal(headingAngles.rho);

	Graphics::ActorEntity* Entity = this->GetGraphicsEntity();
	Entity->SetBaseAnimation(animName, 0.2f, 0.0f, true, true, 0.2f);
}

} // namespace Tools
