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
/// \file    : TcpIocpServer.h
/// \brief   : TCP��ɶ˿ڷ�����
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-24
///============================================================================
#ifndef __TCP_IOCP_SERVER_H__
#define __TCP_IOCP_SERVER_H__

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <WinSock2.h>
#include <MSWSock.h>

#include "TypeDefine.h"
#include "ITcpNetServer.h"
#include "ListTmpl.h"
#include "Thread.h"
#include "CacheTmpl.h"

//=============================================================================
// �׽���IO��������
enum ENUM_IO_TYPE
{
	IO_ACCEPT,
	IO_READ,
	IO_WRITE,
};

// ����IO���ݽṹ
struct OVERLAPPEDPLUS 
{
	OVERLAPPED m_Overlapped;					// ÿһ���ص�����������ص��ṹ
	SOCKET	m_hSocket;							// ������������ʹ�õ�Socket
	ENUM_IO_TYPE m_enIOType;					// ��ʶ�������������(��Ӧ�����ö��)
	WSABUF	m_wsaBuffer;						// WSA���͵Ļ�����
	char	m_szBuffer[MAX_PACK_BUFFER_SIZE];	// �����WSABUF�������ַ��Ļ�����
	uint16_t m_nBufferSize;						// ����������
	uint64_t m_nOverlapKey;						// �ص��ṹ��ʶ
};

//=============================================================================
class CTcpIocpServer : public ITcpNetServer
{
public:
	CTcpIocpServer(void);
	~CTcpIocpServer(void);

public:
	/// ����TCP������
	virtual BOOL Create(uint16_t nSvrPort, ITcpServerEvent* pSvrEvent);
	/// ����TCP������
	virtual void Destroy(void);

	/// ���Context�Ƿ���Ч
	virtual BOOL ContextIsValid(const CTcpContext* pContext);
    //�Ͽ�ָ������
    virtual BOOL ResetContext(CTcpContext* pContext);

	/// ��������
	virtual uint32_t Send(SOCKET hSocket, const char* szDataBuffer, 
		uint16_t nDataSize);

private:
	/// ������ɶ˿�
	HANDLE CreateCompletePort(void);
	/// ������ɶ˿�
	void DestroyCompletePort(HANDLE hHandle);

	/// ����SOCKET�׽���
	SOCKET CreateSocket(uint16_t nSvrPort);
	/// ����SOCKET�׽���
	void DestroySocket(SOCKET hSocket);

	/// ����ACCEPT�¼�
	HANDLE CreateAcceptEvent(SOCKET hSocket);
	/// ����ACCEPT�¼�
	void DestroyAcceptEvent(HANDLE hEvent);

	/// ����AcceptEx����ָ��
	LPFN_ACCEPTEX GetAcceptExPtr(SOCKET hSocket);
	LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockAddrs(SOCKET hSocket);

private:
	/// �����ص��ṹ�ڴ�
	OVERLAPPEDPLUS* MallocOverlap(ENUM_IO_TYPE enIOType);
	/// �ͷ��ص��ṹ�ڴ�
	void FreeOverlap(OVERLAPPEDPLUS* pOverlap);

	/// ���AcceptEx����
	BOOL AddAcceptEx(OVERLAPPEDPLUS* pOverlap);
	/// ɾ��AcceptEx����
	BOOL RemoveAcceptEx(OVERLAPPEDPLUS* pOverlap);
	/// �ر�AcceptEx�����е�ȫ��SOCKET
	void CloseAllAcceptExSocket(void);
	/// ���AcceptEx���еĳ�ʱ����
	void CheckAcceptExTimeout(void);
	/// ���AcceptEx���гߴ�
	uint32_t GetAcceptExCount(void) const;

	/// ���Context
	BOOL AddTcpContext(CTcpContext* pContext);
	/// ɾ��Context
	BOOL RemoveTcpContext(CTcpContext* pContext);
	/// �ر�����Context
	void CloseAllContext(void);
	/// �ر���Ч����
	void CheckInvalidContext(void);
	/// �����������
	uint32_t GetTcpContextCount(void) const;

private:
	/// �����������
	BOOL DealRecvData(uint32_t nRecvDataLen, CTcpContext *pContext,
		OVERLAPPEDPLUS* pOverlapPlus);

	/// ��ɶ˿��̺߳���
	void CompletePortFunc(void);
	/// ���Ӽ���̺߳���
	void ConnectCheckFunc(void);

	/// ��ɶ˿��߳�
	static unsigned int CompletePortThread(void *pParam);
	/// ���Ӽ���߳�
	static unsigned int ConnectCheckThread(void* pParam);

private:
	HANDLE m_hIocp;					///< ��ɶ˿ھ��

	SOCKET m_hListenSocket;			///< SOCKET�׽���
	ITcpServerEvent* m_pEvent;		///< �¼��ص��ӿ�ָ��

	HANDLE m_hAcceptEvent;			///< �����¼�
	CTcpContext m_oListenContext;	/// �����˿ڵ������ľ��

	/// Context����
	CListTmpl<CTcpContext*> m_ContextList;
	/// Context�����ٽ�
	mutable CCriticalSection m_ContextListLock;

	/// ���Ӷ���
	CListTmpl<OVERLAPPEDPLUS*> m_AcceptExList;
	/// ���Ӷ����ٽ�
	mutable CCriticalSection m_AcceptExListLock;

	// AcceptEx �� GetAcceptExSockaddrs �ĺ���ָ�룬���ڵ�����������չ����
	LPFN_ACCEPTEX m_lpfnAcceptEx;                
	LPFN_GETACCEPTEXSOCKADDRS m_lpfnGetAcceptExSockAddrs; 

	CThread m_CompletePotrThread;	///< ��ɶ˿��߳�
	CThread m_CheckThread;			///< ����߳�

	/// �ص��ṹ����
	CCacheTmpl<OVERLAPPEDPLUS> m_OverloapCache;	

	//Write overlap
	WSAOVERLAPPED m_oWriteOverlap;      
	CCriticalSection m_oWriteOverlapLock;
};

#endif //_WIN32
#endif //__TCP_IOCP_SERVER_H__
