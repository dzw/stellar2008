//------------------------------------------------------------------------------
/**
*/
#include "stdneb.h"
#include "wow/m2/m2charjoint.h"

namespace WOW
{

Vec3D fixCoordSystem(Vec3D v)
{
	return Vec3D(v.x, v.z, -v.y);
}

Vec3D fixCoordSystem2(Vec3D v)
{
	return Vec3D(v.x, v.z, v.y);
}

Quaternion fixCoordSystemQuat(Quaternion v)
{
	return Quaternion(-v.x, -v.z, v.y, v.w);
}

#ifdef WotLK
void 
M2CharJoint::Init(const Ptr<IO::Stream> f, ModelBoneDef &b, uint32 *global, const Ptr<IO::Stream> *animfiles)
{
	calc = false;

	parent = b.parent;
	pivot = fixCoordSystem(b.pivot);
	billboard = (b.flags & MODELBONE_BILLBOARD) != 0;
	//billboard = false;

	boneDef = b;
	
	trans.init(b.translation, f, global, animfiles);
	rot.init(b.rotation, f, global, animfiles);
	scale.init(b.scaling, f, global, animfiles);
	trans.fix(fixCoordSystem);
	rot.fix(fixCoordSystemQuat);
	scale.fix(fixCoordSystem2);
}
#else
void 
M2CharJoint::Init(unsigned char *f, ModelBoneDef &b, uint32 *global)
{
	calc = false;

	parent = b.parent;
	pivot = fixCoordSystem(b.pivot);
	billboard = (b.flags & MODELBONE_BILLBOARD) != 0;
	//billboard = false;

	boneDef = b;
	
	trans.init(b.translation, f, global);
	rot.init(b.rotation, f, global);
	scale.init(b.scaling, f, global);
	trans.fix(fixCoordSystem);
	rot.fix(fixCoordSystemQuat);
	scale.fix(fixCoordSystem2);
}
#endif

}; // namespace Nebula2
//------------------------------------------------------------------------------

