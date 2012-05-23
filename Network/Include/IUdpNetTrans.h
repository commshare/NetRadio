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
/// \file    : IUdpNetTrans.h
/// \brief   : UDP���紫��ӿ���
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-23
///============================================================================

#ifndef __I_UDP_NET_TRANS_H__
#define __I_UDP_NET_TRANS_H__

#include "TypeDefine.h"
#include "IEncrypt.h"
#include "InterfaceDefine.h"

//=============================================================================
#ifdef	_WIN32
#ifndef _UDPNETTRANS_EXPORT
#ifdef	_DEBUG
#pragma comment(lib,"UdpNetTransD.lib")
#else
#pragma comment(lib,"UdpNetTrans.lib")
#endif	//_DEBUG
#endif	//_UDPNETTRANS_EXPORT
#endif	//_WIN32

//=============================================================================
/// UDP�����¼�
enum ENUM_UDP_NET_EVENT
{
	UDP_NET_EVENT_NONE,			///< ռλ�� ������
	UDP_NET_EVENT_OPEN,			///< ��
	UDP_NET_EVENT_CLOSE,		///< �ر�
	UDP_NET_EVENT_RECV,			///< ��������
	UDP_NET_EVENT_SEND,			///< �������� 
};

//=============================================================================
// class IUdpNetEvent
class IUdpNetEvent
{
public:
	// �����¼��ص�
	virtual void OnEvent(ENUM_UDP_NET_EVENT enEvent) = 0;
};

//=============================================================================
// {F7D02430-C59C-4EC7-B216-92501AF1CB47}
DEFINE_GUID(CLSID_IUdpNetTrans, 
	0xf7d02430, 0xc59c, 0x4ec7, 0xb2, 0x16, 0x92, 0x50, 0x1a, 0xf1, 0xcb, 0x47);

//=============================================================================
// class IUdpNetTrans
class IUdpNetTrans
{
public:
	/// ����ʵ��
	virtual BOOL Open(IUdpNetEvent* pNetEvent, const char* szLocalAddr = NULL, 
		uint16_t nPort = 0) = 0;
	/// �Ƿ񴴽�
	virtual BOOL IsOpen(void) const = 0;
	/// �ر�
	virtual void Close(void) = 0;

	/// ���ü�������
	virtual BOOL SetEncryptType(ENUM_ENCRYPT_TYPE enType = ENUM_ENCRYPT_AES) = 0;
	/// ���ü�����Կ
	virtual BOOL SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize) = 0;

	/// �������ݰ�(�첽��ʽ)
	virtual uint32_t Send(const char* szBuffer, uint32_t nBufferSize, 
		const char* szSvrAddr, uint16_t nSvrPort) = 0;

	virtual uint32_t Send(const char* szBuffer, uint32_t nBufferSize, 
		const sockaddr_in* pSvrAddr) = 0;

	virtual uint32_t Send(const char* szBuffer, uint32_t nBufferSize, 
		uint32_t nSvrAddr, uint16_t nSvrPort) = 0;

	/// �������ݰ�
	virtual uint32_t Recv(char* szBuffer, uint32_t nBufferSize, 
		char* szSvrAddr, uint16_t& nSvrPort) = 0;

	virtual uint32_t Recv(char* szBuffer, uint32_t nBufferSize, 
		sockaddr_in* pSvrAddr) = 0;

	virtual uint32_t Recv(char* szBuffer, uint32_t nBufferSize, 
		uint32_t& nSvrAddr, uint16_t& nSvrPort) = 0;

};

//=============================================================================
// API
/// ����UDP����ӿ�
IRESULT CreateUdpNetTrans(const CLSID& oInterfaceID, void** ppInterface);

/// �ͷ�UDP����ӿ�
IRESULT DestroyUdpNetTrans(const CLSID& oInterfaceID, void* pInterface);

#endif //__I_UDP_NET_TRANS_H__
