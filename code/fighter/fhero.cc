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
FHero::FHero():
curAnim(AID_Idle)
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
			NextAnim(AID_Idle);
		}
	}
}

void
FHero::NextAnim(BYTE action)
{
	if (!this->model.isvalid())
		return;

	const Ptr<M2Entity>& entity = this->model.downcast<M2Entity>();

	switch (this->curAnim)
	{
	case AID_JumpStart:
		{
			if (entity->IsAnimFinish())
				this->curAnim = AID_Jump;
		}
		break;
	//case AID_Jump:
	//	{
	//		//if (与地面的做判断，如果小于等于地面高度)
	//	//	{
	//	//		this->curAnim = AID_JumpEnd;
	//	//	}
	//	}
	//	break;
	//case AID_JumpEnd:
	//	{
	//		//if (entity->IsAnimFinish())
	//		//	this->curAnim = AID_Idle;
	//	}
	//	break;
	case AID_Attack:
		{
			if (entity->IsAnimFinish())
				this->curAnim = action;
		}
		break;
	default:
		this->curAnim = action;
	}

	SetCurrentAnimation(this->curAnim);
}

void 
FHero::SetSpeed(float f)
{
	this->smoothedPosition.SetGain(f);
}

void 
FHero::SetCurrentAnimation(int id)
{
	if (this->model.isvalid())
		this->model.downcast<M2Entity>()->SetAnimation(id);
}

void
FHero::SetPosition(const Math::vector& dir)
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
}

void
FHero::Walk(const Math::vector& dir, const Util::String& animName)
{
	//SetPosition(dir);

	NextAnim(AID_Walk);

	//SetSpeed(-3.0f);
}

void
FHero::Run(const Math::vector& dir, const Util::String& animName)
{
	//SetPosition(dir);

	NextAnim(AID_Run);
	//SetSpeed(-8.0f);
}

//------------------------------------------------------------------------------
/**
	跳比较特殊，有三种状态：原地跳、走跳（走的时候跳）、跑跳（跑的时候跳）
	考虑到落地时候不同的动作，所以前两种跳是一样，最后一种不同。
	dir:走跳、跑跳才用到，原地跳时设0
	isRuning:是否是跑步状态
*/
void
FHero::Jump(const Math::vector& dir, const Util::String& animName, bool isRuning)
{
	//if (dir.length() > 0)
	//	SetPosition(dir);

	NextAnim(AID_JumpStart);
	this->curAnim = AID_JumpStart;

	/*if (isRuning)
		SetSpeed(-8.0f);
	else
		SetSpeed(-3.0f);*/
}

void
FHero::Defend(const Math::vector& dir, const Util::String& animName)
{
	NextAnim(AID_Defend);
}

void
FHero::Attack(const Math::vector& dir, const Util::String& animName)
{
	NextAnim(AID_Attack);
}

} // namespace Tools
