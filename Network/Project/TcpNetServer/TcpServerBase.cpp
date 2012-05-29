#include "TcpServerBase.h"
#include "DebugTrace.h"

CTcpServerBase::CTcpServerBase(void)
	: m_hListenSocket(INVALID_SOCKET)
	, m_pTcpEvent(NULL)
{
}


CTcpServerBase::~CTcpServerBase(void)
{
}

/// ���ü�������
BOOL CTcpServerBase::SetEncryptType(ENUM_ENCRYPT_TYPE enType)
{
	if(m_SendPackBuffer.SetEncryptType(enType))
	{
		m_EncryptInfo.SetEncryptType(enType);
		return TRUE;
	}
	return FALSE;
}

/// ���ü�����Կ
BOOL CTcpServerBase::SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize)
{
	if(m_SendPackBuffer.SetEncryptKey(szEncryptKey, nKeySize))
	{
		m_EncryptInfo.SetEncryptKey(szEncryptKey, nKeySize);
		return TRUE;
	}
	return FALSE;
}

/// ���Context�Ƿ���Ч
BOOL CTcpServerBase::ContextIsValid(const CTcpContext* pContext)
{
	// �������
	ASSERT(pContext);
	if(NULL == pContext)
		return FALSE;

	BOOL bResult = FALSE;
	POSITION pos = (POSITION)pContext->m_i64ContextKey;

	CCriticalAutoLock loAutoLock(m_ContextListLock);

	// ͨ��KEYֵȡ��Contextָ�루��ָ���п����Ѿ�ʧЧ�������޸ĸ�ָ��)
	CTcpContext* pTempContext = m_ContextList.GetAt(pos);
	// �ж�ָ���ַ�Ƿ���ͬ
	if(pTempContext == pContext)
	{
		// �ж�SOCKET����Ƿ���ͬ
		if(pTempContext->m_hSocket == pContext->m_hSocket)
			bResult = TRUE;
	}
	return bResult;
}

BOOL CTcpServerBase::ResetContext(CTcpContext* pContext)
{
	// �������
	ASSERT(pContext);
	if(NULL == pContext)
		return FALSE;

	BOOL bResult = FALSE;
	POSITION pos = (POSITION)pContext->m_i64ContextKey;

	CCriticalAutoLock loAutoLock(m_ContextListLock);

	// ͨ��KEYֵȡ��Contextָ�루��ָ���п����Ѿ�ʧЧ�������޸ĸ�ָ��)
	CTcpContext* pTempContext = m_ContextList.GetAt(pos);
	// �ж�ָ���ַ�Ƿ���ͬ
	if(pTempContext == pContext)
	{
		// �ж�SOCKET����Ƿ���ͬ
		if(pTempContext->m_hSocket == pContext->m_hSocket 
			&& pTempContext->m_oSocketAddr.sin_addr.s_addr 
			== pContext->m_oSocketAddr.sin_addr.s_addr)
		{
			DestroySocket(pContext->m_hSocket);
			pContext->m_hSocket = INVALID_SOCKET;
			bResult = TRUE;
		}
	}
	return bResult;
}

//=============================================================================
/// ����SOCKET�׽���
SOCKET CTcpServerBase::CreateSocket(uint16_t nSvrPort)
{
	// ��������Socket
	SOCKET hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	return hSocket;
}

/// ����SOCKET�׽���
void CTcpServerBase::DestroySocket(SOCKET hSocket)
{
	closesocket(hSocket);
	hSocket = INVALID_SOCKET;
}

//=============================================================================
/// ����Context
CTcpContext* CTcpServerBase::CreateContext(void)
{
	CTcpContext* pContext = m_pTcpEvent->MallocContext();
	if(NULL != pContext)
	{
		// ����Context�е�PackBuffer
		pContext->m_TcpPackBuffer.Create(m_EncryptInfo.GetEncryptType());
		// ���ü�����Կ
		pContext->m_TcpPackBuffer.SetEncryptKey(m_EncryptInfo.GetEncryptKey(), 
			m_EncryptInfo.GetEncryptKeySize());
	}
	return pContext;
}

/// �ͷ�Context
void CTcpServerBase::DestroyContext(CTcpContext* pContext)
{
	if(NULL != pContext)
	{
		// ����Context�е�PackBuffer
		pContext->m_TcpPackBuffer.Destroy();
	}
}

/// ���Context
BOOL CTcpServerBase::AddTcpContext(CTcpContext* pContext)
{
	// �������
	ASSERT(pContext);
	if(NULL == pContext)
		return FALSE;

	CCriticalAutoLock loAutoLock(m_ContextListLock);
	POSITION pos = m_ContextList.AddTail(pContext);
	pContext->m_i64ContextKey = (uint64_t)pos;
	return TRUE;
}

/// ɾ��Context
BOOL CTcpServerBase::RemoveTcpContext(CTcpContext* pContext)
{
	// �������
	ASSERT(pContext);
	if(NULL == pContext)
		return FALSE;

	BOOL bResult = FALSE;
	POSITION pos = (POSITION)pContext->m_i64ContextKey;

	CCriticalAutoLock loAutoLock(m_ContextListLock);

	// ͨ��KEYֵȡ��Contextָ�루��ָ���п����Ѿ�ʧЧ�������޸ĸ�ָ��)
	CTcpContext* pTempContext = m_ContextList.GetAt(pos);
	// �ж�ָ���ַ�Ƿ���ͬ
	if(pTempContext == pContext)
	{
		// �ж�SOCKET����Ƿ���ͬ
		if(pTempContext->m_hSocket == pContext->m_hSocket)
		{
			m_ContextList.RemoveAt(pos);
			DestroyContext(pContext);
			bResult = TRUE;
		}
	}
	return bResult;
}

/// �ر�����Context
void CTcpServerBase::CloseAllContext(void)
{
	//�ر�����������SOCKET
	CCriticalAutoLock loAutoLock(m_ContextListLock);

	POSITION pos = m_ContextList.GetHeadPosition();
	while(NULL != pos)
	{
		CTcpContext* pContext = m_ContextList.GetNext(pos);
		DestroySocket(pContext->m_hSocket);
		pContext->m_hSocket = INVALID_SOCKET;
	}
}

/// �ر���Ч����
void CTcpServerBase::CheckInvalidContext(void)
{
	CCriticalAutoLock loAutoLock(m_ContextListLock);

	//�����Ч����
	POSITION pos = m_ContextList.GetHeadPosition();
	while(NULL != pos)
	{
		CTcpContext* pContext = m_ContextList.GetNext(pos);
		if(!pContext->CheckValid())
		{
			DestroySocket(pContext->m_hSocket);
			pContext->m_hSocket = INVALID_SOCKET;
		}
	}
}

/// �����������
uint32_t CTcpServerBase::GetTcpContextCount(void) const
{
	CCriticalAutoLock loAutoLock(m_ContextListLock);
	return m_ContextList.GetCount();
}

//=============================================================================
/// �����������
BOOL CTcpServerBase::DealRecvData(const char* szDataBuffer, uint32_t nDataSize, 
	CTcpContext *pContext)
{
	// �������
	ASSERT(szDataBuffer);
	ASSERT(nDataSize);
	ASSERT(pContext);
	if(0 == nDataSize || NULL == szDataBuffer || NULL == pContext)
		return FALSE;

	char szRecvBuffer[MAX_PACK_BUFFER_SIZE] = {0};
	uint16_t nRecvSize = MAX_PACK_BUFFER_SIZE;
	uint16_t nTimeStamp = 0;

	const char* pBuffer = szDataBuffer;
	uint16_t nSize = nDataSize;

	for(;;)
	{
		// ���
		int32_t nPackSize = pContext->m_TcpPackBuffer.UnPack(pBuffer, 
			nSize, szRecvBuffer, nRecvSize, nTimeStamp);
		if(nPackSize <= 0)
			break;

		// �ص�����
		m_pTcpEvent->OnRecvData(szRecvBuffer, nRecvSize, pContext);

		// �ж��Ƿ�ȫ������
		if(nPackSize < nSize)
		{
			pBuffer = pBuffer + nPackSize;
			nSize = nSize - nPackSize;
		}
	}

	return TRUE;
}