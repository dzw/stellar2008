#pragma once
#ifndef RENDERUTIL_CHASECAMERAUTIL_H
#define RENDERUTIL_CHASECAMERAUTIL_H
//------------------------------------------------------------------------------
/**
    @class RenderUtil::ChaseCameraUtil


    (C) 2009 cTuo
*/
#include "math/matrix44.h"
#include "math/float2.h"
#include "math/point.h"
#include "math/polar.h"
#include "math/scalar.h"
#include "math/pfeedbackloop.h"
#include "graphics/graphicsentity.h"

//------------------------------------------------------------------------------
namespace RenderUtil
{
class ChaseCameraUtil
{
public:
    /// constructor
    ChaseCameraUtil();

	void SetEntity(const Ptr<Graphics::GraphicsEntity>& e);
    /// camera reset
	virtual void Reset();
    /// update the camera matrix
    void Update(Timing::Time time, bool interpolate);
    /// get the current camera transform
    const Math::matrix44& GetCameraTransform() const;
	/// camera orbit
	virtual void SetOrbit(float dRho, float dTheta, Timing::Time frameTime = 1.0f);
	/// camera distance change
	virtual void SetDistance(float z);

	/// do a collision check
	//virtual Math::point DoCollideCheck(const Math::point& from, const Math::point& to);
protected:
	Math::polar angles;
	float distance;
	Math::PFeedbackLoop<Math::point> position;
	Math::PFeedbackLoop<Math::point> lookat;
	Math::matrix44 cameraTransform;

	float minDistance;
	float maxDistance;
	float distanceStep;
	float linearGain;
	float angularGain;
	float lowStop;
	float highStop;
	float angularVelocity;
	Math::vector cameraOffset;	// lookatÎ»ÖÃµÄÆ«ÒÆ

	float defaultTheta;
	float defaultRho;

	Ptr<Graphics::GraphicsEntity> entity;
};

inline const Math::matrix44& 
ChaseCameraUtil::GetCameraTransform() const
{
	return this->cameraTransform;
}

} // namespace RenderUtil
//------------------------------------------------------------------------------
#endif
