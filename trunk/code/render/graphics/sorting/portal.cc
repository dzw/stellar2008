//------------------------------------------------------------------------------
//  portal.cc
//  (C) 2008 ctuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/sorting/portal.h"
#include "math/vector.h"
#include "math/plane.h"

namespace Graphics
{
ImplementClass(Graphics::Portal, 'PORL', Core::RefCounted);

using namespace Math;

//------------------------------------------------------------------------------
/**
*/
Portal::Portal() :
	posGroup(0),
	negGroup(0),
	isOpen(false),
	isIntersected(false)
{
}

//------------------------------------------------------------------------------
/**
*/
Portal::~Portal()
{
	posGroup = 0;
	negGroup = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
Portal::SetConvexZone(const Ptr<ConvexZone>& pos, const Ptr<ConvexZone>& neg)
{
	n_assert(!posGroup.isvalid());
	n_assert(!negGroup.isvalid());

	posGroup = pos;
	negGroup = neg;
}

//------------------------------------------------------------------------------
/**
 排列方式:
  d------a
  |      |
  |      |
  c------b
*/
void 
Portal::SetPlane(const vector& a, const vector& b, const vector& c, const vector& d)
{
	//plane pp(b, a, c);
	//plane ppp(a, d, c);
	
	//p = pp;
	//p[1] = ppp;

	vector center = (d + b) * 0.5f;
	vector vmax = vector::maximize(d, b);
	boundbox.set(center, vmax - center);

	vertex[0] = a;
	vertex[1] = b;
	vertex[2] = c;
	vertex[3] = d;

	vector vecDown, vecLTR;
	vecDown = vertex[0] - vertex[1];
	vecLTR = vertex[0] - vertex[3];
	vecLTR = vector::normalize(vecLTR);
	planNormal = vector::normalize(vector::cross3(vecDown, vecLTR));
	
	plane pp(vertex[0], planNormal);
	p = pp;
}

//------------------------------------------------------------------------------
/**
*/
const Math::plane& 
Portal::GetPlane()const
{
	return p;
}

//------------------------------------------------------------------------------
/**
*/
const bbox& 
Portal::GetBoundBox()const
{
	return boundbox; 
}

//------------------------------------------------------------------------------
/**
	返回与传入ConvexZone不同的变量
*/
const Ptr<ConvexZone>& 
Portal::GetOtherZone(const Ptr<ConvexZone>& g) const
{
	if (g == posGroup)
		return negGroup;
	else
		return posGroup;
}

//------------------------------------------------------------------------------
/**
*/
const float 
Portal::GetWidth() const
{
	return n_abs(vertex[3].x() - vertex[0].x());
}

//------------------------------------------------------------------------------
/**
*/
const float 
Portal::GetHeight() const
{
	return n_abs(vertex[1].y() - vertex[0].y());
}

//------------------------------------------------------------------------------
/**
*/
const vector 
Portal::GetCenter() const
{
	return vector(boundbox.center());
}

//------------------------------------------------------------------------------
/**
*/
void 
Portal::SetOpen(bool b)
{
	this->isOpen = b;
}

//------------------------------------------------------------------------------
/**
*/
bool 
Portal::IsOpen()
{
	return this->isOpen;
}

//------------------------------------------------------------------------------
/**
	线段和面做检测，判断是否穿过了portal
*/
bool 
Portal::IntersectLine(const float4& startPoint, const float4& endPoint)
{
	plane pp(vertex[0], vertex[1], vertex[2]);
	//plane ppp(vertex[0], vertex[3], vertex[2]);

	vector out;
	if (pp.intersectline(startPoint, endPoint, out))
	{
		vector pmin = vector::minimize(startPoint, endPoint);
		vector pmax = vector::maximize(startPoint, endPoint);

		if ((pmin.x() <= out.x() && out.x() <= pmax.x()) &&
			(pmin.y() <= out.y() && out.y() <= pmax.y()) &&
			(pmin.z() <= out.z() && out.z() <= pmax.z()) && boundbox.contains(out))
			return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
float 
Portal::Distance(Math::vector& v)
{
	return vector::dot3(v, planNormal);
}

//------------------------------------------------------------------------------
/**
	当在室外的时候，找到与室内相连的zone
*/
const Ptr<ConvexZone>& 
Portal::GetIndoorZone()
{
	if (!this->negGroup->IsOutDoor())
		return this->negGroup;

	if (!this->posGroup->IsOutDoor())
		return this->posGroup;

	// 不会执行到
	return this->posGroup;
}

}
