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
/// \file    : TcpEpollServer.h
/// \brief   : TCP��ɶ˿ڷ�����
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-24
///============================================================================
#ifndef __TCP_EPOLL_SERVER_H__
#define __TCP_EPOLL_SERVER_H__

#ifndef _WIN32

#include <time.h>
#include <stdlib.h>

#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/resource.h>

#include "TypeDefine.h"
#include "TcpServerBase.h"
#include "ListTmpl.h"
#include "Thread.h"
#include "CacheTmpl.h"

//=============================================================================
class CTcpEpollServer  : public CTcpServerBase
{
public:
	CTcpEpollServer(void);
	~CTcpEpollServer(void);

public:
	/// ����TCP������
	virtual BOOL Create(uint16_t nSvrPort, ITcpServerEvent* pSvrEvent,
		ENUM_ENCRYPT_TYPE enType);
	/// ����TCP������
	virtual void Destroy(void);

	/// ��������
	virtual uint32_t Send(SOCKET hSocket, const char* szDataBuffer,
		uint16_t nDataSize);

private:
	/// ����Epoll
	int CreateEpoll(uint32_t nThreadNumber);
	/// ����Epoll
	void DestroyEpoll(int hEpollHandle);
	/// EpollAccept
	BOOL EpollAcceptSocket(SOCKET hSocket, const sockaddr_in& SockAddr);

	/// ����SOCKET�׽���
	SOCKET CreateSocket(uint16_t nSvrPort);
	/// ����SOCKET�׽���
	void DestroySocket(SOCKET hSocket);

private:
	/// ��ɶ˿��̺߳���
	void EpollWaitFunc(void);
	/// ���Ӽ���̺߳���
	void ConnectCheckFunc(void);

	/// ��ɶ˿��߳�
	static unsigned int EpollWaitThread(void *pParam);
	/// ���Ӽ���߳�
	static unsigned int ConnectCheckThread(void* pParam);

private:
	int m_hEpollHandle;				///< EPOLL���

	CTcpContext m_ListenContext;	///< ���������ľ��

	/// TCP���ݰ�����
	CCacheTmpl<tcp_packet_t> m_PacketCache;

	CThread m_EpollWaitThread;		///< Epoll�ȴ��߳�
	CThread m_CheckThread;			///< ����߳�
};

#endif //_WIN32

#endif //__TCP_EPOLL_SERVER_H__

