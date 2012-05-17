/******************************************************************************
* Copyright (c) 2012, Letion
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the University of California, Berkeley nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/
///============================================================================
/// \file    : DebugTrace.h
/// \brief   : ��־������ͷ�ļ�
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-16
///============================================================================
#ifndef __DEBUG_TRACE_H__
#define __DEBUG_TRACE_H__

#include "TypeDefine.h"
#include "CriticalSection.h"

//=============================================================================
#ifdef WIN32
#pragma warning(disable:4141 4996)
#endif

#ifdef	_WIN32
#ifndef _DEBUGTRACE_EXPORT
#ifdef	_DEBUG
#pragma comment(lib,"libDebugTraceD.lib")
#else
#pragma comment(lib,"libDebugTrace.lib")
#endif	//_TCPNETTRANS_EXPORT
#endif	//_TCPNETTRANS_EXPORT
#endif	//_WIN32

//=============================================================================
#ifdef ASSERT
#undef ASSERT
#endif

//  ���¶���ASSERT��  
#ifdef _DEBUG
#define ASSERT(f)		if(f)			\
							;			\
						else            \
							CDebugTrace::AssertFail((#f), __FILE__, __LINE__)
#else
#define ASSERT(f)
#endif

//=============================================================================
//RecordLog������ʽ����־���
//eg: RecordLog(1,"����:" << lszName<<"����:"<< 26 << "�Ա�" << TRUE);	
#define RecordLog(level, args) \
	if(!CDebugTrace::CanTrace(level)) ; else \
	CDebugTrace::EndTrace(CDebugTrace::BeginTrace(level, __FILE__, __LINE__) << args << '\n')

//TraceLogXϵ��������printf��ʽ���,X��ʾ��־����
//eg:TRACE0("������%s,���䣺%d,�Ա�:%d",lszName,26,TRUE);
#define TraceLogFatal \
	if (!CDebugTrace::CanTrace(CDebugTrace::FATAL_LEVEL)) 	;  else\
	CDebugTrace::BeginTrace(FATAL_LEVEL, __FILE__, __LINE__).TraceFormat

#define TraceLogAlert \
	if (!CDebugTrace::CanTrace(CDebugTrace::ALERT_LEVEL)) 	;  else\
	CDebugTrace::BeginTrace(ALERT_LEVEL, __FILE__, __LINE__).TraceFormat

#define TraceLogError \
	if (!CDebugTrace::CanTrace(CDebugTrace::ERROR_LEVEL)) 	;  else\
	CDebugTrace::BeginTrace(ERROR_LEVEL, __FILE__, __LINE__).TraceFormat

#define TraceLogWarn \
	if (!CDebugTrace::CanTrace(CDebugTrace::WARN_LEVEL)) 	;  else\
	CDebugTrace::BeginTrace(WARN_LEVEL, __FILE__, __LINE__).TraceFormat

#define TraceLogInfo \
	if (!CDebugTrace::CanTrace(CDebugTrace::INFO_LEVEL)) 	;  else\
	CDebugTrace::BeginTrace(INFO_LEVEL, __FILE__, __LINE__).TraceFormat

#define TraceLogDebug \
	if (!CDebugTrace::CanTrace(CDebugTrace::DEBUG_LEVEL)) 	;  else\
	CDebugTrace::BeginTrace(DEBUG_LEVEL, __FILE__, __LINE__).TraceFormat

//ȡ��TRACE����
#undef TRACE
#undef TRACE_FATAL
#undef TRACE_ALERT
#undef TRACE_ERROR
#undef TRACE_WARN
#undef TRACE_INFO
#undef TRACE_DEBUG

//���¶���TRACE���
#ifdef _DEBUG			
//���԰�
#define TRACE RecordLog	//��ӡ��־

#define TRACE_FATAL TraceLogFatal
#define TRACE_ALERT TraceLogAlert
#define TRACE_ERROR	TraceLogError
#define TRACE_WARN  TraceLogWarn
#define TRACE_INFO  TraceLogInfo
#define TRACE_DEBUG TraceLogDebug
#else					
//���а�
#define TRACE(level, args) 
#define TRACE_FATAL ;
#define TRACE_ALERT ;
#define TRACE_ERROR	;
#define TRACE_WARN  ;
#define TRACE_INFO  ;
#define TRACE_DEBUG ;

#endif //_DEBUG

//=============================================================================
#define MAX_TRACE_BUFFER_SIZE	(1024*4)

#ifndef MAX_PATH
#define MAX_PATH 512
#endif

//=============================================================================
// class CDebugTrace
class CDebugTrace
{
public:
	// ��ӡѡ��
	enum Options
	{
		Timestamp		= 1,	///< ��ӡʱ��
		LogLevel		= 2,	///< ��ӡ��־����
		FileAndLine		= 4,	///< ��ӡ�ļ������к�
		AppendToFile	= 8,	///< ����־׷�ӵ��ļ�
		PrintToConsole	= 16,	///< �����־������̨
	};

	// ��־����
	typedef enum _TraceLevel
	{
		FATAL_LEVEL,			///< ��������
		ALERT_LEVEL,			///< ���ش���
		ERROR_LEVEL,			///< һ���Դ���
		WARN_LEVEL,				///< ����
		INFO_LEVEL,				///< ��Ϣ
		DEBUG_LEVEL,			///< ������Ϣ				
	}trace_level_t;

public:
	CDebugTrace(void);
	~CDebugTrace(void);

public:
	//������־����
	static void SetTraceLevel(trace_level_t nLogLevel);

	//������־�ļ���
	static void SetLogFileName(const char* szLogFile);

	// ����TRACEѡ��
	static void SetTraceOptions(uint32_t nOptions);

	//ȡ��TRACEѡ��
	static uint32_t GetTraceOptions(void);

	//�жϸ��������Ƿ���Դ�ӡ
	static BOOL CanTrace(trace_level_t nLogLevel);					

	//��ʼ��ӡ
	static CDebugTrace& BeginTrace(trace_level_t nLogLevel, 
		const char *pSrcFile,int iSrcLine);	

	//������ӡ
	static void EndTrace(CDebugTrace& oDebugTrace);

	//����ʧ�ܴ�����
	static void AssertFail(const char* strCond, char* strFile, uint32_t nLine);

public:
	//���պ���printf�����Ƹ�ʽ��ӡ��־
	void TraceFormat(const char* pFmt,...);	

public:
	//���·ֱ����������������
	CDebugTrace& operator << (bool bValue);
	CDebugTrace& operator << (char cValue);
	CDebugTrace& operator << (int8_t nValue);
	CDebugTrace& operator << (uint8_t nValue);	
	CDebugTrace& operator << (int16_t nValue);
	CDebugTrace& operator << (uint16_t nValue);
	CDebugTrace& operator << (int32_t nValue);
	CDebugTrace& operator << (uint32_t nValue);
	CDebugTrace& operator << (int64_t nValue);
	CDebugTrace& operator << (uint64_t nValue);
	CDebugTrace& operator << (float fValue);
	CDebugTrace& operator << (double fValue);
	CDebugTrace& operator << (const char *apStrVal);	

private:
	char m_szPrintBuffer[MAX_TRACE_BUFFER_SIZE];	///< ������
	uint32_t m_nDataSize;							///< ���泤��

	CCriticalSection m_oCriticalSection;			///< �ٽ���

private:
	static trace_level_t m_nLogLevel;				///< ��־����
	static char		m_szLogFile[MAX_PATH];			///< ��־�ļ�
	static uint32_t m_nTraceOptions;				///< ��ӡ��־ѡ��
};


#endif //__DEBUG_TRACE_H__
