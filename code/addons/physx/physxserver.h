#ifndef PHYSX_PHYSXSERVER_H
#define PHYSX_PHYSXSERVER_H
//------------------------------------------------------------------------------
/**
    @class PhysX::PhysXServer
    
    (C) 2010 
*/
#include "core/refcounted.h"

#include "NxController.h"
#include "UserAllocator.h"
#include "ErrorStream.h"
#include "DebugRenderer.h"

//------------------------------------------------------------------------------
namespace PhysX
{

class PhysXServer : public Core::RefCounted
{
    DeclareClass(PhysXServer);
	DeclareSingleton(PhysXServer);
public:

    /// default constructor
    PhysXServer();
    /// destructor
    virtual ~PhysXServer();
	virtual void Open();
	virtual void Close();
	void CreateScene();

protected:
	bool InitCooking(NxUserAllocator* allocator=0, NxUserOutputStream* outputStream=0);
	void ReleaseScene();

	NxPhysicsSDK*	physicsSDK;
	NxCookingInterface *cooking;
    NxScene* scene;
	NxControllerManager* controllerManager;
	UserAllocator*	allocator;
};

}; // namespace Physics
//------------------------------------------------------------------------------
#endif
    