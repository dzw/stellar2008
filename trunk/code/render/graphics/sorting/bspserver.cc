//------------------------------------------------------------------------------
//  bspserver.cc
//  (C) 2008 ctuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/sorting/bspserver.h"

namespace Graphics
{
ImplementClass(Graphics::BspServer, 'BSPR', Core::RefCounted);
ImplementSingleton(Graphics::BspServer);

//------------------------------------------------------------------------------
/**
*/
BspServer::BspServer() :
	isOpen(false)
{
	ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
BspServer::~BspServer()
{
	DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
BspServer::Open()
{
	n_assert(!this->isOpen);
	this->isOpen = true;
}

//------------------------------------------------------------------------------
/**
*/
void
BspServer::Close()
{
	n_assert(this->isOpen);
}

//------------------------------------------------------------------------------
/**
*/
void 
BspServer::AddWMOBsp(const Resources::ResourceId& name, Ptr<BspNode>& bsp)
{
	wmoBsps.Add(name, bsp);
}

}