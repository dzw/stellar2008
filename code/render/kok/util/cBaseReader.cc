#include "stdafx.h"
/*----------------------------------------------------------------------*/
// ����: cBaseReader.cpp
// ˵��:
/*----------------------------------------------------------------------*/
//#include "cBaseReader.h"
#include "NHelper.h"
//---------------------------------------------------------------------------

char cBaseReader::m_pchDelimiter[] = { "{}()[]=,.!+-_*/%:;^<>?@&|#\\\'\"\r\n\t\a\b\v " };

int  cBaseReader::m_aDelimiter[] = {
	ttBRACEOPEN,    // {
	ttBRACECLOSE,   // }
	ttPARENOPEN,    // (
	ttPARENCLOSE,   // )
	ttBRACKETOPEN,  // [
	ttBRACKETCLOSE, // ]
	ttEQUALMARK,    // =
	ttCOMMA,        // ,
	ttDOT,          // .
	ttEXCLA,        // !
	ttPLUS,         // +
	ttMINUS,        // -
	ttULINE,        // _
	ttSTAR,         // *
	ttSLASH,        // /
	ttPERCENT,      // %
	ttCOLON,        // :
	ttSEMICOLON,    // ;
	ttHAT,          // ^
	ttBELOW,        // <
	ttGREATER,      // >
	ttQUERY,        // ?
	ttAT,           // @
	ttAND,          // &
	ttBAR,          // |
	ttSHARP,        // #
	ttBACKSLASH,    // ��б��
	ttQUOTE,        // \'
	ttDQUOTE,       // \"
	ttRETURN,       // '\r'
	ttNEXTLINE,     // '\n'
	ttTAB,          // '\t'
	ttBELL,         // '\a'
	ttBACKSPACE,    // '\b'
	ttVERTTAB,      // '\v'
	ttSPACE,        // ' '
	//
	ttNUMBER,
	ttALPHA
};

//---------------------------------------------------------------------------
// ���� : cBaseReader
// ���� : Constructor
// ���� : ��
// �ش� : ��
// ��ע : ��
//---------------------------------------------------------------------------
cBaseReader::cBaseReader()
{
	m_pFileData       = NULL;  // ���浵�����ϵ�λ��
	m_bAlready        = false; // ȷ�϶�ȡ���
}

//---------------------------------------------------------------------------
// ���� : ~cBaseReader
// ���� : Destructor
// ���� : ��
// �ش� : ��
// ��ע : ��
//---------------------------------------------------------------------------
cBaseReader::~cBaseReader()
{
	m_bAlready = false;

	if (m_pFileData)
	{
		delete [] m_pFileData;
		m_pFileData = NULL;
	} // end of if (m_pFileData)
}

//---------------------------------------------------------------------------
// ���� : LoadFromMemory
// ���� : �Ӽ������ж�ȡ
// ���� : ��
// �ش� : ��
// ��ע : ��
//---------------------------------------------------------------------------
bool cBaseReader::LoadFromMemory(char* data, int size)
{
	// �����еĵ�������ɾ��
	m_bAlready = false;

	// �����ָ��
	if (m_pFileData)
	{
		delete [] m_pFileData;
		m_pFileData = NULL;
	} // end of if (m_pFileData)
	// �жϻ�Ȧ
	for (;;)
	{
		// ������С����
		if (size <= 0)
		{
			CallErrorMsg(errFileSize);
			break;
		} // end of if (size <= 0)
		// ȡ�����ϴ�С
		m_iFileSize = size;
		// ���ô�ſռ�(������һ��λԪ���'\0'��β��Ԫ)
		m_pFileData = _NEW char[m_iFileSize + 1];
		// ���������ô���
		if (m_pFileData == NULL)
		{
			CallErrorMsg(errAlloc);
			break;
		} // end of if (m_pFileData == NULL)
		// ��ȡ����
		memcpy(m_pFileData, data, size);
		// ���ϵ�β�˲���
		m_pFileData[m_iFileSize] = '\0';
		// �������
		m_bAlready = true;
		// ���������ɹ�
		return true;
	} // end of for (;;)

	// ��������ʧ��
	return false;
}

//---------------------------------------------------------------------------
// Name: LoadFromMemoryX
// Desc:
//---------------------------------------------------------------------------
bool cBaseReader::LoadFromMemoryX(char* szStringL, char* szContext, char* 
								  szStringR)
{
	if (szContext == NULL) return false;
	if (*szContext == '\0') return false;
	// �����еĵ�������ɾ��
	m_bAlready = false;

	// �����ָ��
	if (m_pFileData)
	{
		delete [] m_pFileData;
		m_pFileData = NULL;
	} // end of if (m_pFileData)
	// �жϻ�Ȧ
	for (;;)
	{
		char* pszString;
		int iLenL = 0, 
			iLenContext = 0, 
			iLenR = 0;

		if (szStringL && (*szStringL != '\0')) iLenL = strlen(szStringL);
		iLenContext = strlen(szContext);
		if (szStringR && (*szStringR != '\0')) iLenR = strlen(szStringR);

		// ȡ�����ϴ�С
		m_iFileSize = iLenL + iLenContext + iLenR;
		// ���ô�ſռ�(������һ��λԪ���'\0'��β��Ԫ)
		m_pFileData = _NEW char[m_iFileSize + 1];
		// ���������ô���
		if (m_pFileData == NULL)
		{
			CallErrorMsg(errAlloc);
			break;
		} // end of if (m_pFileData == NULL)

		pszString = m_pFileData;

		if (iLenL > 0)
		{
			memcpy(pszString, szStringL, iLenL);
			pszString += iLenL;
		} // end of if (iLenL > 0)

		memcpy(pszString, szContext, iLenContext);
		pszString += iLenContext;

		if (iLenR > 0)
		{
			memcpy(pszString, szStringR, iLenR);
		} // end of if (iLenR > 0)

		// ���ϵ�β�˲���
		m_pFileData[m_iFileSize] = '\0';
		// �������
		m_bAlready = true;
		// ���������ɹ�
		return true;
	} // end of for (;;)

	// ��������ʧ��
	return false;
}

//---------------------------------------------------------------------------
// ���� : LoadFromFile
// ���� : �ӵ����ж�ȡ
// ���� : ��
// �ش� : ��
// ��ע : ��
//---------------------------------------------------------------------------
bool cBaseReader::LoadFromFile(char* fileName)
{
	if(fileName == NULL) return false;
	if(*fileName == '\0') return false;

	m_bAlready = false;
	// �����ָ��
	if (m_pFileData)
	{
		delete [] m_pFileData;
		m_pFileData = NULL;
	} // end of if (m_pFileData)

	for (;;)
	{
		HANDLE hFile;
		// ��������
		hFile = NFile32::fileOpen(fileName, NFile32::fmOpenRead);

		if (hFile == INVALID_HANDLE_VALUE) 
		{
			CallErrorMsg(errFileOpen);
			break;
		} // end of if (hFile == INVALID_HANDLE_VALUE)
		//    cFileOperator fop;

		//    if (fop.QueryFile(fileName) == false)
		//    {
		//      CallErrorMsg(errFileOpen);
		//      break;
		//    } // end of if (hFile == INVALID_HANDLE_VALUE)
		// ȡ�õ�����С
		//    m_iFileSize = fop.GetFileSize();
		m_iFileSize = NFile32::fileGetSize(hFile);
		// ���ô�ſռ�(������һ��λԪ���'\0'��β��Ԫ)
		m_pFileData = _NEW char[m_iFileSize + 1];
		if (m_pFileData == NULL)
		{
			CallErrorMsg(errAlloc);
			NFile32::fileClose(hFile);
			break;
		} // end of if (m_pFileData == NULL)
		// ��ȡ��������
		NFile32::fileRead(hFile, m_pFileData, m_iFileSize);
		//    memcpy(m_pFileData, fop.GetFileData(), m_iFileSize);
		// ���ϵ�β�˲���
		m_pFileData[m_iFileSize] = '\0';
		// �������
		m_bAlready = true;
		// �ǵ�Ҫ�رյ���
		NFile32::fileClose(hFile);
		// ���������ɹ�
		return true;
	} // end of for (; hFile != INVALID_HANDLE_VALUE;)

	// ��������ʧ��
	return false;
}

//---------------------------------------------------------------------------
// ���� : PickChar
// ���� : ȡ��һ����Ԫ
// ���� : bPick (ֻ����PickedType���ı�m_iPickedType��m_szPicked)
// �ش� : ��
// ��ע : PickCharֻȡ"��Ԫ", ȴʹ��m_szPicked��Ž��, ����Ϊ�����պ�Խ˫��Ԫ���ֵĹ�ϵ.
//---------------------------------------------------------------------------
int  cBaseReader::PickChar(char* &pNextChar, bool bPick)
{
	int pickedType;
	char *curnChar, *nextChar, *pDelimiter;

	curnChar = NULL; // Ŀǰ����Ԫ
	nextChar = pNextChar;

	// Ԥ��Ϊδ֪...
	pickedType = ttUNKNOWN;

	// ��һ���־��ǵ�����β...
	if (*nextChar == '\0')
	{
		if (bPick) m_iPickedType = ttFILEEND;
		// ����ֱ���뿪, ��Ҫ��goto������ȥ��.
		return ttFILEEND;
	} // end of if (*nextChar == '\0')

	// ȡ����һ����Ԫ
	curnChar = nextChar;
	nextChar = (char*)_mbsinc((unsigned const char*)curnChar);

	pDelimiter = NULL;
	// ����ҵ������������, �����������, ���ض�Ӧ��Type
	pDelimiter = strchr(m_pchDelimiter, *curnChar);
	if (pDelimiter)
	{
		pickedType = m_aDelimiter[pDelimiter - m_pchDelimiter];
		goto PICKCHAR_QUIT;
	} // end of if (pCh)

	// isdigit ֻ���ܱ�׼��Ԫ�ıȶ�, ��Ԫ��ֵ���ܳ��� 128
	if (strchr("0123456789", *curnChar))
	{
		pickedType = ttNUMBER;
		goto PICKCHAR_QUIT;
	} // end of if (isdigit(*curnChar))

	// ��ȡ�õ���һ��ʶ����, ����һ��keyword
	// "ͷ����"ֻ����Ϊascii��׼��Ӣ��
	// isalpha ֻ���ܱ�׼��Ԫ�ıȶ�, ��Ԫ��ֵ���ܳ��� 128
	if (strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", 
		*curnChar)) // var or command ����. �����Ǳ���������
	{
		pickedType = ttALPHA;
		goto PICKCHAR_QUIT;
	} // end of if (isalpha(*curnChar))

PICKCHAR_QUIT:
	if (bPick)
	{
		int len;

		pNextChar = nextChar;
		len = nextChar - curnChar;

		if (len > 0)
		{
			// ˫λԪ
			if (len > 1) pickedType = ttMULTIBYTE;
			// ȡ�ü�¼Token�ļ�����ָ��, �Ƚ�m_szToken��Ϊ "".
			char* tokenPtr = m_szPicked;
			// ����, ����˫��Ԫ����?
			memcpy(tokenPtr, curnChar, len);
			*(tokenPtr + len) = '\0';
		} // end of if (len > 0)
		// ��¼��ε� pickedType
		m_iPickedType = pickedType;
	} // end of if (bTestPick)

	return pickedType;
}

//---------------------------------------------------------------------------
// Name: BackupPtr
// Desc:
//---------------------------------------------------------------------------
void cBaseReader::BackupPtr() // ����Ŀǰ�� m_pNextChar
{
	if (m_bAlready == false) return;

	m_pBackupPtr = m_pNextChar;
}

//---------------------------------------------------------------------------
// Name: ReloadPtr
// Desc:
//---------------------------------------------------------------------------
void cBaseReader::ReloadPtr() // ���ر��ݵ� m_pNextChar
{
	if (m_bAlready == false) return;

	if (m_pBackupPtr)
	{
		m_pNextChar = m_pBackupPtr;
		m_pBackupPtr = NULL;
	} // end of if (m_pBackupPtr)
}

//---------------------------------------------------------------------------
// ���� : Reset
// ���� :
// ���� : ��
// �ش� : ��
// ��ע : ��
//---------------------------------------------------------------------------
bool cBaseReader::Reset()
{
	// ȷ�����е���������.
	if (m_bAlready == false)
	{
		CallErrorMsg(errFileReady);
		return false;
	} // end of if (m_bAlready == false)

	// ��ͷ��ʼ����
	m_pNextChar = m_pFileData;
	m_pBackupPtr = NULL;

	return true;
}

//---------------------------------------------------------------------------
//===========================================================================
//---------------------------------------------------------------------------
// ���� : CallErrorMsg
// ���� :
// ���� : ��
// �ش� : ��
// ��ע : ��
//---------------------------------------------------------------------------
void cBaseReader::CallErrorMsg(int no)
{
	char str[128];

	sprintf(str, "Reader Error No. %d\r\n", no);
	OutputDebugString(str);
}

//---------------------------------------------------------------------------
// Name: IsWhiteSpace
// Desc: whiteSpace��Ԫ, win32 SDK ����Ϊ 0x9 - 0xD, 0x20 ����Ԫ
//---------------------------------------------------------------------------
bool cBaseReader::IsWhiteSpace(int tokenType)
{
	switch (tokenType)
	{
	case ttRETURN:
	case ttNEXTLINE:
	case ttTAB:
	case ttBACKSPACE:
	case ttSPACE:
		return true;
	} // end of switch (tokenType)

	return false;
}

//---------------------------------------------------------------------------
