//------------------------------------------------------------------------------
//  cECReader.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/util/cECReader.h"

namespace KOK
{

cECReader::stKEYWORDITEM g_szECKeyword[] = 
{
	"true",  cECReader::ttTRUE,
	"TRUE",  cECReader::ttTRUE,
	"false", cECReader::ttFALSE,
	"FALSE", cECReader::ttFALSE,
	"",      cECReader::ttUNKNOWN
};

//---------------------------------------------------------------------------
// Name: cECReader
// Desc: contrustor
//---------------------------------------------------------------------------
cECReader::cECReader()
{
	*m_szToken = '\0';
}

//---------------------------------------------------------------------------
// Name: ~cECReader
// Desc:
//---------------------------------------------------------------------------
cECReader::~cECReader()
{

}

//---------------------------------------------------------------------------
// Name: GetToken
// Desc:
//---------------------------------------------------------------------------
int cECReader::GetToken()
{
	int tokenType, type;

	m_szToken[0] = '\0';

	// 忽略 SPACING...
	EatSpace();

	// 记录下目前的指标.(在做正负号判断时, 有需要用到.)
	BackupPtr();
	// 取得下一个字.
	type = PickChar(m_pNextChar);
	// SPACING 之后再判定是否为档案结尾!!!
	if (type == ttFILEEND)
	{
		tokenType = ttFILEEND;
		return tokenType;
	} // end of if (ttFILEEND)

	// 忽略注解... 取得了第一条斜线 '/'
	if (type == ttSLASH)
	{
		// 取得了双斜线...
		switch (*GetNextChar())
		{
		case '/': // 取得了第二条斜线 '/'
			{
				// 跳过第二个斜线...
				PickChar(m_pNextChar);
				// 已经确定这一行是注解了...
				tokenType = ttCOMMENT;
				// 找到换行就结束(ttNEXTLINE)
				for (;;)
				{
					switch (PickChar(m_pNextChar))
					{
					case ttFILEEND: // 即使已取得档案结尾, 还是要把之前的 COMMENT 形态先送回去...
					case ttNEXTLINE:
						return tokenType;
					} // end of switch (GetPickedType())
				} // end of for (;;)
			} // end of case '/'
		case '*': // 取得了星星 '*'
			{
				// 跳过星星
				PickChar(m_pNextChar);
				// 已经确定以下的资讯全是注解了...
				tokenType = ttCOMMENT;
				// 找到配对的 '*' '/'
				for (;;)
				{
					type = PickChar(m_pNextChar);
					// 档案结尾, 不处理了...
					// 找到星星了.
					if (type == ttSTAR)
					{
						// 看看星星之后是不是斜线...
						if (PickChar(m_pNextChar) == ttSLASH)
						{
							// 找到斜线了, 离开吧函式吧.
							return tokenType; // 找到了 '星星斜线' 任务完成, 离开吧.
						} // end of if (ttSLASH)
					} // end of if (tokenType == ttSTAR)
					// 出现档案结尾, 还是要把之前的 COMMENT 形态先送回去...
					if (type == ttFILEEND)
					{
						return tokenType;
					} // end of if (ttFILEEND)
				} // end of for (;;)
			} // end of case '*'
		} // end of switch (tokenType)
	} // end of if (tokenType == ttSLASH)
	// 整数, 浮点数处理
	bool bDigits = false; // 接下来的Token形态是数字?

	switch (type)
	{
	case ttPLUS: // 收到了正负号
	case ttMINUS:
		EatSpace(); // 清除前置的SPACING
		// (这个时候, 之前的正负符号已经从m_szPicked上移除了)
		// 在符号之后找到了数字!!!
		if (PickChar(m_pNextChar, false) == ttNUMBER)
		{
			// 复制文字内容...
			if (type == ttMINUS) strcat(m_szToken, "-");
			PickChar(m_pNextChar);     // 抓出下一个数字.
			bDigits = true; // 是数字...
		} // end of if (nextChar == ttNUMBER)
		else
		{
			ReloadPtr(); // 回复指标
			PickChar(m_pNextChar);  // 重新取回正负符号
		} //
		break; // end of '+', '-'
	case ttNUMBER:
		bDigits = true;
		break;
	} // end of switch (GetPickedChar())

	// 确定取得了数字...
	if (bDigits)
	{
		bool hasDot = false; // 小数点处理...
		// 复制文字内容...(已经确定是数字型态了)
		strcat(m_szToken, GetPicked());
		// 处理剩余的资讯.
		for (;;)
		{
			// 取得下一个字的类型
			type = PickChar(m_pNextChar, false);
			switch (type)
			{
			case ttNUMBER: // 找到了数字
				PickChar(m_pNextChar);
				// 复制文字内容...
				strcat(m_szToken, GetPicked());
				continue; // end of ttNUMBER
			case ttDOT: // 找到了小数点
				if (hasDot)
				{
					// 已经有小数点了, 离开回圈吧.
					break;
				}
				else
				{ // 目前的Token还没设定小数点...
					// 取出这个小数点. 小数点后面没有数字没关系, 最多是这个小数点被忽略				,
						// 但语法上还是允许.
						PickChar(m_pNextChar);
					// 如果小数点后面还有数字的话...(确认这是小数点了)
					if (PickChar(m_pNextChar, false) == ttNUMBER)
					{
						// 小数点后面还有数字, 判定这是一个浮点数
						// 复制小数点.
						strcat(m_szToken, GetPicked());
						// 顺便把下个数字也取出来, 免得等一下又重做一次.
						PickChar(m_pNextChar);
						strcat(m_szToken, GetPicked());
						hasDot = true;
					} // end of if (ttNUMBER
				} // end of if (还没设立小数点)
				continue;
			} // end of switch (nextType)
			break; // 离开回圈
		} // end of for (;;)

		if (hasDot) tokenType = ttFLOAT; // 取得了浮点数
		else        tokenType = ttINT;   // 取得了整数

		return tokenType;
	} // end of if (bDigits)
	// 字串处理
	if (type == ttDQUOTE) return ParseQuotedString();

	// 识别字处理
	switch (type)
	{
	case ttULINE:
	case ttALPHA:
		{
			ParseIdentifier();
			return OnGotIdentifier();
		} // end of 识别字处理
	} // end of switch (GetPickedType())

	// 复制文字内容...
	strcat(m_szToken, GetPicked());

	tokenType = type;
	return tokenType;
}

//---------------------------------------------------------------------------
// Name: ParseQuotedString
// Desc:
//---------------------------------------------------------------------------
int  cECReader::ParseQuotedString()
{
	for (;;)
	{
		switch (PickChar(m_pNextChar))
		{
		case ttDQUOTE:  // 字串结尾...
		case ttFILEEND: // 档案结尾...
			break;
		default: // 记录下目前的字
			strcat(m_szToken, GetPicked());
			continue;
		} // end of switch (PickChar())
		break;
	} // end of for (;;)

	return ttSTRING;
}

//---------------------------------------------------------------------------
// Name: ParseIdneitfier
// Desc:
//---------------------------------------------------------------------------
void cECReader::ParseIdentifier()
{
	for (;;)
	{
		// 复制文字内容...
		strcat(m_szToken, GetPicked());
		// 只取得下一个字的形态...
		switch (PickChar(m_pNextChar, false))
		{
		case ttULINE:
		case ttALPHA:
		case ttNUMBER:
			PickChar(m_pNextChar);
			continue;
		} // end of switch (PickChar(false))
		// 找不到合法IDENTIFIER字母了, 离开回圈吧.
		break;
	} // end of for (;;)
}

//---------------------------------------------------------------------------
// Name: OnGotIdentifier
// Desc:
//---------------------------------------------------------------------------
int  cECReader::OnGotIdentifier()
{
	cECReader::stKEYWORDITEM* pKI;

	pKI = g_szECKeyword;

	for (; *(pKI->m_szName) != '\0';)
	{
		if (strcmp(m_szToken, pKI->m_szName) == 0)  return pKI->m_iTokenType;
		pKI = pKI + 1;
	} // end of for (i = 0; 

	return ttIDENTIFIER;
}

//---------------------------------------------------------------------------
// Name: EatSpace
// Desc:
//---------------------------------------------------------------------------
void cECReader::EatSpace()
{
	// 如果找到的是SPACING, 就全部忽略...
	for (;;)
	{
		// isspacing ?
		switch (PickChar(m_pNextChar, false))
		{
		case ttRETURN:
		case ttNEXTLINE:
		case ttTAB:
		case ttBACKSPACE:
		case ttSPACE:
			// 抽掉这个字元
			PickChar(m_pNextChar);
			continue;
		} // end of switch (GetPickedType())
		break; // 可以离开回圈了.
	} // end of for (;;)
}

//---------------------------------------------------------------------------
// Name: GetTokenInt
// Desc:
//---------------------------------------------------------------------------
int   cECReader::GetTokenInt()
{
	return atoi(m_szToken);
}

//---------------------------------------------------------------------------
// Name: GetTokenFloat
// Desc:
//---------------------------------------------------------------------------
double cECReader::GetTokenDouble()
{
	return atof(m_szToken);
}

//---------------------------------------------------------------------------
__int64 cECReader::GetTokenInt64( void )
{
	return _atoi64( m_szToken );
}

//---------------------------------------------------------------------------
}