#ifndef PHYSX_PHYSXSERVER_H
#define PHYSX_PHYSXSERVER_H
//------------------------------------------------------------------------------
/**
    @class PhysX::PhysXServer
    
    (C) 2010 
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "math/vector.h"

#include "NxPhysics.h"
#include "NxController.h"
//#include "NxControllerManager.h"
#include "ControllerManager.h"
#include "NxControllerManager.h"
#include "MemoryAllocator.h"
#include "ErrorStream.h"
#include "physx/charactercontrolserver.h"
#include "physx/debugrender.h"

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

	const MemoryAllocator* GetMemoryAllocator()const;
	NxScene* GetScene()const;
	void Update(float frameTime);
	void RenderDebug();
	void Pause(bool b);

	bool CreateMeshFromFile(const Util::String& fileName);
	bool CreateTriMesh(DWORD vertexNum, int vertexStrideBytes, void* vertices, 
						   DWORD triangleNum, void* indices, DWORD indexFlag=NX_MF_16_BIT_INDICES);
	
	/// ≤‚ ‘”√
	NxActor* CreateBox(const Math::vector& startPos, const Math::vector& size);
	NxActor* CreateSphere(const Math::vector& startPos, float radius);
	NxActor* CreateCapsule(const Math::vector& startPos, float radius, float height, float startDir=0.0f);
	NxActor* CreateGroundPlane();
	void AddUserDataToActor();

protected:
	void CreateScene();
	void ReleaseScene();
	bool InitCooking(NxUserAllocator* allocator=0, NxUserOutputStream* outputStream=0);
	bool CookTriangleMesh(const NxTriangleMeshDesc& desc, NxStream& stream);
	void CloseCooking();

	bool isPause;
	NxPhysicsSDK*	physicsSDK;
	NxCookingInterface *cooking;
    NxScene* scene;
	MemoryAllocator*	allocator;
	Ptr<CharControlServer> controllerManager;
	//DebugRender dbgRender;
};

inline const MemoryAllocator* 
PhysXServer::GetMemoryAllocator()const
{
	return this->allocator;
}

inline NxScene* 
PhysXServer::GetScene()const
{
	return this->scene;
}

inline void 
PhysXServer::Pause(bool b)
{
	this->isPause = b;
}

}; // namespace Physics
//------------------------------------------------------------------------------
#endif
    