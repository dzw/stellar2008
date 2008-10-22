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
	bool Update();
	
	void SetMaxFps(SizeT maxFps);
	void SetLimit(bool b);
private:
	bool limitFrame;
    float fps;
	float frameTime;
	SizeT frames;
	SizeT maxFps;
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

inline void
FPSUtil::SetLimit(bool b)
{
	this->limitFrame = b;
}

} // namespace RenderUtil
//------------------------------------------------------------------------------
#endif
