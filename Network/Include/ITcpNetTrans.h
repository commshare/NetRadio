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
/// \file    : ITcpNetTrans.h
/// \brief   : TCP���紫��ӿ���
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-17
///============================================================================

#ifndef __I_TCP_NET_TRANS_H__
#define __I_TCP_NET_TRANS_H__

#include "TypeDefine.h"
#include "IEncrypt.h"
#include "InterfaceDefine.h"

//=============================================================================
#ifdef	_WIN32
#ifndef _TCPNETTRANS_EXPORT
#ifdef	_DEBUG
#pragma comment(lib,"TcpNetTransD.lib")
#else
#pragma comment(lib,"TcpNetTrans.lib")
#endif	//_TCPNETTRANS_EXPORT
#endif	//_TCPNETTRANS_EXPORT
#endif	//_WIN32

//=============================================================================
struct sockaddr_in;

//=============================================================================
/// TCP�����¼�
enum ENUM_TCP_NET_EVENT
{
	TCP_NET_EVENT_NONE,			///< ռλ�� ������
	TCP_NET_EVENT_OPEN,			///< ��
	TCP_NET_EVENT_CLOSE,		///< �ر�
	TCP_NET_EVENT_CONNECT,		///< ���ӳɹ�
	TCP_NET_EVENT_DISSCONNECT,	///< ���ӶϿ�
	TCP_NET_EVENT_RECV,			///< ��������
	TCP_NET_EVENT_SEND,			///< �������� 
};

//=============================================================================
// class ITcpNetEvent
class ITcpNetEvent
{
public:
	// �����¼��ص�
	virtual void OnEvent(ENUM_TCP_NET_EVENT enEvent) = 0;
};

//=============================================================================
// ITcpNetTrans�ӿڱ�ʾ
// {78353EBC-EF1D-4E68-8E1A-558B70B36223}
DEFINE_GUID(CLSID_ITcpNetTrans, 
	0x78353ebc, 0xef1d, 0x4e68, 0x8e, 0x1a, 0x55, 0x8b, 0x70, 0xb3, 0x62, 0x23);

//=============================================================================
// class ITcpNetTrans
class ITcpNetTrans
{
public:
	/// ����ʵ��
	virtual BOOL Open(ITcpNetEvent* pNetEvent, uint16_t nPort = 0);
	/// �Ƿ񴴽�
	virtual BOOL IsOpen(void) const = 0;
	/// �ر�
	virtual void Close(void) = 0;

	/// ���ü�������
	virtual BOOL SetEncryptType(ENUM_ENCRYPT_TYPE enType = ENUM_ENCRYPT_AES) = 0;
	/// ���ü�����Կ
	virtual BOOL SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize) = 0;

	/// ���ӵ��Է�
	virtual BOOL Connet(const char* szSvrAddr, uint16_t nSvrPort) = 0;
	/// �Ƿ�������
	virtual BOOL IsConnet(void) const = 0;

	/// �������ݰ�(�첽��ʽ)
	virtual uint32_t Send(const char* szBuffer, uint32_t nBufferSize) = 0;
	/// �������ݰ�
	virtual uint32_t Recv(char* szBuffer, uint32_t nBufferSize) = 0;
};

//=============================================================================
// API
/// ����TCP����ӿ�
IRESULT CreateTcpNetTrans(const CLSID& oInterfaceID, void** ppInterface);

/// �ͷ�TCP����ӿ�
IRESULT DestroyTcpNetTrans(const CLSID& oInterfaceID, void* pInterface);


#endif //__I_TCP_NET_TRANS_H__