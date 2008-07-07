//------------------------------------------------------------------------------
//  m2character.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "wow/m2/animation/animmanage.h"

namespace WOW
{

AnimManager::AnimManager(ModelAnimation *anim) {
	AnimIDSecondary = -1;
	AnimIDMouth = -1;
	anims = anim;
	AnimParticles = false;

	Count = 1;
	PlayIndex = 0;
	CurLoop = 0;
	animList[0].AnimID = 0;
	animList[0].Loops = 0;

	if (anims != NULL) { 
		Frame = anims[0].timeStart;
		TotalFrames = anims[0].timeEnd - anims[0].timeStart;
	} else {
		Frame = 0;
		TotalFrames = 0;
	}

	Speed = 1.0f;
	mouthSpeed = 1.0f;

	Paused = false;
}

AnimManager::~AnimManager() {
	anims = NULL;
}


void AnimManager::AddAnim(unsigned int id, short loops) {
	if (Count > 3)
		return;

	animList[Count].AnimID = id;
	animList[Count].Loops = loops;
	Count++;
}

void AnimManager::Set(short index, unsigned int id, short loops) {
	// error check, we currently only support 4 animations.
	if (index > 3)
		return;

	animList[index].AnimID = id;
	animList[index].Loops = loops;


	// Just an error check for our "auto animate"
	if (index == 0) {
		if (Count == 0)
			Count = 1;
		PlayIndex = index;
		Frame = anims[id].timeStart;
		TotalFrames = anims[id].timeEnd - anims[id].timeStart;
	}
}

void AnimManager::Play() {
	PlayIndex = 0;
	//if (Frame == 0 && PlayID == 0) {
	CurLoop = animList[PlayIndex].Loops;
	Frame = anims[animList[PlayIndex].AnimID].timeStart;
	TotalFrames = anims[animList[PlayIndex].AnimID].timeEnd - anims[animList[PlayIndex].AnimID].timeStart;
	//}

	Paused = false;
	AnimParticles = false;
}

void AnimManager::Stop() {
	Paused = true;
	PlayIndex = 0;
	Frame = anims[animList[0].AnimID].timeStart;
	CurLoop = animList[0].Loops;
}

void AnimManager::Pause(bool force) {
	if (Paused && force == false) {
		Paused = false;
		AnimParticles = !Paused;
	} else {
		Paused = true;
		AnimParticles = !Paused;
	}
}

void AnimManager::Next() {
	if(CurLoop == 1) {
		PlayIndex++;
		if (PlayIndex >= Count) {
			Stop();
			return;
		}

		CurLoop = animList[PlayIndex].Loops;
	} else if(CurLoop > 1) {
		CurLoop--;
	}

	Frame = anims[animList[PlayIndex].AnimID].timeStart;
}

void AnimManager::Prev() {
	if(CurLoop >= animList[PlayIndex].Loops) {
		PlayIndex--;

		if (PlayIndex < 0) {
			Stop();
			return;
		}

		CurLoop = animList[PlayIndex].Loops;
	} else if(CurLoop < animList[PlayIndex].Loops) {
		CurLoop++;
	}

	Frame = anims[animList[PlayIndex].AnimID].timeEnd;
}

int AnimManager::Tick(int time) {
	if((Count < PlayIndex) )
		return -1;

	Frame += int(time*Speed);

	// animate our mouth animation
	if (AnimIDMouth > -1) {
		FrameMouth += (time*mouthSpeed);

		if (FrameMouth >= anims[AnimIDMouth].timeEnd) {
			FrameMouth -= (anims[AnimIDMouth].timeEnd - anims[AnimIDMouth].timeStart);
		} else if (FrameMouth < anims[AnimIDMouth].timeStart) {
			FrameMouth += (anims[AnimIDMouth].timeEnd - anims[AnimIDMouth].timeStart);
		}
	}

	// animate our second (upper body) animation
	if (AnimIDSecondary > -1) {
		FrameSecondary += (time*Speed);

		if (FrameSecondary >= anims[AnimIDSecondary].timeEnd) {
			FrameSecondary -= (anims[AnimIDSecondary].timeEnd - anims[AnimIDSecondary].timeStart);
		} else if (FrameSecondary < anims[AnimIDSecondary].timeStart) {
			FrameSecondary += (anims[AnimIDSecondary].timeEnd - anims[AnimIDSecondary].timeStart);
		}
	}

	if (Frame >= anims[animList[PlayIndex].AnimID].timeEnd) {
		Next();
		return 1;
	} else if (Frame < anims[animList[PlayIndex].AnimID].timeStart) {
		Prev();
		return 1;
	}

	return 0;
}

unsigned int AnimManager::GetFrameCount() {
	return (anims[animList[PlayIndex].AnimID].timeEnd - anims[animList[PlayIndex].AnimID].timeStart);
}


void AnimManager::NextFrame()
{
	//AnimateParticles();
	int id = animList[PlayIndex].AnimID;
	Frame += int((anims[id].timeEnd - anims[id].timeStart) / 60);
	TimeDiff = int((anims[id].timeEnd - anims[id].timeStart) / 60);
}

void AnimManager::PrevFrame()
{
	//AnimateParticles();
	int id = animList[PlayIndex].AnimID;
	Frame -= int((anims[id].timeEnd - anims[id].timeStart) / 60);
	TimeDiff = int((anims[id].timeEnd - anims[id].timeStart) / 60) * -1;
}

void AnimManager::SetFrame(unsigned int f)
{
	//TimeDiff = f - Frame;
	Frame = f;
}

int AnimManager::GetTimeDiff()
{
	int t = TimeDiff;
	TimeDiff = 0;
	return t;
}

void AnimManager::SetTimeDiff(int i)
{
	TimeDiff = i;
}

void AnimManager::Clear() {
	Stop();
	Paused = true;
	PlayIndex = 0;
	Count = 0;
	CurLoop = 0;
	Frame = 0;
}

};