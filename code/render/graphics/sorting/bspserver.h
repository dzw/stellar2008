#ifndef N_BSPSERVER_H
#define N_BSPSERVER_H
//------------------------------------------------------------------------------
/**
	@class BspServer

	���ܣ�
	1����ģ�Ͱ�BSP�ָδʵ��
	2������WMO��BSP��Ϣ


	Ҫʵ��BSP��ײ��Ҫ�����£�
	  ��ȡgroup�����֣���ȡ�ö�Ӧ��ShapeNode
	  ͨ�����ֵ�BspServerȡ��BspNode��һ���ڵ�

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
	/// ����Group���ֱ���BSP
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
