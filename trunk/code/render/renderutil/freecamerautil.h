#pragma once
#ifndef RENDERUTIL_FREECAMERAUTIL_H
#define RENDERUTIL_FREECAMERAUTIL_H
//------------------------------------------------------------------------------
/**
    @class RenderUtil::FreeCameraUtil

    (C) 2008 by ctuo
*/
#include "math/matrix44.h"
#include "math/float2.h"
#include "math/point.h"
#include "math/polar.h"
#include "math/scalar.h"

//------------------------------------------------------------------------------
namespace RenderUtil
{
class FreeCameraUtil
{
public:
	/// constructor
	FreeCameraUtil();
	///
	~FreeCameraUtil(void);

	///
	void CalculateViewMatrix();

	/// Gets
	float GetYaw() const;
	///
	float GetPitch() const;
	///
	float GetRoll() const;
	///
	Math::vector GetPosition() const;	

	/// Move operations
	void MoveForward(float amount);
	///
	void MoveRight(float amount);
	///
	void MoveUp(float amount);

	/// rotate around x axis
	void Yaw(float amount); 
	/// rotate around x axis
	void Pitch(float amount); 
	/// rotate around z axis
	void Roll(float amount); 	


	/// setup the object
	void Setup(const Math::vector& defPos);
	/// reset the object to its default settings
	void Reset();
	/// update the view matrix
	void Update();
	/// get the current camera transform
	const Math::matrix44& GetCameraTransform() const;
	///
	void UpdateMouseDelta();
	/// 
	void SetMouseMovement(const Math::float2& v);
	///
	void SetLeftButton(bool b);
	/// 加速
	void SetSpeedUp(float speed);
private:
	float RestrictAngleTo360Range(float angle) const;
	
	/// camera position
	Math::vector m_position;
	/// rotation around the y axis
	float m_yaw;  
	/// rotation around the x axis
	float m_pitch; 
	/// rotation around the z axis
	float m_roll; 
	/// camera axis
	Math::vector m_up,m_look,m_right; 
	/// camera transform
	Math::matrix44 cameraTransform;
	///
	bool isDirty;

	/// 光标位置
	Math::float2 mouseMovement;
	Math::float2 mouseDelta;
	Math::float2 lastMousePosition;

	float framesToSmoothMouseData;
	/// 旋转速度
	Math::float2 rotVelocity;

	float rotationScaler;

	bool mleftButton;
	float speedUp;
};

//------------------------------------------------------------------------------
/**
*/
inline void 
FreeCameraUtil::SetSpeedUp(float speed)
{
	this->speedUp = speed;
}

//------------------------------------------------------------------------------
/**
*/
inline void
FreeCameraUtil::SetLeftButton(bool b)
{
	this->mleftButton = b;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
FreeCameraUtil::GetCameraTransform() const
{
	return this->cameraTransform;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
FreeCameraUtil::GetYaw() const 
{
	return m_yaw;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
FreeCameraUtil::GetPitch() const 
{
	return m_pitch;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
FreeCameraUtil::GetRoll() const 
{
	return m_roll;
}

//------------------------------------------------------------------------------
/**
*/
inline Math::vector 
FreeCameraUtil::GetPosition() const 
{
	return m_position;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
FreeCameraUtil::MoveForward(float amount) 
{
	m_position += m_look*amount*speedUp;
	isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
FreeCameraUtil::MoveRight(float amount) 
{
	m_position += m_right*amount*speedUp;
	isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
FreeCameraUtil::MoveUp(float amount) 
{
	m_position += m_up*amount*speedUp;
	isDirty = true;
}

} // namespace RenderUtil
//------------------------------------------------------------------------------
#endif
