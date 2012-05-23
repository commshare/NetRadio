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
/// \file    : TcpNetTrans.h
/// \brief   : TCP���紫����
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-21
///============================================================================
#ifndef __TCP_NET_TRANS_H__
#define __TCP_NET_TRANS_H__

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
#include "ITcpNetTrans.h"
#include "TcpPackBuffer.h"
#include "TcpPacketCache.h"
#include "TcpPacketQueue.h"
#include "TcpEventQueue.h"

//=============================================================================
/// TCP_NET_STATE
enum ENUM_TCP_STATE
{
	ENUM_TCP_STATE_NONE,			///< ռλ�� ������
	ENUM_TCP_STATE_OPEN,			///< ��
	ENUM_TCP_STATE_CLOSE,			///< �ر�
	ENUM_TCP_STATE_CONNECT,			///< ���ӳɹ�
	ENUM_TCP_STATE_DISSCONNECT,		///< ���ӶϿ�
};

//=============================================================================
class CTcpNetTrans : public ITcpNetTrans
{
public:
	CTcpNetTrans(void);
	~CTcpNetTrans(void);

public:
	/// ����ʵ��
	virtual BOOL Open(ITcpNetEvent* pNetEvent, uint16_t nPort=0);
	/// �Ƿ񴴽�
	virtual BOOL IsOpen(void) const;
	/// �ر�
	virtual void Close(void);

	/// ���ü�������
	virtual BOOL SetEncryptType(ENUM_ENCRYPT_TYPE enType = ENUM_ENCRYPT_AES);
	/// ���ü�����Կ
	virtual BOOL SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize);

	/// ���ӵ��Է�
	virtual BOOL Connet(const char* szSvrAddr, uint16_t nSvrPort);
	/// ���������Ϣ
	virtual BOOL GetConnectAddr(char* szSvrAddr, uint16_t& nSvrPort);
	/// �Ƿ�������
	virtual BOOL IsConnet(void) const;

	/// �������ݰ�(�첽��ʽ)
	virtual uint32_t Send(const char* szBuffer, uint32_t nBufferSize);
	/// �������ݰ�
	virtual uint32_t Recv(char* szBuffer, uint32_t nBufferSize);

private:
	/// ����TCP״̬
	ENUM_TCP_STATE SetTcpState(ENUM_TCP_STATE enNewState);
	/// ���TCP״̬
	ENUM_TCP_STATE GetTcpState(void) const;

	/// TCP�հ�������
	void TcpRecvThreadFunc(void);
	/// TCP����������
	void TcpSendThreadFunc(void);
	/// TCP״̬��麯��
	void TcpCheckStateFunk(void);

private:
	//TCP�հ������߳�
	static unsigned int TcpRecvThread(void *pParam);
	//TCP���������߳�
	static unsigned int TcpSendThread(void *pParam);
	//TCP״̬����߳�
	static unsigned int TcpCheckStateThread(void *pParam);

private:
	ENUM_ENCRYPT_TYPE m_enEncryptType;	///< ��������
	ITcpNetEvent* m_pNetEvent;			///< �����¼��ص���ָ��
	SOCKET m_hSocket;					///< SOCKET���

	char m_szSvrAddr[MAX_URL_ADDR_SIZE];///< ������IP��ַ
	uint16_t m_nSvrPort;				///< �������˿�

	ENUM_TCP_STATE m_enTcpState;		///< ״̬��

	CThread	m_CheckStateThread;			///< ״̬����߳�
	CThread m_SendThread;				///< �����߳�
	CThread m_RecvThread;				///< �����߳�

	CTcpEventQueue m_EventQueue;		///< �¼�����

	CEvent	m_CheckEvent;				///< ״̬����¼�
	CEvent	m_SendEvent;				///< �����¼�

	CTcpPackBuffer m_PackBuffer;		///< TCP���
	CTcpPacketCache m_PacketCache;		///< TCP���ݰ�����

	CTcpPacketQueue m_SendPacketQueue;	///< TCP���ݰ����Ͷ���
	CTcpPacketQueue m_RecvPacketQueue;	///< TCP���ݰ����ն���

	CCriticalSection m_oSendQueueLock;	///< ���Ͷ����ٽ�
	CCriticalSection m_oRecvQueueLock;	///< ���ն����ٽ�
};

#endif //__TCP_NET_TRANS_H__

