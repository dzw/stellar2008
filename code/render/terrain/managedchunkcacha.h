#pragma once
#ifndef TERRAIN_MANAGEDCHUNKCACHA_H
#define TERRAIN_MANAGEDCHUNKCACHA_H
//------------------------------------------------------------------------------
/**
    @class Terrain::ManagedWorld
    
	管理所有渲染用地形chunk的数据缓冲（如顶点），用于渲染，这里可能根据摄像机离
	地面距离设置显示的chunk数量，添加新的chunk时需要，如果缓冲中还有可用空间，
	就直接添加，否则需要把看不见的chunk都删除了，再添加。

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
