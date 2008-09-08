//------------------------------------------------------------------------------
//  terrainreader.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/load/terrainreader.h"
#include "kok/terrain/terraindef.h"
#include "util/string.h"
#include "kok/terrain/districtnode.h"
#include "models/attributes.h "
#include "memory/memory.h"
#include "util/hashmap.h"

namespace KOK
{
ImplementClass(KOK::TerrainReader, 'KKTR', Models::ModelReader);

using namespace IO;
using namespace Util;
using namespace Math;
using namespace Memory;

//------------------------------------------------------------------------------
/**
*/
TerrainReader::TerrainReader():
	terrain(0),
	mapBuffer(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TerrainReader::~TerrainReader()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
TerrainReader::Open()
{
	n_assert(mapBuffer == NULL);
	n_assert(this->stream.isvalid());
	n_assert(this->model.isvalid());

	this->terrain = this->model.downcast<Terrain>();
	this->stream->SetAccessMode(Stream::ReadWriteAccess);
    if (this->stream->Open())
    {
		this->stream->Seek(3, Stream::Begin);

		return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
TerrainReader::Close()
{
    n_assert(this->IsOpen());
    this->terrain = 0;

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
TerrainReader::FillModel()
{
	// @test code
	//for (int i = 0; i < 5; i++)
	//{
	//	for (int j = 0; j < 5; j++)
	//	{
	//		Ptr<DistrictNode> node = DistrictNode::Create();
	//		//node->Init(0, 0);
	//		String name;
	//		name.Format("%2d%2d", i, j);
	//		node->SetName(name);
	//		node->SetString(Attr::Shader, "shd:terrain");
	//		node->SetTerrainMeshGrid(this->terrain->terrMeshGrid);
	//		node->SetPosition(i, j);
	//		this->terrain->AttachNode(node.upcast<Models::ModelNode>());
	//	}
	//}
	

	LoadToField();

	this->terrain->ComputeTileMesh();

	TerrainInfo terrInfo = this->terrain->GetTerrainInfo();

	for (SizeT i = 0; i < terrInfo.GetDistrictCountX()/*DISTRICTCACHESIZE*/; i++)
	{
		for (SizeT j = 0; j < terrInfo.GetDistrictCountX(); j++)
		{
			this->terrain->CreateNewDistrict(i, j);
			//Ptr<DistrictNode> node = DistrictNode::Create();
			////node->Init(0, 0);
			//String name;
			//name.Format("dist_%2d_%2d", i,j);
			//node->SetName(Resources::ResourceId(name));
			//node->SetString(Attr::Shader, "shd:terrain");
			//node->SetTerrainMeshGrid(this->terrain->terrMeshGrid);
			//node->SetPosition(i, j);
			//this->terrain->AttachNode(node.upcast<Models::ModelNode>());
		}
	}
    return true;
}

int
TerrainReader::LoadToField()
{
	DWORD  fileId;
	DWORD  tempDWORD;
	int    tempInt;
	String tempStr;

	while (!this->stream->Eof())
	{
		this->stream->Read(&fileId, sizeof(DWORD));

		switch(fileId)
		{
		case TERFILE_SCENE_MAP_ID:
			{
				this->stream->Read(&tempInt, sizeof(int));
				if (tempInt <= 0)
					break;
				ReadString(tempStr, tempInt);

				// todo:设置场景代码字串
			}
			break;
		case TERFILE_TILE_TABLE:		// 读贴图
			{
				// 读放tile的贴图档层级数量
				int layerCount;
				this->stream->Read(&layerCount, sizeof(int));
				for (SizeT i = 0; i < layerCount; i++)
				{
					this->stream->Read(&tempInt, sizeof(int));
					ReadString(tempStr, tempInt);
					this->terrain->AppendTexture(tempStr);
				}
			}
			break;
		case TERFILE_SPECTRUM_NAME:		// 读光谱档名
			{
				this->stream->Read(&tempInt, sizeof(int));
				ReadString(tempStr, tempInt);

				// todo:创建灯光
			}
			break;
		case TERFILE_FOG_TABLE:			// 雾
			{
			}
			break;
		case TERFILE_MAP_SIZE:
			{
				this->stream->Read(&tempInt, sizeof(int));
				n_assert(tempInt > 0);
				this->terrain->SetMapSize(tempInt);
			}
			break;
		case TERFILE_CLIFF_TABLE:		// 读入悬崖总类表
			{
				this->terrain->cliffTable = CliffTable::Create();
				this->terrain->cliffTable->ImportFromMemory(this->stream, this->terrain->GetTextureCount());
			}
			break;
		case TERFILE_CLIFF_LEVEL1:
		case TERFILE_CLIFF_LEVEL2:
			{
				bool shrink = false;
				if (fileId == TERFILE_CLIFF_LEVEL1)
					shrink = true;
				this->terrain->terrMeshGrid->ImportCliffLevelData(this->stream, shrink);
			}
			break;
		case TERFILE_WATER_LEVEL1:
		case TERFILE_WATER_LEVEL2:
			{
				bool shrink =false;
				if (fileId == TERFILE_WATER_LEVEL1)
					shrink = true;
				this->terrain->terrMeshGrid->ImportWaterLevelData(this->stream, shrink);
			}
			break;
		case TERFILE_TRANS_LEVEL1:
		case TERFILE_TRANS_LEVEL2:
			{
				bool shrink = false;
				if (fileId == TERFILE_TRANS_LEVEL1)
					shrink = true;
				this->terrain->terrMeshGrid->ImportTransLevelData(this->stream, shrink);
				
				
			}
			break;
		case TERFILE_TILE:
			{
				int tileSize;
				this->stream->Read(&tileSize, sizeof(int));

				for (int i = 0; i < tileSize; i++)
				{
					this->stream->Read(&tempDWORD, sizeof(DWORD));
					this->terrain->thingTex[i]->dTile = tempDWORD;

					this->stream->Read(&tempDWORD, sizeof(DWORD));
					this->terrain->thingTex[i]->stTile.dwDWORD = tempDWORD;
				}
				//return 0;	// 测试地形直接返回
			}
			break;
		case TERFILE_MAP_HIGH1:
		case TERFILE_MAP_HIGH2:
			{
				bool shrink = false;
				if (fileId == TERFILE_MAP_HIGH1)
					shrink = true;
				this->terrain->terrMeshGrid->ImportMapHeightData(this->stream, shrink);
			}
			break;
		case TERFILE_CLIFF_ID1:
		case TERFILE_CLIFF_ID2:
			{
				bool shrink = false;
				if (fileId == TERFILE_CLIFF_ID1)
					shrink = true;
				this->terrain->terrMeshGrid->ImportCliffIdData(this->stream, shrink);
			}
			break;
		case TERFILE_THING_LIST:		
			{
				// 读地上物清单
				LoadModels();
				return 0;
			}
			break;
		case TERFILE_SOUND_SOURCE_LIST:
			{
				// 读入音效档清单
				/*if(m_pAudioManage)
				{
					m_pAudioManage->SetFilePath("Audio\\Condition\\");
					Offset = m_pAudioManage->LoadToFileList(mem,iFileSize);
					mem += Offset;
				}*/
				LoadAudios();
			}
			break;
		case TERFILE_SOUND_LIST:
			{
				// 弄睲虫
				/*if(m_pSoundDB)
				{
					Offset = m_pSoundDB->LoadToFile(mem,iFileSize);
					mem += Offset;
				}*/
			}
			break;
		case TERFILE_SMALL_MAP_COLOR:
			{
				// 弄瓜ノま︹计秖
				//int l_SMCSize = 0 ;
				//if(iFileSize < sizeof(int)) return -1;
				//iFileSize -= sizeof(int); 
				//mem = f_MemRead(&l_SMCSize, mem, sizeof(int));
				//// skip this...
				//if ((DWORD)iFileSize<l_SMCSize*sizeof(DWORD)) return -1;
				//iFileSize -= (l_SMCSize*sizeof(DWORD));
				//mem += (l_SMCSize*sizeof(DWORD));
			}
			break;
		case TERFILE_LIGHT_OBJ_LIST:
			{
				// 糶方
				//if(m_pLightManage)
				//{
				//	// 051229 cyhsieh
				//	//m_pLightManage->LoadToFile(mem);
				//	Offset = m_pLightManage->LoadToFile( mem );
				//	mem += Offset;

				//	g_pGameTerrain->UpdateThingtLightObj();
				//}
			}
			break;
		case TERFILE_STOP_POINT:
			{
				/*int size = m_TerrainMapInfo.GetTotalTileCount(); 
				BYTE *pTemp = _NEW BYTE[size];
				if( pTemp )
				{
					mem = f_MemRead( pTemp, mem, sizeof(BYTE) * size );
					m_StopPoint.ImportFromMemory( pTemp );
					SAFE_DELETE_ARRAY( pTemp );
				}*/
			}
			break;
		case TERFILE_FIXED_TIME:
			{
				/*int iFixedTime = -1;
				mem = f_MemRead( &iFixedTime, mem, sizeof(int) );
				SetFixedTime( iFixedTime );*/
			}
			break;
		case TERFILE_CLOUD_TEXNAME1:
			{
				// 弄禟瓜
				//if( iFileSize < sizeof(int) )
				//{
				//	return -1;
				//}
				//iFileSize -= sizeof(int); 
				//mem = f_MemRead( &TempInt, mem, sizeof(int) );

				//// 弄禟瓜郎
				//if( (DWORD)iFileSize < sizeof(char) * TempInt ) 
				//	return -1;

				//iFileSize -= (sizeof(char) * TempInt); 
				//mem = f_MemRead( &szCh, mem, sizeof(char) * TempInt );
				//szCh[TempInt] = '\0';

				//if( g_pIWeatherSystem )
				//{
				//	g_pIWeatherSystem->SetCloudTexture1( szCh );
				//}
			}
			break;
		case TERFILE_CLOUD_TEXNAME2:
			{
				// 弄禟瓜
				//if( iFileSize < sizeof(int) )
				//{
				//	return -1;
				//}
				//iFileSize -= sizeof(int); 
				//mem = f_MemRead( &TempInt, mem, sizeof(int) );

				//// 弄禟瓜郎
				//if( (DWORD)iFileSize < sizeof(char) * TempInt ) 
				//	return -1;

				//iFileSize -= (sizeof(char) * TempInt); 
				//mem = f_MemRead( &szCh, mem, sizeof(char) * TempInt );
				//szCh[TempInt] = '\0';

				//if( g_pIWeatherSystem )
				//{
				//	g_pIWeatherSystem->SetCloudTexture2( szCh );
				//}
			}
			break;
		case TERFILE_CLOUD_TEXNAME3:
			{
				// 弄禟瓜
				//if( iFileSize < sizeof(int) )
				//{
				//	return -1;
				//}
				//iFileSize -= sizeof(int); 
				//mem = f_MemRead( &TempInt, mem, sizeof(int) );

				//// 弄禟瓜郎
				//if( (DWORD)iFileSize < sizeof(char) * TempInt ) 
				//	return -1;

				//iFileSize -= (sizeof(char) * TempInt); 
				//mem = f_MemRead( &szCh, mem, sizeof(char) * TempInt );
				//szCh[TempInt] = '\0';

				//if( g_pIWeatherSystem )
				//{
				//	g_pIWeatherSystem->SetCloudTexture3( szCh );
				//}
			}
			break;
		case TERFILE_WATER_TEXNAME:
			{
				//// 弄嘿
				//if( iFileSize < sizeof(int) )
				//{
				//	return -1;
				//}
				//iFileSize -= sizeof(int);
				//mem = f_MemRead( &TempInt, mem, sizeof(int) );

				//// 弄嘿
				//if( (DWORD)iFileSize < sizeof(char) * TempInt )
				//{
				//	return -1;
				//}

				//iFileSize -= sizeof(char) * TempInt;
				//mem = f_MemRead( &szCh, mem, sizeof(char) * TempInt );
				//szCh[TempInt] = '\0';
				//if( g_mWater )
				//{
				//	g_mWater->SetTextureName( szCh );
				//}
			}
			break;
		case TERFILE_SHOW_SUN:
			{
				/*if( iFileSize < sizeof(BYTE) )
				{
					return -1;
				}
				iFileSize -= sizeof(BYTE);
				mem = f_MemRead( &TempBYTE, mem, sizeof(BYTE) );
				if( TempBYTE == 1 && g_pIWeatherSystem )
				{
					g_pIWeatherSystem->SetShowSun( false );
				}*/
			}
			break;
		case TERFILE_MESH_SCALE:
			{
				this->stream->Read(&this->terrain->tileMeshScale, sizeof(BYTE));
			}
			break;
		case TERFILE_FOG:
			{
				//DWORD dwFogStart, dwFogEnd, dwFogColor;

				//// Fog Start
				//if( iFileSize < sizeof(DWORD) ) 
				//{
				//	return -1;
				//}
				//iFileSize -= sizeof(DWORD); 
				//mem = f_MemRead( &dwFogStart, mem, sizeof(DWORD) );

				//// Fog End
				//if( iFileSize < sizeof(DWORD) ) 
				//{
				//	return -1;
				//}
				//iFileSize -= sizeof(DWORD); 
				//mem = f_MemRead( &dwFogEnd, mem, sizeof(DWORD) );

				//// Fog Color
				//if( iFileSize < sizeof(DWORD) ) 
				//{
				//	return -1;
				//}
				//iFileSize -= sizeof(DWORD); 
				//mem = f_MemRead( &dwFogColor, mem, sizeof(DWORD) );

				//if( g_mFogManage )
				//{
				//	if( dwFogStart == 0 && dwFogEnd == 0 )
				//	{
				//		g_mFogManage->SetEnable( false );
				//	}
				//	else
				//	{
				//		g_mFogManage->SetFogDistance( (float)dwFogStart, (float)dwFogEnd );

				//		// 060414 cyhsieh 铭砞﹚
				//		BYTE byR = (BYTE)dwFogColor & 0xFF;
				//		BYTE byG = (BYTE)( dwFogColor >> 8 ) & 0xFF;
				//		BYTE byB = (BYTE)( dwFogColor >> 16 ) & 0xFF;
				//		dwFogColor = 0xFF000000 | ( byR << 16 ) | ( byG << 8 ) | byB;
				//		g_mFogManage->SetFogColor( dwFogColor );

				//		g_mFogManage->SetEnable( true );
				//	}
				//}
			}
			break;
		case TERFILE_WATERREFLECT_COLOR:
			{
				/*if( iFileSize < sizeof(DWORD) ) 
				{
					return -1;
				}
				iFileSize -= sizeof(DWORD); 
				mem = f_MemRead( &m_dwWaterReflectColor, mem, sizeof(DWORD) );*/
			}
			break;
		case TERFILE_SPEEDTREE:
			{
				/*Offset = LoadSpeedTreeData( mem, iFileSize );
				mem += Offset;*/
			}
			break;
		case TERFILE_SUN_PITCHYAWANGLE:
			{
				/*if( iFileSize < sizeof(DWORD) )
				{
					return -1;
				}
				iFileSize -= sizeof(DWORD); 
				mem = f_MemRead( &TempDWORD, mem, sizeof(DWORD) );
				if( g_mLights )
				{
					WORD wPitchAngle = (WORD)( ( TempDWORD >> 16 ) & 0xFFFF );
					WORD wYawAngle = (WORD)( TempDWORD & 0xFFFF );
					g_mLights->SetSunPitchYawAngle( wPitchAngle, wYawAngle );
				}*/
			}
			break;
		case TERFILE_VENDOR_AREA:
			{
				//// 耚舥跋
				//if( iFileSize < 4 * sizeof(WORD) )
				//{
				//	return -1;
				//}
				//iFileSize -= 4 * sizeof(WORD);

				//WORD wStartX, wStartZ, wEndX, wEndZ;
				//// 絛瞅癬﹍X
				//mem = f_MemRead( &wStartX, mem, sizeof(WORD) );
				//// 絛瞅癬﹍Z
				//mem = f_MemRead( &wStartZ, mem, sizeof(WORD) );
				//// 絛瞅沧翴X
				//mem = f_MemRead( &wEndX, mem, sizeof(WORD) );
				//// 絛瞅沧翴Z
				//mem = f_MemRead( &wEndZ, mem, sizeof(WORD) );

				//sAreaRange* pAreaRange = AddAreaRange( E_AREATYPE_VENDOR );
				//if( pAreaRange )
				//{
				//	pAreaRange->m_wStartX = wStartX;
				//	pAreaRange->m_wStartZ = wStartZ;
				//	pAreaRange->m_wEndX = wEndX;
				//	pAreaRange->m_wEndZ = wEndZ;
				//}
			}
			break;
		case TERFILE_DANCE_AREA:
			{
				//// 铬籖跋
				//if( iFileSize < 4 * sizeof(WORD) )
				//{
				//	return -1;
				//}
				//iFileSize -= 4 * sizeof(WORD);

				//WORD wStartX, wStartZ, wEndX, wEndZ;
				//// 絛瞅癬﹍X
				//mem = f_MemRead( &wStartX, mem, sizeof(WORD) );
				//// 絛瞅癬﹍Z
				//mem = f_MemRead( &wStartZ, mem, sizeof(WORD) );
				//// 絛瞅沧翴X
				//mem = f_MemRead( &wEndX, mem, sizeof(WORD) );
				//// 絛瞅沧翴Z
				//mem = f_MemRead( &wEndZ, mem, sizeof(WORD) );

				//sAreaRange* pAreaRange = AddAreaRange( E_AREATYPE_DANCE );
				//if( pAreaRange )
				//{
				//	pAreaRange->m_wStartX = wStartX;
				//	pAreaRange->m_wStartZ = wStartZ;
				//	pAreaRange->m_wEndX = wEndX;
				//	pAreaRange->m_wEndZ = wEndZ;
				//}
			}
			break;
		case TERFILE_SAFE_AREA:
			{
				//// 跋
				//if( iFileSize < 4 * sizeof(WORD) )
				//{
				//	return -1;
				//}
				//iFileSize -= 4 * sizeof(WORD);

				//WORD wStartX, wStartZ, wEndX, wEndZ;
				//// 絛瞅癬﹍X
				//mem = f_MemRead( &wStartX, mem, sizeof(WORD) );
				//// 絛瞅癬﹍Z
				//mem = f_MemRead( &wStartZ, mem, sizeof(WORD) );
				//// 絛瞅沧翴X
				//mem = f_MemRead( &wEndX, mem, sizeof(WORD) );
				//// 絛瞅沧翴Z
				//mem = f_MemRead( &wEndZ, mem, sizeof(WORD) );

				//sAreaRange* pAreaRange = AddAreaRange( E_AREATYPE_SAFE );
				//if( pAreaRange )
				//{
				//	pAreaRange->m_wStartX = wStartX;
				//	pAreaRange->m_wStartZ = wStartZ;
				//	pAreaRange->m_wEndX = wEndX;
				//	pAreaRange->m_wEndZ = wEndZ;
				//}
			}
			break;
		case TERFILE_FISHING_AREA:
			{
				//// 敞辰跋
				//if( iFileSize < 4 * sizeof(WORD) )
				//{
				//	return -1;
				//}
				//iFileSize -= 4 * sizeof(WORD);

				//WORD wStartX, wStartZ, wEndX, wEndZ;
				//// 絛瞅癬﹍X
				//mem = f_MemRead( &wStartX, mem, sizeof(WORD) );
				//// 絛瞅癬﹍Z
				//mem = f_MemRead( &wStartZ, mem, sizeof(WORD) );
				//// 絛瞅沧翴X
				//mem = f_MemRead( &wEndX, mem, sizeof(WORD) );
				//// 絛瞅沧翴Z
				//mem = f_MemRead( &wEndZ, mem, sizeof(WORD) );

				//sAreaRange* pAreaRange = AddAreaRange( E_AREATYPE_FISHING );
				//if( pAreaRange )
				//{
				//	pAreaRange->m_wStartX = wStartX;
				//	pAreaRange->m_wStartZ = wStartZ;
				//	pAreaRange->m_wEndX = wEndX;
				//	pAreaRange->m_wEndZ = wEndZ;
				//}
			}
			break;
		case TERFILE_AREARANGE_LIST:
			{
				//// 跋办絛瞅
				//BYTE byAreaCount;
				//BYTE byAreaType;
				//BYTE byAreaRangeCount;
				//sAreaRange* pAreaRange;
				//WORD wStartX, wStartZ, wEndX, wEndZ;

				//if( iFileSize < sizeof(BYTE) )
				//{
				//	return -1;
				//}
				//iFileSize -= sizeof(BYTE);

				//// 跋办摸计秖
				//mem = f_MemRead( &byAreaCount, mem, sizeof(BYTE) );

				//for( BYTE i = 0; i < byAreaCount; i++ )
				//{
				//	if( iFileSize < 2 * sizeof(BYTE) )
				//	{
				//		return -1;
				//	}
				//	iFileSize -= 2 * sizeof(BYTE);

				//	// 摸
				//	mem = f_MemRead( &byAreaType, mem, sizeof(BYTE) );
				//	// 计秖
				//	mem = f_MemRead( &byAreaRangeCount, mem, sizeof(BYTE) );

				//	int iDataSize = 4 * sizeof(WORD) * byAreaRangeCount;
				//	if( iFileSize < iDataSize )
				//	{
				//		return -1;
				//	}
				//	iFileSize -= iDataSize;

				//	// 絛瞅
				//	for( BYTE j = 0; j < byAreaRangeCount; j++ )
				//	{
				//		// 癬﹍X
				//		mem = f_MemRead( &wStartX, mem, sizeof(WORD) );
				//		// 癬﹍Z
				//		mem = f_MemRead( &wStartZ, mem, sizeof(WORD) );
				//		// 沧翴X
				//		mem = f_MemRead( &wEndX, mem, sizeof(WORD) );
				//		// 沧翴Z
				//		mem = f_MemRead( &wEndZ, mem, sizeof(WORD) );
				//		if( g_pGameTerrain )
				//		{
				//			pAreaRange = g_pGameTerrain->AddAreaRange( byAreaType );
				//			if( pAreaRange )
				//			{
				//				pAreaRange->m_wStartX = wStartX;
				//				pAreaRange->m_wStartZ = wStartZ;
				//				pAreaRange->m_wEndX = wEndX;
				//				pAreaRange->m_wEndZ = wEndZ;
				//			}
				//		}
				//	}
				//}
			}
		}
	}

	return true;
}

void 
TerrainReader::ReadString(String& str, int len)
{
	MemoryMark mm(MemStack);

	char* buf = new(MemStack, len+1)char;
	/*str.Reserve(len+1);
	char* buf = (char*)str.AsCharPtr();*/
	this->stream->Read(buf, len);
	buf[len] = 0;
	str = buf;
}

void
TerrainReader::LoadModels(/*const Ptr<Stream>& stream*/)
{
	MemoryMark mm(MemStack);

	BYTE *pData = NULL;
	//sThingModelData *TempModelData = NULL;
	//cThing *TempThingData = NULL;

	DWORD l_TempDWORD  = 0 ;
	int   l_TempINT    = 0 ;
	bool  l_TempBool   = false ;
	int   l_iModelSize = 0 ;
	int   l_iObjSize   = 0 ;
	int   i,j;
	int   l_iClass , l_iType;

	DWORD dwSpecialID = 0; // 特殊ID

	// 模型档信息
	HashMap<DWORD, sThingModelData, Util::DwordCompare> models;

	// 读取地上物模型清单数量
	stream->Read(&l_iModelSize, sizeof(int));

	for(i=0;i<l_iModelSize;i++)
	{
		//TempModelData = NULL;
		// 读取模型的ID
		stream->Read(&l_TempDWORD, sizeof(DWORD));

		// 读取Class id
		stream->Read(&l_iClass, sizeof(int));

		// 读取ThingType
		stream->Read(&l_iType, sizeof(int));
		int   iNameSize = 0;

		// 读取模型档名字串长度
		stream->Read(&iNameSize, sizeof(int));

		if(iNameSize > 0)
		{
			char *l_szChar = new(MemStack, iNameSize+1)char;
			Memory::Clear(l_szChar, iNameSize+1);

			// 读取模型档名
			stream->Read(l_szChar, sizeof(char)*iNameSize);
			l_szChar[iNameSize] = '\0';

			//TempModelData = AddModelDataID(l_TempDWORD,l_iClass , l_iType,l_szChar);
			sThingModelData model;
			model.dwThingModelID = l_TempDWORD;
			model.iClass = l_iClass;
			model.iType = l_iType;
			model.szModelName = l_szChar;
			models.Add(l_TempDWORD, model);

			if(l_TempDWORD == 0)
			{
				//（严重）不因该有这样的值
				dwSpecialID = model.dwThingModelID;
			}
		}
	}

	//cThing *TempData = NULL;
	int   l_dThingSzie = 0 ;

	// 读取地上物的数量
	stream->Read(&l_dThingSzie, sizeof(int));

	DWORD l_dwThingID = 0 ;
	DWORD l_dwModelDataID = 0 ;
	int   l_iTempX = 0;
	int   l_iTempY = 0;
	int   l_iTempZ = 0;
	int   l_iTempRightAngle = 0;
	int   l_iTempState = 0 ;
	vector vecTempDistance;
//	bool load_backend;

	for(i=0;i<l_dThingSzie;i++)
	{
		// 读取地上物的ID
		//TempData = NULL;

		stream->Read(&l_dwThingID, sizeof(DWORD));

		// 读取模型的ID
		stream->Read(&l_dwModelDataID, sizeof(DWORD));

		if(l_dwModelDataID == 0)
		{
			//（严重）不因该有这样的值
			l_dwModelDataID = dwSpecialID;
		}  

		stream->Read(&l_iTempX, sizeof(int));		// 读取 X 座标
		stream->Read(&l_iTempY, sizeof(int));		// 读取 Y 座标
		stream->Read(&l_iTempZ, sizeof(int));		// 读取 Z 座标
		/*vecTempDistance=vector((float)l_iTempX,(float)l_iTempY,(float)l_iTempZ)-m_vecSceneInitPos;
		if (vecTempDistance.length()>=m_fBackEndLoadDistance)
		{
			load_backend=true;
		}
		else
		{
			load_backend=false;
		}*/

		stream->Read(&l_iTempRightAngle, sizeof(int)); // 读取方向角

		// 读取状态
		stream->Read(&l_iTempState, sizeof(int));

		// 读取显示的ID
		int l_iRepeatID = 0 ;
		stream->Read(&l_iRepeatID, sizeof(int));

		Ptr<ThingEntity> entity;
		sThingModelData model;
		if (models.Find(l_dwModelDataID, model))
		{
			entity = AddThing(model);
		}

		//TempModelData = ForageModelDataID(l_dwModelDataID);

		/*if( TempModelData )
		{
			// 解析加载模型
			TempData = AddThing(l_dwThingID,TempModelData->iClass ,
				TempModelData->iType,TempModelData->szModelName,load_backend);
		}*/

		//if(TempData) 
		//	TempData->m_dwModelDataID = l_dwModelDataID;

		//if(l_iFileSize < (sizeof(int)*7)) return -1;
		//l_iFileSize -= (sizeof(int)*7); 

		

		// 读取物件数量
		stream->Read(&l_iObjSize, sizeof(int));

		for(j=0;j<l_iObjSize;j++)
		{
			// 读取物件的贴图
			stream->Read(&l_TempINT, sizeof(int));

			if (entity.isvalid())
				entity->SetTextureId(j, l_TempINT);
			/*if(TempData)
				TempData->SetTexture(j,l_TempINT);*/

		}

		matrix44 matTranslation, matScale, matRotate;
		matTranslation = matrix44::identity();
		matScale = matrix44::identity();
		matRotate = matrix44::identity();
		if(entity.isvalid())
		{
			float reviseX = 0.0f;
			float reviseZ = 0.0f;
			int tempAngle = l_iTempRightAngle;
			if (tempAngle < 0)
				tempAngle += 4;
			switch(tempAngle)
			{
			case 1:
				{
					reviseX = 0.0f;
					reviseZ = COMP * 1.0f;
				}
				break;
			case 2:
				{
					reviseX = COMP * 1.0f;
					reviseZ = COMP * 1.0f;
				}
				break;
			case 3:
				{
					reviseX = COMP * 1.0f;
					reviseZ = 0.0f;
				}
				break;
			}

			matTranslation = matrix44::translation(Math::vector(l_iTempX + reviseX, l_iTempY, l_iTempZ + reviseZ));
			 
			//TempData->m_iX = l_iTempX;
			//TempData->m_iY = l_iTempY;
			//TempData->m_iZ = l_iTempZ;
			//TempData->m_iRightAngle = l_iTempRightAngle;
			//TempData->SetRepeatID(l_iRepeatID);
			////TempData->iRepeatID   = l_iRepeatID;
			//TempData->m_iState      = l_iTempState;

			/*if(l_iTempState)
			{
			l_iTempState = l_iTempState;
			}*/
		}

		// 有设定旋转或放大缩小
		if( l_iTempRightAngle < 0 ) 
		{
			float fScale;

			stream->Read(&fScale, sizeof(float));
			matScale = matrix44::scaling(fScale, fScale, fScale);

			stream->Read(&matRotate, sizeof(matrix44));

			/*if (TempData)
			{
				TempData->m_mxScale=matScale;
				TempData->m_mxRotate=matRotate;
			}*/
		}
		else
		{
			// 旋转几个直角
			float direction = l_iTempRightAngle * 90.0f;
			matRotate = matrix44::rotationy((((int)direction+360)%360)*(N_PI/180.0f));
		}

		/*matrix44 matBlend;
		Math::vector waveRotationAxis = Math::vector(1.0f, 0.0f, 0.0f);
		matrix44::rotationaxis(waveRotationAxis, n_sin(reviseX + reviseZ + waveTime));*/

		matrix44 transform;
		transform = matrix44::identity();
		transform = matrix44::multiply(transform, matScale);
		transform = matrix44::multiply(transform, matRotate);
		transform = matrix44::multiply(transform, matTranslation);
		entity->SetTransform(transform);

		/*if( TempData )
		{
			TempData->setIntoSceneMap();
		}*/
	}
}

void
TerrainReader::LoadAudios(/*const Ptr<Stream>& stream*/)
{
	MemoryMark mm(MemStack);

	DWORD l_TempDWORD        = 0 ;
	int i;

	int l_iSoundSize = 0;
	char *l_szChar = new(MemStack, 80)char;

	String szFilePath;

	// 读取声音档清单数量
	stream->Read(&l_iSoundSize, sizeof(int));

	for(i=0;i<l_iSoundSize;i++)
	{
		// 读取声音档的ID
		stream->Read(&l_TempDWORD, sizeof(DWORD));
		int   l_iNameSize = 0;

		// 读取声音档名字串长度
		stream->Read(&l_iNameSize, sizeof(int));
		Memory::Clear(l_szChar, sizeof(char)*80);

		// 读取声音档档名
		stream->Read(&l_szChar, sizeof(char)*l_iNameSize);

		//AddAudio(AUDIO_FT_3D,l_szChar,l_TempDWORD) ;
	}
}

Ptr<ThingEntity>
TerrainReader::AddThing(const sThingModelData& model)
{
	String name;

	switch(model.iClass)
	{
	case 0: name = "mbld:"; break;
	case 1: name = "mart:"; break;
	case 2: name = "mnat:"; break;
	case 3: name = "mitr:"; break;
	case 4: name = "mclf:"; break;
	}

	name += model.szModelName;
	name += ".obj";

	Ptr<ThingEntity> entity = ThingEntity::Create();
	entity->SetResourceId(name);

	this->terrain->thingEntitys.Append(entity);

	return entity;
}

} // namespace Models
