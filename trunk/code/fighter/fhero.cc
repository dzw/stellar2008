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
	FCameraManager::Instance()->SetChaseObject((FObject*)this);

	FighterApplication* fighterApp = (FighterApplication*)FighterApplication::Instance();
	Time time = fighterApp->GetTime();
	const matrix44& mat = this->model->GetTransform();
	this->smoothedPosition.Reset(time, 0.001f, -3.0f, mat.getpos_component());

	polar headingAngle(mat.getz_component());
    this->smoothedHeading.Reset(time, 0.001f, -3.0f, headingAngle.rho);
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
		// always evaluate the feedback loops to keep them in valid time delta
		FighterApplication* fighterApp = (FighterApplication*)FighterApplication::Instance();
		Time time = fighterApp->GetTime();
		this->smoothedPosition.Update(time);
		this->smoothedHeading.Update(time);

		// if current state and goal are already close to each other,
		// we don't send a position update
		// NOTE: this check must be made before the feedback look update!
		vector posError = this->smoothedPosition.GetState() - this->smoothedPosition.GetGoal();

		// only send update transform if anything changed
		if ((posError.length() > 0.1f))
		{
			// construct the new entity matrix
			//matrix44 mat = this->graphicsEntities[0]->GetTransform(); 
			//mat.translate(this->smoothedPosition.GetState());

			matrix44 matrix = matrix44::rotationy(this->smoothedHeading.GetState());
			matrix.translate(this->smoothedPosition.GetState());
			this->model->SetTransform(matrix);
			//n_printf("Pos: %f %f %f, %f\n", posError.x(), posError.y(), posError.z(), posError.length());
		}
		else
		{
			SetCurrentAnimation(50);
		}
	}
}

void 
FHero::SetSpeed(float f)
{
	this->smoothedPosition.SetGain(f);
}

void
FHero::Walk(const Math::vector& dir, const Util::String& animName)
{
	Math::vector direction = dir;
	direction = FCameraManager::Instance()->TransformDirection(dir);
	direction.y() = 0.0f;
	direction = vector::normalize(direction);

	/*const matrix44&*/matrix44 entityMatrix = this->model->GetTransform();
	//matrix44 mat = this->graphicsEntities[0]->GetTransform(); 
	//mat.translate(dir);
	this->smoothedPosition.SetGoal(entityMatrix.getpos_component()+direction);
	//this->graphicsEntities[0]->SetTransform(mat);

	this->lookatDirection = direction;
	matrix44 fixedTransform = matrix44::lookatlh(entityMatrix.getpos_component(), entityMatrix.getpos_component() + this->lookatDirection, vector::upvec());
	polar headingAngles(fixedTransform.getz_component());
	this->smoothedHeading.SetGoal(headingAngles.rho-1.57f);	// 方向不对，需要-1.57

	this->model.downcast<M2Entity>()->SetAnimation(54);

	SetSpeed(-3.0f);
}

void
FHero::Run(const Math::vector& dir, const Util::String& animName)
{
	Math::vector direction = dir;
	direction = FCameraManager::Instance()->TransformDirection(dir);
	direction.y() = 0.0f;
	direction = vector::normalize(direction);

	/*const matrix44&*/matrix44 entityMatrix = this->model->GetTransform();
	//matrix44 mat = this->graphicsEntities[0]->GetTransform(); 
	//mat.translate(dir);
	this->smoothedPosition.SetGoal(entityMatrix.getpos_component()+direction);
	//this->graphicsEntities[0]->SetTransform(mat);

	this->lookatDirection = direction;
	matrix44 fixedTransform = matrix44::lookatlh(entityMatrix.getpos_component(), entityMatrix.getpos_component() + this->lookatDirection, vector::upvec());
	polar headingAngles(fixedTransform.getz_component());
	this->smoothedHeading.SetGoal(headingAngles.rho-1.57f);	// 方向不对，需要-1.57

	this->model.downcast<M2Entity>()->SetAnimation(53);
	SetSpeed(-8.0f);
}

void 
FHero::SetCurrentAnimation(int id)
{
	if (this->model.isvalid())
		this->model.downcast<M2Entity>()->SetAnimation(id);
}

} // namespace Tools
