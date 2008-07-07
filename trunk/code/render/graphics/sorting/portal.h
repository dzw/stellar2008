#ifndef N_PORTAL_H
#define N_PORTAL_H
//------------------------------------------------------------------------------
/**
	@class Portal

	(C) 2008 ctuo
*/
#include "core/refcounted.h"
#include "graphics/sorting/convexzone.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class ConvexZone;

class Portal : public Core::RefCounted
{
	DeclareClass(Portal);
public:
	/// constructor
	Portal();
	/// destructor
	~Portal();
	/// set relationship between group and portal
	void SetConvexZone(const Ptr<ConvexZone>& pos, const Ptr<ConvexZone>& neg);
	/// set plane of portal
	void SetPlane(const Math::vector& a, const Math::vector& b, const Math::vector& c, const Math::vector& d);
	/// get bound box
	const Math::bbox& GetBoundBox() const;
	/// return another groupcell between group1 and group2
	const Ptr<ConvexZone>& GetOtherZone(const Ptr<ConvexZone>& g) const;
	/// get plane width
	const float GetWidth() const;
	/// 
	const float GetHeight() const;
	/// get center
	const Math::vector GetCenter() const;
	/// 
	const Math::plane& GetPlane()const;
	///
	void SetOpen(bool b);
	///
	bool IsOpen();
	/// 检测是否与线段交叉
	bool IntersectLine(const Math::float4& startPoint, const Math::float4& endPoint);
	/// 设置是否被检测过 
	void SetIntersected(bool b);
	/// 是否已经被检测过，不要重复检测
	bool IsIntersected() const;
	///
	const Math::vector* GetVertices()const;
	///
	float Distance(Math::vector& v);
	/// 
	///
	const Ptr<ConvexZone>& GetIndoorZone();
private:
	/// 与group相连
	Ptr<ConvexZone> posGroup, negGroup;
	/// 4个点
	Math::vector vertex[4];
	/// 只需一个面就可以，用于判断正面或反面。
	Math::plane p;
	/// 
	Math::bbox boundbox;
	/// 是否已经遍历过，防止反复，如A-portal-B A通过portal看到B，B也能看到A，经过一次Portal后就不能回退，否则会出错（死循环）
	bool isOpen;
	///
	Math::vector planNormal;
	///
	bool isIntersected;
};

//------------------------------------------------------------------------------
/**
*/
inline void 
Portal::SetIntersected(bool b)
{
	this->isIntersected = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
Portal::IsIntersected() const
{
	return this->isIntersected;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::vector* 
Portal::GetVertices()const
{
	return vertex;
}

}
#endif