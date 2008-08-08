//------------------------------------------------------------------------------
//  mayacamerautil.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "renderutil/playercamerautil.h"

namespace RenderUtil
{
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
PlayerCameraUtil::PlayerCameraUtil()
{
}
    
//------------------------------------------------------------------------------
/**
*/
void 
PlayerCameraUtil::SetViewParams( const vector &vEye, const vector& vLookAt, const vector& vUp )
{
	// Set attributes for the view matrix
	this->eye    = vEye;
	this->lookAt = vLookAt;
	this->upVec  = vUp;

	this->view.normalize(this->lookAt - this->eye);
	this->cross.cross3(this->view, this->upVec);

	this->matView = matrix44::lookatlh(this->eye, this->lookAt, this->upVec);
}

//------------------------------------------------------------------------------
/**
*/
void 
PlayerCameraUtil::SetProjParams( float fFOV, float fNearPlane, float fFarPlane )
{
	// Set attributes for the projection matrix
	this->fov        = fFOV;
	this->nearPlane  = fNearPlane;
	this->farPlane   = fFarPlane;

	this->matProj = matrix44::perspfovlh(this->fov, this->aspect, this->nearPlane, this->farPlane);
}

//------------------------------------------------------------------------------
/**
	计算画面上的2D座标
*/
void 
PlayerCameraUtil::Compute3DScreenPos(float f3DX, float f3DY, float f3DZ,
								 float &fScreenX, float &fScreenY, float &fScreenZ)
{
	matrix44 matTransform = matrix44::multiply(this->matView, this->matProj);
	vector vResult( f3DX, f3DY, f3DZ );

	vResult = vector::transformcoord(vResult, matTransform);
	if( vResult.z() < 0.0f )
	{
		fScreenX = -1.0f;
		fScreenY = -1.0f;
		fScreenZ = -1.0f;
		return;
	}

	int iHalfWidth = this->screenWidth >> 1;
	int iHalfHeight = this->screenHeight >> 1;
	fScreenX = vResult.x() * iHalfWidth + iHalfWidth;
	fScreenY = vResult.y() * -iHalfHeight + iHalfHeight;
	fScreenZ = vResult.z();
}

//------------------------------------------------------------------------------
/**
*/
void 
PlayerCameraUtil::SetScreenSize( int iWidth, int iHeight )
{
	this->screenWidth = iWidth;
	this->screenHeight = iHeight;
	this->aspect = (float)this->screenWidth / this->screenHeight;
}

} // namespace RenderUtil