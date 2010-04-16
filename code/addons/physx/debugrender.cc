//------------------------------------------------------------------------------
//  physx/util.cc
//  (C) 2010
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physx/debugrender.h"
#include "math/matrix44.h"
#include "coregraphics/shaperenderer.h"
#include "coregraphics/vertexcomponent.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "coregraphics/primitivegroup.h"
#include "math/matrix44.h"
#include "util/array.h"
#include "NxDebugRenderable.h"

namespace PhysX
{

using namespace CoreGraphics;
using namespace Math;
using namespace Util;
using namespace Resources;

DebugRender::DebugRender()
{
	this->shaderInstance = ShaderServer::Instance()->CreateShaderInstance(ResourceId("shd:debugrender"));
	ShaderServer::Instance()->SetActiveShaderInstance(this->shaderInstance);
	ShaderFeature::Mask mask = ShaderServer::Instance()->FeatureStringToMask("ShapeColor");
	this->shaderInstance->SelectActiveVariation(mask);
}

DebugRender::~DebugRender()
{
	this->shaderInstance = 0;
}

void
DebugRender::RenderData(const NxDebugRenderable& data)
{
	Array<VertexComponent> components;
	components.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3));
	components.Append(VertexComponent(VertexComponent::Color, 0, VertexComponent::UByte4));
        

	// render point
	NxU32 NbPoints = data.getNbPoints();
	if (NbPoints > 0)
	{
		const NxDebugPoint* Points = data.getPoints();
		Ptr<VertexBuffer> vb = VertexBuffer::Create();
		Ptr<MemoryVertexBufferLoader> vbLoader = MemoryVertexBufferLoader::Create();
		vbLoader->Setup(components,  NbPoints, (void*)&(Points[0]), sizeof(NxDebugPoint)*NbPoints);
		vb->SetLoader(vbLoader.upcast<ResourceLoader>());
		vb->Load();
		vb->SetLoader(0);
		vbLoader = 0;
		
		PrimitiveGroup group;
		group.SetBaseVertex(0);
		group.SetNumVertices(NbPoints);
		group.SetPrimitiveTopology(PrimitiveTopology::PointList);

		ShaderServer* ss = ShaderServer::Instance();
		ss->ResetFeatureBits();
		ss->SetActiveShaderInstance(this->shaderInstance);
		this->shaderInstance->Begin();
		this->shaderInstance->BeginPass(0);
		RenderDevice::Instance()->SetVertexBuffer(vb);
		RenderDevice::Instance()->SetPrimitiveGroup(group);
		RenderDevice::Instance()->Draw();
		this->shaderInstance->EndPass();
		this->shaderInstance->End();
	}

    // Render lines
	NxU32 NbLines = data.getNbLines();
	if (NbLines > 0)
    {
		int lineNum = NbLines*2;
		Array<NxDebugPoint> lines;
		const NxDebugLine* Lines = data.getLines();
		while(NbLines--)
		{
			NxDebugPoint p;
        	p.p = Lines->p0;
        	p.color = Lines->color;
			lines.Append(p);

			NxDebugPoint p1;
        	p1.p = Lines->p1;
        	p1.color = Lines->color;
			lines.Append(p1);

	      	Lines++;
		}
		
        Ptr<VertexBuffer> vb = VertexBuffer::Create();
		Ptr<MemoryVertexBufferLoader> vbLoader = MemoryVertexBufferLoader::Create();
		vbLoader->Setup(components,  lineNum, &(lines[0]), sizeof(NxDebugPoint)*lineNum);
		vb->SetLoader(vbLoader.upcast<ResourceLoader>());
		vb->Load();
		vb->SetLoader(0);
		vbLoader = 0;
		
		PrimitiveGroup group;
		group.SetBaseVertex(0);
		group.SetNumVertices(lineNum);
		group.SetPrimitiveTopology(PrimitiveTopology::LineList);

		ShaderServer* ss = ShaderServer::Instance();
		ss->ResetFeatureBits();
		ss->SetActiveShaderInstance(this->shaderInstance);
		this->shaderInstance->Begin();
		this->shaderInstance->BeginPass(0);
		RenderDevice::Instance()->SetVertexBuffer(vb);
		RenderDevice::Instance()->SetPrimitiveGroup(group);
		RenderDevice::Instance()->Draw();
		this->shaderInstance->EndPass();
		this->shaderInstance->End();
    }
    
    // Render triangles
	NxU32 NbTris = data.getNbTriangles();
	if (NbTris>0)
    {
		int triangleNum = NbTris*3;
        Array<NxDebugPoint> tris;
        const NxDebugTriangle* Triangles = data.getTriangles();

        while(NbTris--)
        {
			NxDebugPoint p0;
            p0.color = Triangles->color;
			p0.p = Triangles->p0;
			tris.Append(p0);

			NxDebugPoint p1;
            p1.color = Triangles->color;
            p1.p = Triangles->p1;
			tris.Append(p1);

			NxDebugPoint p2;
            p2.color = Triangles->color;
            p2.p = Triangles->p2;
			tris.Append(p2);

            Triangles++;
        }
        
		Ptr<VertexBuffer> vb = VertexBuffer::Create();
		Ptr<MemoryVertexBufferLoader> vbLoader = MemoryVertexBufferLoader::Create();
		vbLoader->Setup(components,  triangleNum, &(tris[0]), sizeof(NxDebugPoint)*triangleNum);
		vb->SetLoader(vbLoader.upcast<ResourceLoader>());
		vb->Load();
		vb->SetLoader(0);
		vbLoader = 0;
		
		PrimitiveGroup group;
		group.SetBaseVertex(0);
		group.SetNumVertices(triangleNum);
		group.SetPrimitiveTopology(PrimitiveTopology::TriangleList);

		ShaderServer* ss = ShaderServer::Instance();
		ss->ResetFeatureBits();
		ss->SetActiveShaderInstance(this->shaderInstance);
		this->shaderInstance->Begin();
		this->shaderInstance->BeginPass(0);
		RenderDevice::Instance()->SetVertexBuffer(vb);
		RenderDevice::Instance()->SetPrimitiveGroup(group);
		RenderDevice::Instance()->Draw();
		this->shaderInstance->EndPass();
		this->shaderInstance->End();
    }    

}

//Math::float4 
//PhysXServer::GetColor(NxU32 color) const
//{
//	float4 color;
//    NxF32 Blue= NxF32((color)&0xff)/255.0f;
//    NxF32 Green= NxF32((color>>8)&0xff)/255.0f;
//    NxF32 Red= NxF32((color>>16)&0xff)/255.0f;
//    color.set(Red, Green, Blue, 0.0f);
//	return color;
//}

} // namespace Physics