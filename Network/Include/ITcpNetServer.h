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
/// \file    : ITcpNetServer.h
/// \brief   : TCP����������ӿ�
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-24
///============================================================================

#ifndef	__I_TCP_NET_SERVER_H__
#define __I_TCP_NET_SERVER_H__

#include "TypeDefine.h"
#include "IEncrypt.h"
#include "InterfaceDefine.h"
#include "TcpPackBuffer.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <WinSock2.h>
#include <MSWSock.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h> 
#endif

//=============================================================================
#ifdef	_WIN32
#ifndef _UDPNETTRANS_EXPORT
#ifdef	_DEBUG
#pragma comment(lib,"TcpNetServerD.lib")
#else
#pragma comment(lib,"TcpNetServer.lib")
#endif	//_DEBUG
#endif	//_UDPNETTRANS_EXPORT
#endif	//_WIN32

//=============================================================================
/// TCP���������ݼ�����Կ����
#define TCP_SVR_ENCRYPT_KEY_SIZE		16
/// TCP���������ӳ�ʱʱ��(120��)
#define TCP_SVR_CONNECT_TIMOUT_SOCOND	120	

//=============================================================================
// class CTcpContext
class CTcpContext
{
	friend class CTcpServerBase;
#ifdef _WIN32
	friend class CTcpIocpServer;
	friend class CTcpEpollServer;
#else
	friend class CTcpEpollServer;
#endif
public:
	CTcpContext(void);
	virtual ~CTcpContext(void);

public:
	/// ���SOCKET���
	SOCKET GetSocket(void) const;
	/// ���SOCKET��ַ
	sockaddr_in GetSocketAddr(void) const;

protected:
	virtual BOOL CheckValid(void);

private:
	SOCKET m_hSocket;				///< SOCKET���
	sockaddr_in m_oSocketAddr;		///< SOCKET��ַ
	uint64_t m_i64ContextKey;		///< CONTEXT��ʾ���������޸ģ�
	CTcpPackBuffer m_TcpPackBuffer;	///< TCP�����
};

//=============================================================================
// class ITcpServerEvent
class ITcpServerEvent
{
public:
	// ����CTcpContext
	virtual CTcpContext* MallocContext(void) = 0;
	// ����CTcpContext
	virtual void FreeContext(CTcpContext* pContext) = 0;

	// �յ�����
	virtual void OnRecvData(char* apPacket, uint32_t nPacketSize, 
		CTcpContext * apContext) = 0;
};

//=============================================================================
// {6E9F1210-23F5-43E3-B0FE-E64CFF4DB538}
DEFINE_GUID(CLSID_ITcpNetServer, 
	0x6e9f1210, 0x23f5, 0x43e3, 0xb0, 0xfe, 0xe6, 0x4c, 0xff, 0x4d, 0xb5, 0x38);

//=============================================================================
// class ITcpNetServer
class ITcpNetServer
{
public:
	/// ����TCP������
	virtual BOOL Create(uint16_t nSvrPort, ITcpServerEvent* pSvrEvent, 
		ENUM_ENCRYPT_TYPE enType = ENUM_ENCRYPT_AES) = 0;
	/// ����TCP������
	virtual void Destroy(void) = 0;

	/// ���ü�������
	virtual BOOL SetEncryptType(ENUM_ENCRYPT_TYPE enType) = 0;
	/// ���ü�����Կ
	virtual BOOL SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize) = 0;

	/// ���Context�Ƿ���Ч
	virtual BOOL ContextIsValid(const CTcpContext* pContext) = 0;
	/// �Ͽ�ָ������
	virtual BOOL ResetContext(CTcpContext* pContext) = 0;

	/// ��������
	virtual uint32_t Send(SOCKET hSocket, const char* szDataBuffer, 
		uint16_t nDataSize) = 0;
};

//=============================================================================
/// ����TCP�������ӿ�
IRESULT CreateTcpNetServer(const CLSID& oInterfaceID, void** ppInterface);

/// �ͷ�TCP�������ӿ�
IRESULT DestroyTcpNetServer(const CLSID& oInterfaceID, void* pInterface);

#endif //__I_TCP_NET_SERVER_H__
