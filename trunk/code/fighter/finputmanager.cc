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
	获得本次输入的值
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
	
	FighterApplication* app = (FighterApplication*)FighterApplication::Instance();
	if ((app->GetTime() - lastTime) > intervalTime)	// 超出间隔时间
		this->preSkill = 0;

	IndexT val = this->preSkill + input;
	FSkillManager* skillManager = FSkillManager::Instance();
	if (skillManager->HasSkillValue(val))
	{
		this->preSkill = val;
		return val;
	}
	// 与上个动作不匹配，只做当前动作
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
			cameraManager->SetCameraOrbit(v.x(), v.y(), fighterApp->GetFrameTime());
		}
		if (mouse->WheelForward())
		{
			cameraManager->SetCameraDistance(-1.0f);
		}
		if (mouse->WheelBackward())
		{
			cameraManager->SetCameraDistance(1.0f);
		}
		cameraManager->UpdateCamera(curTime);
	}

	IndexT keyValue = 0;
	bool isDirKeyDown = false;
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::S))
	{
		n_printf("Down S.");
		keyValue = CheckInput(SL_WalkDown);
		//ProcessInputResult(SL_WalkDown);
		isDirKeyDown = true;
		//InputKey k(SL_WalkDown, curTime);
		//this->keyBuffer.Append(k);
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::W))
	{
		n_printf("Down W.");
		keyValue = CheckInput(SL_WalkUp);
		//ProcessInputResult(SL_WalkUp);
		isDirKeyDown = true;
		//InputKey k(SL_WalkUp, curTime);
		//this->keyBuffer.Append(k);
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::A))
	{
		n_printf("Down A.");
		keyValue = CheckInput(SL_WalkLeft);
		//ProcessInputResult(SL_WalkLeft);
		isDirKeyDown = true;
		//InputKey k(SL_WalkLeft, curTime);
		//this->keyBuffer.Append(k);
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::D))
	{
		n_printf("Down D.");
		keyValue = CheckInput(SL_WalkRight);
		//ProcessInputResult(SL_WalkRight);
		isDirKeyDown = true;
		//InputKey k(SL_WalkRight, curTime);
		//this->keyBuffer.Append(k);
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::J))	// 攻击
	{
		n_printf("Down J.");
		keyValue = CheckInput(SL_Attack);
		//ProcessInputResult(SL_Attack);
		//InputKey k(SL_Attack, curTime);
		//this->keyBuffer.Append(k);
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::K))	// 跳跃
	{
		n_printf("Down K.");
		keyValue = CheckInput(SL_Jump);
		//ProcessInputResult(SL_Jump);
		//InputKey k(SL_Jump, curTime);
		//this->keyBuffer.Append(k);
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::L))	// 防守
	{
		n_printf("Down L.");
		keyValue = CheckInput(SL_Defend);
		//ProcessInputResult(SL_Defend);
		//InputKey k(SL_Defend, curTime);
		//this->keyBuffer.Append(k);
	}

	static Timing::Time tt = 0.35;
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::F2))
	{
		tt += 0.05;
	}
	if (inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::F3))
	{
		tt -= 0.05;
		if (tt < 0.001) tt = 0.0;
	}

	if (!isDirKeyDown /*this->keyBuffer.Size() == 0*/)
	{
		if (inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::S))
		{
			n_printf("S.");
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
			n_printf("W.");
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
			n_printf("A.");
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
			n_printf("D.");
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

	ProcessInputResult(keyValue);
	lastTime = fighterApp->GetTime();

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
	处理输入键,并发送结果
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

	ProcessInputResult(val);

	this->keyBuffer.Clear();
}

//------------------------------------------------------------------------------
/**
	处理输入
*/
void
FInputManager::ProcessInputResult(DWORD val)
{

	
	//// 受击处理
	//bool isAttached = 调用受击处理函数;
	//if(isAttached) //如果要处理受击
	//{
	//	return;
	//}

	//If (在眩晕状态)
	//	return;

	//// 连击处理
	//If (当前动作刚结束 && 当前逝去的时间 < 当前动作Delay)
	//{
	//	If (检测与当前动作是否能形成连击)
	//	{
	//		keyValue =连击动作值;
	//	}
	//}
	
	IndexT keyValue = val;
	
	FSkillManager* skManager = FSkillManager::Instance();
	SkillInfo skInfo;
	if (skManager->HasSkillValue(keyValue))
	{
		skInfo = skManager->GetSkillValue(keyValue);
	}

	/*当前动作与新动作（skInfo）进行插值，在这里只做付值操作，每帧更新的时候才真正插值。
		curAction = 当前动作;
	nextAction = skInfo;*/

	switch(skInfo.keyValue)
	{
	case SL_WalkUp:		// 上
		{
			this->hero->Walk(vector(0.0f, 0.0f, 1.0f), skInfo.animName);
			break;
		}
	case SL_WalkDown:		// 下
		{
			this->hero->Walk(vector(0.0f, 0.0f, -1.0f), skInfo.animName);
			break;
		}
	case SL_WalkLeft:		// 左
		{
			this->hero->Walk(vector(-1.0f, 0.0f, 0.0f), skInfo.animName);
			break;
		}
	case SL_WalkRight:		// 右
		{
			// 在跑的状态
			if (this->preSkill == SL_RunRight)
			{
				SkillInfo sk = skManager->GetSkillValue(this->preSkill);
				this->hero->Run(vector(1.0f, 0.0f, 0.0f), sk.animName);
			}
			else
			{
				this->hero->Walk(vector(1.0f, 0.0f, 0.0f), skInfo.animName);
			}
			break;
		}
	case SL_Attack:		// 攻(A)
		{
			break;
		}
	case SL_Jump:		// 跳(J)
		{
			break;
		}
	case SL_Defend:		// 防(D)
		{
			//Graphics::ActorEntity* Entity = this->GetGraphicsEntity();
			//Entity->SetBaseAnimation(skInfo.animName, 0.2f, 0.0f, true, true, 0.2f);
			break;
		}

	case SL_RunRight:		// 奔跑右
		{
			SkillInfo sk = skManager->GetSkillValue(this->preSkill);
			this->hero->Run(vector(1.0f, 0.0f, 0.0f), sk.animName);
			//Graphics::ActorEntity* Entity = this->GetGraphicsEntity();
			//Entity->SetBaseAnimation(skInfo.animName, 0.2f, 0.0f, true, true, 0.2f);
			break;
		}
	case SL_RunLeft:		// 奔跑左
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
