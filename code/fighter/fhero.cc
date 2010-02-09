//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fighter/fhero.h"
#include "graphics/stage.h"
#include "graphics/view.h"
#include "graphics/graphicsserver.h"
#include "fcameramanager.h"
#include "finputmanager.h"
#include "math/polar.h"
#include "wow/m2/m2entity.h"
#include "fmain.h"

namespace Fighter
{
ImplementClass(Fighter::FHero, 'HERO', Fighter::FPlayer);

using namespace Graphics;
using namespace Resources;
using namespace Math;
using namespace WOW;
using namespace Timing;

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
	this->model = M2Entity::Create();
	this->model->SetResourceId(ResourceId("wow:Character\\Bloodelf\\male\\bloodelfmale.m2"));
	stage->AttachEntity(this->model.upcast<GraphicsEntity>());

	this->camera = FCameraManager::Instance()->GetCamera();
	n_assert(this->camera.isvalid());

	FInputManager::Instance()->SetHero(this);
}

void 
FHero::Clear()
{
	if (this->model.isvalid())
	{
		const Ptr<Stage>& stage = GraphicsServer::Instance()->GetDefaultView()->GetStage();
		stage->RemoveEntity(this->model.upcast<GraphicsEntity>());
		this->model = 0;
	}
}

void 
FHero::Update()
{
	if (this->model.isvalid())
	{
		// if current state and goal are already close to each other,
		// we don't send a position update
		// NOTE: this check must be made before the feedback look update!
		vector posError = this->smoothedPosition.GetState() - this->smoothedPosition.GetGoal();
	    
		// always evaluate the feedback loops to keep them in valid time delta
		FighterApplication* fighterApp = (FighterApplication*)FighterApplication::Instance();
		Time time = fighterApp->GetTime();
		this->smoothedPosition.Update(time);
		this->smoothedHeading.Update(time);

		// only send update transform if anything changed
		if ((posError.length() > 0.001f))
		{
			// construct the new entity matrix
			//matrix44 mat = this->graphicsEntities[0]->GetTransform(); 
			//mat.translate(this->smoothedPosition.GetState());

			matrix44 matrix = matrix44::rotationy(this->smoothedHeading.GetState());
			matrix.translate(this->smoothedPosition.GetState());
			this->model->SetTransform(matrix);
		}
		else
		{
			SetCurrentAnimation(50);
		}
	}
}

void
FHero::Walk(const Math::vector& dir, const Util::String& animName)
{
	Math::vector direction = dir;
	direction = FCameraManager::Instance()->TransformDirection(dir);
	direction.y() = 0.0f;
	direction = vector::normalize(direction);

	const matrix44& entityMatrix = this->model->GetTransform();
	//matrix44 mat = this->graphicsEntities[0]->GetTransform(); 
	//mat.translate(dir);
	this->smoothedPosition.SetGoal(entityMatrix.getpos_component()+direction);
	//this->graphicsEntities[0]->SetTransform(mat);

	this->lookatDirection = direction;
	matrix44 fixedTransform = matrix44::lookatlh(entityMatrix.getpos_component(), entityMatrix.getpos_component() + this->lookatDirection, vector::upvec());
	polar headingAngles(fixedTransform.getz_component());
	this->smoothedHeading.SetGoal(headingAngles.rho);

	this->model.downcast<M2Entity>()->SetAnimation(54);
}

void 
FHero::SetCurrentAnimation(int id)
{
	if (this->model.isvalid())
		this->model.downcast<M2Entity>()->SetAnimation(id);
}

} // namespace Tools
