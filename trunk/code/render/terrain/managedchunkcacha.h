#pragma once
#ifndef TERRAIN_MANAGEDCHUNKCACHA_H
#define TERRAIN_MANAGEDCHUNKCACHA_H
//------------------------------------------------------------------------------
/**
    @class Terrain::ManagedWorld
    
	����������Ⱦ�õ���chunk�����ݻ��壨�綥�㣩��������Ⱦ��������ܸ����������
	�������������ʾ��chunk����������µ�chunkʱ��Ҫ����������л��п��ÿռ䣬
	��ֱ����ӣ�������Ҫ�ѿ�������chunk��ɾ���ˣ�����ӡ�

    (C) cTuo
*/
#include "resources/managedresource.h"
#include "terrain/world.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class ManagedChunkCacha : public Resources::ManagedResource
{
    DeclareClass(ManagedChunkCacha);
public:
    /// get contained model resource
    const Ptr<Model>& GetModel() const;

	void CreateCacha(const Resources::ResourceId& resId, SizeT chunkNum);
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Model>&
ManagedChunkCacha::GetModel() const
{
    return this->GetResource().downcast<Model>();
}

} // namespace Models
//------------------------------------------------------------------------------
#endif
