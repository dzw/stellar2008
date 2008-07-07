#pragma once
#ifndef RENDERUTIL_FPSUTIL_H
#define RENDERUTIL_FPSUTIL_H
//------------------------------------------------------------------------------
/**
    fps
*/

#include "timing/timer.h"

//------------------------------------------------------------------------------
namespace RenderUtil
{
class FPSUtil
{
public:
    /// constructor
    FPSUtil();

	float GetFPS();
	void Update();
private:
    float fps;
	SizeT frames;
	Timing::Timer time;
};

//------------------------------------------------------------------------------
/**
*/
inline float 
FPSUtil::GetFPS()
{
	return this->fps;
}

} // namespace RenderUtil
//------------------------------------------------------------------------------
#endif
