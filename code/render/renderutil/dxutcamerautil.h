#pragma once
#ifndef RENDERUTIL_DXUTCAMERAUTIL_H
#define RENDERUTIL_DXUTCAMERAUTIL_H
//------------------------------------------------------------------------------
/**
    @class RenderUtil::DXUTCameraUtil

    Helper class to implement a "Maya camera" with pan/zoom/orbit. Just feed
    input into the class per its setter methods, call Update(), and get 
    the computed view matrix.

    (C) 2007 Radon Labs GmbH
*/
#include "math/matrix44.h"
#include "math/float2.h"
#include "math/point.h"
#include "math/polar.h"
#include "math/scalar.h"

//------------------------------------------------------------------------------
namespace RenderUtil
{
class DXUTCameraUtil
{
public:
    /// constructor
    DXUTCameraUtil();

    /// setup the object
	void Setup(/*const Math::vector& defLookat,*/ const Math::vector& defRight);
    /// reset the object to its default settings
    void Reset();
    /// update the view matrix
    void Update();
    /// get the current camera transform
    const Math::matrix44& GetCameraTransform() const;

    /// set mouse movement
    void SetMouseMovement(const Math::float2& v);

	void ViewChange();
	void UpdateMouseDelta();
	void SetMovement(const Math::vector& keyboardDirection);
	/// set left button of mouse
	void SetLeftButton(bool b);
	/// 获得摄像机位置
	const Math::vector& GetPosition();
	/// 加速
	void SetSpeedUp(float speed);
	// 上下移动
	void MoveVertical(bool isUp);
private:

	Math::vector keyboardDirection;
	float framesToSmoothMouseData;

	/// 光标位置
	Math::float2 mouseMovement;
	Math::float2 mouseDelta;
	Math::float2 lastMousePosition;

	/// 旋转速度
	Math::float2 rotVelocity;

	float rotationScaler;
	float moveScaler;


	Math::vector defaultLookAt;
	Math::vector defaultRight;
	Math::vector lookAt;
	Math::vector right;
	Math::vector up;
	float cameraYawAngle;
	float cameraPitchAngle;

	Math::matrix44 cameraTransform;
	Math::matrix44 cameraWorld;

	bool dirtyMove;
	bool dirtyRotate;
	/// 鼠标左键按下
	bool mleftButton;
};

//------------------------------------------------------------------------------
/**
*/
inline void 
DXUTCameraUtil::MoveVertical(bool isUp)
{
	if (isUp)
		this->keyboardDirection = Math::vector(0.0f, 0.1f, 0.0f);
	else
		this->keyboardDirection = Math::vector(0.0f, -0.1f, 0.0f);
	this->dirtyMove = true;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
DXUTCameraUtil::SetSpeedUp(float speed)
{
	this->moveScaler = speed;
}

//------------------------------------------------------------------------------
/**
*/
inline void
DXUTCameraUtil::SetLeftButton(bool b)
{
	this->mleftButton = b;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
DXUTCameraUtil::GetCameraTransform() const
{
    return this->cameraTransform;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::vector& 
DXUTCameraUtil::GetPosition()
{
	return this->right;
}

} // namespace RenderUtil
//------------------------------------------------------------------------------
#endif
