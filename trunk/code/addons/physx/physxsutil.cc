//------------------------------------------------------------------------------
//  physx/util.cc
//  (C) 2010
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physx/physxutil.h"
#include "physx/userdata.h"
#include "math/matrix44.h"
#include "util/array.h"
#include "coregraphics/shaperenderer.h"

namespace PhysX
{
using namespace Util;
using namespace Math;
using namespace CoreGraphics;

//static float gPlaneData[]={
//    -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
//    1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
//	1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f
//};
static float gPlaneData[]={
    -1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f, 
    1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 
	1.0f, 0.0f, 1.0f,1.0f, 0.0f, -1.0f 
};

typedef NxVec3 Point;
typedef struct _Triangle { NxU32 p0; NxU32 p1; NxU32 p2; } Triangle;

const char* 
PhysXUtil::GetNxSDKCreateError(const NxSDKCreateError& errorCode) 
{
	switch(errorCode) 
	{
		case NXCE_NO_ERROR: return "NXCE_NO_ERROR";
		case NXCE_PHYSX_NOT_FOUND: return "NXCE_PHYSX_NOT_FOUND";
		case NXCE_WRONG_VERSION: return "NXCE_WRONG_VERSION";
		case NXCE_DESCRIPTOR_INVALID: return "NXCE_DESCRIPTOR_INVALID";
		case NXCE_CONNECTION_ERROR: return "NXCE_CONNECTION_ERROR";
		case NXCE_RESET_ERROR: return "NXCE_RESET_ERROR";
		case NXCE_IN_USE_ERROR: return "NXCE_IN_USE_ERROR";
		default: return "Unknown error";
	}
};

void
PhysXUtil::DrawActor(NxActor* actor, bool useShapeUserData)
{
	ShapeRenderer::Instance()->SetZEnable(true);
	ShapeRenderer::Instance()->Begin();

	NxShape*const* shapes = actor->getShapes();
	NxU32 nShapes = actor->getNbShapes();
	while (nShapes--)
	{
		DrawShape(shapes[nShapes], useShapeUserData);
	}

	ShapeRenderer::Instance()->End();
	ShapeRenderer::Instance()->SetZEnable(false);
}

void 
PhysXUtil::DrawShape(NxShape* shape, bool useShapeUserData)
{
    switch(shape->getType())
    {
		case NX_SHAPE_PLANE:
			DrawPlane(shape);
		break;
		case NX_SHAPE_BOX:
			DrawBox(shape);
		break;
		case NX_SHAPE_SPHERE:
			DrawSphere(shape);
		break;
		case NX_SHAPE_CAPSULE:
			DrawCapsule(shape);
		break;
		case NX_SHAPE_CONVEX:
			//DrawConvex(shape, useShapeUserData);
		break;
		case NX_SHAPE_MESH:
			DrawMesh(shape, useShapeUserData);
		break;
		case NX_SHAPE_WHEEL:
			//DrawWheelShape(shape);
			break;
		case NX_SHAPE_HEIGHTFIELD:
			//DrawHeightfield(shape);
			break;
	}
}

Math::matrix44 
PhysXUtil::GetMatrix(const NxVec3& pos, const NxMat33& orient)
{
	float m[16];
	orient.getRowMajorStride4(&(m[0]));
	pos.get(&(m[12]));
	m[3] = m[7] = m[11] = 0.0f;
	m[15] = 1.0f;
	return *(matrix44*)m;
}

void 
PhysXUtil::DrawBox(NxShape* box)
{
	matrix44 mat;
	NxMat34 pose =	box->getGlobalPose();
	NxVec3 boxDim = box->isBox()->getDimensions();
	matrix44 mt = GetMatrix(pose.t, pose.M);
	mat = matrix44::multiply(mt, matrix44::scaling(boxDim.x, boxDim.y, boxDim.z));
	
	ShapeRenderer::Instance()->DrawShape(mat, ShapeRenderer::Box, float4(1.0f, 1.0f, 0.0f, 0.6f));
}

void 
PhysXUtil::DrawPlane(NxShape* plane)
{
	matrix44 mat;
	NxMat34 pose =	plane->getGlobalPose();
	pose.t.y -= 0.1f;
	matrix44 mt = GetMatrix(pose.t, pose.M);
	mat = matrix44::multiply(mt, matrix44::scaling(10240.0f, 0.0f, 10240.0f));

	ShapeRenderer::Instance()->DrawPrimitives(mat, 
		PrimitiveTopology::TriangleList, 2, gPlaneData, 3, float4(1.0f, 1.0f, 0.0f, 0.6f));
}

void 
PhysXUtil::DrawSphere(NxShape* sphere)
{
	matrix44 mat;
	NxMat34 pose = sphere->getGlobalPose();
	matrix44 mt = GetMatrix(pose.t, pose.M);
	NxReal r = sphere->isSphere()->getRadius();
	mat = matrix44::multiply(mt, matrix44::scaling(r, r, r));
	
	ShapeRenderer::Instance()->DrawShape(mat, ShapeRenderer::Sphere, float4(1.0f, 0.0f, 1.0f, 0.6f));
}

void 
PhysXUtil::DrawCapsule(NxShape* capsule)
{
	NxMat34 pose = capsule->getGlobalPose();
	matrix44 mt = GetMatrix(pose.t, pose.M);
	const NxReal & r = capsule->isCapsule()->getRadius();
	const NxReal & h = capsule->isCapsule()->getHeight();
	
	matrix44 mat;

	mat = matrix44::identity();
	mat = matrix44::multiply(matrix44::scaling(r, r, r), matrix44::translation(0.0f, h*0.5f, 0.0f));
	mat = matrix44::multiply(mat, mt);
	ShapeRenderer::Instance()->DrawShape(mat, ShapeRenderer::Sphere, float4(1.0f, 1.0f, 0.0f, 1.0f));
	
	mat = matrix44::identity();
	mat = matrix44::multiply(matrix44::scaling(r, r, r), matrix44::translation(0.0f, -h*0.5f, 0.0f));
	mat = matrix44::multiply(mat, mt);
	ShapeRenderer::Instance()->DrawShape(mat, ShapeRenderer::Sphere, float4(1.0f, 1.0f, 0.0f, 1.0f));
	
	mat = matrix44::identity();
	mat = matrix44::multiply(matrix44::scaling(r, r, h), matrix44::rotationx(n_deg2rad(90.0f)));
	//mat = matrix44::multiply(mat, matrix44::translation(0.0f, h*0.5f, 0.0f));
	//mat = matrix44::multiply(matrix44::scaling(r, h, r), matrix44::rotationx(n_deg2rad(90.0f)));
	mat = matrix44::multiply(mat, mt);
	ShapeRenderer::Instance()->DrawShape(mat, ShapeRenderer::Cylinder, float4(1.0f, 1.0f, 0.0f, 1.0f));
}

void 
PhysXUtil::DrawMesh(NxShape* mesh, bool useShapeUserData)
{
	if(mesh->userData == NULL) return;

	NxMat34 pose = mesh->getGlobalPose();

	void* ptr = NULL;
	if (useShapeUserData) {
		ShapeUserData* sud = (ShapeUserData*)(mesh->userData);
		ptr = sud->mesh;
		if (!(sud && sud->mesh))  return;
	} else {
		ptr = mesh->userData;
	}

	NxTriangleMeshDesc meshDesc = *((NxTriangleMeshDesc*)(ptr));
//	mesh->isTriangleMesh()->getTriangleMesh().saveToDesc(meshDesc);	

	NxU32 nbVerts = meshDesc.numVertices;
	NxU32 nbTriangles = meshDesc.numTriangles;

	Point* points = (Point *)meshDesc.points;
	Triangle* triangles = (Triangle *)meshDesc.triangles;

	matrix44 mat = GetMatrix(pose.t, pose.M);

	//DrawTriangleList(nbTriangles, triangles, points);

	Array<WORD> indices;
	for (int i = 0; i < nbTriangles; i++)
	{
		indices.Append(triangles[i].p0);
		indices.Append(triangles[i].p1);
		indices.Append(triangles[i].p2);
	}

	ShapeRenderer::Instance()->DrawIndexedPrimitives(mat, 
		PrimitiveTopology::TriangleList, nbTriangles, 
		&(points[0].x), nbVerts, 3, &(indices[0]), IndexType::Index16, 
		float4(1.0f, 1.0f, 0.0f, 0.6f));
}

//void 
//PhysXUtil::DrawTriangleList(int iTriangleCount, Triangle *pTriangles, Point *pPoints)
//{
//	static int iBufferSize=0;
//	static float *pfVertexBuffer=NULL;
//	static float *pfNormalBuffer=NULL;
//
//	if(iBufferSize < iTriangleCount*3)
//	{
//		iBufferSize=3*iTriangleCount;
//
//		delete[] pfVertexBuffer;
//		pfVertexBuffer = new float[iBufferSize*3];
//
//		delete[] pfNormalBuffer;
//		pfNormalBuffer = new float[iBufferSize*3];
//	}
//
//	float *pfDestinationVertex=pfVertexBuffer;
//	float *pfDestinationNormal=pfNormalBuffer;
//
//	for(int iTriangle=0; iTriangle<iTriangleCount; iTriangle++)
//	{
//		*pfDestinationVertex++=pPoints[pTriangles->p0].x;
//		*pfDestinationVertex++=pPoints[pTriangles->p0].y;
//		*pfDestinationVertex++=pPoints[pTriangles->p0].z;
//		*pfDestinationVertex++=pPoints[pTriangles->p1].x;
//		*pfDestinationVertex++=pPoints[pTriangles->p1].y;
//		*pfDestinationVertex++=pPoints[pTriangles->p1].z;
//		*pfDestinationVertex++=pPoints[pTriangles->p2].x;
//		*pfDestinationVertex++=pPoints[pTriangles->p2].y;
//		*pfDestinationVertex++=pPoints[pTriangles->p2].z;
//
//		NxVec3 edge1 = pPoints[pTriangles->p1] - pPoints[pTriangles->p0];
//		NxVec3 edge2 = pPoints[pTriangles->p2] - pPoints[pTriangles->p0];
//		NxVec3 normal = edge1.cross(edge2);
//		normal.normalize();
//
//		*pfDestinationNormal++=normal.x;
//		*pfDestinationNormal++=normal.y;
//		*pfDestinationNormal++=normal.z;
//		*pfDestinationNormal++=normal.x;
//		*pfDestinationNormal++=normal.y;
//		*pfDestinationNormal++=normal.z;
//		*pfDestinationNormal++=normal.x;
//		*pfDestinationNormal++=normal.y;
//		*pfDestinationNormal++=normal.z;
//
//		pTriangles++;
//	}
//
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glVertexPointer(3, GL_FLOAT, 0, pfVertexBuffer);
//	glEnableClientState(GL_NORMAL_ARRAY);
//	glNormalPointer(GL_FLOAT, 0, pfNormalBuffer);
//
//	glDrawArrays(GL_TRIANGLES, 0, 3*iTriangleCount);
//
//	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_NORMAL_ARRAY);
//
//	
//}

} // namespace Physics