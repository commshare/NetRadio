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
/// \file    : Thread.h
/// \brief   : �̴߳�����
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-21
///============================================================================
#ifndef __THREAD_H__
#define __THREAD_H__

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

#include "TypeDefine.h"

//=============================================================================
#ifndef THREAD_HANDLE
#ifdef _WIN32
/// �߳̾��
typedef int			THREAD_HANDLE;
#else
/// �߳̾��
typedef pthread_t	THREAD_HANDLE;
#endif
#endif

//=============================================================================
// class CThread
class CThread
{
public:
	CThread(unsigned int (*ThreadFuncPtr)(void *));
	~CThread(void);

public:
	/// ��ʼ�߳�
	BOOL StartThread(void* pThreadParam, uint32_t nThreadCount = 1);
	/// �����߳�
	BOOL StopThread(void);
	/// �ȴ��߳��˳�
	BOOL WaitThreadExit(void);

private:
#ifdef _WIN32
	/// WIN32�µ��̴߳�����
	unsigned int Win32ThreadFunc(void);
	static unsigned int __stdcall Win32ThreadFunc(void* pThreadParam);
#else
	/// linux�µ��̴߳�����
	void LinuxThreadFunc(void);
	static void* LinuxThreadFunc(void* pThreadParam);
#endif

private:
	unsigned int (*m_ThreadFuncPtr)(void *);	///< �̺߳���
	void* m_pThreadParam;						///< �̲߳���

	uint32_t m_nThreadCount;					///< �߳�����
	THREAD_HANDLE* m_pThreadHandle;				///< �߳̾��
};

#endif 
