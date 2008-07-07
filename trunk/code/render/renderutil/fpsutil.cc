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
	frames(0)
{
	this->time.Start();
}

//------------------------------------------------------------------------------
/**
*/
void
FPSUtil::Update()
{
	this->frames++;
	Timing::Time diff = this->time.GetTime();
	if (diff > 1.0f)
	{
		this->fps = (float)(this->frames / diff);
		this->frames = 0;
		this->time.Reset();
	}
}

} // namespace RenderUtil