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
/// \file    : TypeDeff.h
/// \brief   : ͨ�����Ͷ���ͷ�ļ�
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-16
///============================================================================

#ifndef __TYPE_DEFF_H__
#define __TYPE_DEFF_H__

// inttypes.h
#ifdef _WIN32
#include "types\\stdint.h"
#else
#include <stdint.h>
#endif //_WIN32

#ifndef BOOL
typedef int		BOOL;
#define TRUE	1
#define FALSE	0
#endif

#ifndef TCHAR
#ifdef _UNICODE
typedef wchar_t TCHAR;
#else
typedef char	TCHAR;
#endif
#endif //TCHAR

//=============================================================================
/// ������ض���
/// IP��ַ������󳤶�
#define MAX_IP_ADDR_SIZE		16
/// URL��ַ������󳤶�
#define MAX_URL_ADDR_SIZE		128

#ifndef SOCKET
typedef uint32_t SOCKET;
#endif 

/// ��ЧSOCKET���
#ifndef INVALID_SOCKET
#define INVALID_SOCKET			(SOCKET)(~0)
#endif

/// SOCKET����
#ifndef SOCKET_ERROR
#define SOCKET_ERROR			(-1)
#endif

/// ���ݰ���󳤶�
#define MAX_PACK_BUFFER_SIZE	2048

//=============================================================================
/// ������ض���
#ifndef HANDLE
typedef void* HANDLE;
#endif

#ifndef _WIN32
#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE	((HANDLE)(int32_t)-1)
#endif
#endif

//=============================================================================
// struct _tcp_packet_t
typedef struct _tcp_packet_t
{
	char m_szPackBuffer[MAX_PACK_BUFFER_SIZE];	///< ������
	uint16_t m_nPackSize;						///< ���ݳ���
}tcp_packet_t;

//=============================================================================
// struct _udp_packet_t
typedef struct _udp_packet_t
{
	char m_szPackBuffer[MAX_PACK_BUFFER_SIZE];	///< ������
	uint16_t m_nPackSize;						///< ���ݳ���

	uint32_t m_nSvrAddr;						///< ��������ַ
	uint16_t m_nSvrPort;						///< �������˿�
}udp_packet_t;

#endif //__TYPE_DEFF_H__
