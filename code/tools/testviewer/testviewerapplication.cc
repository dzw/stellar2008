//------------------------------------------------------------------------------
//  testviewerapplication.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "tools/testviewer/testviewerapplication.h"
#include "frame/frameserver.h"
#include "coregraphics/debugview.h"
#include "resources/resourcemanager.h"
#include "resources/simpleresourcemapper.h"
#include "graphics/graphicsserver.h"
#include "kok/load/streamterrainloader.h"
#include "io/ioserver.h"
#include "interface/iomsg/mountlpqarchive.h "
#include "kok/model/thing.h"
#include "kok/model/managedthing.h"
#include "kok/model/modeldef.h"
#include "kok/terrain/terrain.h"
#include "kok/terrain/managedterrain.h"
#include "kok/model/managedBeing.h"

namespace Tools
{
using namespace Graphics;
using namespace Math;
using namespace Util;
using namespace Lighting;
using namespace Resources;
using namespace Timing;
using namespace KOK;
using namespace IO;

char* s_aszMPName[mptCOUNT] =
{
	"m_body",  // mptBody = 0, //身体model
	"m_hair",  // mptHair,     //发型model
	"m_head",  // mptHead,     //头部model
	"m_handA", // mptHand,     //左右手掌
	"m_handB", // mptArm,      //左右小臂
	"m_pants", // mptPants,    //下半身model
	"m_footA", // mptFoot,     //左右脚掌
	"m_footB", // mptLeg,      //左右小腿
	"m_cloak", //	mptCloak,    //披风
};

/// 测试代码
String 
GetFilePath(BeingFilePath l_FileType)
{
	String szPath;
	String szMainFilePath = ""; //g_sAppPath.c_str();

	switch( 0 )
	{
	case BEING_TYPE_PLAY:
		szMainFilePath += "Mesh\\Being\\player\\";
		switch(l_FileType)
		{
		case BFP_FRAME:     // 框架
			//szPath = szMainFilePath + "Frame\\";
			szPath = "mbfrm:";
			break;  
		case BFP_BODY:      // 生体或上衣
			//szPath = szMainFilePath + "Body\\";
			szPath = "mbbdy:";
			break;  
		case BFP_HAIR:      // 头发
			//szPath = szMainFilePath + "Hair\\";
			szPath = "mbhar:";
			break;  
		case BFP_HEAD:      // 头及脸型
			//szPath = szMainFilePath + "Head\\";
			szPath = "mbhed:";
			break;  
		case BFP_PANTS:     // 裤子
			//szPath = szMainFilePath + "Pants\\";
			szPath = "mbpat:";
			break;
		case BFP_HAND:      // 手掌
			//szPath = szMainFilePath + "Hand\\";
			szPath = "mbhad:";
			break;
		case BFP_ARM:       // 手臂
			//szPath = szMainFilePath + "Hand\\";
			szPath = "mbarm:";
			break;
		case BFP_FOOT:      // 脚掌
			//szPath = szMainFilePath + "foot\\";
			szPath = "mbfot:";
			break;
		case BFP_LEG:       // 小腿
			//szPath = szMainFilePath + "foot\\";
			szPath = "mbleg:";
			break;
		case BFP_CLOAK:     // 披风
			//szPath = szMainFilePath + "cloak\\";
			szPath = "mbclk:";
			break;
		}
		break;

	case BEING_TYPE_NPC:
		szMainFilePath += "Mesh\\Being\\npc\\";
		szPath = szMainFilePath;
		break;

	case BEING_TYPE_MOB:
		szMainFilePath += "Mesh\\Being\\mob\\";
		szPath = szMainFilePath;
		break;

	case BEING_TYPE_MOUNT:
		szMainFilePath += "Mesh\\Being\\Mount\\";
		switch( l_FileType )
		{
		case BFP_FRAME:
			szPath = szMainFilePath + "Frame\\";
			break;
		case MFP_BODY:
			szPath = szMainFilePath + "Body\\";
			break;
		case MFP_ARMOR:
			szPath = szMainFilePath + "Armor\\";
			break;
		}
		break;
	}

	return szPath;
}


//------------------------------------------------------------------------------
/**
*/
TestViewerApplication::TestViewerApplication()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TestViewerApplication::~TestViewerApplication()
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
TestViewerApplication::Open()
{
    n_assert(!this->IsOpen());
    if (ViewerApplication::Open())
    {
		// setup resource mapper for meshes
		Ptr<SimpleResourceMapper> meshMapper = SimpleResourceMapper::Create();
		//meshMapper->SetPlaceholderResourceId(ResourceId("msh:system/placeholder_s_0.nvx2"));
		meshMapper->SetResourceClass(KOK::Terrain::RTTI);
		meshMapper->SetResourceLoaderClass(StreamTerrainLoader::RTTI);
		meshMapper->SetManagedResourceClass(ManagedTerrain::RTTI);
		ResourceManager::Instance()->AttachMapper(meshMapper.upcast<ResourceMapper>());

		meshMapper = SimpleResourceMapper::Create();
		//meshMapper->SetPlaceholderResourceId(ResourceId("msh:system/placeholder_s_0.nvx2"));
		meshMapper->SetResourceClass(Being::RTTI);
		meshMapper->SetResourceLoaderClass(StreamTerrainLoader::RTTI);
		meshMapper->SetManagedResourceClass(ManagedBeing::RTTI);
		ResourceManager::Instance()->AttachMapper(meshMapper.upcast<ResourceMapper>());

		meshMapper = SimpleResourceMapper::Create();
		//meshMapper->SetPlaceholderResourceId(ResourceId("msh:system/placeholder_s_0.nvx2"));
		meshMapper->SetResourceClass(Thing::RTTI);
		meshMapper->SetResourceLoaderClass(StreamTerrainLoader::RTTI);
		meshMapper->SetManagedResourceClass(ManagedThing::RTTI);
		ResourceManager::Instance()->AttachMapper(meshMapper.upcast<ResourceMapper>());

		// 设置路径
		if (this->args.HasArg("-path"))
		{
			String path = this->args.GetString("-path");

			IoServer::Instance()->SetAssign(Assign("KOK", path));
		}
		else
			n_assert(0);

		// 地物路径
		IoServer::Instance()->SetAssign(Assign("mbld", "Mesh\\Building"));	// 建筑物
		IoServer::Instance()->SetAssign(Assign("mart", "Mesh\\Article"));	// 地上物
		IoServer::Instance()->SetAssign(Assign("mnat", "Mesh\\Nature"));	// 自然物
		IoServer::Instance()->SetAssign(Assign("mitr", "Mesh\\Interior"));	// 空内物
		IoServer::Instance()->SetAssign(Assign("mclf", "Mesh\\Cliff"));		// 悬崖

		// 生物路径  不需要写这么多路径，一个就够了。(Mesh\\Being\\Player)
		IoServer::Instance()->SetAssign(Assign("mbfrm", "Mesh\\Being\\Player\\Frame"));		// 框架（包含骨骼、动作等文件）
		IoServer::Instance()->SetAssign(Assign("mbbdy", "Mesh\\Being\\Player\\Body"));		// 生体或上衣
		IoServer::Instance()->SetAssign(Assign("mbhar", "Mesh\\Being\\Player\\Hair"));		// 头发
		IoServer::Instance()->SetAssign(Assign("mbhed", "Mesh\\Being\\Player\\Head"));		// 头及脸型
		IoServer::Instance()->SetAssign(Assign("mbpat", "Mesh\\Being\\Player\\Pants"));		// 裤子
		IoServer::Instance()->SetAssign(Assign("mbhad", "Mesh\\Being\\Player\\Hand"));		// 手掌
		IoServer::Instance()->SetAssign(Assign("mbarm", "Mesh\\Being\\Player\\Hand"));		// 手臂
		IoServer::Instance()->SetAssign(Assign("mbfot", "Mesh\\Being\\Player\\foot"));		// 脚掌
		IoServer::Instance()->SetAssign(Assign("mbleg", "Mesh\\Being\\Player\\foot"));		// 小腿
		IoServer::Instance()->SetAssign(Assign("mbclk", "Mesh\\Being\\Player\\cloak"));		// 披风

		IoServer::Instance()->SetAssign(Assign("mbnpc", "Mesh\\Being\\npc"));				// npc
		IoServer::Instance()->SetAssign(Assign("mbmob", "Mesh\\Being\\mob"));				// 怪物
		IoServer::Instance()->SetAssign(Assign("mbmfm", "Mesh\\Being\\Mount\\Frame"));		// 坐骑框架
		IoServer::Instance()->SetAssign(Assign("mbmbd", "Mesh\\Being\\Mount\\Body"));		// 坐骑体身
		IoServer::Instance()->SetAssign(Assign("mbmar", "Mesh\\Being\\Mount\\Armor"));		// 坐骑状备

		IoServer::Instance()->SetAssign(Assign("mequp", "Mesh\\Equipment"));				// 装备

		IoServer::Instance()->SetAssign(Assign("mtext", "Mesh\\Being\\Tex"));				// 物生纹理

		LoadLPQFile();

		this->view->SetFrameShader(Frame::FrameServer::Instance()->GetFrameShaderByName(ResourceId("DX9Sample")));

		// 地物模型	
		//alpha测试:mart:m_hal055.obj
		//effect测试:mart:m_hal022.obj
		this->thingEntity = ThingEntity::Create();
		this->thingEntity->SetResourceId(ResourceId("mart:m_hal022.obj"));
		this->thingEntity->SetTextureId(0, 0);
		this->thingEntity->SetTextureId(1, 0);
		this->stage->AttachEntity(this->thingEntity.upcast<GraphicsEntity>());
 
		//// 骨骼动画
		//this->skeletonFactory = cSkeletonSerializerFactory::Create();
		//this->skeletonFactory->Open();

		//this->beingEntity = BeingEntity::Create();
		//for (int i = 0; i < mptCloak; i++)
		//{
		//	String path = GetFilePath((BeingFilePath)(i+1));
		//	String modelName;
		//	switch(i)
		//	{
		//	case mptHair:
		//	case mptHead:
		//		modelName.Format("%s%s%1d%1d%03d.atm", path.AsCharPtr(), s_aszMPName[i], sktMale, 0, 1);
		//		break;
		//	default:
		//		modelName.Format("%s%s%1d%1d%03d.atm", path.AsCharPtr(), s_aszMPName[i], sktMale, 0, 0);
		//	}
		//	this->beingEntity->SetPart(i, modelName, 1);
		//}
		////this->beingEntity->SetTransform(matrix44::translation(0.0f, 0.0f, 0.0f));
		//CreateSkeleton(this->beingEntity, "ph1");
		//this->beingEntity->CreateAnimationAction(100, 1.0, aoDefaultKeyframes, 0.2);
		//this->stage->AttachEntity(this->beingEntity.upcast<GraphicsEntity>());

        return true;
    }
    return false;
}

void
TestViewerApplication::CreateSkeleton(const Ptr<BeingEntity>& entity, const String& skeletonName)
{
	String szPath; 
	BeingType1 iModelType;
	szPath = GetFilePath(BFP_FRAME);
	iModelType = mtPLAYER;
	/*switch (BeingType)
	{
	case BEING_TYPE_PLAY:
		iModelType = mtPLAYER;
		break;
	case BEING_TYPE_MOB:
		iModelType = mtMOB;
		break;
	case BEING_TYPE_NPC:
		iModelType = mtNPC;
		break;
	case BEING_TYPE_MOUNT:
		iModelType = mtMOUNT;
		break;
	}*/

	entity->CreateSkeleton(iModelType, skeletonName, szPath);
}

//------------------------------------------------------------------------------
/**
*/
void
TestViewerApplication::Close()
{
	this->beingEntity = 0;
	this->thingEntity = 0;
	if (this->skeletonFactory.isvalid())
		this->skeletonFactory->Close();
	this->skeletonFactory = 0;

    ViewerApplication::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
TestViewerApplication::OnUpdateFrame()
{

    ViewerApplication::OnUpdateFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
TestViewerApplication::LoadLPQFile()
{
	const char* archiveNames[] = { 
		"audio.lpq",
		"config.lpq", 
		"cttw.lpq",
		"effect.lpq",
		"help.lpq",
		"icons.lpq",
		"interface.lpq",
		"lights.lpq",
		"map.lpq",
		"mapdata.lpq",
		"mesh.lpq", 
		"shader.lpq",
		"shadowmap.lpq",
		"shore.lpq",
		"speedtree.lpq"
	};

	String path;
	for (int i = 0; i < sizeof(archiveNames)/sizeof(archiveNames[0]); i++)
	{
		path = "KOK:";
		path.Append(archiveNames[i]);

		if (IoServer::Instance()->FileExists(path))
		{
			// io local
			this->ioServer->MountLPQArchive(path);

			// io thread
			Ptr<Interface::MountLPQArchive> mountLPQArchiveMsg = Interface::MountLPQArchive::Create();
			mountLPQArchiveMsg->SetURI(path);
			this->ioInterface->Send(mountLPQArchiveMsg.upcast<Messaging::Message>());
		}
	}
}

} // namespace Tools
