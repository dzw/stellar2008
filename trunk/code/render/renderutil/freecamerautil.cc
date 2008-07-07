//------------------------------------------------------------------------------
//  freecamerautil.cc
//  (C) 2008 by ctuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "renderutil/freecamerautil.h"
#include "math/quaternion.h"

namespace RenderUtil
{
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
FreeCameraUtil::FreeCameraUtil():
	isDirty(true),
	m_yaw(0),
	m_pitch(0),
	m_roll(0),
	rotVelocity(0.0, 0.0),
	lastMousePosition(0.0, 0.0),
	mouseMovement(0.0, 0.0),
	mouseDelta(0.0, 0.0),
	framesToSmoothMouseData(2.0f),
	rotationScaler(0.008f),
	speedUp(1.0f)
{
	// Start with an orthagonal camera axis
	m_up=vector(0.0f,1.0f,0.0f);
	m_look=vector(0.0f,0.0f,1.0f);
	m_right=vector(1.0f,0.0f,0.0f);

	this->cameraTransform = matrix44::identity();
	this->m_position = vector::nullvec();

	Setup(vector(0,5, 20));
}

//------------------------------------------------------------------------------
/**
*/
FreeCameraUtil::~FreeCameraUtil(void)
{
}

//------------------------------------------------------------------------------
/**
*/
void 
FreeCameraUtil::CalculateViewMatrix()
{
	/* Start with our camera axis pointing down z
	An alternative method is to just keep adjusting our axis but if we do that the
	axis can start to lose its orthogonal shape (due to floating point innacuracies).
	This could be solved by rebuilding the orthogonal shape each time with the following:
	1. normalising the look vector
	2. creating the up vector from the cross product of the look and the right
	3. normalising up
	4. creating the right vector from the cross product of the look and the up
	5. normalising right
	*/

	m_up=vector(0.0f,1.0f,0.0f);
	m_look=vector(0.0f,0.0f,1.0f);
	m_right=vector(1.0f,0.0f,0.0f);

	// Yaw is rotation around the y axis (m_up)
	// Create a matrix that can carry out this rotation
	//D3DXMATRIX yawMatrix;
	//D3DXMatrixRotationAxis(&yawMatrix, &m_up, m_yaw);
	//// To apply yaw we rotate the m_look & m_right vectors about the m_up vector (using our yaw matrix)
	//D3DXVec3TransformCoord(&m_look, &m_look, &yawMatrix); 
	//D3DXVec3TransformCoord(&m_right, &m_right, &yawMatrix); 

	//// Pitch is rotation around the x axis (m_right)
	//// Create a matrix that can carry out this rotation
	//D3DXMATRIX pitchMatrix;
	//D3DXMatrixRotationAxis(&pitchMatrix, &m_right, m_pitch);
	//// To apply pitch we rotate the m_look and m_up vectors about the m_right vector (using our pitch matrix)
	//D3DXVec3TransformCoord(&m_look, &m_look, &pitchMatrix); 
	//D3DXVec3TransformCoord(&m_up, &m_up, &pitchMatrix); 
	//	
	//// Roll is rotation around the z axis (m_look)
	//// Create a matrix that can carry out this rotation
	//D3DXMATRIX rollMatrix;
	//D3DXMatrixRotationAxis(&rollMatrix, &m_look, m_roll);
	//// To apply roll we rotate up and right about the look vector (using our roll matrix)
	//// Note: roll only really applies for things like aircraft unless you are implementing lean
	//D3DXVec3TransformCoord(&m_right, &m_right, &rollMatrix); 
	//D3DXVec3TransformCoord(&m_up, &m_up, &rollMatrix); 
	//
	// Build the view matrix from the transformed camera axis
	
	matrix44 mat;
	mat = matrix44::rotationyawpitchroll(m_yaw, m_pitch, m_roll);
	/*m_right = vector::transform(m_right, mat);
	m_up = vector::transform(m_up, mat);
	m_look = vector::transform(m_look, mat);*/
	
	this->cameraTransform = matrix44::identity();
	/*this->cameraTransform.set(vector(m_right.x(), m_up.x(), m_look.x()),
		vector(m_right.y(), m_up.y(), m_look.y()),
		vector(m_right.z(), m_up.z(), m_look.z()),
		float4(-vector::dot3(m_position, m_right), -vector::dot3(m_position, m_up), -vector::dot3(m_position, m_look), 1.0));*/


	vector worldUp, worldAhead;
	vector localUp(0, 1, 0);
	vector localAhead(0, 0, 1);
	worldUp = vector::transform(localUp, mat);
	worldAhead = vector::transform(localAhead, mat);
	vector posDeltaWorld = vector::transform(m_position, mat);
	m_position = vector::nullvec();

	m_right += posDeltaWorld;
	m_look = m_right + worldAhead;
	m_up = worldUp;

	
	float4 r0, r1, r2, r3;
	vector zaxis = vector::normalize(m_look - m_right);
	vector xaxis = vector::normalize(vector::cross3(m_up, zaxis));
	vector yaxis = vector::cross3(zaxis, xaxis);

	r0.x() = xaxis.x();
	r0.y() = yaxis.x();
	r0.z() = zaxis.x();
	r0.w() = 0.0f;

	r1.x() = xaxis.y();
	r1.y() = yaxis.y();
	r1.z() = zaxis.y();
	r1.w() = 0.0f;

	r2.x() = xaxis.z();
	r2.y() = yaxis.z();
	r2.z() = zaxis.z();
	r2.w() = 0.0f;

	r3.x() = -vector::dot3(xaxis, m_right);
	r3.y() = -vector::dot3(yaxis, m_right);
	r3.z() = -vector::dot3(zaxis, m_right);
	r3.w() = 1.0f;

	this->cameraTransform.set(r0, r1, r2, r3);

	/*D3DXMATRIX matview;
	D3DXVECTOR3 at;
	D3DXVECTOR3 vLocalAhead = D3DXVECTOR3(0,0,1);
	D3DXVec3TransformCoord( &at, &vLocalAhead, (D3DXMATRIX*)&mat );
	at = D3DXVECTOR3(m_position.x() + at.x, m_position.y() + at.y, m_position.z() + at.z);
	D3DXMatrixLookAtLH(&matview, (D3DXVECTOR3*)&m_position, &at, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	Memory::Copy(&matview, &this->cameraTransform, sizeof(matrix44));*/

	this->cameraTransform = matrix44::inverse(this->cameraTransform);
}

//------------------------------------------------------------------------------
/**
	Yaw - rotation around y axis
*/
void 
FreeCameraUtil::Yaw(float amount) 
{
	m_yaw+=amount;
	m_yaw=RestrictAngleTo360Range(m_yaw);
	this->isDirty = true;
}	

// Pitch - rotation around x axis
void 
FreeCameraUtil::Pitch(float amount)
{
	m_pitch+=amount;
	m_pitch=RestrictAngleTo360Range(m_pitch);
	this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
	Roll - rotation around z axis
	Note: normally only used for aircraft type cameras rather than land based ones
*/
void 
FreeCameraUtil::Roll(float amount) 
{
	m_roll+=amount;
	m_roll=RestrictAngleTo360Range(m_roll);
	this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
	Keep the angle in the range 0 to 360 (2*PI)
*/
float 
FreeCameraUtil::RestrictAngleTo360Range(float angle) const
{
	while(angle>2*D3DX_PI)
		angle-=2*D3DX_PI;

	while(angle<0)
		angle+=2*D3DX_PI;

	return angle;
}


//------------------------------------------------------------------------------
/**
*/
void
FreeCameraUtil::Setup(const vector& defPos)
{
	m_position = defPos;

	isDirty = true;

	Update();
}

//------------------------------------------------------------------------------
/**
*/
void
FreeCameraUtil::Reset()
{
}

//------------------------------------------------------------------------------
/**
*/
void
FreeCameraUtil::Update()
{
	if (isDirty)
	{
		CalculateViewMatrix();

		isDirty = false;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
FreeCameraUtil::UpdateMouseDelta()
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

//------------------------------------------------------------------------------
/**
*/
void
FreeCameraUtil::SetMouseMovement(const float2& v)
{
	this->mouseMovement = v;

	if (this->mouseMovement != this->lastMousePosition && mleftButton)
	{
		this->UpdateMouseDelta();
		
		Yaw(rotVelocity.x());
		Pitch(rotVelocity.y());
		//m_yaw += rotVelocity.x();
		//m_pitch += rotVelocity.y();

		isDirty = true;
	}
}

} // namespace RenderUtil