#pragma once
#ifndef MODELS_MANAGEDCHUNK_H
#define MODELS_MANAGEDCHUNK_H
//------------------------------------------------------------------------------
/**
    简单的发送消息加载一个chunk，并返回加载的数据，与ManagedMapTile一样，只做加载
*/
#include "resources/managedresource.h"
#include "wow/world/terrain/chunk.h"

//------------------------------------------------------------------------------
namespace Models
{
class ManagedChunk : public Resources::ManagedResource
{
    DeclareClass(ManagedChunk);
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
