#pragma once
#ifndef INTERFACE_PARSESTREAM_H
#define INTERFACE_PARSESTREAM_H
//------------------------------------------------------------------------------
/**
    @class Interface::ParseStream
    
    抽象类
	多线程数据解析接口
    要线程解析数据时使用，继承的类在ParseData中解析具体数据。这样解析的类就可以
	不用在foundation层实现

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
	/// 传入一个已打开的流
    virtual void ParseData(const Ptr<IO::Stream>& stream);
};

inline void 
ParseStream::ParseData(const Ptr<IO::Stream>& stream)
{
}

} // namespace Interface
//------------------------------------------------------------------------------
#endif
