//------------------------------------------------------------------------------
//  mayacamerautil.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "renderutil/dxutcamerautil.h"
#include "math/quaternion.h"

namespace RenderUtil
{
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
DXUTCameraUtil::DXUTCameraUtil() :
    cameraTransform(matrix44::identity()),
	cameraWorld(matrix44::identity()),
	defaultLookAt(0.0f, 0.0f, 1.0f),
	defaultRight(0.0f, 0.0f, 0.0f),
	lookAt(0.0f, 0.0f, 0.0f),
	right(0.0f, 0.0f, 0.0f),
	up(0.0f, 0.0f, 0.0f),
	cameraYawAngle(0.0f),
	cameraPitchAngle(0.0f),
	mouseMovement(0.0, 0.0),
	mouseDelta(0.0f, 0.0f),
	lastMousePosition(0.0, 0.0),
	rotVelocity(0.0f, 0.0f),
	keyboardDirection(0.0f, 0.0f, 0.0f),
	dirtyMove(false),
	dirtyRotate(false),
	mleftButton(false),
	dirtyVertical(false)
{
	framesToSmoothMouseData = 1.0f;
	rotationScaler = 0.008f;
	moveScaler = 5.0f;

	this->Setup(vector(0.0f, 0.0f, 100.0f));
}
    
//------------------------------------------------------------------------------
/**
*/
void
DXUTCameraUtil::Setup(/*const vector& defLookat,*/ const vector& defRight)
{
	defaultRight = defRight;
	defaultLookAt = defaultLookAt;
	
	right = defaultRight;
	lookAt = defaultLookAt;
	up = vector::upvec();//cross3(right, lookAt);
	/*lookAt = vector::normalize(lookAt);
	up = vector::cross3(lookAt, right);
	up = vector::normalize(up);
	right = vector::cross3(lookAt, up);
	right = vector::normalize(right);*/

	this->keyboardDirection = vector(0.1f, 0.1f, 0.1f);
	dirtyRotate = true;
	dirtyMove = true;
	Update();
}

//------------------------------------------------------------------------------
/**
*/
void 
DXUTCameraUtil::ViewChange()
{
	/*float fADet = n_abs(vector::dot3(lookAt, vector::cross3(up, right)));
	if (n_abs(1.0f - fADet) > N_TINY)
	{
		vector::normalize(lookAt);
		float fDot0 = vector::dot3(lookAt, up);
		up -= lookAt*fDot0;
		vector::normalize(up);

		float fDot1 = vector::dot3(up, right);
		fDot0 = vector::dot3(lookAt, right);
		right -= lookAt*fDot0 + up*fDot1;
		vector::normalize(right);
	}*/

	float4 eye = right;
	eye.w() = 1.0f;
	float4 at = lookAt;
	at.w() = 1.0f;
	float4 upp = up;
	upp.w() = 1.0f;
	this->cameraTransform = matrix44::lookatlh(eye, at, upp);

	//float4 r0, r1, r2, r3;
	//vector zaxis = vector::normalize(lookAt - right);
	//vector xaxis = vector::normalize(vector::cross3(up, zaxis));
	//vector yaxis = vector::cross3(zaxis, xaxis);

	//r0.x() = xaxis.x();
	//r0.y() = yaxis.x();
	//r0.z() = zaxis.x();
	//r0.w() = 0.0f;

	//r1.x() = xaxis.y();
	//r1.y() = yaxis.y();
	//r1.z() = zaxis.y();
	//r1.w() = 0.0f;

	//r2.x() = xaxis.z();
	//r2.y() = yaxis.z();
	//r2.z() = zaxis.z();
	//r2.w() = 0.0f;

	//r3.x() = -vector::dot3(xaxis, right);
	//r3.y() = -vector::dot3(yaxis, right);
	//r3.z() = -vector::dot3(zaxis, right);
	//r3.w() = 1.0f;

	//this->cameraTransform.set(r0, r1, r2, r3);
	////this->cameraWorld = matrix44::inverse(this->cameraTransform);
	//this->cameraTransform = matrix44::inverse(this->cameraTransform);


	
}

//------------------------------------------------------------------------------
/**
*/
void 
DXUTCameraUtil::SetMovement(const vector& keyboardDirection)
{
	if (keyboardDirection == vector::nullvec())
		return;

	this->keyboardDirection += keyboardDirection;

	// 高度在这里设置
	this->keyboardDirection.y() = 0.0f;
	dirtyMove = true;
}

//------------------------------------------------------------------------------
/**
*/
void
DXUTCameraUtil::SetMouseMovement(const float2& v)
{
	this->mouseMovement = v;

	if (this->mouseMovement != this->lastMousePosition && mleftButton)
	{
		this->UpdateMouseDelta();
		dirtyRotate = true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
DXUTCameraUtil::Reset()
{
	this->Setup(this->defaultRight);
    this->Update();
}

//------------------------------------------------------------------------------
/**
*/
void
DXUTCameraUtil::Update()
{
	if (!dirtyMove && !dirtyRotate && !dirtyVertical)
		return;

	//UpdateMouseDelta();

	vector posDelta = vector::nullvec();
	//if (this->keyboardDirection != vector::nullvec())
	if (dirtyMove || dirtyVertical)
	{
		posDelta = this->keyboardDirection;
		posDelta = vector::normalize(posDelta);
		posDelta *= moveScaler;
	}

	if (dirtyRotate)
	{
		this->cameraYawAngle += this->rotVelocity.x();
		this->cameraPitchAngle += this->rotVelocity.y();

		this->cameraPitchAngle = n_max(-N_PI / 2.0f, this->cameraPitchAngle);
		this->cameraPitchAngle = n_min(+N_PI / 2.0f, this->cameraPitchAngle);
	}	

	matrix44 cameraRot = matrix44::rotationyawpitchroll(this->cameraYawAngle, this->cameraPitchAngle, 0);

	vector worldUp, worldAhead;
	vector localUp(0, 1, 0);
	vector localAhead(0, 0, 1);
	worldUp = vector::transform(localUp, cameraRot);
	worldAhead = vector::transform(localAhead, cameraRot);
	
	vector posDeltaWorld = vector::transform(posDelta, cameraRot);

	if (!dirtyVertical)
		posDeltaWorld.y() = 0.0f;

	right += posDeltaWorld;
	lookAt = right + worldAhead;
	up = worldUp;

	if (dirtyMove || dirtyRotate || dirtyVertical)
	{
		ViewChange();

		dirtyMove = false;
		dirtyRotate = false;
		dirtyVertical = false;
		this->keyboardDirection = vector::nullvec();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
DXUTCameraUtil::UpdateMouseDelta()
{
	//if (this->lastMousePosition == this->mouseMovement)
	//	return;

	float2 curMouseDelta;

	curMouseDelta = this->mouseMovement - this->lastMousePosition;
	lastMousePosition = this->mouseMovement;
	
	float percentOfNew = 1.0f / framesToSmoothMouseData;
	float percentOfOld = 1.0f - percentOfNew;
	this->mouseDelta.x() = this->mouseDelta.x() * percentOfOld + curMouseDelta.x()*percentOfNew;
	this->mouseDelta.y() = this->mouseDelta.y() * percentOfOld + curMouseDelta.y()*percentOfNew;

	this->rotVelocity = this->mouseDelta * this->rotationScaler;
}

} // namespace RenderUtil