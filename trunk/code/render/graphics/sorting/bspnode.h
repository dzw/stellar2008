#pragma once
#ifndef N_BSPNODE_H
#define N_BSPNODE_H
//------------------------------------------------------------------------------
/**
	@class BspNode

	a bsp node in a bsp tree

	(C) 2008 ctuo
*/
#include "core/refcounted.h"
#include "math/plane.h"

//------------------------------------------------------------------------------
namespace Graphics
{

class BspNode : public Core::RefCounted
{
	DeclareClass(BspNode);
public:
	/// constructor
	BspNode();
	/// destructor
	~BspNode();
	/// set negative child
	void SetNegChild(Ptr<BspNode>& c);
	/// get negative child
	Ptr<BspNode> GetNegChild() const;
	/// set positive child
	void SetPosChild(Ptr<BspNode>& c);
	/// get positive child
	Ptr<BspNode> GetPosChild() const;
	/// set plane used to split this node
	void SetPlane(const Math::plane& p);
	/// get plane used to split this node
	const Math::plane& GetPlane() const;
	/// return true if this is a leaf node (contains geometry)
	bool IsLeaf() const;
	///
	//bool IsContainingNode (const Math::vector& p);

private:
	Ptr<BspNode> negChild;
	Ptr<BspNode> posChild;
	Math::plane splitPlane;
	//Ptr<Cell> node;
};

//------------------------------------------------------------------------------
/**
*/
inline
BspNode::BspNode() :
negChild(0),
posChild(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
inline
BspNode::~BspNode()
{
	if (this->negChild.isvalid())
	{
		n_delete(this->negChild);
		this->negChild = 0;
	}
	if (this->posChild.isvalid())
	{
		n_delete(this->posChild);
		this->posChild = 0;
	}
}

//------------------------------------------------------------------------------
/**
*/
inline
void
BspNode::SetNegChild(Ptr<BspNode>& c)
{
	n_assert(c);
	n_assert(!this->negChild.isvalid());
	this->negChild = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
Ptr<BspNode>
BspNode::GetNegChild() const
{
	return this->negChild;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
BspNode::SetPosChild(Ptr<BspNode>& c)
{
	n_assert(c);
	n_assert(!this->posChild.isvalid());
	this->posChild = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
Ptr<BspNode>
BspNode::GetPosChild() const
{
	return this->posChild;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
BspNode::SetPlane(const Math::plane& p)
{
	this->splitPlane = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::plane&
BspNode::GetPlane() const
{
	return this->splitPlane;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
BspNode::IsLeaf() const
{
	return (this->posChild == 0);
}

}
#endif