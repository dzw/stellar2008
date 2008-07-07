#ifndef N_BSPSERVER_H
#define N_BSPSERVER_H
//------------------------------------------------------------------------------
/**
	@class BspServer

	功能：
	1、将模型按BSP分割，未实现
	2、保存WMO的BSP信息


	要实现BSP碰撞需要做的事：
	  获取group的名字，并取得对应的ShapeNode
	  通过名字到BspServer取得BspNode第一个节点

	(C) 2008 ctuo
*/
#include "core/singleton.h"
#include "graphics/sorting/wmobspnode.h"
#include "resources/resourceid.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class BspServer : public Core::RefCounted
{
	DeclareClass(BspServer);
	DeclareSingleton(BspServer);
public:
	/// constructor
	BspServer();
	/// destructor
	~BspServer();

	/// open the server
	void Open();
	/// close the server
	void Close();
	/// return true if open
	bool IsOpen() const;

	/// build bsp tree
	void AddWMOBsp(const Resources::ResourceId& name, Ptr<BspNode>& bsp);
	
private:
	/// 根据Group名字保存BSP
	Util::Dictionary<Resources::ResourceId, Ptr<BspNode>> wmoBsps;
	bool isOpen;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
BspServer::IsOpen() const
{
	return this->isOpen;
}

}
//------------------------------------------------------------------------------
#endif
