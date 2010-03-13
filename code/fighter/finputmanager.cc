//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "FinputManager.h"
#include "fskillmanager.h"
#include "fcameramanager.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "input/gamepad.h"
#include "input/inputserver.h"
#include "io/stream.h"
#include "io/ioserver.h"
#include "io/xmlreader.h"
#include "fmain.h"

namespace Fighter
{
ImplementClass(Fighter::FInputManager, 'INPT', Core::RefCounted);
ImplementSingleton(Fighter::FInputManager);

using namespace Input;
using namespace IO;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
FInputManager::FInputManager():
isOpen(false),
preSkill(0)
{
    ConstructSingleton;
	
}

//------------------------------------------------------------------------------
/**
*/
FInputManager::~FInputManager()
{
if (this->IsOpen())
    {
        this->Close();
    }
	DestructSingleton;
}

bool 
FInputManager::Open()
{
	n_assert(!this->IsOpen());
	if (!this->ParesRule())
    {
        n_error("Managers::InputRuleManager: Error parsing data:inputrule.xml!");
    }
	this->isOpen = true;

	return true;
}

void 
FInputManager::Close()
{
	n_assert(this->IsOpen());

	this->inputRule.SetSize(0);
	this->hero = 0;
	this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool 
FInputManager::ParesRule()
{
	if (this->inputRule.Size() == 0)
	{
		IoServer* ioServer = IoServer::Instance();
		// create XML reader on file
		Ptr<Stream> stream = ioServer->CreateStream("data:inputrule.xml");
		Ptr<XmlReader> xmlReader = XmlReader::Create();
		xmlReader->SetStream(stream);
		if (xmlReader->Open())
		{
			int charSize = 0;
			if (xmlReader->HasNode("/InputRule"))
			{
				xmlReader->SetToNode("/InputRule");
				n_assert(xmlReader->HasAttr("CharSize"));
				charSize = xmlReader->GetInt("CharSize");
				this->inputRule.SetSize(charSize);
			
				if (xmlReader->SetToFirstChild("Position"))do
				{
					//xmlReader->SetToNode("Position");
					n_assert(xmlReader->HasAttr("Num"));
					int pos = xmlReader->GetInt("Num");
					if (pos < (int)this->inputRule.Size())
					{
						if (xmlReader->SetToFirstChild("Key")) do
						{
							n_assert(xmlReader->HasAttr("KeyValue"));
							this->inputRule[pos].InsertSorted(xmlReader->GetInt("KeyValue"));
						}
						while (xmlReader->SetToNextChild("Key"));
					}
					else
					{
						return false;
					}
				}
				while (xmlReader->SetToNextChild("Position"));
			}
		}
	}

	return true;
}

//------------------------------------------------------------------------------
/**
	��ñ��������ֵ
*/
DWORD 
FInputManager::CheckInvalid(const Util::Array<DWORD>& buffer)const
{
	DWORD val = 0;
	for (SizeT i = 0; i < buffer.Size(); i++)
	{
		if (i >= this->inputRule.Size())
			break;

		IndexT index = this->inputRule[i].BinarySearchIndex(buffer[i]);
		if (index != InvalidIndex)
		{
			val += buffer[i];
		}
	}

	return val;
}

IndexT 
FInputManager::CheckInput(IndexT input)
{
	static float intervalTime = 0.35f;
	static float lastKeyTime = 0.0f;

	FighterApplication* app = (FighterApplication*)FighterApplication::Instance();
	if ((app->GetTime() - lastKeyTime) > intervalTime)	// �������ʱ��
		this->preSkill = 0;
	lastKeyTime = (float)app->GetTime();

	IndexT val = this->preSkill + input;
	FSkillManager* skillManager = FSkillManager::Instance();
	if (skillManager->HasSkillValue(val))
	{
		this->preSkill = val;
		return val;
	}
	// ���ϸ�������ƥ�䣬ֻ����ǰ����
	this->preSkill = 0;
	if (skillManager->HasSkillValue(input))
	{
		this->preSkill = input;
		return input;
	}
	return 0;
}

void 
FInputManager::Update()
{
	if (!this->hero.isvalid())
		return;

	FighterApplication* fighterApp = (FighterApplication*)FighterApplication::Instance();
	InputServer* inputServer = InputServer::Instance();
	const Ptr<Keyboard>& keyboard = inputServer->GetDefaultKeyboard();
	const Ptr<Mouse>& mouse = inputServer->GetDefaultMouse();

	Timing::Time curTime = fighterApp->GetTime();
	if (FCameraManager::HasInstance())
	{
		FCameraManager* cameraManager = FCameraManager::Instance();
		const Math::float2& v = mouse->GetMovement();
		if (mouse->ButtonPressed(MouseButton::RightButton))
		{
			cameraManager->SetCameraOrbit(v.x(), v.y(), (float)fighterApp->GetFrameTime());
		}
		if (mouse->WheelForward())
		{
			cameraManager->SetCameraDistance(-1.0f);
		}
		if (mouse->WheelBackward())
		{
			cameraManager->SetCameraDistance(1.0f);
		}
		cameraManager->UpdateCamera((float)curTime);
	}

	IndexT keyValue    = 0,
		   actionValue = 0;
	bool isDirKeyDown = false;
	
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::J))	// ����
	{
		n_printf("Down J.\n");
		actionValue = CheckInput(SL_Attack);
		//ProcessInputResult(SL_Attack);
		//InputKey k(SL_Attack, curTime);
		//this->keyBuffer.Append(k);
		stopMoving = false;
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::K))	// ��Ծ
	{
		n_printf("Down K.\n");
		actionValue = CheckInput(SL_Jump);
		//ProcessInputResult(SL_Jump);
		//InputKey k(SL_Jump, curTime);
		//this->keyBuffer.Append(k);
		//stopMoving = false;
		stopMoving = false;
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::L))	// ����
	{
		n_printf("Down L.\n");
		actionValue = CheckInput(SL_Defend);
		//ProcessInputResult(SL_Defend);
		//InputKey k(SL_Defend, curTime);
		//this->keyBuffer.Append(k);
		stopMoving = false;
	}

	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::S))
	{
		n_printf("Down S.\n");
		keyValue = CheckInput(SL_WalkDown);
		//ProcessInputResult(SL_WalkDown);
		isDirKeyDown = true;
		//InputKey k(SL_WalkDown, curTime);
		//this->keyBuffer.Append(k);
		stopMoving = true;
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::W))
	{
		n_printf("Down W.\n");
		keyValue = CheckInput(SL_WalkUp);
		//ProcessInputResult(SL_WalkUp);
		isDirKeyDown = true;
		//InputKey k(SL_WalkUp, curTime);
		//this->keyBuffer.Append(k);
		stopMoving = true;
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::A))
	{
		n_printf("Down A.\n");
		keyValue = CheckInput(SL_WalkLeft);
		//ProcessInputResult(SL_WalkLeft);
		isDirKeyDown = true;
		//InputKey k(SL_WalkLeft, curTime);
		//this->keyBuffer.Append(k);
		stopMoving = true;
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::D))
	{
		n_printf("Down D.\n");
		keyValue = CheckInput(SL_WalkRight);
		//ProcessInputResult(SL_WalkRight);
		isDirKeyDown = true;
		//InputKey k(SL_WalkRight, curTime);
		//this->keyBuffer.Append(k);
		stopMoving = true;
	}

	if (!isDirKeyDown && stopMoving/*this->keyBuffer.Size() == 0*/)
	{
		if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::S))
		{
			n_printf("Press S.\n");
			if (this->preSkill == SL_WalkDown || this->preSkill == SL_RunDown)
			{
				keyValue = this->preSkill;
			}
			else
			{
				keyValue = SL_WalkDown;
			}
		}
		if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::W))
		{
			n_printf("Press W.\n");
			if (this->preSkill == SL_WalkUp || this->preSkill == SL_RunUp)
			{
				keyValue = this->preSkill;
			}
			else
			{
				keyValue = SL_WalkUp;
			}
		}
		if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::A))
		{
			n_printf("Press A.\n");
			if (this->preSkill == SL_WalkLeft || this->preSkill == SL_RunLeft)
			{
				keyValue = this->preSkill;
			}
			else
			{
				keyValue = SL_WalkLeft;
			}
		}
		if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::D))
		{
			n_printf("Press D.\n");
			if (this->preSkill == SL_WalkRight || this->preSkill == SL_RunRight)
			{
				keyValue = this->preSkill;
			}
			else
			{
				keyValue = SL_WalkRight;
			}
		}
	}

	if (keyValue != 0)
	{
		UpdatePosition(keyValue);
	}

	if (actionValue != 0)
	{
		UpdateAction(actionValue);
	}
	else if (keyValue != 0)
	{
		UpdateAction(keyValue);
	}

	
	//lastTime = fighterApp->GetTime();

	//if (this->keyBuffer.Size() > 0)
	//{
	//	Util::Array<DWORD> keys;
	//	for (SizeT i = 0; i < this->keyBuffer.Size(); i++)
	//	{
	//		if (curTime - this->keyBuffer[i].t > tt)
	//			keys.Append(this->keyBuffer[i].val);
	//	}
	//	if (keys.Size() > 0)
	//	{
	//		n_printf("%f\n", curTime - lastTime);
	//		ProcessKeyBuffer(keys);
	//		lastTime = curTime;
	//	}
	//}
	//else
	//{
	//	// stop
	//	//this->hero->SetCurrentAnimation(50);
	//}
}

//------------------------------------------------------------------------------
/**
	���������,�����ͽ��
*/
void
FInputManager::ProcessKeyBuffer(const Util::Array<DWORD>& keys)
{
	/*if (keys.Size() == 0)
	{
		return;
	}

	DWORD val = CheckInvalid(keys);
	DWORD firstKey = keys[0];

	ProcessInputResult(val);

	this->keyBuffer.Clear();*/
}

//------------------------------------------------------------------------------
/**
	��������
*/
void
FInputManager::UpdateAction(DWORD keyValue)
{

	
	//// �ܻ�����
	//bool isAttached = �����ܻ�������;
	//if(isAttached) //���Ҫ�����ܻ�
	//{
	//	return;
	//}

	//If (��ѣ��״̬)
	//	return;

	//// ��������
	//If (��ǰ�����ս��� && ��ǰ��ȥ��ʱ�� < ��ǰ����Delay)
	//{
	//	If (����뵱ǰ�����Ƿ����γ�����)
	//	{
	//		keyValue =��������ֵ;
	//	}
	//}
	
	FSkillManager* skManager = FSkillManager::Instance();
	SkillInfo skInfo;
	if (skManager->HasSkillValue(keyValue))
	{
		skInfo = skManager->GetSkillValue(keyValue);
	}

	/*��ǰ�������¶�����skInfo�����в�ֵ��������ֻ����ֵ������ÿ֡���µ�ʱ���������ֵ��
		curAction = ��ǰ����;
	nextAction = skInfo;*/

	switch(skInfo.keyValue)
	{
	case SL_WalkUp:		// ��
		{
			this->hero->Walk(vector(0.0f, 0.0f, 1.0f), skInfo.animName);
			break;
		}
	case SL_WalkDown:		// ��
		{
			this->hero->Walk(vector(0.0f, 0.0f, -1.0f), skInfo.animName);
			break;
		}
	case SL_WalkLeft:		// ��
		{
			this->hero->Walk(vector(-1.0f, 0.0f, 0.0f), skInfo.animName);
			break;
		}
	case SL_WalkRight:		// ��
		{
			// ���ܵ�״̬
			/*if (this->preSkill == SL_RunRight)
			{
				SkillInfo sk = skManager->GetSkillValue(this->preSkill);
				this->hero->Run(vector(1.0f, 0.0f, 0.0f), sk.animName);
			}
			else*/
			{
				this->hero->Walk(vector(1.0f, 0.0f, 0.0f), skInfo.animName);
			}
			break;
		}
	case SL_Attack:		// ��(A)
		{
			this->hero->Attack(vector(0.0f, 0.0f, 0.0f), skInfo.animName);
			break;
		}
	case SL_Jump:		// ��(J)
		{
			bool isRuning = false;
			if (this->preSkill == SL_RunRight || this->preSkill == SL_RunLeft ||
				this->preSkill == SL_RunDown || this->preSkill == SL_RunUp)
			{
				isRuning = true;
			}
			this->hero->Jump(vector(0.0f, 0.0f, 0.0f), skInfo.animName, isRuning);
			break;
		}
	case SL_Defend:		// ��(D)
		{
			this->hero->Defend(vector(0.0f, 0.0f, 0.0f), skInfo.animName);
			break;
		}

	case SL_RunRight:		// ������
		{
			//SkillInfo sk = skManager->GetSkillValue(this->preSkill);
			this->hero->Run(vector(1.0f, 0.0f, 0.0f), skInfo.animName);
			//Graphics::ActorEntity* Entity = this->GetGraphicsEntity();
			//Entity->SetBaseAnimation(skInfo.animName, 0.2f, 0.0f, true, true, 0.2f);
			break;
		}
	case SL_RunLeft:		// ������
		{
			this->hero->Run(vector(-1.0f, 0.0f, 0.0f), skInfo.animName);
			break;
		}
	case SL_RunDown:		
		{
			this->hero->Run(vector(0.0f, 0.0f, -1.0f), skInfo.animName);
			break;
		}
	case SL_RunUp:
		{
			this->hero->Run(vector(0.0f, 0.0f, 1.0f), skInfo.animName);
			break;
		}
	}
}

void
FInputManager::UpdatePosition(DWORD keyValue)
{
	n_assert(this->hero.isvalid());
	
	float speed = -3.0f;
	Math::vector dir = Math::vector::nullvec();
	FSkillManager* skManager = FSkillManager::Instance();
	SkillInfo skInfo;
	if (skManager->HasSkillValue(keyValue))
	{
		skInfo = skManager->GetSkillValue(keyValue);
	}

	switch(skInfo.keyValue)
	{
	case SL_WalkUp:		// ��
		{
			dir.set(0.0f, 0.0f, 1.0f);
			break;
		}
	case SL_WalkDown:		// ��
		{
			dir.set(0.0f, 0.0f, -1.0f);
			break;
		}
	case SL_WalkLeft:		// ��
		{
			dir.set(-1.0f, 0.0f, 0.0f);
			break;
		}
	case SL_WalkRight:		// ��
		{
			dir.set(1.0f, 0.0f, 0.0f);
			break;
		}
	case SL_Attack:		// ��(A)
		{
			//dir = curDir * 0.1f;
			break;
		}
	case SL_Jump:		// ��(J)
		{
			break;
		}
	case SL_Defend:		// ��(D)
		{
			//Graphics::ActorEntity* Entity = this->GetGraphicsEntity();
			//Entity->SetBaseAnimation(skInfo.animName, 0.2f, 0.0f, true, true, 0.2f);
			break;
		}

	case SL_RunRight:		// ������
		{
			speed = -8.0f;
			//SkillInfo sk = skManager->GetSkillValue(this->preSkill);
			dir.set(1.0f, 0.0f, 0.0f);
			//Graphics::ActorEntity* Entity = this->GetGraphicsEntity();
			//Entity->SetBaseAnimation(skInfo.animName, 0.2f, 0.0f, true, true, 0.2f);
			break;
		}
	case SL_RunLeft:		// ������
		{
			speed = -8.0f;
			dir.set(-1.0f, 0.0f, 0.0f);
			break;
		}
	case SL_RunDown:		
		{
			speed = -8.0f;
			dir.set(0.0f, 0.0f, -1.0f);
			break;
		}
	case SL_RunUp:
		{
			speed = -8.0f;
			dir.set(0.0f, 0.0f, 1.0f);
			break;
		}
	}
	this->hero->SetPosition(dir);
	this->hero->SetSpeed(speed);
}

} // namespace Tools
