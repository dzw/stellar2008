//------------------------------------------------------------------------------
//  terraininstance.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/terraininstance.h"
#include "kokterrain/terrainserver.h"
//#include "coregraphics/vertexcomponent.h"
#include "models/visresolver.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"
#include "graphics/worldstage.h"
#include "graphics/terraincell.h"
#include "graphics/cell.h"
#include "coregraphics/debugview.h"

namespace KOK
{
ImplementClass(KOK::TerrainInstance, 'TNIE', Models::ModelInstance);

using namespace Util;
using namespace CoreGraphics;
using namespace Resources;
using namespace Math;
using namespace Models;
using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
TerrainInstance::TerrainInstance():
	terrain(0),
	centerDist(0)
{
	//this->distMeshPool = TerrainServer::Instance()->GetTerrainMeshPool();
}

//------------------------------------------------------------------------------
/**
*/
TerrainInstance::~TerrainInstance()
{
	//this->distMeshPool = 0;
}

void 
TerrainInstance::OnAttachToModel(const Ptr<Model>& model)
{
	ModelInstance::OnAttachToModel(model);

	this->terrain = this->model.downcast<Terrain>();

	const Ptr<WorldStage>& stage = GraphicsServer::Instance()->GetDefaultView()->GetStage().downcast<WorldStage>();
	//stage->SetTerrain(this);

	Ptr<Cell> cell = stage->GetRootCell();
	// 加入到四叉树
	for (IndexT i = 0; i < this->nodeInstances.Size(); i++)
	{
		Ptr<Cell> c = cell->FindEntityContainmentCell(this->nodeInstances[i]->GetModelNode()->GetBoundingBox());
		c.downcast<TerrainCell>()->SetDistrict(this->nodeInstances[i].downcast<DistrictNodeInstance>());
	}
}

//------------------------------------------------------------------------------
/**
    
*/
void
TerrainInstance::Update()
{
	this->renderList.Clear();

	/*vector camPos = GraphicsServer::Instance()->GetDefaultView()->GetCameraEntity()->GetTransform().getrow3();
	if (camPos != savePos)
	{
		CheckDistrict(camPos);
		savePos = camPos;
	}*/
	// node太多，比较费时，所以不更新，用的时候再更新
	//ModelInstance::Update();
}

//------------------------------------------------------------------------------
/**
	设置可见节点用于渲染
*/
void 
TerrainInstance::NotifyVisible(IndexT frameIndex)
{
	DWORD num = this->renderList.Size();
	for (IndexT i = 0; i < num; i++)
	{
		this->renderList[i]->OnNotifyVisible(frameIndex);
	}
	//VisResolver::Instance()->AddVisibleModel(frameIndex, ModelNodeType::Solid, this->model);
	this->renderList1.Clear();
	this->renderList1 = this->renderList;

	for (IndexT i = 0; i < 4; i++)
	{
		renderLayer[i].Clear();
	}

	SizeT texNum = this->terrain->GetTextureCount();
	//this->texRender.Clear();
	//this->texRender.Reserve(texNum);
	////for (SizeT i = 0; i < texNum; i++)
	//	texRender[i] = false;

	// 找出第j层所有drawTable
	for (IndexT j = 0; j < 4; j++)
	{
		// 每个DIST的相应层取出来
		for (IndexT i = 0; i < num; i++)
		{
			// 每个DIST上都有不直4张纹理，所以把所有的都取出来，这个WOW不一样
			for (IndexT n = 0; n < texNum; n++)
			{
				if (renderList[i]->IsRender(j*texNum+n))
				{
					// 排序
					DrawTile d;
					d.dist = renderList[i];
					d.tex = n;
					d.pass = j*texNum+n;

					// 根据纹理、距离排序，每层都相要这两项，和WOW又不一样，WOW中混合层只要根据纹理排序就可以
					renderLayer[j].InsertSorted(d);
					//////////////////////////////////////////////////////////////////////////

					//texRender[n] = true;
				}
			}
		}
	}

	// 更新缓冲


#ifdef NEBULA3_DEBUG
	DebugView::Instance()->AddDebugString("dist", num);
#endif
}

void
TerrainInstance::AppendRenderDist(const Ptr<DistrictNodeInstance>& d)
{
	this->renderList.Append(d);
}

//------------------------------------------------------------------------------
/**
*/
//void 
//TerrainInstance::RenderDebug()
//{
//    IndexT i;
//    SizeT num = this->nodeInstances.Size();
//    for (i = 0; i < num; i++)
//    {
//        this->nodeInstances[i]->RenderDebug();
//    }
//}

//------------------------------------------------------------------------------
/**
	释放不用的顶点缓冲
*/
void
TerrainInstance::AllocVertexPool(const Ptr<DistrictNodeInstance>& d)
{
	if (this->vertexStart == -1)
	{
		if (this->meshPool.isvalid() && this->meshPool->Full())
		{
			for (IndexT i = 0; i < this->nodeInstances.Size(); i++)
			{
				const Ptr<DistrictNodeInstance>& dist = this->nodeInstances[i].downcast<DistrictNodeInstance>();
				if (dist->GetVertexStart() != -1 && !dist->IsVisiable())
				{
					dist->SetVertexStart(-1);
				}
			}
		}

		DWORD vertexStart = TerrainServer::Instance()->GetVertexChunkPool()->Alloc(NULL);
		n_assert(vertexStart != -1);
		d->SetVertexStart(vertexStart);
	}
}

// district
bool
TerrainInstance::CheckValidDistrict(int x, int z)
{
	return x>=0 && z >= 0 && x< this->terrain->GetMapWide() && z < this->terrain->GetMapWide();
}

void
TerrainInstance::CheckDistrict(const vector& pos)
{
	int distSize = this->terrain->GetDistrictSize();
	int offset = this->terrain->GetTilePosOffset();
	int distX = (pos.x()/*-offset*/) / distSize;
	int distZ = (pos.z()/*-offset*/) / distSize;

	if (!this->centerDist.isvalid())
	{
		EnterDistrict(distX, distZ);
	}
	else
	{
		int x = this->centerDist->GetX() * distSize;
		int z = this->centerDist->GetZ() * distSize;

		if (pos.x() < x || (pos.x() > (x + distSize)) || 
			pos.z() < z || (pos.z() > (z + distSize)))
		{
			EnterDistrict(distX, distZ);
		}
	}
}

void
TerrainInstance::EnterDistrict(int x, int z)
{
	if (!CheckValidDistrict(x, z))
		return;

	int half = DISTRICTCACHESIZE / 2;
	this->curX = x;
	this->curZ = z;
	for (SizeT i = 0; i < DISTRICTCACHESIZE; i++)
	{
		for (SizeT j = 0; j < DISTRICTCACHESIZE; j++)
		{
			if (i == half && j == half)	// 中心位置
				this->centerDist = LoadDistrict(x-half+i, z-half+j);
			else
				LoadDistrict(x-half+i, z-half+j);
		}
	}
}

Ptr<DistrictNodeInstance>
TerrainInstance::LoadDistrict(int x, int z)
{
	Ptr<DistrictNodeInstance> newNode;
	if (!CheckValidDistrict(x, z))
		return newNode;

	//Array<Ptr<ModelNode>> dists =  this->terrain->GetNodes();
	// 限制加载数量
	IndexT firstNull = this->nodeInstances.Size();
	for (IndexT i = 0; i < this->nodeInstances.Size(); i++)
	{
		if (this->nodeInstances[i].isvalid())
		{
			Ptr<DistrictNodeInstance> dist = this->nodeInstances[i].downcast<DistrictNodeInstance>();
			if (dist->GetX() == x && dist->GetZ() == z)
				return dist;
		}
		else
		{
			if (i < firstNull)
				firstNull = i;
		}
	}
	if (firstNull == this->nodeInstances.Size())
	{
		int score, maxscore = 0, maxidx = 0; 
		for (IndexT i = 0; i < this->nodeInstances.Size(); i++)
		{
			Ptr<DistrictNodeInstance> dist = this->nodeInstances[i].downcast<DistrictNodeInstance>();
			int X = dist->GetX();
			int Z = dist->GetZ();

			score = int(n_abs(X - this->curX) + n_abs(Z - this->curZ));
			if (score > maxscore)
			{
				maxscore = score;
				maxidx = i;
			}
		}
		firstNull = maxidx;

		// 释放
		//this->terrain->RemoveNode(distNodes[maxidx]);
		this->nodeInstances[maxidx].downcast<DistrictNodeInstance>()->Clear();
	}

	// 加载新的
	/*for (SizeT i = 0; i < this->nodeInstances.Size(); i++)
	{
		Ptr<DistrictNodeInstance> dist = this->nodeInstances[i].downcast<DistrictNodeInstance>();
		if (dist->GetX() == x && dist->GetZ() == z)
			return dist;
	}*/
	Ptr<DistrictNode> dist = this->terrain->CreateNewDistrict(x, z);
	this->nodeInstances[firstNull].downcast<DistrictNodeInstance>()->Reset(dist);

	/*Ptr<ModelNodeInstance> parentNodeInst;
	Ptr<ModelNodeInstance> nodeInstance = dist->CreateNodeInstance();
	nodeInstance->OnAttachToModelInstance(this, dist.upcast<ModelNode>(), parentNodeInst);
	this->nodeInstances.Append(nodeInstance);*/

	return newNode;
}

} // namespace Models
