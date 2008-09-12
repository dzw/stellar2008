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
	"m_body",  // mptBody = 0, //����model
	"m_hair",  // mptHair,     //����model
	"m_head",  // mptHead,     //ͷ��model
	"m_handA", // mptHand,     //��������
	"m_handB", // mptArm,      //����С��
	"m_pants", // mptPants,    //�°���model
	"m_footA", // mptFoot,     //���ҽ���
	"m_footB", // mptLeg,      //����С��
	"m_cloak", //	mptCloak,    //����
};

/// ���Դ���
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
		case BFP_FRAME:     // ���
			//szPath = szMainFilePath + "Frame\\";
			szPath = "mbfrm:";
			break;  
		case BFP_BODY:      // ���������
			//szPath = szMainFilePath + "Body\\";
			szPath = "mbbdy:";
			break;  
		case BFP_HAIR:      // ͷ��
			//szPath = szMainFilePath + "Hair\\";
			szPath = "mbhar:";
			break;  
		case BFP_HEAD:      // ͷ������
			//szPath = szMainFilePath + "Head\\";
			szPath = "mbhed:";
			break;  
		case BFP_PANTS:     // ����
			//szPath = szMainFilePath + "Pants\\";
			szPath = "mbpat:";
			break;
		case BFP_HAND:      // ����
			//szPath = szMainFilePath + "Hand\\";
			szPath = "mbhad:";
			break;
		case BFP_ARM:       // �ֱ�
			//szPath = szMainFilePath + "Hand\\";
			szPath = "mbarm:";
			break;
		case BFP_FOOT:      // ����
			//szPath = szMainFilePath + "foot\\";
			szPath = "mbfot:";
			break;
		case BFP_LEG:       // С��
			//szPath = szMainFilePath + "foot\\";
			szPath = "mbleg:";
			break;
		case BFP_CLOAK:     // ����
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

		// ����·��
		if (this->args.HasArg("-path"))
		{
			String path = this->args.GetString("-path");

			IoServer::Instance()->SetAssign(Assign("KOK", path));
		}
		else
			n_assert(0);

		// ����·��
		IoServer::Instance()->SetAssign(Assign("mbld", "Mesh\\Building"));	// ������
		IoServer::Instance()->SetAssign(Assign("mart", "Mesh\\Article"));	// ������
		IoServer::Instance()->SetAssign(Assign("mnat", "Mesh\\Nature"));	// ��Ȼ��
		IoServer::Instance()->SetAssign(Assign("mitr", "Mesh\\Interior"));	// ������
		IoServer::Instance()->SetAssign(Assign("mclf", "Mesh\\Cliff"));		// ����

		// ����·��  ����Ҫд��ô��·����һ���͹��ˡ�(Mesh\\Being\\Player)
		IoServer::Instance()->SetAssign(Assign("mbfrm", "Mesh\\Being\\Player\\Frame"));		// ��ܣ������������������ļ���
		IoServer::Instance()->SetAssign(Assign("mbbdy", "Mesh\\Being\\Player\\Body"));		// ���������
		IoServer::Instance()->SetAssign(Assign("mbhar", "Mesh\\Being\\Player\\Hair"));		// ͷ��
		IoServer::Instance()->SetAssign(Assign("mbhed", "Mesh\\Being\\Player\\Head"));		// ͷ������
		IoServer::Instance()->SetAssign(Assign("mbpat", "Mesh\\Being\\Player\\Pants"));		// ����
		IoServer::Instance()->SetAssign(Assign("mbhad", "Mesh\\Being\\Player\\Hand"));		// ����
		IoServer::Instance()->SetAssign(Assign("mbarm", "Mesh\\Being\\Player\\Hand"));		// �ֱ�
		IoServer::Instance()->SetAssign(Assign("mbfot", "Mesh\\Being\\Player\\foot"));		// ����
		IoServer::Instance()->SetAssign(Assign("mbleg", "Mesh\\Being\\Player\\foot"));		// С��
		IoServer::Instance()->SetAssign(Assign("mbclk", "Mesh\\Being\\Player\\cloak"));		// ����

		IoServer::Instance()->SetAssign(Assign("mbnpc", "Mesh\\Being\\npc"));				// npc
		IoServer::Instance()->SetAssign(Assign("mbmob", "Mesh\\Being\\mob"));				// ����
		IoServer::Instance()->SetAssign(Assign("mbmfm", "Mesh\\Being\\Mount\\Frame"));		// ������
		IoServer::Instance()->SetAssign(Assign("mbmbd", "Mesh\\Being\\Mount\\Body"));		// ��������
		IoServer::Instance()->SetAssign(Assign("mbmar", "Mesh\\Being\\Mount\\Armor"));		// ����״��

		IoServer::Instance()->SetAssign(Assign("mequp", "Mesh\\Equipment"));				// װ��

		IoServer::Instance()->SetAssign(Assign("mtext", "Mesh\\Being\\Tex"));				// ��������

		LoadLPQFile();

		this->view->SetFrameShader(Frame::FrameServer::Instance()->GetFrameShaderByName(ResourceId("DX9Sample")));

		// ����ģ��	
		//alpha����:mart:m_hal055.obj
		//effect����:mart:m_hal022.obj
		this->thingEntity = ThingEntity::Create();
		this->thingEntity->SetResourceId(ResourceId("mart:m_hal022.obj"));
		this->thingEntity->SetTextureId(0, 0);
		this->thingEntity->SetTextureId(1, 0);
		this->stage->AttachEntity(this->thingEntity.upcast<GraphicsEntity>());
 
		//// ��������
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
