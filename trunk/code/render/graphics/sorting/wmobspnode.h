#ifndef N_WMOBSPNODE_H
#define N_WMOBSPNODE_H
//------------------------------------------------------------------------------
/**
	@class WMOBspNode

	wmo bsp node

	(C) 2008 ctuo
*/
#include "graphics/sorting/bspnode.h"
#include "util/array.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class WMOBspNode : public BspNode
{
	DeclareClass(WMOBspNode);
public:
	/// constructor
	WMOBspNode();
	/// destructor
	~WMOBspNode();
	///
	void AddNodeData(Util::Array<unsigned short>& ib);

private:
	Util::Array<unsigned short> indexBuffer;
};

}
#endif