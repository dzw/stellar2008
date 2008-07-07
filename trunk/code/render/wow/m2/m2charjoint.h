#ifndef M2_CHARJOINT_H
#define M2_CHARJOINT_H
//------------------------------------------------------------------------------
/**
    @class nCharJoint
    @ingroup Character

    @brief A joint in a character skeleton.

     - 06-Feb-03   floh    fixed for Nebula2

    (C) 2002 RadonLabs GmbH
*/
#include "core/types.h"
#include "math/point.h"
#include "math/matrix44.h"
#include "math/quaternion.h"
#include "util/string.h"
#include "util/fixedarray.h"

#include "wow/m2/animation/matrix.h"
#include "wow/m2/animation/animated.h"
#include "wow/m2/animation/quaternion.h"
#include "wow/m2/animation/vec3d.h"

namespace WOW
{
//------------------------------------------------------------------------------
class M2CharJoint
{
public:
    /// constructor
    M2CharJoint();
    /// destructor
    ~M2CharJoint();
    void Evaluate(const Util::FixedArray<M2CharJoint>& allbones, int anim, int time, bool rotate = true);
	void Init(unsigned char *f, ModelBoneDef &b, int *global);

	const Math::matrix44& GetSkinMatrix44();
	void ClearUptodateFlag();

private:
    Animated<Vec3D> trans;
	//Animated<Quaternion> rot;
	Animated<Quaternion, PACK_QUATERNION, Quat16ToQuat32> rot;
	Animated<Vec3D> scale;

	Vec3D pivot, transPivot;
	int parent;

	bool billboard;
	Matrix mat;
	Matrix mrot;

	ModelBoneDef boneDef;	
	bool calc;


	Math::matrix44 skinMatrix;
};



//------------------------------------------------------------------------------
/**
*/
inline
M2CharJoint::M2CharJoint() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
M2CharJoint::~M2CharJoint()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Clear the uptodate flag. This flag is used in Evaluate() to check
    whether a parent joint has already been evaluated.
*/
inline
void
M2CharJoint::ClearUptodateFlag() 
{
    this->calc = false;
}

//------------------------------------------------------------------------------
/**
    This computes the skinning matrix from the pose matrix, the translation,
    the rotation and the scale of the joint. The parent joint must already be
    uptodate!
*/
inline
void
M2CharJoint::Evaluate(const Util::FixedArray<M2CharJoint>& allbones, int anim, int time, bool rotate)
{
    if (!this->calc)
    {
        
		Matrix m;
		Quaternion q;

		bool tr = rot.used || scale.used || trans.used || billboard;
		if (tr) {
			m.translation(pivot);

			if (trans.used) {
				Vec3D tr = trans.getValue(anim, time);
				m *= Matrix::newTranslation(tr);
			}

			if (rot.used && rotate) {
				q = rot.getValue(anim, time);
				m *= Matrix::newQuatRotate(q);
			}

			if (scale.used) {
				Vec3D sc = scale.getValue(anim, time);
				m *= Matrix::newScale(sc);
			}

			//if (billboard) {			
			//	float modelview[16];
			//	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

			//	Vec3D vRight = Vec3D(modelview[0], modelview[4], modelview[8]);
			//	Vec3D vUp = Vec3D(modelview[1], modelview[5], modelview[9]); // Spherical billboarding
			//	//Vec3D vUp = Vec3D(0,1,0); // Cylindrical billboarding
			//	vRight = vRight * -1;
			//	m.m[0][2] = vRight.x;
			//	m.m[1][2] = vRight.y;
			//	m.m[2][2] = vRight.z;
			//	m.m[0][1] = vUp.x;
			//	m.m[1][1] = vUp.y;
			//	m.m[2][1] = vUp.z;
			//}

			m *= Matrix::newTranslation(pivot*-1.0f);

		} else m.unit();

		if (parent > -1) {
			allbones[parent].Evaluate(allbones, anim, time, rotate);
			mat = allbones[parent].mat * m;
		} else mat = m;

		// transform matrix for normal vectors ... ??
		if (rot.used && rotate) {
			if (parent>=0)
				mrot = allbones[parent].mrot * Matrix::newQuatRotate(q);
			else
				mrot = Matrix::newQuatRotate(q);
		} else mrot.unit();

		transPivot = mat * pivot;

		calc = true;
    }
}



}; // namespace Nebula2
//------------------------------------------------------------------------------
#endif
