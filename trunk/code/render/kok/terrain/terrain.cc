//------------------------------------------------------------------------------
//  terrain.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/terrain/terrain.h"
#include "kok/terrain/districtnode.h"
#include "models/attributes.h"

#include "models/modelnodetype.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/shaderinstance.h"
#include "lighting/lightserver.h"
#include "graphics/modelentity.h"
#include "resources/resourcemanager.h"


#include "coregraphics/vertexchunkpool.h"
#include "kok/terrain/terrainentity.h"
#include "kok/terrain/terraininstance.h"

#include "graphics/staticquadtreestagebuilder.h"

#include "coregraphics/renderdevice.h"
#include "coregraphics/debugview.h"

#include "graphics/stage.h"

namespace KOK
{
ImplementClass(KOK::Terrain, 'TERN', Models::Model);

using namespace Util;
using namespace Resources;
using namespace Models;
using namespace CoreGraphics;
using namespace Graphics;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
Terrain::Terrain():
	tileMeshScale(1),
	tilePosOffset(0.0),
	meshData(0),
	thingTex(0)
{
	this->terrRender = TerrainRender::Create();
	this->terrRender->Init();
}

//------------------------------------------------------------------------------
/**
*/
Terrain::~Terrain()
{
	this->terrMeshGrid = 0;
	this->cliffTable = 0;
	this->terrRender = 0;

	if (this->thingTex)
	{
		for (SizeT i = 0; i < this->terrInfo.GetTotalTileCount(); i++)
			n_delete(this->thingTex[i]);
		n_delete_array(this->thingTex);
	}

	if (this->meshData)
	{
		for (SizeT i = 0; i < this->terrInfo.GetTotalTileCount(); i++)
			n_delete(this->meshData[i]);
		n_delete_array(this->meshData);
	}
}

Ptr<ModelInstance> 
Terrain::CreateInstance()
{
	Ptr<ModelInstance> modelInstance = (ModelInstance*)TerrainInstance::Create();
	this->AttachInstance(modelInstance);
	return modelInstance;
}

//------------------------------------------------------------------------------
/**
*/
void
Terrain::Unload()
{    
	ResourceManager* resManager = ResourceManager::Instance();
	IndexT i;
	for (i = 0; i < this->textures.Size(); i++)
	{
		resManager->DiscardManagedResource(this->textures[i].upcast<ManagedResource>());
	}
	this->textures.Clear();

	// call parent class
    Model::Unload();
}

//------------------------------------------------------------------------------
/**
*/
void 
Terrain::ComputeTileMesh()
{
	float size = (float)(COMP * this->tileMeshScale);
	TerrainMeshData* pMeshData;
	int iIndex = 0;
	float shadowOffset = 1.0f;
	float shadowSize = DISTRICT_VERTS * size * 8.0f;
	float errorShadowUV = shadowOffset / 512.0f;
	float uvScale = (512.0f - 2.0f*shadowOffset) / (shadowSize * 512.0f);
	float uvOffsetX, uvOffsetZ;

	//this->tilePosOffset = 0;

	// 阴影为8X8个District使用一张贴图，贴图轴内缩shadowOffset个pixel值
	for (SizeT j = 0; j < this->terrInfo.GetTileCountX(); j++)
	{
		uvOffsetZ = (j/64) * shadowSize;
		for (SizeT i = 0; i < this->terrInfo.GetTileCountX(); i++)
		{
			uvOffsetX = (i/64) * shadowSize;

			iIndex = i + j * this->terrInfo.GetTileCountX();
			pMeshData = this->meshData[iIndex];

			pMeshData->meshData[0].p.set(i * size - this->tilePosOffset,
				this->terrMeshGrid->CalcTileHeightWithCliffWater(i, j),
				j * size - this->tilePosOffset);
			pMeshData->meshData[0].n = this->terrMeshGrid->GetGridNormal(i, j);
			pMeshData->byWaterLevel[0] = this->terrMeshGrid->GetGridWaterLevel(i, j);
			pMeshData->meshData[0].t5.set((pMeshData->meshData[0].p.x + this->tilePosOffset - uvOffsetX) * uvScale + errorShadowUV,
				(pMeshData->meshData[0].p.z + this->tilePosOffset - uvOffsetZ) * uvScale + errorShadowUV);
			this->terrMeshGrid->GetGridPosition(i, j) = pMeshData->meshData[0].p;

			pMeshData->meshData[1].p.set(i * size - this->tilePosOffset, 
				this->terrMeshGrid->CalcTileHeightWithCliffWater(i, j+1),
										(j+1) * size - this->tilePosOffset);
			pMeshData->meshData[1].t5.set((pMeshData->meshData[1].p.x + this->tilePosOffset - uvOffsetX) * uvScale + errorShadowUV,
				(pMeshData->meshData[1].p.z + this->tilePosOffset - uvOffsetZ) * uvScale + errorShadowUV);
			if (j == this->terrInfo.GetTileCountX() - 1)
				this->terrMeshGrid->GetGridPosition(i, j+1) = pMeshData->meshData[1].p;


			pMeshData->meshData[2].p.set((i+1) * size - this->tilePosOffset,
				this->terrMeshGrid->CalcTileHeightWithCliffWater(i+1, j),
										 j * size - this->tilePosOffset);
			pMeshData->meshData[2].n = this->terrMeshGrid->GetGridNormal(i+1, j);
			pMeshData->byWaterLevel[2] = this->terrMeshGrid->GetGridWaterLevel(i+1, j);
			pMeshData->meshData[2].t5.set((pMeshData->meshData[2].p.x + this->tilePosOffset - uvOffsetX) * uvScale + errorShadowUV,
				(pMeshData->meshData[2].p.z + this->tilePosOffset - uvOffsetZ) * uvScale + errorShadowUV);
			if (i == (this->terrInfo.GetTileCountX() - 1))
				this->terrMeshGrid->GetGridPosition(i+1, j) = pMeshData->meshData[2].p;

			pMeshData->meshData[3].p.set((i+1) * size - this->tilePosOffset,
				this->terrMeshGrid->CalcTileHeightWithCliffWater(i+1, j+1),
				(j +1) * size - this->tilePosOffset);
			pMeshData->meshData[3].n = this->terrMeshGrid->GetGridNormal(i+1, j+1);
			pMeshData->byWaterLevel[3] = this->terrMeshGrid->GetGridWaterLevel(i+1, j+1);
			pMeshData->meshData[3].t5.set((pMeshData->meshData[3].p.x + this->tilePosOffset - uvOffsetX) * uvScale + errorShadowUV,
				(pMeshData->meshData[3].p.z + this->tilePosOffset - uvOffsetZ) * uvScale + errorShadowUV);
			if (i == (this->terrInfo.GetTileCountX() - 1) && j == (this->terrInfo.GetTileCountX() - 1))
				this->terrMeshGrid->GetGridPosition(i+1, j+1) = pMeshData->meshData[3].p;


			UpdateTileTex(i, j);
		}
	}
}

void
Terrain::UpdateTileTex(int iX, int iY)
{
	if (iX < 0 || iX >= (int)this->terrInfo.GetTileCountX() ||
		iY < 0 || iY >= (int)this->terrInfo.GetTileCountX())
	{
		return;
	}

	if (this->meshData == NULL)
		return;

	int iIndex = iX + iY * this->terrInfo.GetTileCountX();
	DWORD dwTexData = this->thingTex[iIndex]->dTile;
	TerrainMeshData* pMeshData = this->meshData[iIndex];
	stDWORD cellInfo;
	BYTE byTexture;
	BYTE byID;

	// 首先取出底图的 Pattern Index
	byTexture = (BYTE)( ( dwTexData >> 8 ) & 0xF );
	// 接着取出底图的 Texture Index 0~17
	byID = (BYTE)( dwTexData & 0xFF );
	pMeshData->SetTextureData( byTexture, byID, 0 );

	cellInfo.dwDWORD = this->thingTex[iIndex]->stTile.dwDWORD;

	for( int i = 0; i < 3; i++ )
	{
		byTexture = ( cellInfo.bBYTE[i] >> 4 ) & 0xF;
		if( byTexture )
		{
			byID = cellInfo.bBYTE[i] & 0xF;
			pMeshData->SetTextureData( byTexture, byID, i + 1 );
		}
	}
}



void 
Terrain::SetMapSize(SizeT mapSize)
{
	this->terrInfo.SetTileCountX(mapSize);

	this->tilePosOffset = ((this->tileMeshScale - 1.0f) * COMP * this->terrInfo.GetTileCountX()) * 0.5f;

	this->thingTex = n_new_array(ThingTex*, this->terrInfo.GetTotalTileCount());
	n_assert(this->thingTex != NULL);
	for (SizeT i = 0; i < this->terrInfo.GetTotalTileCount(); i++)
	{
		this->thingTex[i] = n_new(ThingTex);
		n_assert(this->thingTex[i]);
	}

	if (!this->terrMeshGrid.isvalid())
		this->terrMeshGrid = TerrainMeshGrid::Create();
	this->terrMeshGrid->Init(this->terrInfo.GetGridCountX(), this->terrInfo.GetTotalGridCount());

	this->meshData = n_new_array(TerrainMeshData*, this->terrInfo.GetTotalTileCount());
	if (this->meshData)
	{
		for (SizeT i = 0; i < this->terrInfo.GetTotalTileCount(); i++)
		{
			this->meshData[i] = n_new(TerrainMeshData);
		}
	}

	//bbox box;
	//vector min, max;
	//min.set(-tilePosOffset, -5000.0f, -tilePosOffset);
	//max.set(scalar(GetMapWide() * GetTileSize() - tilePosOffset), 5000.0f, scalar(GetMapWide() * GetTileSize() - tilePosOffset));
	//box.pmin = min;
	//box.pmax = max;
	//this->SetBoundingBox(box);
}

Ptr<DistrictNode>
Terrain::CreateNewDistrict(int x, int z)
{
	Ptr<DistrictNode> newNode;
	String name;
	name.Format("dist_%2d_%2d", x,z);

	if (this->HasNode(name))
	{
		newNode = this->LookupNode(name).downcast<DistrictNode>();
		return newNode;
	}

	newNode = DistrictNode::Create();
	newNode->SetName(Resources::ResourceId(name));
	//newNode->SetString(Attr::Shader, "shd:kokterrain");
	//newNode->SetString(Attr::DiffMap0, "mapdata\\border0100.dds");
	newNode->SetPosition(x, z);
	this->AttachNode(newNode.upcast<Models::ModelNode>());
	//newNode->LoadResources();
	newNode->CreateMeshData();

	return newNode;
}

//------------------------------------------------------------------------------
/**
	创建四叉树
*/
void
Terrain::CreateQuadTree(const Ptr<Cell>& root)
{
	int num = this->terrInfo.GetDistrictCountX();

	Ptr<StaticQuadtreeStageBuilder> quadTreeStageBuilder = StaticQuadtreeStageBuilder::Create();    
	bbox levelBox;
	/*levelBox.pmin = point(-100000, -100000, -100000);
	levelBox.pmax = point(-10000, -10000, -10000);*/
	levelBox.pmin = point(N_MAXREAL, N_MAXREAL, N_MAXREAL);
	levelBox.pmax = point(N_MINREAL, N_MINREAL, N_MINREAL);
	quadTreeStageBuilder->InitQuadtree(levelBox, 6);
	quadTreeStageBuilder->ClearBoundingBox();

	for (SizeT i = 0; i < this->nodes.Size(); i++)
	{
		int col = i / num;
		int row = i % num;
		const bbox& b = this->nodes[i]->GetBoundingBox();
		quadTreeStageBuilder->SetLeafNodeBoundingBox(row, col, b);
	}

	quadTreeStageBuilder->UpdateBoundingBox();
	quadTreeStageBuilder->AttachToCell(root);

	// 设置cellId
	for (SizeT i = 0; i < this->nodes.Size(); i++)
	{
		Ptr<Cell> c = root->FindEntityContainmentCell(this->nodes[i]->GetBoundingBox());
		if (c.isvalid())
			c->SetCellId(i);
	}

	this->SetBoundingBox(quadTreeStageBuilder->GetRootBoundingBox());
}

void 
Terrain::AppendTexture(const String& resId)
{
	String name;
	name.Format("mapdata\\%s.dds", resId.AsCharPtr());
	Ptr<ManagedTexture> managedTexture = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, name).downcast<ManagedTexture>();
	this->textures.Append(managedTexture);
}

void 
Terrain::AddVisibleDistrict(IndexT id)
{
	if(id < 0 && id >= this->nodes.Size())
		return;
	
	this->renderList.Append(this->nodes[id].downcast<DistrictNode>());
}

void 
Terrain::UpdateRenderList(IndexT frameIndex)
{
	this->frameIndex = frameIndex;

	// 排序
	IndexT num = this->renderList.Size();
	SizeT texNum = this->textures.Size();

	
	// 找出第j层所有drawTable
	for (IndexT j = 0; j < 4; j++)
	{
		this->renderLayer[j].Clear();
		// 每个DIST的相应层取出来
		for (IndexT i = 0; i < num; i++)
		{
			// 每个DIST上都有不直4张纹理，所以把所有的都取出来，这个WOW不一样
			for (IndexT n = 0; n < texNum; n++)
			{
				if (renderList[i]->IsRender(j*texNum+n))
				{
					DrawTile d;
					d.dist = renderList[i];
					d.tex = n;
					d.pass = j*texNum+n;

					// 根据纹理、距离排序，每层都相要这两项，和WOW又不一样，WOW中混合层只要根据纹理排序就可以
					renderLayer[j].InsertSorted(d);
				}
			}
		}
	}

	for (IndexT i = 0; i < num; i++)
	{
		renderList[i]->NotifyVisible(frameIndex);
	}

	renderList.Clear();

	DebugView::Instance()->AddDebugString("dist", num);
}

//------------------------------------------------------------------------------
/**
	释放不用的顶点缓冲块
*/
void
Terrain::UpdateVertexPool()
{
	// vertex buffer
	const Ptr<VertexChunkPool>& pool = TerrainEntity::Instance()->GetVertexChunkPool();

	for (IndexT i = 0; i < this->nodes.Size(); i++)
	{
		const Ptr<DistrictNode>& dist = this->nodes[i].downcast<DistrictNode>();
		if (dist->GetVertexStart() != -1 && dist->GetFrameIndex() != this->frameIndex)
		{
			pool->Free(dist->GetVertexStart());
			dist->SetVertexStart(-1);
		}
	}
	
}


//------------------------------------------------------------------------------
/**
根据纹理不同渲染
*/
void 
Terrain::Render(const ModelNodeType::Code& nodeFilter, const Frame::LightingMode::Code& lightingMode, CoreGraphics::ShaderFeature::Mask& shaderFeatures)
{
	String mask;
	//const Array<Ptr<ModelNode> >& modelNodes = this->GetVisibleModelNodes(nodeFilter);
	
	//if (!terrRender->ApplySharedState(textures[0]))
	//	return;

	ShaderServer* shaderServer = ShaderServer::Instance();
	ShaderServer::Instance()->SetActiveShaderInstance(this->terrRender->GetShaderInstance());
	const Ptr<ShaderInstance>& shaderInst = shaderServer->GetActiveShaderInstance();

	//const Ptr<TerrainInstance>& terrInstance = this->instances[0].downcast<TerrainInstance>();
	//////////////////////////////////////////////////////////////////////////
	//  排序渲染@1  shader-texture-distance 方式
	//
	Ptr<VertexChunkPool> chunkPool = TerrainEntity::Instance()->GetVertexChunkPool();
	Ptr<IndexBufferPool> indexPool = TerrainEntity::Instance()->GetIndexBufferPool();
	RenderDevice::Instance()->SetVertexBuffer(chunkPool->GetBuffer());
	RenderDevice::Instance()->SetIndexBuffer(indexPool->GetBuffer());

	terrRender->ApplyTransform();
	for (SizeT i = 0; i < 4; i++)
	{
		mask.Format("Solid|KOK%d", i+1);
		shaderInst->SelectActiveVariation(shaderServer->FeatureStringToMask(mask));
		//if (LightingMode::None == this->lightingMode)
		{
			SizeT numPasses = shaderInst->Begin();
			n_assert(1 == numPasses);
			shaderInst->BeginPass(0);
		}

		const Array<DrawTile>& drawList = renderLayer[i];//terrInstance->GetDrawList(i);
		for (SizeT j = 0; j < drawList.Size(); j++)
		{
			if (!terrRender->ApplySharedState(textures[drawList[j].tex]))
				continue;
			
			if (drawList[j].dist->SetRenderGroup(drawList[j].pass, drawList[j].tex))
			{
				//drawList[j].dist->ApplyState();
				shaderInst->Commit();
				drawList[j].dist->Render();
				
			}
		}
		
		{
			shaderInst->EndPass();
			shaderInst->End();
		}
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 按纹理排序渲染
	//const Util::Array<Ptr<DistrictNodeInstance>>& distInstances = terrInstance->GetRenderList();
	//const Util::Array<bool>& checks = terrInstance->GetTextureRenderCheck();
	//int texNum = textures.Size();
	//for (SizeT i = 0; i < texNum; i++)
	//{
	//	// 设置纹理到GPU
	//	if (!checks[i] || !terrRender->ApplySharedState(textures[i]))
	//		continue;

	//	for (SizeT j = 0; j < distInstances.Size(); j++)
	//	{
	//		for (int layer = 0; layer < 4; layer++)
	//		{
	//			int renderPass = texNum * layer + i;

	//			// render the node instance
	//			if (distInstances[j]->SetRenderGroup(renderPass, i))
	//			{
	//				mask.Format("Solid|KOK%d", layer+1);
	//				shaderInst->SelectActiveVariation(shaderServer->FeatureStringToMask(mask));
	//				//if (LightingMode::None == this->lightingMode)
	//				{
	//					SizeT numPasses = shaderInst->Begin();
	//					shaderInst->BeginPass(0);
	//				}

	//				distInstances[j]->ApplyState();
	//				shaderInst->Commit();
	//				distInstances[j]->Render();

	//				{
	//					shaderInst->EndPass();
	//					shaderInst->End();
	//				}
	//			}

	//		}
	//	}
	//}
	//////////////////////////////////////////////////////////////////////////

	////for (SizeT layer = 0; layer < 4; layer++)
	//{
	//	for (SizeT j = 0; j < textures.Size(); j++)
	//	{
	//		// 设置纹理到GPU
	//		if (!terrRender->ApplySharedState(textures[j]))
	//			continue;

	//		// if lighting mode is Off, we can render all node instances with the same shader

	//		IndexT modelNodeIndex;  
	//		for (modelNodeIndex = 0; modelNodeIndex < modelNodes.Size(); modelNodeIndex++)
	//		{
	//			// render instances
	//			const Array<Ptr<ModelNodeInstance> >& nodeInstances = modelNodes[modelNodeIndex]->GetVisibleModelNodeInstances(nodeFilter);
	//			IndexT nodeInstIndex;
	//			for (nodeInstIndex = 0; nodeInstIndex < nodeInstances.Size(); nodeInstIndex++)
	//			{
	//				// 渲染每个DIST相应的层
	//				const Ptr<DistrictNodeInstance>& nodeInstance = nodeInstances[nodeInstIndex].downcast<DistrictNodeInstance>();

	//				// if single-pass lighting is enabled, we need to setup the lighting 
	//				// shader states
	//				// FIXME: This may set a new shader variation for every node instance
	//				// which is expensive! Would be better to sort node instances by number
	//				// of active lights!!!

	//				//if (LightingMode::SinglePass == this->lightingMode)
	//				//{
	//				//	// setup lighting render states
	//				//	// NOTE: this may change the shader feature bit mask which may select
	//				//	// a different shader variation per entity
	//				//	const Ptr<ModelEntity>& modelEntity = nodeInstance->GetModelInstance()->GetModelEntity();
	//				//	lightServer->ApplyModelEntityLights(modelEntity);
	//				//	shaderInst->SelectActiveVariation(shaderServer->GetFeatureBits());
	//				//	SizeT numPasses = shaderInst->Begin();
	//				//	n_assert(1 == numPasses);
	//				//	shaderInst->BeginPass(0);
	//				//}

	//				for (int layer = 0; layer < 4; layer++)
	//				{
	//					int renderPass = textures.Size() * layer + j;

	//					// render the node instance
	//					if (nodeInstance->SetRenderGroup(renderPass, j))
	//					{
	//						mask.Format("Solid|KOK%d", layer+1);
	//						shaderInst->SelectActiveVariation(shaderServer->FeatureStringToMask(mask));
	//						//if (LightingMode::None == this->lightingMode)
	//						{
	//							SizeT numPasses = shaderInst->Begin();
	//							n_assert(1 == numPasses);
	//							shaderInst->BeginPass(0);
	//						}

	//						nodeInstance->ApplyState();
	//						shaderInst->Commit();
	//						nodeInstance->Render();
	//						
	//						{
	//							shaderInst->EndPass();
	//							shaderInst->End();
	//						}
	//					}

	//				}
	//				/*if (LightingMode::SinglePass == this->lightingMode)
	//				{
	//				shaderInst->EndPass();
	//				shaderInst->End();
	//				}*/
	//			}
	//		}
	//	}

	//	//if (LightingMode::None == this->lightingMode)
	//						
	//}
}

void 
Terrain::AttachThingsToStage(const Ptr<Graphics::Stage>& stage)
{
	if (!stage.isvalid())
		return;

	for (SizeT i = 0; i < this->thingEntitys.Size(); i++)
		stage->AttachEntity(this->thingEntitys[i].upcast<GraphicsEntity>());
}

} // namespace Models
