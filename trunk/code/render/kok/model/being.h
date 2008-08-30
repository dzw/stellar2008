#pragma once
#ifndef KOK_BEING_H
#define KOK_BEING_H
//------------------------------------------------------------------------------
/**
    @class KOK::Being

	�൱��we�� cMeshSerializer

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
	/// ���ڴ����/������Դ
	virtual bool SetupFromStream(const Ptr<IO::Stream>& stream);

	void SetComputeMeshRepeat(bool b);
	/// ��״��ʱ����Ҫ������������ģ���õ���ͬ��ŵ�����
	void LoadTexture(int texId);
private:

	/// �汾
	int meshVersion;
	/// �����ظ���mesh(�����õ�)
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

    