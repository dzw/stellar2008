#pragma once
#ifndef GRAPHICS_STATICQUATREESTAGEBUILDER_H
#define GRAPHICS_STATICQUATREESTAGEBUILDER_H
//------------------------------------------------------------------------------
/**
    @class Graphics::StaticQuadtreeStageBuilder
    
	��̬�Ĳ���

    (C) 2008 cTuo
*/
#include "graphics/stagebuilder.h"
#include "util/quadtree.h"
#include "attr/attribute.h"
#include "graphics/cell.h"
#include "foundation/math/bbox.h"

//------------------------------------------------------------------------------
namespace Attr
{
    DeclareInt(QuadTreeStageBuilderDepth, 'QTBD', Attr::ReadOnly);
    DeclareFloat4(QuadTreeStageBuilderCenter, 'QTBC', Attr::ReadOnly);
    DeclareFloat4(QuadTreeStageBuilderExtents, 'QTBE', Attr::ReadOnly);
};
//------------------------------------------------------------------------------
namespace Graphics
{
class StaticQuadtreeStageBuilder : public StageBuilder
{
    DeclareClass(StaticQuadtreeStageBuilder);
public:
    /// constructor
    StaticQuadtreeStageBuilder();
    /// destructor
    virtual ~StaticQuadtreeStageBuilder();
	/// ��ʼ���Ĳ���
	void InitQuadtree(const Math::bbox& box, int level);
	/// ����Ҷ�ӽڵ�boundbox
	void SetLeafNodeBoundingBox(int row, int col, const Math::bbox& b);
	///
	void AttachToCell(const Ptr<Graphics::Cell>& cell);
    /// �����ڵ㣬���뵽stage
    virtual void BuildStage(const Ptr<Stage>& stage);
    /// set quad tree depth and boudning box
    void SetQuadTreeSettings(uchar depth, const Math::bbox& worldBBox);
	void UpdateBoundingBox();
	void ClearBoundingBox();

	const Math::bbox& GetRootBoundingBox()const;
private:
    /// create a quad tree and its children, recursively
    Graphics::Cell* CreateQuadTreeCell(Graphics::Cell* parentCell, uchar curLevel, ushort curCol, ushort curRow);

    int numCellsBuilt;
    Util::QuadTree<Cell> quadTree;
	Ptr<Graphics::Cell> rootCell;
};

inline const Math::bbox& 
StaticQuadtreeStageBuilder::GetRootBoundingBox()const
{
	return this->rootCell->GetBoundingBox();
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
    