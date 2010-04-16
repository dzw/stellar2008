//------------------------------------------------------------------------------
//  view.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/view.h"
#include "graphics/stagebuilder.h"
#include "graphics/modelentity.h"
#include "models/visresolver.h"
#include "coregraphics/transformdevice.h"
#include "coregraphics/shaperenderer.h"
#include "lighting/lightserver.h"
#include "lighting/shadowserver.h"
#include "coregraphics/debugview.h"
#ifdef PHYSX
#include "addons/physx/physxserver.h"
#endif

#if NEBULA3_DEBUG
#include "graphics/cell.h"
#endif

namespace Graphics
{
ImplementClass(Graphics::View, 'GRVW', Core::RefCounted);

using namespace Util;
using namespace Models;
using namespace CoreGraphics;
using namespace Lighting;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
View::View() :
    isAttachedToServer(false),
	needUpdate(false),
	showDebugInfo(false),
	showPhysX(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
View::~View()
{
    // check that we've been properly cleaned up
    n_assert(!this->isAttachedToServer);
    n_assert(!this->stage.isvalid());
    n_assert(!this->camera.isvalid());
    n_assert(!this->renderTarget.isvalid());
    n_assert(!this->frameShader.isvalid());
    n_assert(this->dependencies.IsEmpty());
}

//------------------------------------------------------------------------------
/**
*/
void
View::OnAttachToServer()
{
    n_assert(!this->IsAttachedToServer());
    this->isAttachedToServer = true;
}

//------------------------------------------------------------------------------
/**
*/
void
View::OnRemoveFromServer()
{
    n_assert(this->IsAttachedToServer());
    if (this->camera.isvalid())
    {
        this->camera->OnRemoveFromView(this);
        this->camera = 0;
    }
    this->stage = 0;
    this->renderTarget = 0;
    this->frameShader = 0;
    this->dependencies.Clear();
    this->isAttachedToServer = false;
}

//------------------------------------------------------------------------------
/**
    This method updates the visibility links for the view's camera.
    This method should usually be called before Render().
*/
void
View::UpdateVisibilityLinks()
{
    n_assert(this->camera.isvalid());
	matrix44 pos = this->camera->GetTransform();
	if (this->camPos != pos)
	{
		this->needUpdate = true;
		this->camPos = pos;
		this->stage->UpdateCameraLinks(this->camera);
	}
	else
		this->needUpdate = false;
}

//------------------------------------------------------------------------------
/**
    This attaches visible lights to the light and shadow server.
    @todo: currently this methods needs to go over all visible
    graphics entities to find the lights... 
*/
void
View::ResolveVisibleLights()
{
    n_assert(this->camera.isvalid());
    LightServer* lightServer = LightServer::Instance();
    ShadowServer* shadowServer = ShadowServer::Instance();

    lightServer->BeginAttachVisibleLights();
    shadowServer->BeginAttachVisibleLights();
    const Array<Ptr<GraphicsEntity> >& visLinks = this->camera->GetLinks(GraphicsEntity::CameraLink);
    IndexT i;
    SizeT num = visLinks.Size();
    for (i = 0; i < num; i++)
    {
        const Ptr<GraphicsEntity>& curEntity = visLinks[i];
        if (GraphicsEntity::LightType == curEntity->GetType())
        {
            const Ptr<AbstractLightEntity>& lightEntity = curEntity.downcast<AbstractLightEntity>();
            lightServer->AttachVisibleLight(lightEntity);
            if (lightEntity->GetCastShadows())
            {
                shadowServer->AttachVisibleLight(lightEntity);
            }
        }
    }
    shadowServer->EndAttachVisibleLights();
    lightServer->EndAttachVisibleLights();
}

//------------------------------------------------------------------------------
/**
    Resolve all visible ModelNodeInstances by following the visibility
    links of our camera. This is necessary as preparation for rendering.
*/
void
View::ResolveVisibleModelNodeInstances()
{
    n_assert(this->camera.isvalid());
    VisResolver* visResolver = VisResolver::Instance();
    visResolver->IncreaseFrameIndex();

	// 限制不要每帧都更新，隔五帧更新一次
	/*static IndexT lastFrameIndex = -1;
	if (visResolver->GetFrameIndex() <= lastFrameIndex+3)
		return;
	lastFrameIndex = visResolver->GetFrameIndex();*/

	visResolver->BeginResolve();
    const Array<Ptr<GraphicsEntity> >& visLinks = this->camera->GetLinks(GraphicsEntity::CameraLink);
    IndexT i;
    SizeT num = visLinks.Size();
    for (i = 0; i < num; i++)
    {
        const Ptr<GraphicsEntity>& curEntity = visLinks[i];
        if (GraphicsEntity::ModelType == curEntity->GetType() ||
			GraphicsEntity::TerrainType == curEntity->GetType())
        {
			curEntity->AttachVisibleInstance();
			/*const Ptr<ModelEntity>& modelEntity = curEntity.downcast<ModelEntity>();
            visResolver->AttachVisibleModelInstance(modelEntity->GetModelInstance());*/
        }
    }
    visResolver->EndResolve();

	DebugView::Instance()->AddDebugString("visible entity", num);
}

//------------------------------------------------------------------------------
/**
    This method renders the current view into the render target. This method
    must be called sometimes after UpdateVisibilityLinks()
*/
void
View::Render()
{
    n_assert(this->frameShader.isvalid());      
    n_assert(this->camera.isvalid());

	if (needUpdate)
	{
		//LightServer* lightServer = LightServer::Instance();
		//ShadowServer* shadowServer = ShadowServer::Instance();
		//lightServer->BeginFrame(this->camera);
		//shadowServer->BeginFrame(this->camera);

		// resolve visible light source
		//this->ResolveVisibleLights();

		// draw the shadow pass
		//ShadowServer::Instance()->UpdateShadowBuffers();

		// resolve visible ModelNodeInstances
		this->ResolveVisibleModelNodeInstances();
	}

    // render the world...
    TransformDevice* transformDevice = TransformDevice::Instance();
    transformDevice->SetProjTransform(this->camera->GetProjTransform());
    transformDevice->SetViewTransform(this->camera->GetViewTransform());
    transformDevice->ApplyViewSettings();
    this->frameShader->Render();

	if (needUpdate)
	{
		// tell light and shadow servers that rendering is finished
		//shadowServer->EndFrame();
		//lightServer->EndFrame();
	}

	if (showDebugInfo)
	{
		this->RenderDebug();
		this->RenderDebugString();
	}

#ifdef PHYSX
	if (showPhysX)
	{
		if (PhysX::PhysXServer::HasInstance())
			PhysX::PhysXServer::Instance()->RenderDebug();
	}
#endif
	//RenderDebug();
	//this->RenderDebugString();
}

//------------------------------------------------------------------------------
/**
    Renders a debug visualization. Can be called alone or after Render().
*/
void
View::RenderDebug()
{
//#if NEBULA3_DEBUG
    n_assert(this->frameShader.isvalid());

    // setup global transforms...
    TransformDevice* transformDevice = TransformDevice::Instance();
    transformDevice->SetProjTransform(this->camera->GetProjTransform());
    transformDevice->SetViewTransform(this->camera->GetViewTransform());

    // just call OnRenderDebug on all graphics entities visible through our camera
    ShapeRenderer::Instance()->Begin();
    const Array<Ptr<GraphicsEntity> >& visLinks = this->camera->GetLinks(GraphicsEntity::CameraLink);
    IndexT i;
    SizeT num = visLinks.Size();
    for (i = 0; i < num; i++)
    {
        const Ptr<GraphicsEntity>& curEntity = visLinks[i];
        curEntity->OnRenderDebug();
    }

	// cell debug
	//this->stage->GetRootCell()->OnRenderDebug();

    ShapeRenderer::Instance()->End();
//#endif
}

//------------------------------------------------------------------------------
/**
    Renders a debug visualization. Must be called inside Begin and End of shaperenderer.
*/
void
View::RenderDebugSimple()
{
    n_assert(this->frameShader.isvalid());
    
    const Array<Ptr<GraphicsEntity> >& visLinks = this->camera->GetLinks(GraphicsEntity::CameraLink);
    IndexT i;
    SizeT num = visLinks.Size();
    for (i = 0; i < num; i++)
    {
        const Ptr<GraphicsEntity>& curEntity = visLinks[i];
        curEntity->OnRenderDebug();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
View::SetCameraEntity(const Ptr<CameraEntity>& newCameraEntity)
{
    // assert if cam is not attached to a stage
    n_assert2(!newCameraEntity.isvalid() || newCameraEntity->IsAttachedToStage(), "Camera has to be attached to a stage first!");
    if (this->camera.isvalid())
    {
        this->camera->OnRemoveFromView(this);
        this->camera = 0;
    }
    if (newCameraEntity.isvalid())
    {
        this->camera = newCameraEntity;
        this->camera->OnAttachToView(this);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
View::RenderDebugString()
{
	vector pos = this->camera->GetTransform().getrow3();
	String output;
	output.Format("%f, %f, %f", pos.x(), pos.y(), pos.z());
	DebugView::Instance()->AddDebugString("Camera Pos", output);

	DebugView::Instance()->Render();

	//// 显示坐标
	//rectangle<float> rect(0, 0, 1, 1);
	//vector pos;// = this->freeCameraUtil.GetPosition();
	//char szPos[128];
	////sprintf(szPos, "%f, %f, %f", pos.x(), pos.y(), pos.z());
	////FontServer::Instance()->DrawText(szPos, float4(1, 0, 0, 1), rect, 0, true);

	//Array<Ptr<GraphicsEntity>> modelentites = this->stage->GetEntities();

	//int nLoaded = 0, nUnload = 0, totalModels = 0;
	//for (SizeT i = 0; i < modelentites.Size(); i++)
	//{
	//	if (modelentites[i]->IsA(ModelEntity::RTTI))
	//	{
	//		totalModels++;
	//		if (modelentites[i].downcast<ModelEntity>()->GetModelResourceState() == Resources::Resource::Loaded)
	//			nLoaded++;
	//		else
	//			nUnload++;
	//	}
	//}

	//pos = this->camera->GetTransform().getrow3();
	//sprintf(szPos, "Entities: %d\n Models:%d, Loaded Num:%d Unload Num:%d, %f, %f, %f", modelentites.Size(), totalModels, nLoaded, nUnload, pos.x(), pos.y(), pos.z());
	//FontServer::Instance()->DrawText(szPos, float4(1, 0, 0, 1), rect, 0, true);
	//rect.top += 0.021;

	//for (SizeT i = 0; i < modelentites.Size(); i++)
	//{
	//	/*if (i > 10)
	//		break;*/

	//	pos = modelentites[i]->GetTransform().getrow3();
	//	if (modelentites[i]->IsA(ModelEntity::RTTI))
	//		sprintf(szPos, "%d: %f, %f, %f %s", i, pos.x(), pos.y(), pos.z(), modelentites[i].downcast<ModelEntity>()->GetResourceId().Value().AsCharPtr());
	//	else
	//		sprintf(szPos, "%d: %f, %f, %f", i, pos.x(), pos.y(), pos.z());

	//	rect.top += 0.021;
	//	rect.bottom += 0.021;

	//	FontServer::Instance()->DrawText(szPos, float4(1, 0, 0, 1), rect, 0, true);
	//}
}

Math::vector 
View::GetCameraPos()
{
	return this->camera->GetTransform().getrow3();
}

} // namespace Graphics
