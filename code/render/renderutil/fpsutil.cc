//------------------------------------------------------------------------------
//  mayacamerautil.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "renderutil/fpsutil.h"


namespace RenderUtil
{

//------------------------------------------------------------------------------
/**
*/
FPSUtil::FPSUtil():
	fps(0.0f),
	frames(0),
	limitFrame(false)
{
	this->time.Start();
	SetMaxFps(1000);
}

//------------------------------------------------------------------------------
/**
*/
bool
FPSUtil::Update()
{
	Timing::Time diff = this->time.GetTime();
	static Timing::Time prevTime = 0.0f;
	
	if (this->limitFrame && diff - prevTime + 0.001 < this->frameTime)
	{
		n_sleep(0.001);
		return false;
	}
	prevTime = diff;
	this->frames++;
	
	if (diff > 1.0f)
	{
		this->fps = (float)(this->frames / diff);
		this->frames = 0;
		this->time.Reset();
		prevTime = this->time.GetTime();
	}
	
	return true;
}

void
FPSUtil::SetMaxps(SizeT maxFps)
{
	this->maxFps = maxFps;
	this->frames = 0;
	this->fps = 0;
	this->time.Reset();
	this->frameTime = 1.0f/maxFps;
}

} // namespace RenderUtil