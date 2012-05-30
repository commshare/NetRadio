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
/// \file    : EpollSendQueue.h
/// \brief   : Epoll���Ͷ���
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-30
///============================================================================
#ifndef __EPOLL_SEND_QUEUE_H__
#define __EPOLL_SEND_QUEUE_H__

#ifndef _WIN32

#include "TypeDefine.h"
#include "CacheTmpl.h"
#include "CriticalSection.h"

//=============================================================================
/// ���Ͷ��з���ߴ�
#define SEND_QUEUE_GROUP_SIZE	16

//=============================================================================
typedef struct _packet_node_t
{
	SOCKET m_hSocket;					///< SOCKET���
	tcp_packet_t* m_pPacket;			///< TCP���ݰ�ָ��ָ��
}packet_node_t;

//=============================================================================
class CEpollSendQueue
{
public:
	CEpollSendQueue(void);
	~CEpollSendQueue(void);

public:
	/// �������ݰ�
	BOOL SendPacket(SOCKET hSocket, tcp_packet_t* pPacket);
	/// �������ݰ�
	void SendPacket(SOCKET hSocket);
	/// ɾ��ָ��SOCKET��ȫ�����ݰ�
	void RemoveAllPacket(SOCKET hSocket);
	/// ɾ��ȫ�����ݰ�
	void RemoveAllPacket(void);

public:
	/// ���һ�����е����ݰ�
	tcp_packet_t* GetFreePacket(void);

private:
	/// ����SOCKET���ڵķ���
	uint32_t CalcSocketGroup(SOCKET hSocket);

private:
	/// TCP���ݰ�����
	CCacheTmpl<tcp_packet_t> m_PacketCache;
	/// ���ݰ��ڵ㻺��
	CCacheTmpl<packet_node_t> m_PacketNodeCache;

	/// ���Ͷ���
	CListTmpl<packet_node_t*> m_SendQueue[SEND_QUEUE_GROUP_SIZE];		
	/// ���Ͷ����ٽ�
	CCriticalSection m_SendQueueLock[SEND_QUEUE_GROUP_SIZE];
};

#endif //_WIN32
#endif //__EPOLL_SEND_QUEUE_H__
