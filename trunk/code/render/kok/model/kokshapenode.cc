//------------------------------------------------------------------------------
//  KokShapeNode.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/model/kokshapenode.h"
#include "kok/model/kokshapenodeinstance.h"
#include "memory/memory.h"
#include "coregraphics/primitivegroup.h"
#include "coregraphics/renderdevice.h"

#include "coregraphics/StreamMeshLoader.h"
#include "coregraphics/MemoryVertexBufferLoader.h"
#include "coregraphics/MemoryIndexBufferLoader.h"
#include "coregraphics/streammeshloader.h"
#include "coregraphics/shaderserver.h"
#include "resources/SharedResourceServer.h"


namespace KOK
{
ImplementClass(KOK::KokShapeNode, 'KSNE', Models::TransformNode);

using namespace IO;
using namespace Util;
using namespace Models;
using namespace Resources;
using namespace Math;
using namespace Memory;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
KokShapeNode::KokShapeNode():
	mesh(0),
	verticesBuffer(0),
	indicesBuffer(0),
	skinWeights(0),
	attributeTable(0),
	m_pMaterial(0),
	m_pMaterialAnimator(0),
	shaderInstance(0),
	diffuseColor(0),
	ambientColor(0),
	specularColor(0),
	emissiveColor(0)
{
}

//------------------------------------------------------------------------------
/**
*/
KokShapeNode::~KokShapeNode()
{
	
}

//------------------------------------------------------------------------------
/**
*/
Resource::State
KokShapeNode::GetResourceState() const
{
	return Resource::Loaded;
}

//------------------------------------------------------------------------------
/**
*/
void
KokShapeNode::LoadResources()
{
    // call parent class
    TransformNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
KokShapeNode::UnloadResources()
{    
	Resources::SharedResourceServer::Instance()->UnregisterSharedResource(this->mesh.upcast<Resource>());
	this->mesh = 0;
	this->shaderInstance = 0;

    TransformNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelNodeInstance> 
KokShapeNode::CreateNodeInstance()const
{
	Ptr<ModelNodeInstance> instance = (ModelNodeInstance*)KokShapeNodeInstance::Create();
	return instance;
}

//------------------------------------------------------------------------------
/**
*/
bool
KokShapeNode::ApplySharedState()
{
	if (!this->mesh.isvalid())
		return false;

	TransformNode::ApplySharedState();

	ShaderServer::Instance()->SetActiveShaderInstance(this->shaderInstance);

	return true;
}

//------------------------------------------------------------------------------
/**
	bBlend:不是摇动的地物不需要用MDLVTX格式，浪费一个float
*/
void
KokShapeNode::LoadFromStream(const Ptr<Stream>& stream, 
							 int iVersion, 
							 bool bMirrorZ, 
							 bool bCompuiteDuplicateVertexCollection,
							 bool bBlend)
{
	String strTmp;
	int nameLen;
	stream->Read(&nameLen, sizeof(int));
	ReadString(stream, strTmp, sizeof(char)*nameLen);

	this->SetName(strTmp);

	// 无用？浪费！！！
	//matrix44 mat;
	//stream->Read(&mat.getrow0(), sizeof(float)*16);
	stream->Seek(sizeof(float)*16, Stream::Current);

	// 读取顶点数据
	bbox boundingbox;
	boundingbox.pmin = Math::vector(N_MAXREAL, N_MAXREAL, N_MAXREAL);
	boundingbox.pmax = Math::vector(N_MINREAL, N_MINREAL, N_MINREAL);
	stream->Read(&this->verticesBufferSize, sizeof(DWORD));
	if (this->verticesBufferSize > 0)
	{
		this->verticesBuffer = new VertexFVF[this->verticesBufferSize];

		for (SizeT i = 0; i < this->verticesBufferSize; i++)
		{
			stream->Read(&(this->verticesBuffer[i].p), sizeof(Math::vector3));
			//this->addSubMeshDuplicateVertexCollections(i, bComputieDuplicateVertexCollection);
			stream->Read(&(this->verticesBuffer[i].n), sizeof(Math::vector3));

			if (bMirrorZ)
			{
				this->verticesBuffer[i].p.z = -this->verticesBuffer[i].p.z;
				this->verticesBuffer[i].n.z = -this->verticesBuffer[i].n.z;
			}
			stream->Read(&(this->verticesBuffer[i].tex), sizeof(float)*2);

			boundingbox.extend(point(this->verticesBuffer[i].p.x,
				this->verticesBuffer[i].p.y,
				this->verticesBuffer[i].p.z));
		}
	}
	this->SetBoundingBox(boundingbox);

	// 读取索引数据
	stream->Read(&this->indexBufferSize, sizeof(DWORD));
	if (this->indexBufferSize > 0)
	{
		this->indicesBuffer = n_new_array(WORD, this->indexBufferSize);

		if (bMirrorZ)
		{
			for (SizeT i = 0; i < this->indexBufferSize; i += 3)
			{
				stream->Read(&this->indicesBuffer[i],   sizeof(WORD));
				stream->Read(&this->indicesBuffer[i+2], sizeof(WORD));
				stream->Read(&this->indicesBuffer[i+1], sizeof(WORD));
			}
		}
		else
		{
			stream->Read(this->indicesBuffer, sizeof(WORD) * this->indexBufferSize);
		}
	}

	// 读取skinWeight资料的长度
	stream->Read(&this->skinWeightNum, sizeof(DWORD));
	if (this->skinWeightNum > 0)
	{
		this->skinWeights[this->skinWeightNum];

		for (SizeT i = 0; i < this->skinWeightNum; i++)
		{
			this->skinWeights[i].importSkinWeightFromMemory(stream);
		}
	}

	if (iVersion >= 5)
	{
		stream->Read(&this->effectType,     sizeof(DWORD));
		stream->Read(&this->ribbonWidth,  sizeof(float));
		stream->Read(&this->ribbonLength, sizeof(float));
	}
	this->modelType = GetThingType(this->GetName().Value());

	// 读取attribTable资料
	stream->Read(&this->attributeTableSize, sizeof(DWORD));
	if (this->attributeTableSize > 0)
	{
		this->attributeTable = n_new_array(AttributeRange, this->attributeTableSize);
		stream->Read(this->attributeTable, sizeof(AttributeRange)*this->attributeTableSize);

		// 读取material资料
		stream->Read(&m_dwNumMaterial, sizeof(DWORD));
		if (m_dwNumMaterial > 0)
		{
			m_pMaterial = n_new_array(cMaterial, m_dwNumMaterial);
			m_pMaterialAnimator = n_new_array(cMaterialAnimator, m_dwNumMaterial);

			for (SizeT i = 0; i < m_dwNumMaterial; i++)
				ImportMaterialFromMemory(stream, i, iVersion);
		}
	}


	// 创建mesh
	CreateMesh();

	CreateMaterial();
}

EThingSubMeshSpecialType 
KokShapeNode::GetThingType(String name)
{
	if (name.Length())
	{
		if (name.CheckStringExist("m_hd"))
			return TYPE_HD;
		else if (name.CheckStringExist("m_light"))
			return TYPE_LIGHT; 
		else if (name.CheckStringExist("m_repeat"))
			return TYPE_REPEAT; 
		else if (name.CheckStringExist("m_select"))
			return TYPE_SELECT; 
		else if (name.CheckStringExist("m_water"))
			return TYPE_WATER; 
		else if (name.CheckStringExist("e_ff"))
			return TYPE_EFF; 
		else if (name.CheckStringExist("e_flame"))
			return TYPE_EFLAME; 
		else
			return TYPE_NORMAL;
	}
	else
		return TYPE_NORMAL;
}

void 
KokShapeNode::ImportMaterialFromMemory( const Ptr<Stream>& stream, int iIndex, int iVersion )
{
	// 读取贴图名称长度
	int iTextureNameLength = 0;
	stream->Read(&iTextureNameLength, sizeof(int));
	m_pMaterial[iIndex].m_iNameSize = iTextureNameLength;

	// 读取贴图名称
	if( iTextureNameLength > 0 )
	{
		String tmp;
		ReadString(stream, tmp, iTextureNameLength);
		// 加上纹理编号才能读到图
		m_pMaterial[iIndex].m_pszTextName.Format("%s%02d", tmp.AsCharPtr(), iIndex);
		m_pMaterial[iIndex].LoadTexture(); 
	}

	// 读取材质
	m_pMaterial[iIndex].m_D3DMaterial.Diffuse.r = m_pMaterial[iIndex].m_D3DMaterial.Ambient.r = 1.0f;
	m_pMaterial[iIndex].m_D3DMaterial.Diffuse.g = m_pMaterial[iIndex].m_D3DMaterial.Ambient.g = 1.0f;
	m_pMaterial[iIndex].m_D3DMaterial.Diffuse.b = m_pMaterial[iIndex].m_D3DMaterial.Ambient.b = 1.0f;
	m_pMaterial[iIndex].m_D3DMaterial.Diffuse.a = m_pMaterial[iIndex].m_D3DMaterial.Ambient.a = 1.0f;

	// Set the RGBA for diffuse reflection.
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Diffuse,  sizeof(float)*4);
	// Set the color and sharpness of specular highlights.
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Power,    sizeof(float)  );
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Specular, sizeof(float)*4);
	// Set the RGBA for emissive color.
	stream->Read(&m_pMaterial[iIndex].m_D3DMaterial.Emissive, sizeof(float)*4);
	

	if( iVersion >= 5 )
	{
		DWORD dwAlphaBlendType;
		stream->Read(&dwAlphaBlendType, sizeof(DWORD));
		m_pMaterial[iIndex].SetAlphaBlendType( dwAlphaBlendType );

		DWORD dwTextureAniRows;
		stream->Read(&dwTextureAniRows, sizeof(DWORD));
		m_pMaterial[iIndex].SetTextureAniRows( dwTextureAniRows );

		DWORD dwTextureAniCols;
		stream->Read(&dwTextureAniCols, sizeof(DWORD));
		m_pMaterial[iIndex].SetTextureAniCols( dwTextureAniCols );

		DWORD dwTextureAniRepeat;
		stream->Read(&dwTextureAniRepeat, sizeof(DWORD));
		m_pMaterial[iIndex].SetTextureAniRepeat( dwTextureAniRepeat );

		ImportMaterialAnimationFromMemory(stream, iIndex);
	}
}

void 
KokShapeNode::ImportMaterialAnimationFromMemory( const Ptr<Stream>& stream, int iIndex )
{
	Memory::MemoryMark mm(MemStack);

	DWORD dwKey;
	DWORD dwType;
	DWORD dwTotalKeys;
	D3DXVECTOR2 vPos;
	D3DXVECTOR2 vScale;
	float* pTimeTmp = NULL;
	D3DXVECTOR2* pVecTmp = NULL;
	float* pFloatTemp = NULL;
	// extended material animation diffuse color
	D3DXCOLOR* pColorTemp = NULL;

	do
	{
		stream->Read(&dwType, sizeof(DWORD));

		switch( dwType )
		{
		case 1: // offset
			stream->Read(&dwTotalKeys, sizeof(DWORD));
			pTimeTmp = new(MemStack) float[dwTotalKeys];
			pVecTmp = new(MemStack) D3DXVECTOR2[dwTotalKeys];
			stream->Read(pTimeTmp, sizeof(float)*dwTotalKeys);
			stream->Read(pVecTmp, sizeof(D3DXVECTOR2)*dwTotalKeys);
			for( dwKey = 0; dwKey < dwTotalKeys; dwKey++ )
			{
				m_pMaterialAnimator[iIndex].AddPosKey( pTimeTmp[dwKey], pVecTmp[dwKey] );
			}
			break;
		case 2: // tiling
			stream->Read(&dwTotalKeys, sizeof(DWORD));
			pTimeTmp = new(MemStack) float[dwTotalKeys];
			pVecTmp = new(MemStack) D3DXVECTOR2[dwTotalKeys];
			stream->Read(pTimeTmp, sizeof(float)*dwTotalKeys);
			stream->Read(pVecTmp, sizeof(D3DXVECTOR2)*dwTotalKeys);
			for( dwKey = 0; dwKey < dwTotalKeys; dwKey++ )
			{
				m_pMaterialAnimator[iIndex].AddScaleKey( pTimeTmp[dwKey], pVecTmp[dwKey] );
			}
			break;
		case 3: // wangle
			stream->Read(&dwTotalKeys, sizeof(DWORD));
			pTimeTmp = new(MemStack) float[dwTotalKeys];
			pFloatTemp = new(MemStack) float[dwTotalKeys];
			stream->Read(pTimeTmp, sizeof(float)*dwTotalKeys);
			stream->Read(pFloatTemp, sizeof(float)*dwTotalKeys);
			for( dwKey = 0; dwKey < dwTotalKeys; dwKey++ )
			{
				m_pMaterialAnimator[iIndex].AddRotateZKey( pTimeTmp[dwKey], pFloatTemp[dwKey] );
			}
			break;
			// 070205 cyhsieh extended material animation diffuse color
		case 4: // diffuse & opacity
			stream->Read(&dwTotalKeys, sizeof(DWORD));
			pTimeTmp = new(MemStack) float[dwTotalKeys];
			pColorTemp = new(MemStack) D3DXCOLOR[dwTotalKeys];
			stream->Read(pTimeTmp, sizeof(float)*dwTotalKeys);
			stream->Read(pColorTemp, sizeof(D3DXCOLOR)*dwTotalKeys);
			for( dwKey = 0; dwKey < dwTotalKeys; dwKey++ )
			{
				m_pMaterialAnimator[iIndex].AddDiffuseOpacityKey( pTimeTmp[dwKey], pColorTemp[dwKey] );
			}
			break;
			// 070212 cyhsieh extended material animation texture address
		case 5:
			stream->Read(&dwTotalKeys, sizeof(DWORD));
			m_pMaterialAnimator[iIndex].SetAddressFlag( dwTotalKeys );
			break;
		}

	} while( dwType != 0 );
}

void 
KokShapeNode::Render()
{
	// 设置灯光
	//if(g_mLights)
	//{
	//	g_mLights->bLightSystem = bLightSystem;

	//	SetLightData();

	//	g_mLights->bForceAmbientColor = m_bForceAmbientColor ; 
	//	g_mLights->ForceAmbientColor  = m_colForceAmbientColor ;
	//	g_mLights->ShowLight();
	//	if(m_pEffect)
	//	{
	//		//jingjie add 2005.08.14 给予vertex shader local space平行光方向,Ia,Id参数 
	//		//目前不用到specular所以Is不给 以后或许可以考虑加入
	//		D3DXMATRIXA16 mInverseWorlMatrix;
	//		D3DXVECTOR4 vLocallightDir;
	//		D3DXVECTOR4 vlight_pos = D3DXVECTOR4(g_mLights->GetSunDir()->x,g_mLights
	//		->GetSunDir()->y, g_mLights->GetSunDir()->z,0);
	//		D3DXMatrixInverse( &mInverseWorlMatrix, NULL, &m_matMatrix);
	//		D3DXVec4Transform( &vLocallightDir,&vlight_pos , &mInverseWorlMatrix );
	//		D3DXVec3Normalize( (D3DXVECTOR3 *)&vLocallightDir, (D3DXVECTOR3 *)
	//		&vLocallightDir );
	//		m_pEffect->SetVector( "g_LocallightDir", &vLocallightDir );
	//		m_pEffect->SetVector( "g_I_Ambient", &g_mLights->GetSunAmibentColor());
	//		m_pEffect->SetVector( "g_I_Diffuse", &g_mLights->GetSunDiffuseColor());
	//	}
	//}

	for (SizeT i = 0; i < attributeTableSize; i++)
	{
		RenderBatch(i);
	}
}

void 
KokShapeNode::RenderBatch(IndexT index)
{
	const AttributeRange& batch = attributeTable[index];
	const cMaterial& material = m_pMaterial[index];

	if (!batch.FaceCount || !batch.VertexCount || !material.GetTexture().isvalid())
		return;

	//if(bWaterTexture)
	//{
	//	DXTEST(pd3dDevice->SetMaterial(&(g_mWater->m_pMaterial[0].m_D3DMaterial) ));
	//}
	//else
	//{
	//	if( bUseTexMaterial )
	//	{
	//		DXTEST(pd3dDevice->SetMaterial(&(m_pMaterial[i].m_D3DMaterial)));

	//		// 070205 cyhsieh extended material animation
	//		const D3DXMATRIXA16* pMat = m_pMaterial[i].GetTextureTransformMatrix();
	//		if( pMat )
	//		{
	//		bUseTextureTransform = true;
	//		pd3dDevice->SetTransform( D3DTS_TEXTURE0, pMat );
	//		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	//		}
	//	}
	//	else
	//	{
	//		DXTEST(pd3dDevice->SetMaterial(&Mat));
	//	}
	//}
	//if(bWaterTexture)
	//{
	//	if(g_mWater->m_pTexture)
	//	{
	//		DXTEST(pd3dDevice->SetTexture( 0,g_mWater->m_pTexture->GetTexture(pd3dDevice)));
	//	}
	//	else
	//	{
	//		DXTEST(pd3dDevice->SetTexture( 0, NULL));
	//	}
	//} 
	//else if(m_pMaterial)
	//{
	//	cMaterial *l_pMate = &m_pMaterial[i];
	//	if(l_pMate->m_pTexture)
	//	{
	//		DXTEST(pd3dDevice->SetTexture( 0, l_pMate->m_pTexture->GetTexture(pd3dDevice)));
	//	}
	//	else
	//	{
	//		DXTEST(pd3dDevice->SetTexture( 0, NULL));
	//	}
	//} 
	//else 
	//{
	//	DXTEST(pd3dDevice->SetTexture( 0, NULL));
	//}

	// 缺少水面贴图和纹理动画的支持
	if(batch.VertexCount && batch.FaceCount)
	{
		this->diffuseColor->SetVector(float4(
			material.m_D3DMaterial.Diffuse.r,
			material.m_D3DMaterial.Diffuse.g,
			material.m_D3DMaterial.Diffuse.b,
			material.m_D3DMaterial.Diffuse.a));
		this->ambientColor->SetVector(float4(
			material.m_D3DMaterial.Ambient.r,
			material.m_D3DMaterial.Ambient.g,
			material.m_D3DMaterial.Ambient.b,
			material.m_D3DMaterial.Ambient.a));
		this->specularColor->SetVector(float4(
			material.m_D3DMaterial.Specular.r,
			material.m_D3DMaterial.Specular.g,
			material.m_D3DMaterial.Specular.b,
			material.m_D3DMaterial.Specular.a));
		this->emissiveColor->SetVector(float4(
			material.m_D3DMaterial.Emissive.r,
			material.m_D3DMaterial.Emissive.g,
			material.m_D3DMaterial.Emissive.b,
			material.m_D3DMaterial.Emissive.a));
		this->diffMap->SetTexture(material.GetTexture());
		this->shaderInstance->Commit();

		this->mesh->ApplyPrimitives(index);
		RenderDevice::Instance()->Draw();
	}
}


void
KokShapeNode::CreateMesh()
{
	if (this->mesh.isvalid())
		return;

	Util::Array<CoreGraphics::VertexComponent> vertexComponents;
	if (vertexComponents.Size() == 0)
	{
		vertexComponents.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3));
		vertexComponents.Append(VertexComponent(VertexComponent::Normal, 0, VertexComponent::Float3));
		//vertexComponents.Append(VertexComponent(VertexComponent::Color, 0, VertexComponent::Float4));
		vertexComponents.Append(VertexComponent(VertexComponent::TexCoord, 0, VertexComponent::Float2));
	}

	// setup new vertex buffer
	Ptr<VertexBuffer> vertexBuffer = VertexBuffer::Create();
	Ptr<MemoryVertexBufferLoader> vbLoader = MemoryVertexBufferLoader::Create();
	vbLoader->Setup(vertexComponents, verticesBufferSize, verticesBuffer, verticesBufferSize * sizeof(VertexFVF));
	vertexBuffer->SetLoader(vbLoader.upcast<ResourceLoader>());
	vertexBuffer->Load();
	vertexBuffer->SetLoader(0);
	n_assert(vertexBuffer->GetState() == VertexBuffer::Loaded);


	Ptr<IndexBuffer> indexBuffer = IndexBuffer::Create();
	Ptr<MemoryIndexBufferLoader> ibLoader = MemoryIndexBufferLoader::Create();
	ibLoader->Setup(IndexType::Index16, indexBufferSize, indicesBuffer, sizeof(WORD)*indexBufferSize);
	indexBuffer->SetLoader(ibLoader.upcast<ResourceLoader>());
	indexBuffer->Load();
	indexBuffer->SetLoader(0);
	n_assert(indexBuffer->GetState() == IndexBuffer::Loaded);


	Util::Array<CoreGraphics::PrimitiveGroup> primGroups;

	for (SizeT i = 0; i < attributeTableSize; i++)
	{
		PrimitiveGroup primGroup;
		primGroup.SetBaseVertex(attributeTable[i].VertexStart);
		primGroup.SetNumVertices(attributeTable[i].VertexCount);
		primGroup.SetBaseIndex(attributeTable[i].FaceStart);
		primGroup.SetNumIndices(attributeTable[i].FaceCount*3);

		primGroup.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
		primGroups.Append(primGroup);
	}

	Resources::ResourceId meshID(this->GetName());
	n_assert(!Resources::SharedResourceServer::Instance()->HasSharedResource(meshID));
	mesh = Resources::SharedResourceServer::Instance()->CreateSharedResource(meshID, CoreGraphics::Mesh::RTTI,CoreGraphics::StreamMeshLoader::RTTI).downcast<CoreGraphics::Mesh>();
	mesh->SetState(Resource::Loaded);
	mesh->SetVertexBuffer(vertexBuffer);
	mesh->SetIndexBuffer(indexBuffer);
	mesh->SetPrimitiveGroups(primGroups);
}


void
KokShapeNode::CreateMaterial()
{
	this->shaderInstance = ShaderServer::Instance()->CreateShaderInstance(Resources::ResourceId("shd:kokmodel"));

	this->diffuseColor = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("DiffuseColor"));
	this->ambientColor = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("AmbientColor"));
	this->specularColor = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("SpecularColor"));
	this->emissiveColor = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("EmissiveColor"));
	this->diffMap = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("DiffMap0"));
}
}