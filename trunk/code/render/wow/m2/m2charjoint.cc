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

//------------------------------------------------------------------------------
/**
*/
void 
M2CharJoint::Init(unsigned char *f, ModelBoneDef &b, int *global)
{
	calc = false;

	parent = b.parent;
	pivot = fixCoordSystem(Vec3D(b.pivot.x, b.pivot.y, b.pivot.z));
	billboard = (b.flags & 8) != 0;
	//billboard = false;

	boneDef = b;

	trans.init(b.translation, f, global);
	rot.init(b.rotation, f, global);
	scale.init(b.scaling, f, global);
	trans.fix(fixCoordSystem);
	rot.fix(fixCoordSystemQuat);
	scale.fix(fixCoordSystem2);
}

//------------------------------------------------------------------------------
/**
Return the 4x4 skinning matrix. This is the current evaluated matrix
multiplied by the inverse bind pose matrix.
*/
const Math::matrix44&
M2CharJoint::GetSkinMatrix44()
{
	skinMatrix.set(
		Math::float4(mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3]), 
		Math::float4(mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3]),
		Math::float4(mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3]),
		Math::float4(mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3])
		);
	skinMatrix = Math::matrix44::transpose(skinMatrix);
	return this->skinMatrix;
}

}; // namespace Nebula2
//------------------------------------------------------------------------------

