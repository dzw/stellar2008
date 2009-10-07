//------------------------------------------------------------------------------
//  mayacamerautil.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "renderutil/chasecamerautil.h"
#include "math/quaternion.h"

namespace RenderUtil
{
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
ChaseCameraUtil::ChaseCameraUtil() :
    distance(10.0f),
	cameraOffset(0.0f, 1.0f, 0.0f),
	linearGain(-30.0f),
	angularGain(-50.0f),
	angularVelocity(25.0f),
	lowStop(-1.57f),
	highStop(1.57f),
	distanceStep(1.0f), 
	minDistance(2.0f),
	maxDistance(25.0f)
{
}

//------------------------------------------------------------------------------
/**
*/
void
ChaseCameraUtil::Update(Timing::Time time, bool interpolate)
{
	// compute the lookat point in global space
	matrix44 m44 = this->entity->GetTransform();    
	const point pos = m44.getpos_component();
	matrix44 rot = m44;
	rot.setpos_component(point(0,0,0));
	vector lookatPoint = pos + float4::transform(this->cameraOffset, rot);    
	
	//vector lookatPoint(9.4432240, -0.11253703, 8.0850058);
	//point pos(3.4379511f, -1.6279244f, 8.6359825f);
	//this->angles.set(-0.34906587, 0.17453294);
	
	matrix44 m = matrix44::translation(0, 0, this->distance);
	m = matrix44::multiply(m, matrix44::rotationx(this->angles.theta));
	m = matrix44::multiply(m, matrix44::rotationy(this->angles.rho));
	float4 newpos = m.getpos_component();
	newpos += pos;
	m.setpos_component(newpos);

	// compute the collided goal position        
	point goalPos = m.getpos_component();
	//goalPos.set(11.068977, -0.75603068, 16.895416);

//#if __USE_PHYSICS__    
//	goalPos = this->DoCollideCheck(lookatPoint, goalPos);
//#endif
	if (!interpolate)
	{
		//Timing::Time time = InputTimeSource::Instance()->GetTime();
		this->position.Reset(time, 0.0001f, this->linearGain, goalPos);
		this->lookat.Reset(time, 0.0001f, this->angularGain, lookatPoint);
	}

	// feed and update the feedback loops
	//Timing::Time time = InputTimeSource::Instance()->GetTime();
	this->position.SetGoal(goalPos);
	this->lookat.SetGoal(lookatPoint);
	this->position.Update(time);
	this->lookat.Update(time);

	cameraTransform = matrix44::lookatlh(this->position.GetState(), this->lookat.GetState(), vector::upvec());
}

//------------------------------------------------------------------------------
/**
	Handle a camera distance change.
*/
void
ChaseCameraUtil::SetDistance(float d)
{
	this->distance = n_clamp(this->distance + (d * distanceStep), minDistance, maxDistance);
}

//------------------------------------------------------------------------------
/**
	Handle a camera reset.
*/
void
ChaseCameraUtil::Reset()
{
	//const matrix44 m = this->entity->GetTransform();
	//this->angles.set(m.getz_component());
	this->angles.set(-0.34906587f, 0.17453294f);
	//this->angles.theta = this->defaultTheta;
	//this->angles.rho = this->defaultRho;
	//this->cameraDistance = this->entity->GetFloat(Attr::CameraDistance);
	if (this->entity.isvalid())
	{
		const matrix44 m = this->entity->GetTransform();
		this->position.Reset(0.0f, 0.0001f, this->linearGain, m.getpos_component()- (m.getz_component() * 5.0f));
		this->lookat.Reset(0.0f, 0.0001f, this->angularGain, m.getpos_component() - (m.getz_component() * 10.0f));
	}
}

//------------------------------------------------------------------------------
/**
	Handle a camera orbit.
*/
void
ChaseCameraUtil::SetOrbit(float dRho, float dTheta, Timing::Time frameTime)
{
	this->angles.rho += dRho * angularVelocity * (float)frameTime;
	this->angles.theta += -dTheta * angularVelocity * (float)frameTime;
	this->angles.theta = n_clamp(this->angles.theta, lowStop, highStop);
}

//------------------------------------------------------------------------------
/**
	Do a ray check between 'from' and 'to' and return a replacement
	point for 'to'.
*/
//point
//ChaseCameraProperty::DoCollideCheck(const point& from, const point& to)
//{
//	static const vector up(0.0f, 1.0f, 0.0f);
//	matrix44 m = matrix44::lookatrh(from, to, up);
//	float outContactDist = 1.0f;
//#if __USE_PHYSICS__    
//	Physics::PhysicsUtil::RayBundleCheck(from, to, up, m.getx_component(), 0.25f, this->collideExcludeSet, outContactDist);
//#endif    
//	vector vec = vector::normalize(to - from);    
//	point newTo = from + vec * outContactDist;
//	return newTo;
//}

void 
ChaseCameraUtil::SetEntity(const Ptr<Graphics::GraphicsEntity>& e)
{
	this->entity = e;
	Reset();
}

} // namespace RenderUtil