#pragma once
#ifndef MODELS_MANAGEDCHUNK_H
#define MODELS_MANAGEDCHUNK_H
//------------------------------------------------------------------------------
/**
    �򵥵ķ�����Ϣ����һ��chunk�������ؼ��ص����ݣ���ManagedMapTileһ����ֻ������
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
