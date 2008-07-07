#pragma once
#ifndef WOW_OPENMAPTILE_H
#define WOW_OPENMAPTILE_H
//------------------------------------------------------------------------------
/**
    打开一个ADT文件的消息，然后再发送ChunkReadStream消息，从打开的ADT取数据
*/
#include "interface/iomsg/iomessage.h"

//------------------------------------------------------------------------------
namespace WOW
{
class OpenMapTile : public Interface::IOMessage
{
    DeclareClass(OpenMapTile);
    DeclareMsgId;
};

} // namespace Interface
//------------------------------------------------------------------------------
#endif
