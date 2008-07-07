//------------------------------------------------------------------------------
//  bspnode.cc
//  (C) 2008 ctuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/sorting/wmobspnode.h"

namespace Graphics
{
	ImplementClass(Graphics::WMOBspNode, 'WBSP', Graphics::BspNode);

//------------------------------------------------------------------------------
/**
*/
WMOBspNode::WMOBspNode()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
WMOBspNode::~WMOBspNode()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
WMOBspNode::AddNodeData(Util::Array<unsigned short>& ib)
{
	indexBuffer = ib;
}

}