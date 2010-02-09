//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "FinputManager.h"
#include "fskillmanager.h"
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
isOpen(false)
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
		{
			break;
		}

		IndexT index = this->inputRule[i].BinarySearchIndex(buffer[i]);
		if (index != InvalidIndex)
		{
			val += buffer[i];
		}
	}

	return val;
}

void 
FInputManager::Update()
{
	if (!this->hero.isvalid())
		return;

	InputServer* inputServer = InputServer::Instance();
	const Ptr<Keyboard>& keyboard = inputServer->GetDefaultKeyboard();
	const Ptr<Mouse>& mouse = inputServer->GetDefaultMouse();

	FighterApplication* fighterApp = (FighterApplication*)FighterApplication::Instance();
	Timing::Time curTime = fighterApp->GetTime();
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::S))
	{
		n_printf("Down S.");
		InputKey k(SL_WalkDown, curTime);
		this->keyBuffer.Append(k);
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::W))
	{
		n_printf("Down W.");
		InputKey k(SL_WalkUp, curTime);
		this->keyBuffer.Append(k);
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::A))
	{
		n_printf("Down A.");
		InputKey k(SL_WalkLeft, curTime);
		this->keyBuffer.Append(k);
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::D))
	{
		n_printf("Down D.");
		InputKey k(SL_WalkRight, curTime);
		this->keyBuffer.Append(k);
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::J))	// ����
	{
		n_printf("Down J.");
		InputKey k(SL_Attack, curTime);
		this->keyBuffer.Append(k);
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::K))	// ��Ծ
	{
		n_printf("Down K.");
		InputKey k(SL_Jump, curTime);
		this->keyBuffer.Append(k);
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::L))	// ����
	{
		n_printf("Down L.");
		InputKey k(SL_Defend, curTime);
		this->keyBuffer.Append(k);
	}

	static Timing::Time tt = 0.35;
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::F2))	// ��Ծ
	{
		tt += 0.05;
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::F3))	// ����
	{
		tt -= 0.05;
		if (tt < 0.001) tt = 0.0;
	}

	if (this->keyBuffer.Size() == 0)
	{
		if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::S))
		{
			n_printf("S.");
			Util::Array<DWORD> keys;
			keys.Append(SL_WalkDown);
			ProcessKeyBuffer(keys);
		}
		if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::W))
		{
			n_printf("W.");
			Util::Array<DWORD> keys;
			keys.Append(SL_WalkUp);
			ProcessKeyBuffer(keys);
		}
		if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::A))
		{
			n_printf("A.");
			Util::Array<DWORD> keys;
			keys.Append(SL_WalkLeft);
			ProcessKeyBuffer(keys);
		}
		if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::D))
		{
			n_printf("D.");
			Util::Array<DWORD> keys;
			keys.Append(SL_WalkRight);
			ProcessKeyBuffer(keys);
		}
	}

	if (this->keyBuffer.Size() > 0)
	{
		Util::Array<DWORD> keys;
		for (SizeT i = 0; i < this->keyBuffer.Size(); i++)
		{
			if (curTime - this->keyBuffer[i].t > tt)
				keys.Append(this->keyBuffer[i].val);
		}
		if (keys.Size() > 0)
		{
			n_printf("%f\n", curTime - lastTime);
			ProcessKeyBuffer(keys);
			lastTime = curTime;
		}
	}
	else
	{
		// stop
		this->hero->SetCurrentAnimation(50);
	}
}

//------------------------------------------------------------------------------
/**
	���������,�����ͽ��
*/
void
FInputManager::ProcessKeyBuffer(const Util::Array<DWORD>& keys)
{
	if (keys.Size() == 0)
	{
		return;
	}

	DWORD val = CheckInvalid(keys);
	DWORD firstKey = keys[0];

	ProcessInputResult(val, firstKey);

	this->keyBuffer.Clear();
}

//------------------------------------------------------------------------------
/**
	��������
*/
void
FInputManager::ProcessInputResult(DWORD val, DWORD firstKey)
{
	DWORD keyValue = val;
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
			if (this->preSkill == SL_RunRight)
			{
				SkillInfo sk = skManager->GetSkillValue(this->preSkill);
				this->hero->Walk(vector(1.0f, 0.0f, 0.0f), sk.animName);
			}
			else
			{
				this->hero->Walk(vector(1.0f, 0.0f, 0.0f), skInfo.animName);
			}
			break;
		}
	case SL_Attack:		// ��(A)
		{
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
			//Graphics::ActorEntity* Entity = this->GetGraphicsEntity();
			//Entity->SetBaseAnimation(skInfo.animName, 0.2f, 0.0f, true, true, 0.2f);
			break;
		}
	case SL_RunLeft:		// ������
		{
			break;
		}
	case SL_RunDown:		
		{
			break;
		}
	case SL_RunUp:
		{
			break;
		}
	}
}

} // namespace Tools
