#ifndef PHYSX_CHARACTERCONTROLSERVER_H
#define PHYSX_CHARACTERCONTROLSERVER_H
//------------------------------------------------------------------------------
/**
    @class PhysX::PhysXServer
    
    (C) 2010 
*/

#include "core/refcounted.h"
#include "core/singleton.h"
#include "math/vector.h"
#include "ControllerManager.h"
#include "NxControllerManager.h"
#include "NxExtended.h"
#include "nxactor.h"

namespace PhysX
{

enum GameGroup
{
	GROUP_NON_COLLIDABLE,
	GROUP_COLLIDABLE_NON_PUSHABLE,
	GROUP_COLLIDABLE_PUSHABLE,
};
#define COLLIDABLE_MASK	(1<<GROUP_COLLIDABLE_NON_PUSHABLE) | (1<<GROUP_COLLIDABLE_PUSHABLE)

class CharControlServer : public Core::RefCounted
{
	DeclareClass(CharControlServer);
	DeclareSingleton(CharControlServer);
public:
	CharControlServer();
	~CharControlServer();
	void Open();
	void Close();
	bool IsOpen();

	//void ReleaseCharacterControllers(NxScene& scene);

	/// character relation
	void CreateCharacter(const NxVec3& startPos, float radius=0.5f, float height=2.0f);
	void UpdateCharacter(float deltaTime);
	NxU32 MoveCharacter(const NxVec3& dispVector, NxF32 elapsedTime, NxU32 collisionGroups, NxF32 heightDelta);
	const NxExtendedVec3& GetCharacterPos(NxU32 characterIndex);
	NxActor* GetCharacterActor(NxU32 characterIndex);
	bool ResetCharacterPos(NxU32 index, const NxVec3& pos);
	bool UpdateCharacterExtents(NxU32 index, bool& increase);
	void SetCollisionGroups(NxU32 groups);

	void RenderCharacters();

	/// jump
	void StartJump(float v0);
	void StopJump();
	float GetHeight(float elapsedTime);
	/// set direction
	void SetDirection(float x, float z);
	Math::vector GetDirection()const;
	/// pushCharacter is true when moving otherwise flase
	void SetPushCharacter(bool b);
	///
	void SetCharacterSpeed(float speed);
protected:
	bool isOpen;
	bool pushCharacter;
	bool isJump;
	float jumpTime;
	float v0;
	float characterSpeed;

	NxVec3 characterVec;
	NxU32 collisionGroups;
	NxControllerManager* ccManager;
	NxController* character;
};

inline bool 
CharControlServer::IsOpen()
{
	return this->isOpen;
}

inline void 
CharControlServer::SetCollisionGroups(NxU32 groups)
{
	this->collisionGroups = groups;
}

inline void 
CharControlServer::SetDirection(float x, float z)
{
	this->characterVec.x = x;
	this->characterVec.z = z;
}

inline Math::vector 
CharControlServer::GetDirection()const
{
	return Math::vector(this->characterVec.x,
						this->characterVec.y,
						this->characterVec.z);
}

//------------------------------------------------------------------------------
/**
	移动时设为true(KeyPress时),否则会flase(KeyUp时)
*/
inline void 
CharControlServer::SetPushCharacter(bool b)
{
	this->pushCharacter = b;
}

inline void 
CharControlServer::SetCharacterSpeed(float speed)
{
	this->characterSpeed = speed;
}

};
#endif
