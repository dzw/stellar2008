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
//#include "wow/m2/m2entity.h"
#include "fmain.h"
#include "graphics/actorentity.h"

namespace Fighter
{
ImplementClass(Fighter::FHero, 'HERO', Fighter::FPlayer);

using namespace Graphics;
using namespace Resources;
using namespace Math;
//using namespace WOW;
using namespace Timing;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
FHero::FHero():
curAnim(AID_Idle),
dbgSpeed(-3.0f),
dbgMove(1.0f),
stopMove(false),
stopTurn(false),
curAnimTime(0)
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

	this->model = ActorEntity::Create();//M2Entity::Create();
	this->model->SetResourceId(ResourceId("mdl:characters/mensch_m.n2"/*"wow:Character\\Bloodelf\\male\\bloodelfmale.m2"*/));

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
#ifdef NEBULA3_DEBUG
	if (GetAsyncKeyState(VK_PRIOR) && 0x80 > 0)	// page up
		dbgSpeed += 0.2f;
	if (GetAsyncKeyState(VK_NEXT) && 0x80 > 0)	// page down
		dbgSpeed -= 0.2f;
	if (GetAsyncKeyState(VK_HOME) && 0x80 > 0)	// home 
		dbgMove += 0.2f;
	if (GetAsyncKeyState(VK_END) && 0x80 > 0)	// end
		dbgMove -= 0.2f;
#endif

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
		//else
		{
			const Ptr<ActorEntity>& entity = this->model.downcast<ActorEntity>();
			if (!entity->IsOverlayAnimationActive())
				NextAnim(AID_Idle);
		}
	}
}

void 
FHero::NextAnim(BYTE action)
{
	if (!this->model.isvalid() /*|| this->curAnim == action*/)
		return;
	const Ptr<ActorEntity>& entity = this->model.downcast<ActorEntity>();

	DWORD fadeout = 500;

	// 限制攻击等动作最大帧，同步所有攻击动作
	static int lastTime = 0;
	if (curAnimTime > 0)
	{
		int curTime = timeGetTime();
		curAnimTime = curAnimTime - (curTime - lastTime);
		lastTime = curTime;
		if (curAnimTime > 0)
		{
			if (!entity->IsOverlayAnimationActive())
				SetCurrentAnimation(AID_Idle, fadeout);
			return;
		}
	}
	else
	{
		if (action != AID_Idle && action != AID_Walk && action != AID_Run)
			curAnimTime = 1500;
	}

	/*BYTE action;
	if (!this->animQueue.IsEmpty())
		action = this->animQueue.Dequeue();
	else
		action = AID_Idle;*/

	switch (action)
	{
	case AID_Walk:
	case AID_Run:
		{
			if (entity->IsOverlayAnimationActive() &&
				(  this->curAnim == AID_JumpStart 
				|| this->curAnim == AID_JumpEnd 
				|| this->curAnim == AID_Jump
				|| this->curAnim == AID_Attack))
			{
				return;
			}
		}
		break;
	case AID_JumpStart:
		{
			fadeout = 200;
		}
		break;
	case AID_JumpEnd:
		{
			fadeout = 200;
		}
		break;
	}

	switch (this->curAnim)
	{
	case AID_JumpStart:
		{
			if (action == AID_Attack)
			{
				/*SetSecondAnimation(AID_Attack);
				return;*/
				this->curAnim = AID_Attack;
			}
			else
				this->curAnim = AID_JumpEnd;
			fadeout = 200;
		}
		break;
	//case AID_Jump:
		{
			//if (与地面的做判断，如果小于等于地面高度)
		//	{
		//		this->curAnim = AID_JumpEnd;
		//	}
			//this->curAnim = AID_JumpEnd;
		}
		//break;
	//case AID_JumpEnd:
	//	{
	//		//if (entity->IsAnimFinish())
	//			this->curAnim = action;
	//			stopMove = false;
	//			stopTurn = false;
	//	}
	//	break;
	/*case AID_Attack:
		{
			this->curAnim = action;
		}
		break;*/
	default:
		{
			this->curAnim = action;
			stopTurn = false;
			stopMove = false;
		}
	}

	SetCurrentAnimation(this->curAnim, fadeout);

#ifdef NEBULA3_DEBUG
	//if (this->curAnim != 50 )
		//n_printf("curanim: %d, %d\n", this->curAnim, action);
#endif
}

void 
FHero::SetSpeed(float f)
{
	this->smoothedPosition.SetGain(f);
}

void 
FHero::SetCurrentAnimation(int id, DWORD fadeout)
{
	String name;
	switch (id)
	{
	case AID_Idle:
		name = "idle";
		break;
	case AID_Walk:
		name = "walk";
		break;
	case AID_Run:
		name = "run";
		break;
	case AID_RunAttack:
		name = "runattack";
		break;
	case AID_JumpStart:
		name = "jumpstart";
		break;
	case AID_Jump:
		name = "jump";
		break;
	case AID_JumpEnd:
		name = "jumpend";
		break;
	case AID_Attack:
		name = "attack";
		break;
	case AID_Defend:
		name = "defend";
		break;
	case AID_Died:
		name = "died";
		break;
	}
	if (this->model.isvalid() && !name.IsEmpty())
	{
		this->model.downcast<ActorEntity>()->SetOverlayAnimation(name);
		//this->model.downcast<ActorEntity>()->SetBaseAnimation(name, 0.2f, 0.0f, true, true, 0.2f);  //fadeout / 1000.0f);
	}
}

void
FHero::SetPosition(const Math::vector& dir, float dist)
{
	if (stopMove)
		return;

	Math::vector direction = dir;
	direction = FCameraManager::Instance()->TransformDirection(dir);
	direction = vector::normalize(direction);
	direction.y() = 0.0f;

	/*const matrix44&*/matrix44 entityMatrix = this->model->GetTransform();
	//matrix44 mat = this->graphicsEntities[0]->GetTransform(); 
	//mat.translate(dir);
	this->smoothedPosition.SetGoal(entityMatrix.getpos_component()+direction*dist);
	//this->graphicsEntities[0]->SetTransform(mat);

	SetDirection(dir);
}

void 
FHero::Move(float dist)
{
	if (stopMove)
		return;

	Math::vector pos = this->lookatDirection * dist;
	matrix44 entityMatrix = this->model->GetTransform();
	this->smoothedPosition.SetGoal(entityMatrix.getpos_component()+pos);
}

void 
FHero::SetDirection(const Math::vector& dir)
{
	if (stopTurn)
		return;

	curDir = dir;
	Math::vector direction = dir;
	direction = FCameraManager::Instance()->TransformDirection(dir);
	direction = vector::normalize(direction);
	direction.y() = 0.0f;

	this->lookatDirection = direction;
	matrix44 entityMatrix = this->model->GetTransform();
	matrix44 fixedTransform = matrix44::lookatlh(entityMatrix.getpos_component(), entityMatrix.getpos_component() + this->lookatDirection, vector::upvec());
	polar headingAngles(fixedTransform.getz_component());
	this->smoothedHeading.SetGoal(headingAngles.rho/*-1.57f*/);	// 方向不对，需要-1.57
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

	if (this->curAnim == AID_Run)
	{
		Move(7.0f);
		SetSpeed(-2.10f);
	}
	else if (this->curAnim == AID_Walk)
	{
		Move(2.0f);
		SetSpeed(-3.0f);
	}

	NextAnim(AID_JumpStart);
	stopMove = true;
	stopTurn = true;
	//NextAnim(AID_Jump);
	//SetAttachAnimation(AID_JumpEnd);

	//this->curAnim = AID_JumpStart;

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
	//if (this->model.isvalid())
	//	this->model.downcast<M2Entity>()->SetSecondaryAnim(AID_Attack);

	if (this->curAnim == AID_Run)
	{
		Move(3.0f);
		SetSpeed(-2.8f);
		NextAnim(AID_RunAttack);
	}
	else if (this->curAnim == AID_Walk)
	{
		SetPosition(curDir);
		SetSpeed(-3.0f);
		NextAnim(AID_Attack);
	}
	else
	{
		Move(0.3f);
		SetSpeed(-1.0f);
		NextAnim(AID_Attack);
	}
	stopTurn = true;
	stopMove = true;
}

} // namespace Tools
