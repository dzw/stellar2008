#pragma once
#ifndef WOW_OPENMAPTILE_H
#define WOW_OPENMAPTILE_H
//------------------------------------------------------------------------------
/**
    ��һ��ADT�ļ�����Ϣ��Ȼ���ٷ���ChunkReadStream��Ϣ���Ӵ򿪵�ADTȡ����
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
