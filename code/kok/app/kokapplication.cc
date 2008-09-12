//------------------------------------------------------------------------------
//  testviewerapplication.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "app/kokapplication.h"
#include "io/ioserver.h"
#include "interface/iomsg/mountlpqarchive.h "
#include "input/keyboard.h"
#include "input/mouse.h"
#include "math/vector.h"

#include "resources/resourcemanager.h"
#include "resources/simpleresourcemapper.h"
#include "graphics/graphicsserver.h"
#include "kok/load/streamterrainloader.h"

#include "kok/model/thing.h"
#include "kok/model/managedthing.h"
#include "kok/model/modeldef.h"
#include "graphics/simplestagebuilder.h"
#include "frame/frameserver.h"

namespace KOK
{
using namespace Resources;
using namespace IO;
using namespace Util;
using namespace Input;
using namespace Math;
using namespace Graphics;
using namespace App;
using namespace Frame;

String GetFilePath(BeingFilePath l_FileType);

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

//------------------------------------------------------------------------------
/**
*/
KOKApplication::KOKApplication()/*:
	terrainServer(0)*/
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
KOKApplication::~KOKApplication()
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
KOKApplication::Open()
{
    n_assert(!this->IsOpen());
    if (RenderApplication::Open())
	{
		GraphicsServer* gfxServer = GraphicsServer::Instance();

		this->stage = gfxServer->CreateStage(StringAtom("DefaultStage"), SimpleStageBuilder::Create());
		this->dxutCameraUtil.Setup(Math::vector(0, 0, 0/*870, 200, 340*//*-600, 180, 76*/)/*vector(18468.0f, 177.0f, 14292.66f)*/);
		this->cameraEntity = CameraEntity::Create();
		cameraEntity->SetTransform(this->dxutCameraUtil.GetCameraTransform());
		this->stage->AttachEntity(cameraEntity.upcast<GraphicsEntity>());

		// setup a default view
		this->view = gfxServer->CreateView(View::RTTI, StringAtom("DefaultView"), true);
		this->view->SetStage(this->stage);        
		//this->view->SetFrameShader(FrameServer::Instance()->GetFrameShaderByName(ResourceId(DEFAULT_FRAMESHADER_NAME)));
		this->view->SetCameraEntity(cameraEntity);

		// setup resource mapper for meshes
		Ptr<SimpleResourceMapper> meshMapper = SimpleResourceMapper::Create();
		//meshMapper->SetPlaceholderResourceId(ResourceId("msh:system/placeholder_s_0.nvx2"));
		meshMapper->SetResourceClass(Terrain::RTTI);
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

		// ����·��
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

		IoServer::Instance()->SetAssign(Assign("mtext", "Mesh\\Being\\Tex"));				// ��������

		IoServer::Instance()->SetAssign(Assign("mequp", "Mesh\\Equipment"));				// װ��

		LoadLPQFile();

		this->view->SetFrameShader(Frame::FrameServer::Instance()->GetFrameShaderByName(ResourceId("DX9Sample")));

		/*this->terrainServer = TerrainServer::Create();
		this->terrainServer->Open();
		this->terrainServer->LoadTerrain(ResourceId("map\\10300.sce"));*/

		// ����
		this->terrainEntity = TerrainEntity::Create();
		this->terrainEntity->SetResourceId(ResourceId("map\\10300.sce"));
		this->stage->AttachEntity(this->terrainEntity.upcast<GraphicsEntity>());

		// ����ģ��
		/*this->thingEntity = ThingEntity::Create();
		this->thingEntity->SetResourceId(ResourceId("mbld:m_hbs000.obj"));
		this->stage->AttachEntity(this->thingEntity.upcast<GraphicsEntity>());*/

		// ��̬ģ��
		/*this->beingEntity = BeingEntity::Create();
		for (int i = 0; i < mptCloak; i++)
		{
			String path = GetFilePath((BeingFilePath)(i+1));
			String modelName;
			switch(i)
			{
			case mptHair:
			case mptHead:
				modelName.Format("%s\%s%1d%1d%03d.atm", path.AsCharPtr(), s_aszMPName[i], sktMale, 0, 1);
				break;
			default:
				modelName.Format("%s\%s%1d%1d%03d.atm", path.AsCharPtr(), s_aszMPName[i], sktMale, 0, 0);
			}
			this->beingEntity->SetPart(i, modelName, 1);
		}
		this->stage->AttachEntity(this->beingEntity.upcast<GraphicsEntity>());*/

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
KOKApplication::Close()
{
	/*this->terrainServer->Close();
	this->terrainServer = 0;*/
	
	this->terrainEntity = 0;
	this->thingEntity = 0;
	this->beingEntity = 0;

	GraphicsServer* gfxServer = GraphicsServer::Instance();
	gfxServer->DiscardView(this->view);
	this->view = 0;
	gfxServer->DiscardStage(this->stage);
	this->stage = 0;
	this->cameraEntity = 0;

    RenderApplication::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
KOKApplication::OnUpdateFrame()
{
    RenderApplication::OnUpdateFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
KOKApplication::OnRenderFrame()
{
	GraphicsServer::Instance()->OnFrame(this->timer.GetTime());
	//if (isDebugRender)
	//	this->view->RenderDebug();
}

void 
KOKApplication::OnProcessInput()
{
	InputServer* inputServer = InputServer::Instance();
	const Ptr<Keyboard>& keyboard = inputServer->GetDefaultKeyboard();
	const Ptr<Mouse>& mouse = inputServer->GetDefaultMouse();

	this->dxutCameraUtil.SetMouseMovement(mouse->GetMovement());
	this->dxutCameraUtil.MoveUp(mouse->WheelBackward());
	this->dxutCameraUtil.MoveDown(mouse->WheelForward());
	this->dxutCameraUtil.SetLeftButton(mouse->ButtonPressed(MouseButton::LeftButton));

	Math::vector kbDir = Math::vector::nullvec();
	float moveScaler = 5.0f;
	float speedUp = 1.0f;
	// process keyboard input
	if (keyboard->KeyDown(Key::Escape))
	{
		this->SetQuitRequested(true);
	}
	if (keyboard->KeyDown(Key::Space))
	{
		
	}

	if (keyboard->KeyPressed(Key::W))
	{
		kbDir.z() += 0.1f;
	}
	if (keyboard->KeyPressed(Key::S))
	{
		kbDir.z() -= 0.1f;
	}
	if (keyboard->KeyPressed(Key::D))
	{
		// right hand
		//kbDir.x() -= 0.1f;

		// left hand
		kbDir.x() += 0.1f;
	}
	if (keyboard->KeyPressed(Key::A))
	{
		// right hand
		//kbDir.x() += 0.1f;

		// left hand
		kbDir.x() -= 0.1f;
	}
	if (keyboard->KeyPressed(Key::P))
	{
		speedUp += 1.0f;
		this->dxutCameraUtil.SetSpeedUp(speedUp);
	}
	if (keyboard->KeyPressed(Key::O))
	{
		speedUp -= 1.0f;
		this->dxutCameraUtil.SetSpeedUp(speedUp);
	}

	this->dxutCameraUtil.SetMovement(kbDir);
	this->dxutCameraUtil.Update();

	this->cameraEntity->SetTransform(this->dxutCameraUtil.GetCameraTransform());

}

//------------------------------------------------------------------------------
/**
*/
void
KOKApplication::LoadLPQFile()
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

} // namespace Tools
