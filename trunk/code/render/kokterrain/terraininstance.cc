//------------------------------------------------------------------------------
//  terraininstance.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/terraininstance.h"
#include "kokterrain/terrain.h"
#include "models/model.h"
#include "graphics/stage.h"
#include "graphics/cell.h"
#include "graphics/modelentity.h"
#include "models/modelnodetype.h"
#include "models/visresolver.h"

namespace KOK
{
ImplementClass(KOK::TerrainInstance, 'TNIE', Models::ModelInstance);

using namespace Models;
using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
TerrainInstance::TerrainInstance()
{
}

//------------------------------------------------------------------------------
/**
*/
TerrainInstance::~TerrainInstance()
{
}

void 
TerrainInstance::OnAttachToModel(const Ptr<Model>& model)
{
	this->model = model;
}

//------------------------------------------------------------------------------
/**
    
*/
void
TerrainInstance::Update()
{
	/*vector camPos = GraphicsServer::Instance()->GetDefaultView()->GetCameraEntity()->GetTransform().getrow3();
	if (camPos != savePos)
	{
		CheckDistrict(camPos);
		savePos = camPos;
	}*/
	// node̫�࣬�ȽϷ�ʱ�����Բ����£��õ�ʱ���ٸ���
	//ModelInstance::Update();
}

//------------------------------------------------------------------------------
/**
*/
void 
TerrainInstance::NotifyVisible(IndexT frameIndex)
{
	// ���¿ɼ�DIST
	const Util::Array<Ptr<Cell>>& visibleCells = this->GetModelEntity()->GetStage()->GetVisibleCell();
	if (visibleCells.Size() == 0)
		return;

	Ptr<Terrain> terr = this->model.downcast<Terrain>();
	for (SizeT i = 0; i < visibleCells.Size(); i++)
		terr->AddVisibleDistrict(visibleCells[i]->GetCellId());

	terr->UpdateRenderList(frameIndex);

	VisResolver::Instance()->AddVisibleModel(frameIndex, ModelNodeType::Solid, this->GetModel());
}

} // namespace Models
