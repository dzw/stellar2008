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

	// ���� SPACING...
	EatSpace();

	// ��¼��Ŀǰ��ָ��.(�����������ж�ʱ, ����Ҫ�õ�.)
	BackupPtr();
	// ȡ����һ����.
	type = PickChar(m_pNextChar);
	// SPACING ֮�����ж��Ƿ�Ϊ������β!!!
	if (type == ttFILEEND)
	{
		tokenType = ttFILEEND;
		return tokenType;
	} // end of if (ttFILEEND)

	// ����ע��... ȡ���˵�һ��б�� '/'
	if (type == ttSLASH)
	{
		// ȡ����˫б��...
		switch (*GetNextChar())
		{
		case '/': // ȡ���˵ڶ���б�� '/'
			{
				// �����ڶ���б��...
				PickChar(m_pNextChar);
				// �Ѿ�ȷ����һ����ע����...
				tokenType = ttCOMMENT;
				// �ҵ����оͽ���(ttNEXTLINE)
				for (;;)
				{
					switch (PickChar(m_pNextChar))
					{
					case ttFILEEND: // ��ʹ��ȡ�õ�����β, ����Ҫ��֮ǰ�� COMMENT ��̬���ͻ�ȥ...
					case ttNEXTLINE:
						return tokenType;
					} // end of switch (GetPickedType())
				} // end of for (;;)
			} // end of case '/'
		case '*': // ȡ�������� '*'
			{
				// ��������
				PickChar(m_pNextChar);
				// �Ѿ�ȷ�����µ���Ѷȫ��ע����...
				tokenType = ttCOMMENT;
				// �ҵ���Ե� '*' '/'
				for (;;)
				{
					type = PickChar(m_pNextChar);
					// ������β, ��������...
					// �ҵ�������.
					if (type == ttSTAR)
					{
						// ��������֮���ǲ���б��...
						if (PickChar(m_pNextChar) == ttSLASH)
						{
							// �ҵ�б����, �뿪�ɺ�ʽ��.
							return tokenType; // �ҵ��� '����б��' �������, �뿪��.
						} // end of if (ttSLASH)
					} // end of if (tokenType == ttSTAR)
					// ���ֵ�����β, ����Ҫ��֮ǰ�� COMMENT ��̬���ͻ�ȥ...
					if (type == ttFILEEND)
					{
						return tokenType;
					} // end of if (ttFILEEND)
				} // end of for (;;)
			} // end of case '*'
		} // end of switch (tokenType)
	} // end of if (tokenType == ttSLASH)
	// ����, ����������
	bool bDigits = false; // ��������Token��̬������?

	switch (type)
	{
	case ttPLUS: // �յ���������
	case ttMINUS:
		EatSpace(); // ���ǰ�õ�SPACING
		// (���ʱ��, ֮ǰ�����������Ѿ���m_szPicked���Ƴ���)
		// �ڷ���֮���ҵ�������!!!
		if (PickChar(m_pNextChar, false) == ttNUMBER)
		{
			// ������������...
			if (type == ttMINUS) strcat(m_szToken, "-");
			PickChar(m_pNextChar);     // ץ����һ������.
			bDigits = true; // ������...
		} // end of if (nextChar == ttNUMBER)
		else
		{
			ReloadPtr(); // �ظ�ָ��
			PickChar(m_pNextChar);  // ����ȡ����������
		} //
		break; // end of '+', '-'
	case ttNUMBER:
		bDigits = true;
		break;
	} // end of switch (GetPickedChar())

	// ȷ��ȡ��������...
	if (bDigits)
	{
		bool hasDot = false; // С���㴦��...
		// ������������...(�Ѿ�ȷ����������̬��)
		strcat(m_szToken, GetPicked());
		// ����ʣ�����Ѷ.
		for (;;)
		{
			// ȡ����һ���ֵ�����
			type = PickChar(m_pNextChar, false);
			switch (type)
			{
			case ttNUMBER: // �ҵ�������
				PickChar(m_pNextChar);
				// ������������...
				strcat(m_szToken, GetPicked());
				continue; // end of ttNUMBER
			case ttDOT: // �ҵ���С����
				if (hasDot)
				{
					// �Ѿ���С������, �뿪��Ȧ��.
					break;
				}
				else
				{ // Ŀǰ��Token��û�趨С����...
					// ȡ�����С����. С�������û������û��ϵ, ��������С���㱻����				,
						// ���﷨�ϻ�������.
						PickChar(m_pNextChar);
					// ���С������滹�����ֵĻ�...(ȷ������С������)
					if (PickChar(m_pNextChar, false) == ttNUMBER)
					{
						// С������滹������, �ж�����һ��������
						// ����С����.
						strcat(m_szToken, GetPicked());
						// ˳����¸�����Ҳȡ����, ��õ�һ��������һ��.
						PickChar(m_pNextChar);
						strcat(m_szToken, GetPicked());
						hasDot = true;
					} // end of if (ttNUMBER
				} // end of if (��û����С����)
				continue;
			} // end of switch (nextType)
			break; // �뿪��Ȧ
		} // end of for (;;)

		if (hasDot) tokenType = ttFLOAT; // ȡ���˸�����
		else        tokenType = ttINT;   // ȡ��������

		return tokenType;
	} // end of if (bDigits)
	// �ִ�����
	if (type == ttDQUOTE) return ParseQuotedString();

	// ʶ���ִ���
	switch (type)
	{
	case ttULINE:
	case ttALPHA:
		{
			ParseIdentifier();
			return OnGotIdentifier();
		} // end of ʶ���ִ���
	} // end of switch (GetPickedType())

	// ������������...
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
		case ttDQUOTE:  // �ִ���β...
		case ttFILEEND: // ������β...
			break;
		default: // ��¼��Ŀǰ����
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
		// ������������...
		strcat(m_szToken, GetPicked());
		// ֻȡ����һ���ֵ���̬...
		switch (PickChar(m_pNextChar, false))
		{
		case ttULINE:
		case ttALPHA:
		case ttNUMBER:
			PickChar(m_pNextChar);
			continue;
		} // end of switch (PickChar(false))
		// �Ҳ����Ϸ�IDENTIFIER��ĸ��, �뿪��Ȧ��.
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
	// ����ҵ�����SPACING, ��ȫ������...
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
			// ��������Ԫ
			PickChar(m_pNextChar);
			continue;
		} // end of switch (GetPickedType())
		break; // �����뿪��Ȧ��.
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