#ifndef WOW_ANIMMANAGER_H
#define WOW_ANIMMANAGER_H
//------------------------------------------------------------------------------
/**
    This will be our animation manager
	instead of using a STL vector or list or table, etc.  
	Decided to just limit it upto 4 animations to loop through - for experimental testing.
	The second id and loop count will later be used for being able to have a primary and secondary animation.

	Currently, this is more of a "Wrapper" over the existing code
	but hopefully over time I can remove and re-write it so this is the core.
*/
#include "core/types.h"
#include "modelheaders.h"

namespace WOW
{
//------------------------------------------------------------------------------

struct AnimInfo {
	short Loops;
	unsigned int AnimID;
};

class AnimManager {
	ModelAnimation *anims;
	
	bool Paused;
	bool AnimParticles;

	AnimInfo animList[4];

	unsigned int Frame;		// Frame number we're upto in the current animation
	unsigned int TotalFrames;

	int AnimIDSecondary;
	unsigned int FrameSecondary;

	int AnimIDMouth;
	unsigned int FrameMouth;
	
	short Count;			// Total index of animations
	short PlayIndex;		// Current animation index we're upto
	short CurLoop;			// Current loop that we're upto.

	int TimeDiff;			// Difference in time between each frame

	float Speed;			// The speed of which to multiply the time given for Tick();
	float mouthSpeed;

	bool curAnimFinish;		// 当前动画播放结束
public:
	AnimManager(ModelAnimation *anim);
	~AnimManager();
	
	void AddAnim(unsigned int id, short loop); // Adds an animation to our array.
	void Set(short index, unsigned int id, short loop); // sets one of the 4 existing animations and changes it (not really used currently)
	
	void SetSecondary(int id) {
		AnimIDSecondary = id;
		FrameSecondary = anims[id].timeStart;
	}
	void ClearSecondary() { AnimIDSecondary = -1; }
	int GetSecondaryID() { return AnimIDSecondary; }
	unsigned int GetSecondaryFrame() { return FrameSecondary; }

	// For independent mouth movement.
	void SetMouth(int id) {
		AnimIDMouth = id;
		FrameMouth = anims[id].timeStart;
	}
	void ClearMouth() { AnimIDMouth = -1; }
	int GetMouthID() { return AnimIDMouth; }
	unsigned int GetMouthFrame() { return FrameMouth; }
	void SetMouthSpeed(float speed) {
		mouthSpeed = speed;
	}

	void Play(); // Players the animation, and reconfigures if nothing currently inputed
	void Stop(); // Stops and resets the animation
	void Pause(bool force = false); // Toggles 'Pause' of the animation, use force to pause the animation no matter what.
	
	void Next(); // Plays the 'next' animation or loop
	void Prev(); // Plays the 'previous' animation or loop

	int Tick(int time);

	unsigned int GetFrameCount();
	unsigned int GetFrame() {return Frame;}
	void SetFrame(unsigned int f);
	void SetSpeed(float speed) {Speed = speed;}
	float GetSpeed() {return Speed;}
	
	void PrevFrame();
	void NextFrame();

	void Clear();
	void Reset() { Count = 0; }

	bool IsPaused() { return Paused; }
	bool IsParticlePaused() { return !AnimParticles; }
	void AnimateParticles() { AnimParticles = true; }

	unsigned int GetAnim() { return animList[PlayIndex].AnimID; }

	int GetTimeDiff();
	void SetTimeDiff(int i);

	bool IsCurAnimFinish()const;
};

inline bool
AnimManager::IsCurAnimFinish()const
{
	return this->curAnimFinish;
}

}; // namespace Nebula2
//------------------------------------------------------------------------------
#endif
