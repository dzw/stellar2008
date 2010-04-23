//------------------------------------------------------------------------------
//  PhysX/CharControlServer.cc
//  (C) 2010
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "charactercontrolserver.h"
#include "NxPhysics.h"
#include "NxBoxController.h"
#include "NxCapsuleController.h"
#include <stdio.h>
#include "physx/physxserver.h"


namespace PhysX
{
ImplementClass(PhysX::CharControlServer, 'PCCM', Core::RefCounted);
ImplementSingleton(PhysX::CharControlServer);

//#define DEBUG_CCT
//#define MAX_NB_PTS	100
//const NxVec3	gStartPos(0.0f, 10.0f, 0.0f);  // 3
//const NxVec3	gInitialExtents(0.5f, 1.0f, 0.5f);

const NxF32 G = -9.8;
const NxVec3 gDefaultGravity(0,-9.8,0);

#pragma warning(disable:4244)

class ControllerHitReport : public NxUserControllerHitReport
{
public:
	virtual NxControllerAction  onShapeHit(const NxControllerShapeHit& hit)
	{
		//		renderTerrainTriangle(hit.faceID);

		if(1 && hit.shape)
		{
			NxCollisionGroup group = hit.shape->getGroup();
			if(group!=GROUP_COLLIDABLE_NON_PUSHABLE)
			{
				NxActor& actor = hit.shape->getActor();
				if(actor.isDynamic())
				{
					

					// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
					// useless stress on the solver. It would be possible to enable/disable vertical pushes on
					// particular objects, if the gameplay requires it.
					if(hit.dir.y==0.0f)
					{
						NxF32 coeff = actor.getMass() * hit.length * 10.0f;
						actor.addForceAtLocalPos(hit.dir*coeff, NxVec3(0,0,0), NX_IMPULSE);
						//						actor.addForceAtPos(hit.dir*coeff, hit.controller->getPosition(), NX_IMPULSE);
						//						actor.addForceAtPos(hit.dir*coeff, hit.worldPos, NX_IMPULSE);
					}
				}
			}
		}

		return NX_ACTION_NONE;
	}

	virtual NxControllerAction  onControllerHit(const NxControllersHit& hit)
	{
		return NX_ACTION_NONE;
	}

} gControllerHitReport;


CharControlServer::CharControlServer():
isOpen(false),
isJump(false),
pushCharacter(false),
jumpTime(0.0f),
v0(0.0f),
character(0),
collisionGroups(COLLIDABLE_MASK),
characterSpeed(10.0f)
{
	ConstructSingleton;

	characterVec = NxVec3(1,0,0);
}

CharControlServer::~CharControlServer()
{
	DestructSingleton;
}

void 
CharControlServer::Open()
{
	n_assert(!this->isOpen);
	n_assert(PhysXServer::HasInstance());

	PhysXServer* physXServer = PhysXServer::Instance();
	ccManager = NxCreateControllerManager((NxUserAllocator*)physXServer->GetMemoryAllocator());
	this->isOpen = true;
}

void 
CharControlServer::Close()
{
	n_assert(this->isOpen);
	NxReleaseControllerManager(ccManager);
}

void 
CharControlServer::UpdateCharacter(float deltaTime)
{
	NxVec3 disp = gDefaultGravity;

	if (pushCharacter)
	{
		NxVec3 horizontalDisp = this->characterVec;
		horizontalDisp.y = 0.0f;
		horizontalDisp.normalize();
		disp += horizontalDisp * this->characterSpeed;
	}

	disp *= deltaTime;
	NxF32 height = GetHeight(deltaTime); // compute height(Y) in jumping
	if (height != 0.0f)
	{
		disp.y += height;
	}

	NxU32 collisionFlags = MoveCharacter(disp, deltaTime, this->collisionGroups, height);
	if (collisionFlags & NXCC_COLLISION_DOWN)  
		StopJump();

	ccManager->updateControllers();
}

void 
CharControlServer::CreateCharacter(const NxVec3& startPos, float radius, float height)
{
	NxSece* scene = PhysXServer::Instance()->GetScene(); 
	//if(i&1)
	//{
	//	NxBoxControllerDesc desc;
	//	NxVec3 tmp			= startPos[i];
	//	desc.position.x		= tmp.x;
	//	desc.position.y		= tmp.y;
	//	desc.position.z		= tmp.z;
	//	desc.extents		= gInitialExtents * scale;
	//	desc.upDirection	= NX_Y;
	//	desc.slopeLimit		= 0;
	//	desc.slopeLimit		= cosf(NxMath::degToRad(45.0f));
	//	desc.skinWidth		= SKINWIDTH;
	//	desc.stepOffset		= 0.5;
	//	//	desc.stepOffset	= 0.01f;
	//	//		desc.stepOffset		= 0;
	//	//		desc.stepOffset		= 10;
	//	desc.callback		= &gControllerHitReport;
	//	ccManager->createController(&scene, desc);
	//}
	//else
	//{
		NxCapsuleControllerDesc desc;
		//NxVec3 tmp			= startPos[i];
		desc.position.x		= startPos.x;
		desc.position.y		= startPos.y;
		desc.position.z		= startPos.z;
		desc.radius			= radius;
		desc.height			= height;
		desc.upDirection	= NX_Y;
		//		desc.slopeLimit		= cosf(NxMath::degToRad(45.0f));
		desc.slopeLimit		= 0;
		desc.skinWidth		= 0.1f;
		desc.stepOffset		= 0.5;
		//	desc.stepOffset	= 0.01f;
		//		desc.stepOffset		= 0;	// Fixes some issues
		//		desc.stepOffset		= 10;
		desc.callback		= &gControllerHitReport;
		character = ccManager->createController(scene, desc);
	//}
	//return ccManager->getController(0);
}

//void 
//CharControlServer::ReleaseCharacterControllers(NxScene& scene)
//{
//	ccManager->purgeControllers();
//}

#ifdef DEBUG_CCT
static NxVec3 gLimitPos(0,0,0);	// Debug
#endif
NxU32 
CharControlServer::MoveCharacter(const NxVec3& dispVector, NxF32 elapsedTime, NxU32 collisionGroups, NxF32 heightDelta)
{
	if (!character) return 0;

#ifndef DEBUG_CCT
	{
		NxU32 collisionFlags;
		NxVec3 d = dispVector*elapsedTime;
		if(heightDelta!=0.0f)
			d.y+=heightDelta;
		character->move(d, collisionGroups, 0.000001f, collisionFlags);
		return collisionFlags;
	}
#else
	{
		NxF32 sharpness = 1.0f;

		NxU32 collisionFlags;
		NxVec3 d = dispVector*elapsedTime;
		if(heightDelta!=0.0f)
			d.y+=heightDelta;

		d.y = 0.0f;

		NxController* c = ccManager->getController(characterIndex);
		c->move(d, collisionGroups, 0.000001f, collisionFlags, sharpness);

		NxVec3 pos = c->getDebugPosition();

		d.x = d.z = 0.0f;
		d.y = -100.0f;

		NxU32 collisionFlags2;
		c->move(d, collisionGroups, 0.001f, collisionFlags2, sharpness);

		gLimitPos = c->getDebugPosition();

		c->setPosition(pos);

		return collisionFlags;
	}
#endif
}


static NxExtendedVec3 zero(0,0,0);
const NxExtendedVec3& 
CharControlServer::GetCharacterPos(NxU32 characterIndex)
{
	return ccManager->getController(characterIndex)->getFilteredPosition();
}

NxActor* 
CharControlServer::GetCharacterActor(NxU32 characterIndex)
{
	return ccManager->getController(characterIndex)->getActor();
}

bool 
CharControlServer::ResetCharacterPos(NxU32 index, const NxVec3& pos)
{
	return ccManager->getController(index)->setPosition(NxExtendedVec3(pos.x, pos.y, pos.z));
}

bool 
CharControlServer::UpdateCharacterExtents(NxU32 index, bool& increase)
{
	if (!PhysXServer::HasInstance())
		return false;
	NxScene* scene = PhysXServer::Instance()->GetScene();
	if (scene == 0)
		return false;

	if(index&1)
	{
		NxBoxController* c = static_cast<NxBoxController*>(ccManager->getController(index));
		NxVec3 extents = c->getExtents();
		NxF32 inc = 1.0f;
		NxExtendedVec3 pos = GetCharacterPos(index);
		if (increase)
		{
			extents.y += inc;
			pos.y += inc;
		} 
		else 
		{ 
			extents.y -= inc;
			pos.y -= inc;
		}

		if(1)
		{
			NxBounds3 worldBounds;
			worldBounds.setCenterExtents(NxVec3(pos.x, pos.y, pos.z), extents);
			c->setCollision(false);	// Avoid checking overlap with ourself
			bool Status = scene->checkOverlapAABB(worldBounds);
			c->setCollision(true);
			if(Status)
			{
				printf("Can not resize box!\n");
				return false;
			}
		}

		increase = !increase;	// Increase or decrease height each time we're called

		// WARNING: the SDK currently doesn't check for collisions when changing extents, so if you're close
		// to a wall you might end up penetrating it. In some cases you might also fall through the level.
		// A more advanced implementation will take care of that later.
		c->setPosition(pos);
		return c->setExtents(extents);
	}
	else
	{
		NxCapsuleController* c = static_cast<NxCapsuleController*>(ccManager->getController(index));
		NxF32 height = c->getHeight();
		NxF32 radius = c->getRadius();
		NxF32 inc = 1.0f;
		NxExtendedVec3 pos = GetCharacterPos(index);
		if (increase)
		{
			height += inc;
			pos.y += inc*0.5f;
		} 
		else 
		{ 
			height -= inc;
			pos.y -= inc*0.5f;
		}

		if(1)
		{
			NxCapsule worldCapsule;
			worldCapsule.p0.x = worldCapsule.p1.x = pos.x;
			worldCapsule.p0.y = worldCapsule.p1.y = pos.y;
			worldCapsule.p0.z = worldCapsule.p1.z = pos.z;
			worldCapsule.p0.y -= height*0.5f;
			worldCapsule.p1.y += height*0.5f;
			worldCapsule.radius = radius;
			c->setCollision(false);	// Avoid checking overlap with ourself
			bool Status = scene->checkOverlapCapsule(worldCapsule);
			c->setCollision(true);
			if(Status)
			{
				printf("Can not resize capsule!\n");
				return false;
			}
		}

		increase = !increase;	// Increase or decrease height each time we're called

		// WARNING: the SDK currently doesn't check for collisions when changing height, so if you're close
		// to a wall you might end up penetrating it. In some cases you might also fall through the level.
		// A more advanced implementation will take care of that later.
		c->setPosition(NxExtendedVec3(pos.x, pos.y, pos.z));
		return c->setHeight(height);
	}
}

void 
CharControlServer::StartJump(float v0)
{
	if (isJump)  return;
	jumpTime = 0.0f;
	v0	= v0;
	isJump = true;
}

void 
CharControlServer::StopJump()
{
	isJump = false;
}

float 
CharControlServer::GetHeight(float elapsedTime)
{
	if (!isJump)  return 0.0f;
	NxF32 Vt = v0 + G*jumpTime; // Vt = Vo + GT
	jumpTime += elapsedTime;
	return Vt*elapsedTime + 1/2*G*elapsedTime*elapsedTime; // S = VtT + 1/2GT^2
}

void 
CharControlServer::RenderCharacters()
{
//	NxU32 number = ccManager->getNbControllers();
//	for(NxU32 i=0;i<number;i++)
//	{
//		if(i&1)
//		{
//			//#ifdef USE_BOX_CONTROLLER
//			glDisable(GL_LIGHTING);
//
//			NxBoxController* bc = static_cast<NxBoxController*>(ccManager->getController(i));
//
//			// Catch current extents
//			const NxVec3& extents = bc->getExtents();
//
//			// Generate 8 corners of the bbox
//			NxExtendedVec3 tmp = GetCharacterPos(i);
//			NxVec3 min = NxVec3(tmp.x, tmp.y, tmp.z) - extents;
//			NxVec3 max = NxVec3(tmp.x, tmp.y, tmp.z) + extents;
//
//			NxVec3 e = extents;
//			e.y += SKINWIDTH;
//			min = NxVec3(tmp.x, tmp.y, tmp.z) - e;
//			max = NxVec3(tmp.x, tmp.y, tmp.z) + e;
//
//			NxVec3 pts[8];
//			pts[0] = NxVec3(min.x, min.y, min.z);
//			pts[1] = NxVec3(max.x, min.y, min.z);
//			pts[2] = NxVec3(max.x, max.y, min.z);
//			pts[3] = NxVec3(min.x, max.y, min.z);
//			pts[4] = NxVec3(min.x, min.y, max.z);
//			pts[5] = NxVec3(max.x, min.y, max.z);
//			pts[6] = NxVec3(max.x, max.y, max.z);
//			pts[7] = NxVec3(min.x, max.y, max.z);
//
//			// Render box
//
//			static float vertexBuf[24*3];
//			vertexBuf[ 0] = pts[0].x;
//			vertexBuf[ 1] = pts[0].y;
//			vertexBuf[ 2] = pts[0].z;
//			vertexBuf[ 3] = pts[1].x;
//			vertexBuf[ 4] = pts[1].y;
//			vertexBuf[ 5] = pts[1].z;
//			vertexBuf[ 6] = pts[1].x;
//			vertexBuf[ 7] = pts[1].y;
//			vertexBuf[ 8] = pts[1].z;
//			vertexBuf[ 9] = pts[2].x;
//			vertexBuf[10] = pts[2].y;
//			vertexBuf[11] = pts[2].z;
//			vertexBuf[12] = pts[2].x;
//			vertexBuf[13] = pts[2].y;
//			vertexBuf[14] = pts[2].z;
//			vertexBuf[15] = pts[3].x;
//			vertexBuf[16] = pts[3].y;
//			vertexBuf[17] = pts[3].z;
//			vertexBuf[18] = pts[3].x;
//			vertexBuf[19] = pts[3].y;
//			vertexBuf[20] = pts[3].z;
//			vertexBuf[21] = pts[0].x;
//			vertexBuf[22] = pts[0].y;
//			vertexBuf[23] = pts[0].z;
//			vertexBuf[24] = pts[4].x;
//			vertexBuf[25] = pts[4].y;
//			vertexBuf[26] = pts[4].z;
//			vertexBuf[27] = pts[5].x;
//			vertexBuf[28] = pts[5].y;
//			vertexBuf[29] = pts[5].z;
//			vertexBuf[30] = pts[5].x;
//			vertexBuf[31] = pts[5].y;
//			vertexBuf[32] = pts[5].z;
//			vertexBuf[33] = pts[6].x;
//			vertexBuf[34] = pts[6].y;
//			vertexBuf[35] = pts[6].z;
//			vertexBuf[36] = pts[6].x;
//			vertexBuf[37] = pts[6].y;
//			vertexBuf[38] = pts[6].z;
//			vertexBuf[39] = pts[7].x;
//			vertexBuf[40] = pts[7].y;
//			vertexBuf[41] = pts[7].z;
//			vertexBuf[42] = pts[7].x;
//			vertexBuf[43] = pts[7].y;
//			vertexBuf[44] = pts[7].z;
//			vertexBuf[45] = pts[4].x;
//			vertexBuf[46] = pts[4].y;
//			vertexBuf[47] = pts[4].z;
//			vertexBuf[48] = pts[0].x;
//			vertexBuf[49] = pts[0].y;
//			vertexBuf[50] = pts[0].z;
//			vertexBuf[51] = pts[4].x;
//			vertexBuf[52] = pts[4].y;
//			vertexBuf[53] = pts[4].z;
//			vertexBuf[54] = pts[1].x;
//			vertexBuf[55] = pts[1].y;
//			vertexBuf[56] = pts[1].z;
//			vertexBuf[57] = pts[5].x;
//			vertexBuf[58] = pts[5].y;
//			vertexBuf[59] = pts[5].z;
//			vertexBuf[60] = pts[3].x;
//			vertexBuf[61] = pts[3].y;
//			vertexBuf[62] = pts[3].z;
//			vertexBuf[63] = pts[7].x;
//			vertexBuf[64] = pts[7].y;
//			vertexBuf[65] = pts[7].z;
//			vertexBuf[66] = pts[2].x;
//			vertexBuf[67] = pts[2].y;
//			vertexBuf[68] = pts[2].z;
//			vertexBuf[69] = pts[6].x;
//			vertexBuf[70] = pts[6].y;
//			vertexBuf[71] = pts[6].z;
//
//			glEnableClientState(GL_VERTEX_ARRAY);
//			glVertexPointer(3, GL_FLOAT, 0, vertexBuf);
//
//			//		if(i!=0 || !gJump) 
//			glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
//			//		else               glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
//
//			glDrawArrays(GL_LINES, 0, 24);
//			glDisableClientState(GL_VERTEX_ARRAY);
//			//#endif
//		}
//		else
//		{
//			//#ifdef USE_CAPSULE_CONTROLLER
//			glEnable(GL_LIGHTING);
//			glEnable(GL_BLEND);
//			glBlendFunc(GL_ONE, GL_ONE);
//			glDepthMask(GL_FALSE);
//
//			NxCapsuleController* cc = static_cast<NxCapsuleController*>(ccManager->getController(i));
//
//#ifdef DEBUG_CCT
//			const NxExtendedVec3& pos = i ? getCharacterPos(i) : gLimitPos;
//#else
//			const NxExtendedVec3& pos = GetCharacterPos(i);
//#endif
//			NxF32 r = cc->getRadius();
//			const NxF32 h = cc->getHeight();
//			if(gDrawSkinWidth)	r += SKINWIDTH;
//
//			glDisable(GL_LIGHTING);
//			glDisable(GL_BLEND);
//			static GLfloat colorBuf[] = {
//				1.0f, 0.0f, 0.0f, 1.0f,
//				1.0f, 0.0f, 0.0f, 1.0f,
//				0.0f, 1.0f, 0.0f, 1.0f,
//				0.0f, 1.0f, 0.0f, 1.0f,
//				0.0f, 0.0f, 1.0f, 1.0f,
//				0.0f, 0.0f, 1.0f, 1.0f,
//				1.0f, 1.0f, 0.0f, 1.0f,
//				1.0f, 1.0f, 0.0f, 1.0f
//			};
//
//			static GLfloat vertexBuf[8*3];
//			vertexBuf[ 0] = pos.x;
//			vertexBuf[ 1] = pos.y;
//			vertexBuf[ 2] = pos.z;
//			vertexBuf[ 3] = pos.x+h;
//			vertexBuf[ 4] = pos.y;
//			vertexBuf[ 5] = pos.z;
//			vertexBuf[ 6] = pos.x;
//			vertexBuf[ 7] = pos.y;
//			vertexBuf[ 8] = pos.z;
//			vertexBuf[ 9] = pos.x;
//			vertexBuf[10] = pos.y+h;
//			vertexBuf[11] = pos.z;
//			vertexBuf[12] = pos.x;
//			vertexBuf[13] = pos.y;
//			vertexBuf[14] = pos.z;
//			vertexBuf[15] = pos.x;
//			vertexBuf[16] = pos.y;
//			vertexBuf[17] = pos.z+h;
//			vertexBuf[18] = pos.x;
//			vertexBuf[19] = pos.y-h*0.5f-r;
//			vertexBuf[20] = pos.z;
//			vertexBuf[21] =	pos.x;
//			vertexBuf[22] = pos.y+h*0.5f+r;
//			vertexBuf[23] = pos.z;
//
//			glEnableClientState(GL_VERTEX_ARRAY);
//			glVertexPointer(3, GL_FLOAT, 0, vertexBuf);
//			glEnableClientState(GL_COLOR_ARRAY);
//			glColorPointer(4, GL_FLOAT, 0, colorBuf);
//
//			glDrawArrays(GL_LINES, 0, 8);
//
//			glDisableClientState(GL_COLOR_ARRAY);
//			glDisableClientState(GL_VERTEX_ARRAY);
//
//			glEnable(GL_BLEND);
//			glEnable(GL_LIGHTING);
//
//			float glmat[16];
//			memset(glmat, 0, 16*4);
//			glmat[0] = glmat[5] = glmat[10] = glmat[15] = 1.0f;
//			glmat[12] = pos.x;
//			glmat[13] = pos.y;
//			glmat[14] = pos.z;
//
//			glPushMatrix();
//			glMultMatrixf(&(glmat[0]));
//			NxVec3 color(1.0f, 1.0f, 1.0f);
//			DrawCapsule(color, r, h);
//			glPopMatrix();
//			//#endif
//		}
//
//
//
//		glEnableClientState(GL_VERTEX_ARRAY);
//		float line1[2][3] = { {gPts[i].x, gPts[i].y, gPts[i].z}, {gPts[i].x+1.0f, gPts[i].y, gPts[i].z}};
//		glVertexPointer(3, GL_FLOAT, 0, line1);
//		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
//		glDrawArrays(GL_LINES, 0, 2);
//		float line2[2][3] = { {gPts[i].x, gPts[i].y, gPts[i].z}, {gPts[i].x, gPts[i].y+1.0f, gPts[i].z}};
//		glVertexPointer(3, GL_FLOAT, 0, line2);
//		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
//		glDrawArrays(GL_LINES, 0, 2);
//		float line3[2][3] = { {gPts[i].x, gPts[i].y, gPts[i].z}, {gPts[i].x, gPts[i].y, gPts[i].z+1.0f}};
//		glVertexPointer(3, GL_FLOAT, 0, line3);
//		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
//		glDrawArrays(GL_LINES, 0, 2);
//		glDisableClientState(GL_VERTEX_ARRAY);
//
//#ifdef DRAW_CONTACT_DATA
//		// Draw contact data, if valid
//		if(gNbContactPts)
//		{
//			NxVec3* t = new NxVec3[gNbContactPts*2];
//			for(NxU32 i=0;i<gNbContactPts;i++)
//			{
//				t[i*2+0] = gContactPt[i];
//				t[i*2+1] = gContactPt[i] + gContactNorm[i] * 10.0f;
//			}
//			glEnableClientState(GL_VERTEX_ARRAY);
//			glVertexPointer(3, GL_FLOAT, 0, t);
//			glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
//			glDrawArrays(GL_LINES, 0, gNbContactPts*2);
//			glDisableClientState(GL_VERTEX_ARRAY);
//			delete[]t;
//			gNbContactPts = 0;
//		}
//#endif
//
//#ifdef DRAW_TOUCHED_TRIANGLE
//		// Draw touched triangle, if valid
//		if(gValidTouchedTri)
//		{
//			glDisable(GL_CULL_FACE);
//			/*			glEnableClientState(GL_VERTEX_ARRAY);
//			glVertexPointer(3, GL_FLOAT, 0, &gTouchedTri);
//			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
//			glDrawArrays(GL_TRIANGLES, 0, 3);
//			glDisableClientState(GL_VERTEX_ARRAY);*/
//
//			NxVec3 t[6];
//			t[0] = gTouchedTri.verts[0];
//			t[1] = gTouchedTri.verts[1];
//			t[2] = gTouchedTri.verts[1];
//			t[3] = gTouchedTri.verts[2];
//			t[4] = gTouchedTri.verts[2];
//			t[5] = gTouchedTri.verts[0];
//			glEnableClientState(GL_VERTEX_ARRAY);
//			glVertexPointer(3, GL_FLOAT, 0, t);
//			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
//			glDrawArrays(GL_LINES, 0, 6);
//			glDisableClientState(GL_VERTEX_ARRAY);
//			glEnable(GL_CULL_FACE);
//		}
//#endif
//	}
//
//	//#ifdef USE_CAPSULE_CONTROLLER
//	glDisable(GL_BLEND);
//	glDepthMask(GL_TRUE);
//	//#endif
//
//	glEnable(GL_LIGHTING);
}

}