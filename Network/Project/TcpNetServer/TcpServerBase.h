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
/// \file    : TcpServerBase.h
/// \brief   : TCP����������
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-28
///============================================================================
#ifndef __TCP_SERVER_BASE_H__
#define __TCP_SERVER_BASE_H__

#include "TypeDefine.h"
#include "ITcpNetServer.h"
#include "ListTmpl.h"
#include "Thread.h"
#include "CacheTmpl.h"
#include "EncryptInfo.h"

//=============================================================================
class CTcpServerBase : public ITcpNetServer
{
public:
	CTcpServerBase(void);
	virtual ~CTcpServerBase(void);

public:
	/// ���ü�������
	virtual BOOL SetEncryptType(ENUM_ENCRYPT_TYPE enType);
	/// ���ü�����Կ
	virtual BOOL SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize);

	/// ���Context�Ƿ���Ч
	virtual BOOL ContextIsValid(const CTcpContext* pContext);
	//�Ͽ�ָ������
	virtual BOOL ResetContext(CTcpContext* pContext);

protected:
	/// ����SOCKET�׽���
	virtual SOCKET CreateSocket(uint16_t nSvrPort);
	/// ����SOCKET�׽���
	virtual void DestroySocket(SOCKET hSocket);

protected:
	/// ����Context
	CTcpContext* CreateContext(void);
	/// �ͷ�Context
	void DestroyContext(CTcpContext* pContext);

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

protected:
	/// �����������
	BOOL DealRecvData(const char* szDataBuffer, uint32_t nDataSize, 
		CTcpContext *pContext);

protected:
	SOCKET m_hListenSocket;				///< SOCKET�׽���
	ITcpServerEvent* m_pTcpEvent;		///< �¼��ص��ӿ�ָ��

	/// ���ʹ����
	CTcpPackBuffer m_SendPackBuffer;	///< TCP���ʹ����

	/// ������Ϣ
	CEncryptInfo m_EncryptInfo;

	/// Context����
	CListTmpl<CTcpContext*> m_ContextList;
	/// Context�����ٽ�
	mutable CCriticalSection m_ContextListLock;
};

#endif //__TCP_SERVER_BASE_H__
