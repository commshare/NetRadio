#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <stdarg.h>
#include <string.h>
#include "DebugTrace.h"

//=============================================================================
char CDebugTrace::m_szLogFile[MAX_PATH] = {0};
CDebugTrace::trace_level_t CDebugTrace::m_nLogLevel = CDebugTrace::ERROR_LEVEL;
uint32_t CDebugTrace::m_nTraceOptions = CDebugTrace::Timestamp;

static CDebugTrace goDebugTrace;

//=============================================================================
CDebugTrace::CDebugTrace(void)
	: m_nDataSize(0)
{
}

CDebugTrace::~CDebugTrace(void)
{
}

CDebugTrace& CDebugTrace::operator << (bool Value)
{
	char* lpWritePtr = m_szPrintBuffer + m_nDataSize;
	if ( Value )
	{
		m_nDataSize += sprintf(lpWritePtr,"%s", "TRUE");
	}
	else
	{
		m_nDataSize += sprintf(lpWritePtr,"%s", "FALSE");
	}

	ASSERT(m_nDataSize <= MAX_TRACE_BUFFER_SIZE);
	return *this;
}

CDebugTrace& CDebugTrace::operator << (char cValue)
{
	char* lpWritePtr = m_szPrintBuffer + m_nDataSize;
	m_nDataSize += sprintf(lpWritePtr,"%c", cValue);

	ASSERT(m_nDataSize <= MAX_TRACE_BUFFER_SIZE);
	return *this;
}

CDebugTrace& CDebugTrace::operator << (int8_t nValue)
{
	char* lpWritePtr = m_szPrintBuffer + m_nDataSize;
	m_nDataSize += sprintf(lpWritePtr,"%PRIi8", nValue);

	ASSERT(m_nDataSize <= MAX_TRACE_BUFFER_SIZE);
	return *this;
}

CDebugTrace& CDebugTrace::operator << (uint8_t nValue)
{
	char* lpWritePtr = m_szPrintBuffer + m_nDataSize;
	m_nDataSize += sprintf(lpWritePtr,"%PRIu8", nValue);

	ASSERT(m_nDataSize <= MAX_TRACE_BUFFER_SIZE);
	return *this;
}

CDebugTrace& CDebugTrace::operator << (int16_t nValue)
{
	char* lpWritePtr = m_szPrintBuffer + m_nDataSize;
	m_nDataSize += sprintf(lpWritePtr,"%PRIi16", nValue);

	ASSERT(m_nDataSize <= MAX_TRACE_BUFFER_SIZE);
	return *this;
}

CDebugTrace& CDebugTrace::operator << (uint16_t nValue)
{
	char* lpWritePtr = m_szPrintBuffer + m_nDataSize;
	m_nDataSize += sprintf(lpWritePtr,"%PRIu16", nValue);

	ASSERT(m_nDataSize <= MAX_TRACE_BUFFER_SIZE);
	return *this;
}

CDebugTrace& CDebugTrace::operator << (int32_t nValue)
{
	char* lpWritePtr = m_szPrintBuffer + m_nDataSize;
	m_nDataSize += sprintf(lpWritePtr,"%PRIi32", nValue);

	ASSERT(m_nDataSize <= MAX_TRACE_BUFFER_SIZE);
	return *this;
}

CDebugTrace& CDebugTrace::operator << (uint32_t nValue)
{
	char* lpWritePtr = m_szPrintBuffer + m_nDataSize;
	m_nDataSize += sprintf(lpWritePtr,"%PRIu32", nValue);

	ASSERT(m_nDataSize <= MAX_TRACE_BUFFER_SIZE);
	return *this;
}

CDebugTrace& CDebugTrace::operator << (int64_t nValue)
{
	char* lpWritePtr = m_szPrintBuffer + m_nDataSize;
	m_nDataSize += sprintf(lpWritePtr,"%PRIi64", nValue);

	ASSERT(m_nDataSize <= MAX_TRACE_BUFFER_SIZE);
	return *this;
}

CDebugTrace& CDebugTrace::operator << (uint64_t nValue)
{
	char* lpWritePtr = m_szPrintBuffer + m_nDataSize;
	m_nDataSize += sprintf(lpWritePtr,"%PRIu64", nValue);

	ASSERT(m_nDataSize <= MAX_TRACE_BUFFER_SIZE);
	return *this;
}

CDebugTrace& CDebugTrace::operator << (float fValue)
{
	char* lpWritePtr = m_szPrintBuffer + m_nDataSize;
	m_nDataSize += sprintf(lpWritePtr,"%f", fValue);

	ASSERT(m_nDataSize <= MAX_TRACE_BUFFER_SIZE);
	return *this;
}

CDebugTrace& CDebugTrace::operator << (double fValue)
{
	char* lpWritePtr = m_szPrintBuffer + m_nDataSize;
	m_nDataSize += sprintf(lpWritePtr,"%f", fValue);

	ASSERT(m_nDataSize <= MAX_TRACE_BUFFER_SIZE);
	return *this;
}

CDebugTrace& CDebugTrace::operator << (const char* apStrVal)
{
	char* lpWritePtr = m_szPrintBuffer + m_nDataSize;
	if(NULL == apStrVal)
	{
		m_nDataSize += sprintf(lpWritePtr,"%s", "NULL");
	}
	else
	{
		m_nDataSize += sprintf(lpWritePtr,"%s", apStrVal);
	}

	ASSERT(m_nDataSize <= MAX_TRACE_BUFFER_SIZE);
	return *this;
}

void CDebugTrace::TraceFormat(const char* pFmt, ...)
{
#ifndef _WIN32
	//��%I64d�滻��%lld
	const char *pPtr1 = pFmt;
	char szFormat[512] = {0};

	char *pPtr2 = strstr(pPtr1,"%I64d");	
	if (pPtr2 != NULL)
	{
		int nCopyLen = 0;
		while (pPtr2 != NULL)
		{
			memcpy(szFormat+nCopyLen,pPtr1,pPtr2-pPtr1);
			nCopyLen += (pPtr2-pPtr1);

			strcpy(szFormat + nCopyLen, "%lld");
			nCopyLen += strlen("%lld");

			pPtr1 = pPtr2 + strlen("%I64d");
			pPtr2 = strstr(pPtr1,"%I64d");
		}
		strcpy(szFormat+nCopyLen,pPtr1);
		pPtr1 = szFormat;
	}

	va_list argptr;
	//��strPrintBuff׷�ӵ�mszPrintBuff��
	char* lpWritePtr = m_szPrintBuffer + m_nDataSize;
	va_start( argptr,pPtr1 );
	m_nDataSize += vsprintf(lpWritePtr, pPtr1, argptr );
	va_end(argptr);

#else
	va_list argptr;
	//��strPrintBuff׷�ӵ�mszPrintBuff��
	char* lpWritePtr = m_szPrintBuffer + m_nDataSize;

	//�ѿɱ�������л���strPrintBuff��
	va_start(argptr, pFmt);
	m_nDataSize += vsprintf(lpWritePtr, pFmt, argptr );	
	va_end( argptr );
#endif

	//����EndTrace������ӡ
	EndTrace(*this);
}

//=============================================================================
// ������־����
void CDebugTrace::SetTraceLevel(trace_level_t nLogLevel)
{
	CDebugTrace::m_nLogLevel = nLogLevel;
}

// ������־�ļ�
void CDebugTrace::SetLogFileName(const char* szLogFile)
{
	if(NULL != szLogFile)
	{
		strcpy(CDebugTrace::m_szLogFile, szLogFile);
	}
}

// ������־���ѡ��
void CDebugTrace::SetTraceOptions(uint32_t nOptions)
{
	CDebugTrace::m_nTraceOptions = nOptions;
}

// �����־���ѡ��
uint32_t CDebugTrace::GetTraceOptions(void)
{
	return CDebugTrace::m_nTraceOptions;
}

// �ж��ܷ������־
BOOL CDebugTrace::CanTrace(trace_level_t nLogLevel)
{
	return nLogLevel <= CDebugTrace::m_nLogLevel;
}

//��ʼ��ӡ
CDebugTrace& CDebugTrace::BeginTrace(trace_level_t nLogLevel, 
	const char *pSrcFile,int iSrcLine)
{
	CDebugTrace *lpDebugTrace = NULL;
	lpDebugTrace = &goDebugTrace;
	lpDebugTrace->m_oCriticalSection.Lock();

	ASSERT(lpDebugTrace != NULL);
	lpDebugTrace->m_nDataSize = 0;	 //�Ѵ�ӡ�����ݳ�����0

	//���Ҫ�����ʱ��,������־�������־������ʱ��( ��:��:���� )
	if( CDebugTrace::m_nTraceOptions & Timestamp ) 
	{
		struct timeb loTimeb;
		ftime( &loTimeb ); 
		struct tm * lstrutime = localtime( &loTimeb.time );

		char lszTraceBuff[20];
		sprintf( lszTraceBuff,"%02d:%02d:%02d:%03d ",\
			lstrutime->tm_hour,lstrutime->tm_min,lstrutime->tm_sec,loTimeb.millitm );

		*lpDebugTrace << lszTraceBuff;
	}

	//���Ҫ�������־����,������־�������־����
	if ( CDebugTrace::m_nTraceOptions & LogLevel )
	{
		*lpDebugTrace << nLogLevel << ' ';
	}

	//���Ҫ�����Դ�ļ������к�,������־�����Դ�ļ������к�
	if ( CDebugTrace::m_nTraceOptions & FileAndLine ) 
	{
		*lpDebugTrace << pSrcFile << "( " << iSrcLine << " ) ";
	}

	//���ض�������
	return *lpDebugTrace;

}

//������ӡ
void CDebugTrace::EndTrace(CDebugTrace& oDebugTrace)
{
#ifdef WIN32
#ifdef _DEBUG
	OutputDebugStringA(oDebugTrace.m_szPrintBuffer); 
#endif
#endif	

	//��Ҫ�����������̨,�����־��Ϣ�ڿ���̨Ҳ��ӡһ��
	if (CDebugTrace::m_nTraceOptions & PrintToConsole ) 
		printf("%s", oDebugTrace.m_szPrintBuffer);

	//��Ҫ��д�ļ�����������־�ļ���,�����־��Ϣд���ļ���
	if ((CDebugTrace::m_nTraceOptions & AppendToFile )
		&& (strlen(CDebugTrace::m_szLogFile) > 1))
	{
		FILE * lfpTraceFile = NULL;
		lfpTraceFile = fopen( CDebugTrace::m_szLogFile, "a" ); 	
		if(lfpTraceFile != NULL )
		{
			fprintf(lfpTraceFile, "%s", oDebugTrace.m_szPrintBuffer);
			fclose(lfpTraceFile);
		}
	}

	oDebugTrace.m_oCriticalSection.UnLock();
}

//����ʧ�ܴ�����
void CDebugTrace::AssertFail(const char* strCond, char* strFile, uint32_t nLine)
{
#ifdef WIN32    
	char szMessage[1024];
	sprintf( szMessage, "\nDebug Assertion Failed!\n File: %s \nLine:%d \nCond: ASSERT( %s ); \n" 
		,strFile, nLine, strCond );    

	//��������Դ�����
	OutputDebugStringA(szMessage); 

	sprintf( szMessage, " Debug Assertion Failed!\n\n Condition:  ASSERT( %s );       \n SourceFile: %s       \n LineNum:    %d \n\n Continue?",
		strCond,strFile,nLine );
	int nResult = MessageBoxA( NULL,szMessage, "Assert failure", MB_OKCANCEL+MB_ICONERROR );    
	if ( nResult == IDCANCEL )
	{
		FatalExit( -1 );
	}
	else
	{
		DebugBreak();
	}
#else    
	//LINUX
	printf( "Debug Assertion Failed!\n\nCondition:  ASSERT( %s );       \nSourceFile: %s       \nLineNum:    %d \n\nContinue?",
		strCond,strFile,nLine );		

	char lcUserInput =  getchar( );	
	if ( lcUserInput != 'y' && lcUserInput != 'Y' )
	{
		exit( -1 );
	}
#endif 
}
