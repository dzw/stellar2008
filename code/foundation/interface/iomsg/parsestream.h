#pragma once
#ifndef INTERFACE_PARSESTREAM_H
#define INTERFACE_PARSESTREAM_H
//------------------------------------------------------------------------------
/**
    @class Interface::ParseStream
    
    ������
	���߳����ݽ����ӿ�
    Ҫ�߳̽�������ʱʹ�ã��̳е�����ParseData�н����������ݡ�������������Ϳ���
	������foundation��ʵ��

    cTuo
*/
#include "interface/iomsg/iomessage.h"
#include "io/stream.h"

//------------------------------------------------------------------------------
namespace Interface
{
class ParseStream : public IOMessage
{
    DeclareClass(ParseStream);
    DeclareMsgId;
public:
	/// ����һ���Ѵ򿪵���
    virtual void ParseData(const Ptr<IO::Stream>& stream);
};

inline void 
ParseStream::ParseData(const Ptr<IO::Stream>& stream)
{
}

} // namespace Interface
//------------------------------------------------------------------------------
#endif
