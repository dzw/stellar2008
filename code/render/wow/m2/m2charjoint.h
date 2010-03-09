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
#include "wow/m2/modelheaders.h"

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
    void Evaluate(M2CharJoint* allbones, 
					  int anim1, int animTime1,
					  int anim2, int animTime2,
					  float lerpValue, bool rotate = true);
	
#ifdef WotLK
	void Init(const Ptr<IO::Stream> f, ModelBoneDef &b, uint32 *global, const Ptr<IO::Stream> *animfiles);
#else
	void Init(unsigned char *f, ModelBoneDef &b, uint32 *global);
#endif
	void ClearUptodateFlag();
	
	const Math::matrix44& GetMatrix()const;
	const Math::matrix44& GetRotateMatrix()const;


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
inline void
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
inline void
M2CharJoint::Evaluate(M2CharJoint* allbones, 
					  int anim1, int animTime1,
					  int anim2, int animTime2,
					  float lerpValue, bool rotate)
{
    if (this->calc)
		return;
        
	Matrix m;
	Quaternion q1,q2;
	Vec3D tr1,tr2,
		  sc1, sc2;
	/*int anim1 = animParam.actionIndex1;
	int anim2 = animParam.actionIndex2;
	int animTime1 = animParam.animTime1;
	int animTime2 = animParam.animTime2;
	float lerpValue = animParam.lerpValue;*/

	if (trans.uses(anim1)) 
	{
		tr1 = trans.getValue(anim1, animTime1);
	}
	if (trans.uses(anim2)) 
	{
		if (trans.uses(anim1))
		{
			tr2 = trans.getValue(anim2, animTime2);
			D3DXVec3Lerp((D3DXVECTOR3*)&tr1, (CONST D3DXVECTOR3*)&tr1, (CONST D3DXVECTOR3*)&tr2, lerpValue);
		}
		else
		{
			tr1 = trans.getValue(anim2, animTime2);
		}
	}

	if (rot.uses(anim1) && rotate) 
	{
		q1 = rot.getValue(anim1, animTime1);
	}
	if (rot.uses(anim2) && rotate) 
	{
		if (rot.uses(anim1) && rotate)
		{
			q2 = rot.getValue(anim2, animTime2);
			D3DXQuaternionSlerp((D3DXQUATERNION*)&q1,(CONST D3DXQUATERNION *)&q1,(CONST D3DXQUATERNION *)&q2,lerpValue);
		}
		else
		{
			q1 = rot.getValue(anim2, animTime2);
		}
	}
	if (scale.uses(anim1)) 
	{
		sc1 = scale.getValue(anim1, animTime1);
	}
	if (scale.uses(anim2)) 
	{
		if (scale.uses(anim1))
		{
			sc2 = scale.getValue(anim2, animTime2);
			D3DXVec3Lerp((D3DXVECTOR3*)&sc1, (CONST D3DXVECTOR3*)&sc1, (CONST D3DXVECTOR3*)&sc2, lerpValue);
		}
		else
		{
			sc1 = scale.getValue(anim2, animTime2);
		}
	}
	bool bTrans = trans.uses(anim1) || trans.uses(anim2);
	bool bRotate = (rot.uses(anim1) || rot.uses(anim2)) && rotate;
	bool bScale = scale.uses(anim1) || scale.uses(anim2);

	if (bTrans || bRotate || bScale)
	{
		m.translation(pivot);
		if (bTrans)
			m *= Matrix::newTranslation(tr1);
		if (bRotate) 
			m *= Matrix::newQuatRotate(q1);
		if (bScale)
			m *= Matrix::newScale(sc1);
		m *= Matrix::newTranslation(pivot*-1.0f);
	}
	else m.unit();

	//bool tr1 = rot.uses(anim1) || scale.uses(anim1) || trans.uses(anim1) || billboard;
	//if (tr1) {
	//	m.translation(pivot);

	//	if (trans.uses(anim1)) {
	//		tr = trans.getValue(anim1, animTime1);
	//		m *= Matrix::newTranslation(tr);
	//	}

	//	if (rot.uses(anim1) && rotate) {
	//		q = rot.getValue(anim1, animTime1);
	//		m *= Matrix::newQuatRotate(q);
	//	}

	//	if (scale.uses(anim1)) {
	//		sc = scale.getValue(anim1, animTime1);
	//		m *= Matrix::newScale(sc);
	//	}

	//	//if (billboard) {			
	//	//	float modelview[16];
	//	//	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

	//	//	Vec3D vRight = Vec3D(modelview[0], modelview[4], modelview[8]);
	//	//	Vec3D vUp = Vec3D(modelview[1], modelview[5], modelview[9]); // Spherical billboarding
	//	//	//Vec3D vUp = Vec3D(0,1,0); // Cylindrical billboarding
	//	//	vRight = vRight * -1;
	//	//	m.m[0][2] = vRight.x;
	//	//	m.m[1][2] = vRight.y;
	//	//	m.m[2][2] = vRight.z;
	//	//	m.m[0][1] = vUp.x;
	//	//	m.m[1][1] = vUp.y;
	//	//	m.m[2][1] = vUp.z;
	//	//}

	//	m *= Matrix::newTranslation(pivot*-1.0f);
	//} else m.unit();

	if (parent > -1) {
		allbones[parent].Evaluate(allbones, anim1, animTime1, anim2, animTime2, lerpValue, rotate);
		mat = allbones[parent].mat * m;
	} else mat = m;

	// transform matrix for normal vectors ... ??
	if (bRotate) {
		if (parent>=0)
			mrot = allbones[parent].mrot * Matrix::newQuatRotate(q1);
		else
			mrot = Matrix::newQuatRotate(q1);
	} else mrot.unit();

	transPivot = mat * pivot;

	calc = true;
}

inline const Math::matrix44& 
M2CharJoint::GetMatrix()const
{
	return *(Math::matrix44*)&mat;
}

inline const Math::matrix44& 
M2CharJoint::GetRotateMatrix()const
{
	return *(Math::matrix44*)&mrot;
}

}; // namespace Nebula2
//------------------------------------------------------------------------------
#endif
