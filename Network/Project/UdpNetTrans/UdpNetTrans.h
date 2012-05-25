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
/// \file    : UdpNetTrans.h
/// \brief   : Udp���紫����
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-23
///============================================================================
#ifndef __UDP_NET_TRANS_H__
#define __UDP_NET_TRANS_H__

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>
#include <WinSock2.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netdb.h>
#include <execinfo.h>
#include <errno.h>
#endif

#include "TypeDefine.h"
#include "Thread.h"
#include "Event.h"
#include "CacheTmpl.h"
#include "IUdpNetTrans.h"
#include "UdpPackBuffer.h"
#include "UdpPacketQueue.h"
#include "UdpEventQueue.h"

//=============================================================================
/// UDP_NET_STATE
enum ENUM_UDP_STATE
{
	ENUM_UDP_STATE_NONE,			///< ռλ�� ������
	ENUM_UDP_STATE_OPEN,			///< ��
	ENUM_UDP_STATE_CLOSE,			///< �ر�
};

//=============================================================================
class CUdpNetTrans : public IUdpNetTrans
{
public:
	CUdpNetTrans(void);
	~CUdpNetTrans(void);

public:
	/// ����ʵ��
	virtual BOOL Open(IUdpNetEvent* pNetEvent, const char* szLocalAddr = NULL, 
		uint16_t nPort = 0);
	/// �Ƿ񴴽�
	virtual BOOL IsOpen(void) const;
	/// �ر�
	virtual void Close(void);

	/// ���ü�������
	virtual BOOL SetEncryptType(ENUM_ENCRYPT_TYPE enType = ENUM_ENCRYPT_AES);
	/// ���ü�����Կ
	virtual BOOL SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize);

	/// �������ݰ�(�첽��ʽ)
	virtual uint32_t Send(const char* szBuffer, uint32_t nBufferSize, 
		const char* szSvrAddr, uint16_t nSvrPort);

	virtual uint32_t Send(const char* szBuffer, uint32_t nBufferSize, 
		const sockaddr_in* pSvrAddr);

	virtual uint32_t Send(const char* szBuffer, uint32_t nBufferSize, 
		uint32_t nSvrAddr, uint16_t nSvrPort);

	/// �������ݰ�
	virtual uint32_t Recv(char* szBuffer, uint32_t nBufferSize, 
		char* szSvrAddr, uint16_t& nSvrPort);

	virtual uint32_t Recv(char* szBuffer, uint32_t nBufferSize, 
		sockaddr_in* pSvrAddr);

	virtual uint32_t Recv(char* szBuffer, uint32_t nBufferSize, 
		uint32_t& nSvrAddr, uint16_t& nSvrPort);

private:
	/// ����UDP״̬
	ENUM_UDP_STATE SetUdpState(ENUM_UDP_STATE enNewState);
	/// ���UDP״̬
	ENUM_UDP_STATE GetUdpState(void) const;

	/// UDP�հ�������
	void UdpRecvThreadFunc(void);
	/// UDP����������
	void UdpSendThreadFunc(void);
	/// UDP״̬��麯��
	void UdpCheckStateFunk(void);

private:
	//UDP�հ������߳�
	static unsigned int UdpRecvThread(void *pParam);
	//UDP���������߳�
	static unsigned int UdpSendThread(void *pParam);
	//UDP״̬����߳�
	static unsigned int UdpCheckStateThread(void *pParam);

private:
	ENUM_ENCRYPT_TYPE m_enEncryptType;	///< ��������
	IUdpNetEvent* m_pNetEvent;			///< �����¼��ص���ָ��
	SOCKET m_hSocket;					///< SOCKET���

	ENUM_UDP_STATE m_enUdpState;		///< ״̬��

	CThread	m_CheckStateThread;			///< ״̬����߳�
	CThread m_SendThread;				///< �����߳�
	CThread m_RecvThread;				///< �����߳�

	CUdpEventQueue m_EventQueue;		///< �¼�����

	CEvent	m_CheckEvent;				///< ״̬����¼�
	CEvent	m_SendEvent;				///< �����¼�

	CUdpPackBuffer m_PackBuffer;		///< UDP���
	CCacheTmpl<udp_packet_t> m_PacketCache;		///< UDP���ݰ�����

	CUdpPacketQueue m_SendPacketQueue;	///< UDP���ݰ����Ͷ���
	CUdpPacketQueue m_RecvPacketQueue;	///< UDP���ݰ����ն���

	CCriticalSection m_oSendQueueLock;	///< ���Ͷ����ٽ�
	CCriticalSection m_oRecvQueueLock;	///< ���ն����ٽ�
};

#endif //__UDP_NET_TRANS_H__
