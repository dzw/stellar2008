#pragma once
#ifndef TERRAIN_TERRAINNODEINSTANCE_H
#define TERRAIN_TERRAINNODEINSTANCE_H
//------------------------------------------------------------------------------
/**
    ��Ⱦһ�����ο飬�����������Ĳ����������TerrainChunkNode��ȡ�ÿ��л���
	����䶥�����ݡ�

    (C)  cTuo
*/
#include "models/nodes/statenodeinstance.h"

//------------------------------------------------------------------------------
namespace Terrain
{
class TerrainNodeInstance : public Models::StateNodeInstance
{
    DeclareClass(TerrainNodeInstance);
public:
    /// constructor
    TerrainNodeInstance();
    /// destructor
    virtual ~TerrainNodeInstance();

	/// perform rendering
	virtual void Render();
protected:
	/// notify that we are visible
	virtual void OnNotifyVisible(IndexT frameIndex);
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
