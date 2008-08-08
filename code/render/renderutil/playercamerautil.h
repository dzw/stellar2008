#pragma once
#ifndef RENDERUTIL_PLAYERCAMERAUTIL_H
#define RENDERUTIL_PLAYERCAMERAUTIL_H
//------------------------------------------------------------------------------
/**
    @class RenderUtil::PlayerCameraUtil

    (C) 2008 cTuo
*/
#include "math/matrix44.h"
#include "math/float2.h"
#include "math/point.h"
#include "math/polar.h"
#include "math/scalar.h"

//------------------------------------------------------------------------------
namespace RenderUtil
{
class PlayerCameraUtil
{
public:
    /// constructor
    PlayerCameraUtil();

	const Math::vector& GetEyePt();
	const Math::vector& GetLookatPt();
	const Math::vector& GetUpVec();
	const Math::vector& GetViewDir();
	const Math::vector& GetCross();

	const Math::matrix44& GetViewMatrix();
	const Math::matrix44& GetProjMatrix();

	float	GetFOV();
	float	GetAspect();
	float	GetNearPlane();
	float	GetFarPlane();

	void SetViewParams( const Math::vector& vEye, const Math::vector& vLookAt, const Math::vector& vUp );
	void SetProjParams( float fFOV, float fNearPlane, float fFarPlane );
	void Compute3DScreenPos(float f3DX, float f3DY, float f3DZ, float &fScreenX,
		float &fScreenY, float &fScreenZ);     

	void SetScreenSize(int iWidth, int iHeight);
	int GetScreenWidth();
	int GetScreenHeight();
private:
	Math::matrix44 matView;
	Math::matrix44 matProj;

	Math::vector eye;
	Math::vector lookAt;
	Math::vector upVec;
	Math::vector view;
	Math::vector cross;

	float fov;
	float aspect;
	float nearPlane;
	float farPlane;

	int screenWidth;
	int screenHeight;
};

//------------------------------------------------------------------------------
/**
*/
const Math::vector& 
PlayerCameraUtil::GetEyePt()
{
	return this->eye; 
}

const Math::vector& 
PlayerCameraUtil::GetLookatPt()            
{ 
	return this->lookAt; 
}

const Math::vector& 
PlayerCameraUtil::GetUpVec()               
{
	return this->upVec; 
}

const Math::vector& 
PlayerCameraUtil::GetViewDir()             
{
	return this->view; 
}

const Math::vector& 
PlayerCameraUtil::GetCross()               
{
	return this->cross; 
}


const Math::matrix44& 
PlayerCameraUtil::GetViewMatrix()        
{
	return this->matView; 
}

const Math::matrix44& 
PlayerCameraUtil::GetProjMatrix()        
{
	return this->matProj; 
}


float	
PlayerCameraUtil::GetFOV()
{
	return this->fov; 
}

float	
PlayerCameraUtil::GetAspect()
{
	return this->aspect; 
}

float	
PlayerCameraUtil::GetNearPlane()
{
	return this->nearPlane; 
}

float	
PlayerCameraUtil::GetFarPlane()
{
	return this->farPlane; 
}

int 
PlayerCameraUtil::GetScreenWidth() 
{
	return this->screenWidth; 
}

int 
PlayerCameraUtil::GetScreenHeight() 
{
	return this->screenHeight; 
};

} // namespace RenderUtil
//------------------------------------------------------------------------------
#endif
