#pragma once
#ifndef KOK_BEING_H
#define KOK_BEING_H
//------------------------------------------------------------------------------
/**
    @class KOK::Being

	相当于we的 cMeshSerializer

    (C) 2008 cTuo
*/
#include "models/model.h"
#include "io/stream.h"

//------------------------------------------------------------------------------
namespace KOK
{

class Being : public Models::Model
{
    DeclareClass(Being);
public:
    /// constructor
    Being();
    /// destructor
    virtual ~Being();

    /// unload the resource, or cancel the pending load
    virtual void Unload();
	/// 从内存加载/解析资源
	virtual bool SetupFromStream(const Ptr<IO::Stream>& stream);

	void SetComputeMeshRepeat(bool b);
	/// 换状的时候需要换纹理，所有子模型用的相同编号的纹理
	void LoadTexture(int texId);
private:

	/// 版本
	int meshVersion;
	/// 计算重复的mesh(悬崖用到)
	bool computeMeshRepeat;
};

inline void 
Being::SetComputeMeshRepeat(bool b)
{
	computeMeshRepeat = b;
}

} // namespace Models
//------------------------------------------------------------------------------
#endif

    