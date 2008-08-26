#include "stdafx.h"
/*----------------------------------------------------------------------*/
// 档名: cBaseReader.cpp
// 说明:
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
	ttBACKSLASH,    // 反斜线
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
// 名称 : cBaseReader
// 功用 : Constructor
// 引数 : 无
// 回传 : 无
// 备注 : 无
//---------------------------------------------------------------------------
cBaseReader::cBaseReader()
{
	m_pFileData       = NULL;  // 储存档案资料的位置
	m_bAlready        = false; // 确认读取完毕
}

//---------------------------------------------------------------------------
// 名称 : ~cBaseReader
// 功用 : Destructor
// 引数 : 无
// 回传 : 无
// 备注 : 无
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
// 名称 : LoadFromMemory
// 功用 : 从记忆体中读取
// 引数 : 无
// 回传 : 无
// 备注 : 无
//---------------------------------------------------------------------------
bool cBaseReader::LoadFromMemory(char* data, int size)
{
	// 将旧有的档案资料删除
	m_bAlready = false;

	// 清除旧指标
	if (m_pFileData)
	{
		delete [] m_pFileData;
		m_pFileData = NULL;
	} // end of if (m_pFileData)
	// 判断回圈
	for (;;)
	{
		// 档案大小错误
		if (size <= 0)
		{
			CallErrorMsg(errFileSize);
			break;
		} // end of if (size <= 0)
		// 取得资料大小
		m_iFileSize = size;
		// 配置存放空间(多配置一个位元存放'\0'结尾字元)
		m_pFileData = _NEW char[m_iFileSize + 1];
		// 记忆体配置错误
		if (m_pFileData == NULL)
		{
			CallErrorMsg(errAlloc);
			break;
		} // end of if (m_pFileData == NULL)
		// 读取资料
		memcpy(m_pFileData, data, size);
		// 资料的尾端补零
		m_pFileData[m_iFileSize] = '\0';
		// 设置旗标
		m_bAlready = true;
		// 档案开启成功
		return true;
	} // end of for (;;)

	// 档案开启失败
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
	// 将旧有的档案资料删除
	m_bAlready = false;

	// 清除旧指标
	if (m_pFileData)
	{
		delete [] m_pFileData;
		m_pFileData = NULL;
	} // end of if (m_pFileData)
	// 判断回圈
	for (;;)
	{
		char* pszString;
		int iLenL = 0, 
			iLenContext = 0, 
			iLenR = 0;

		if (szStringL && (*szStringL != '\0')) iLenL = strlen(szStringL);
		iLenContext = strlen(szContext);
		if (szStringR && (*szStringR != '\0')) iLenR = strlen(szStringR);

		// 取得资料大小
		m_iFileSize = iLenL + iLenContext + iLenR;
		// 配置存放空间(多配置一个位元存放'\0'结尾字元)
		m_pFileData = _NEW char[m_iFileSize + 1];
		// 记忆体配置错误
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

		// 资料的尾端补零
		m_pFileData[m_iFileSize] = '\0';
		// 设置旗标
		m_bAlready = true;
		// 档案开启成功
		return true;
	} // end of for (;;)

	// 档案开启失败
	return false;
}

//---------------------------------------------------------------------------
// 名称 : LoadFromFile
// 功用 : 从档案中读取
// 引数 : 无
// 回传 : 无
// 备注 : 无
//---------------------------------------------------------------------------
bool cBaseReader::LoadFromFile(char* fileName)
{
	if(fileName == NULL) return false;
	if(*fileName == '\0') return false;

	m_bAlready = false;
	// 清除旧指标
	if (m_pFileData)
	{
		delete [] m_pFileData;
		m_pFileData = NULL;
	} // end of if (m_pFileData)

	for (;;)
	{
		HANDLE hFile;
		// 开启档案
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
		// 取得档案大小
		//    m_iFileSize = fop.GetFileSize();
		m_iFileSize = NFile32::fileGetSize(hFile);
		// 配置存放空间(多配置一个位元存放'\0'结尾字元)
		m_pFileData = _NEW char[m_iFileSize + 1];
		if (m_pFileData == NULL)
		{
			CallErrorMsg(errAlloc);
			NFile32::fileClose(hFile);
			break;
		} // end of if (m_pFileData == NULL)
		// 读取档案资料
		NFile32::fileRead(hFile, m_pFileData, m_iFileSize);
		//    memcpy(m_pFileData, fop.GetFileData(), m_iFileSize);
		// 资料的尾端补零
		m_pFileData[m_iFileSize] = '\0';
		// 设置旗标
		m_bAlready = true;
		// 记得要关闭档案
		NFile32::fileClose(hFile);
		// 档案开启成功
		return true;
	} // end of for (; hFile != INVALID_HANDLE_VALUE;)

	// 档案开启失败
	return false;
}

//---------------------------------------------------------------------------
// 名称 : PickChar
// 功用 : 取出一个字元
// 引数 : bPick (只传回PickedType不改变m_iPickedType和m_szPicked)
// 回传 : 无
// 备注 : PickChar只取"字元", 却使用m_szPicked存放结果, 是因为有中日韩越双字元文字的关系.
//---------------------------------------------------------------------------
int  cBaseReader::PickChar(char* &pNextChar, bool bPick)
{
	int pickedType;
	char *curnChar, *nextChar, *pDelimiter;

	curnChar = NULL; // 目前的字元
	nextChar = pNextChar;

	// 预设为未知...
	pickedType = ttUNKNOWN;

	// 下一个字就是档案结尾...
	if (*nextChar == '\0')
	{
		if (bPick) m_iPickedType = ttFILEEND;
		// 这里直接离开, 不要再goto到底下去了.
		return ttFILEEND;
	} // end of if (*nextChar == '\0')

	// 取得下一个字元
	curnChar = nextChar;
	nextChar = (char*)_mbsinc((unsigned const char*)curnChar);

	pDelimiter = NULL;
	// 如果找到的是特殊符号, 分析这个符号, 传回对应的Type
	pDelimiter = strchr(m_pchDelimiter, *curnChar);
	if (pDelimiter)
	{
		pickedType = m_aDelimiter[pDelimiter - m_pchDelimiter];
		goto PICKCHAR_QUIT;
	} // end of if (pCh)

	// isdigit 只接受标准字元的比对, 字元数值不能超过 128
	if (strchr("0123456789", *curnChar))
	{
		pickedType = ttNUMBER;
		goto PICKCHAR_QUIT;
	} // end of if (isdigit(*curnChar))

	// 所取得的是一个识别字, 或是一个keyword
	// "头文字"只允许为ascii标准的英文
	// isalpha 只接受标准字元的比对, 字元数值不能超过 128
	if (strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", 
		*curnChar)) // var or command 文字. 可能是变数或命令
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
			// 双位元
			if (len > 1) pickedType = ttMULTIBYTE;
			// 取得记录Token的记忆体指标, 先将m_szToken设为 "".
			char* tokenPtr = m_szPicked;
			// 符号, 或是双字元文字?
			memcpy(tokenPtr, curnChar, len);
			*(tokenPtr + len) = '\0';
		} // end of if (len > 0)
		// 记录这次的 pickedType
		m_iPickedType = pickedType;
	} // end of if (bTestPick)

	return pickedType;
}

//---------------------------------------------------------------------------
// Name: BackupPtr
// Desc:
//---------------------------------------------------------------------------
void cBaseReader::BackupPtr() // 备份目前的 m_pNextChar
{
	if (m_bAlready == false) return;

	m_pBackupPtr = m_pNextChar;
}

//---------------------------------------------------------------------------
// Name: ReloadPtr
// Desc:
//---------------------------------------------------------------------------
void cBaseReader::ReloadPtr() // 重载备份的 m_pNextChar
{
	if (m_bAlready == false) return;

	if (m_pBackupPtr)
	{
		m_pNextChar = m_pBackupPtr;
		m_pBackupPtr = NULL;
	} // end of if (m_pBackupPtr)
}

//---------------------------------------------------------------------------
// 名称 : Reset
// 功用 :
// 引数 : 无
// 回传 : 无
// 备注 : 无
//---------------------------------------------------------------------------
bool cBaseReader::Reset()
{
	// 确认已有档案读入了.
	if (m_bAlready == false)
	{
		CallErrorMsg(errFileReady);
		return false;
	} // end of if (m_bAlready == false)

	// 从头开始编译
	m_pNextChar = m_pFileData;
	m_pBackupPtr = NULL;

	return true;
}

//---------------------------------------------------------------------------
//===========================================================================
//---------------------------------------------------------------------------
// 名称 : CallErrorMsg
// 功用 :
// 引数 : 无
// 回传 : 无
// 备注 : 无
//---------------------------------------------------------------------------
void cBaseReader::CallErrorMsg(int no)
{
	char str[128];

	sprintf(str, "Reader Error No. %d\r\n", no);
	OutputDebugString(str);
}

//---------------------------------------------------------------------------
// Name: IsWhiteSpace
// Desc: whiteSpace字元, win32 SDK 定义为 0x9 - 0xD, 0x20 等字元
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
