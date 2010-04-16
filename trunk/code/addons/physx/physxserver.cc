//------------------------------------------------------------------------------
//  physics/shape.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "physx/physxserver.h"
#include "physx/physxutil.h"
#include "physx/stream.h"
#include "physx/physxmesh.h"
#include "NxCooking.h"
#include "util/array.h"
#if NEBULA3_DEBUG
#include "physx/userdata.h"
#endif

namespace PhysX
{
ImplementClass(PhysX::PhysXServer, 'PHXS', Core::RefCounted);
ImplementSingleton(PhysX::PhysXServer);

using namespace Util;
using namespace Math;

const NxVec3			DefaultGravity(0.0f, -50.1f, 0.0f);

//------------------------------------------------------------------------------
/**
*/
PhysXServer::PhysXServer() :
isPause(false)
{
    ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
PhysXServer::~PhysXServer()
{
	DestructSingleton;
}

//------------------------------------------------------------------------------
/**

*/
void
PhysXServer::Open()
{
	allocator = n_new(MemoryAllocator);

	bool status = InitCooking();
	if (!status) {
		n_printf("Unable to initialize the cooking library. Please make sure that you have correctly installed the latest version of the AGEIA PhysX SDK.");
		return;
	}

	// Initialize PhysicsSDK
	ErrorStream errorStream;
	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	physicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, allocator, &errorStream, desc, &errorCode);
	if(physicsSDK == NULL) 
	{
		n_printf("\nSDK create error (%d - %s).\nUnable to initialize the PhysX SDK.\n\n", errorCode, PhysXUtil::GetNxSDKCreateError(errorCode));
		return;
	}
#if SAMPLES_USE_VRD
	// The settings for the VRD host and port are found in SampleCommonCode/SamplesVRDSettings.h
	if (physicsSDK->getFoundationSDK().getRemoteDebugger())
		physicsSDK->getFoundationSDK().getRemoteDebugger()->connect(SAMPLES_VRD_HOST, SAMPLES_VRD_PORT, SAMPLES_VRD_EVENTMASK);
#endif

	physicsSDK->setParameter(NX_SKIN_WIDTH, 0.025f);
	physicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1.0f);
	physicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);

	CreateScene();

	controllerManager = CharControlManager::Create();
	controllerManager->Open();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXServer::Close()
{
	CloseCooking();
	ReleaseScene();

	if (this->controllerManager.isvalid())
	{
		this->controllerManager->Close();
		this->controllerManager = 0;
	}
	if (physicsSDK) NxReleasePhysicsSDK(physicsSDK);
	physicsSDK = NULL;

	if(allocator!=NULL)
	{
		n_delete(allocator);
		allocator=NULL;
	}
}

void
PhysXServer::CreateScene()
{
	// Create a scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity = DefaultGravity;
	scene = physicsSDK->createScene(sceneDesc);
	if(scene == NULL) 
	{
		n_printf("\nError: Unable to create a PhysX scene.\n\n");
		return;
	}

	NxMaterial * defaultMaterial = scene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);
}

void 
PhysXServer::ReleaseScene()
{
	if(physicsSDK)
	{
		if(scene)
		{
			controllerManager->ReleaseCharacterControllers(*scene);
			//NxReleaseControllerManager(controllerManager);
			physicsSDK->releaseScene(*scene);
			scene = NULL;
		}
	}
}

bool
PhysXServer::InitCooking(NxUserAllocator* allocator, NxUserOutputStream* outputStream)
{
	//return true;

	this->cooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
	if (!this->cooking) return false;
	return this->cooking->NxInitCooking(allocator, outputStream);
}

bool 
PhysXServer::CookTriangleMesh(const NxTriangleMeshDesc& desc, NxStream& stream)
{
	//return true;

	if ( !this->cooking ) return false;
	return this->cooking->NxCookTriangleMesh(desc,stream);
}

void 
PhysXServer::CloseCooking()
{
	//return;

	if ( !this->cooking ) return;
	this->cooking->NxCloseCooking();
}

//------------------------------------------------------------------------------
/**
	增加调试信息,在创建完所有actor后调用，否则有些mesh无法在调试模式下显示
*/
void 
PhysXServer::AddUserDataToActor()
{
#if NEBULA3_DEBUG
	if (scene)
		AddUserDataToActors(scene);
#endif
}

//------------------------------------------------------------------------------
/**
	vertexNum:顶点数量
	vertexStrideBytes:顶点步长,每个顶点的数据大小
	vertices:顶点数据指针（可能包含pos,nor,color等）
	indexNum:几何数量!
	indices:索引数据
	indexFlag:索引标记，１６或３２位
*/
bool 
PhysXServer::CreateTriMesh(DWORD vertexNum, int vertexStrideBytes, void* vertices, 
						   DWORD triangleNum, void* indices, DWORD indexFlag)
{
	// Build physical model
	NxTriangleMeshDesc meshDesc;
	meshDesc.numVertices				= vertexNum;
	meshDesc.numTriangles				= triangleNum;
	meshDesc.pointStrideBytes			= vertexStrideBytes;
	meshDesc.triangleStrideBytes		= /*(indexFlag== NX_MF_16_BIT_INDICES) ?3*sizeof(NxU16):*/3*sizeof(NxU32);
	meshDesc.points						= vertices;
	meshDesc.triangles					= indices;
	//meshDesc.flags						= indexFlag;
	meshDesc.flags = (scene->getSimType() == NX_SIMULATION_HW) ?  NX_MF_HARDWARE_MESH :0;

	meshDesc.heightFieldVerticalAxis	= NX_Y;
	meshDesc.heightFieldVerticalExtent	= -1000.0f;

	NxTriangleMeshShapeDesc meshShapeDesc;

	MemoryWriteBuffer writeBuffer;
	bool status = CookTriangleMesh(meshDesc, writeBuffer);
	if (!status) {
		n_printf("Unable to cook a triangle mesh.");
		return false;
	}

	meshShapeDesc.meshData = this->physicsSDK->createTriangleMesh(MemoryReadBuffer(writeBuffer.data));

	meshShapeDesc.group					= GROUP_COLLIDABLE_NON_PUSHABLE;
	if (scene->getSimType() == NX_SIMULATION_HW)
		meshShapeDesc.meshPagingMode = NX_MESH_PAGING_AUTO;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&meshShapeDesc);
	this->scene->createActor(actorDesc);

	return true;
}

bool
PhysXServer::CreateMeshFromFile(const Util::String& fileName)
{
	Ptr<PhysXMesh> mesh = PhysXMesh::Create();
	mesh->SetFilename(fileName);
	mesh->Load();
	if (!mesh->IsLoaded())
		return false;

	bool ret = CreateTriMesh(mesh->GetNumVertices(), mesh->GetVertexByteSize(), mesh->GetVertexPointer(),
		mesh->GetNumIndices()/3, mesh->GetIndexPointer());
	mesh->UnmapIndexBuffer();
	mesh->UnmapVertexBuffer();
	mesh = 0;
	return ret;

	// test
	/*float vertices[]={
    -1000.0f, 0.0f, -1000.0f, -1000.0f, 0.0f,  1000.0f, 
     1000.0f, 0.0f,  1000.0f, -1000.0f, 0.0f, -1000.0f, 
	 1000.0f, 0.0f,  1000.0f,  1000.0f, 0.0f, -1000.0f 
	};
	DWORD index[] = {0,1,2,3,4,5};
	return CreateTriMesh(6, 3*sizeof(float), vertices, 2, index);*/
}

NxActor*  
PhysXServer::CreateBox(const Math::vector& startPos, const Math::vector& size)
{
	//NxReal boxStartHeight = 3.5; 

	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a box, 1m on a side
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions.set(size.x(), size.y(), size.z());
	boxDesc.localPose.t = NxVec3(0, 0, 0);
	actorDesc.shapes.pushBack(&boxDesc);

	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 10.0f;
	actorDesc.globalPose.t	= NxVec3(startPos.x(),startPos.y(), startPos.z());	
	n_assert(actorDesc.isValid());
	return scene->createActor(actorDesc);	
}

NxActor* 
PhysXServer::CreateSphere(const Math::vector& startPos, float radius)
{
	// Set the sphere starting height to 3.5m so box starts off falling onto the ground
	//NxReal sphereStartHeight = 3.5; 

	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a sphere, 1m on radius
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius		= radius;
	sphereDesc.localPose.t	= NxVec3(0, 0, 0);

	actorDesc.shapes.pushBack(&sphereDesc);
	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 10.0f;
	actorDesc.globalPose.t	= NxVec3(startPos.x(),startPos.y(), startPos.z());	
	return scene->createActor(actorDesc);
}

//------------------------------------------------------------------------------
/**
	startDir:开始方向（旋转角度0-360），默认为0(坚着)
*/
NxActor* 
PhysXServer::CreateCapsule(const Math::vector& startPos, float radius, float height, float startDir)
{
	// Set the capsule starting height to 3.5m so box starts off falling onto the ground
	//NxReal capsuleStartHeight = 3.5; 

	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a sphere, 1m on radius
	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.radius		= radius;
	capsuleDesc.height		= height;
	capsuleDesc.localPose.t = NxVec3(0, 0, 0);
	
	//Rotate capsule shape
	NxQuat quat(startDir, NxVec3(0, 0, 1));
	NxMat33 m33(quat);
	capsuleDesc.localPose.M = m33;

	actorDesc.shapes.pushBack(&capsuleDesc);
	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 10.0f;
	actorDesc.globalPose.t	= NxVec3(startPos.x(),startPos.y(), startPos.z());

	////Rotate actor
	//NxQuat quat1(45, NxVec3(1, 0, 0));
	//NxMat33 m331(quat1);
	//actorDesc.globalPose.M = m331;

	return scene->createActor(actorDesc);
}

NxActor* 
PhysXServer::CreateGroundPlane()
{
    // Create a plane with default descriptor
    NxPlaneShapeDesc planeDesc;
    NxActorDesc actorDesc;
    actorDesc.shapes.pushBack(&planeDesc);
    return scene->createActor(actorDesc);
}

void 
PhysXServer::Update(float frameTime)
{
	if (this->scene == 0)
		return;

	// Start collision and dynamics for delta time since the last frame
	if (!isPause)
	{
		scene->simulate(frameTime);
		scene->flushStream();
		scene->fetchResults(NX_RIGID_BODY_FINISHED, true);
	}
}

void
PhysXServer::RenderDebug()
{
	if (scene == 0)
		return;

	// Render all the actors in the scene
    NxU32 nbActors = scene->getNbActors();
    NxActor** actors = scene->getActors();
    while (nbActors--)
    {
        NxActor* actor = *actors++;
		PhysXUtil::DrawActor(actor, true);

        // Handle shadows
        /*if (shadows)
        {
			DrawActorShadow(actor, false);
        }*/
    }


	/*const NxDebugRenderable* pkRenderState = 0;
    pkRenderState = scene->getDebugRenderable();
	dbgRender.RenderData(*pkRenderState);*/
}




}