#ifndef MSG_INPUTRESULTMSG_H
#define MSG_INPUTRESULTMSG_H
//------------------------------------------------------------------------------
/**
    @class GraphicsFeature::InputResult

    ����������
    
    (C) 2009 cTuo
*/
#include "messaging/message.h"

//------------------------------------------------------------------------------
namespace FightingFeature
{
class FightingInputResultMsg : public Messaging::Message
{
    DeclareClass(FightingInputResultMsg);
    DeclareMsgId;

public:
    /// constructor
    FightingInputResultMsg();

	void SetFirstKey(DWORD d);
	DWORD GetFirstKey()const;

	void SetValue(DWORD d);
	DWORD GetValue()const;
private:
	/// ��һ����ֵ
	DWORD firstKey;
	/// ���μ�ֵ
    DWORD val;
};

inline void 
FightingInputResultMsg::SetFirstKey(DWORD d)
{
	this->firstKey = d;
}

inline DWORD 
FightingInputResultMsg::GetFirstKey()const
{
	return this->firstKey;
}

inline void 
FightingInputResultMsg::SetValue(DWORD d)
{
	this->val = d;
}

inline DWORD 
FightingInputResultMsg::GetValue()const
{
	return val;
}

}; // namespace GraphicsFeature
//------------------------------------------------------------------------------
#endif