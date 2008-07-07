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
	/// ����Ƿ����߶ν���
	bool IntersectLine(const Math::float4& startPoint, const Math::float4& endPoint);
	/// �����Ƿ񱻼��� 
	void SetIntersected(bool b);
	/// �Ƿ��Ѿ�����������Ҫ�ظ����
	bool IsIntersected() const;
	///
	const Math::vector* GetVertices()const;
	///
	float Distance(Math::vector& v);
	/// 
	///
	const Ptr<ConvexZone>& GetIndoorZone();
private:
	/// ��group����
	Ptr<ConvexZone> posGroup, negGroup;
	/// 4����
	Math::vector vertex[4];
	/// ֻ��һ����Ϳ��ԣ������ж�������档
	Math::plane p;
	/// 
	Math::bbox boundbox;
	/// �Ƿ��Ѿ�����������ֹ��������A-portal-B Aͨ��portal����B��BҲ�ܿ���A������һ��Portal��Ͳ��ܻ��ˣ�����������ѭ����
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