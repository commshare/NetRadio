#include "TcpNetTrans.h"
#include "DebugTrace.h"
#include "NetworkAPI.h"

//=============================================================================
#ifdef _WIN32
#pragma comment(lib,"Ws2_32.lib")
#endif

//=============================================================================
#define DEFAULT_ENCRYPT_KEY			"24698B5E-69E9-485B-BAB7-FC685D4AEFCC"
#define DEFAULT_ENCRYPT_KEY_SIZE	16

//=============================================================================
CTcpNetTrans::CTcpNetTrans(void)
	: m_CheckStateThread(&TcpCheckStateThread)
	, m_SendThread(&TcpSendThread)
	, m_RecvThread(&TcpRecvThread)
{
	m_pNetEvent = NULL;
	m_hSocket = INVALID_SOCKET;

	m_enTcpState = ENUM_TCP_STATE_NONE;
	m_enEncryptType = ENUM_ENCRYPT_AES;

	memset(m_szSvrAddr, 0, sizeof(m_szSvrAddr));
	m_nSvrPort = 0;

#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
}


CTcpNetTrans::~CTcpNetTrans(void)
{
	Close();

#ifdef _WIN32
	WSACleanup();
#endif
}

BOOL CTcpNetTrans::Open(ITcpNetEvent* pNetEvent, uint16_t nPort)
{
	// �������
	ASSERT(pNetEvent);
	if(NULL == pNetEvent)
	{
		return FALSE;
	}

	// �ظ��������
	if(INVALID_SOCKET != m_hSocket)
	{
		return FALSE;
	}

	m_pNetEvent = pNetEvent;
	BOOL bResult = FALSE;

	do 
	{
		// �����׽���
		m_hSocket = socket(AF_INET, SOCK_STREAM, 0);
		if(INVALID_SOCKET == m_hSocket)
		{
			TraceLogError("CTcpNetTrans::Open ����SOCKET�׽���ʧ�� Error=%d\n", 
				WSAGetLastError());
			break;
		}

		// �󶨶˿�
		if(0 != nPort)
		{
			SOCKADDR_IN addr;
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = INADDR_ANY;
			addr.sin_port = htons(nPort);

			if (SOCKET_ERROR == bind(m_hSocket, (LPSOCKADDR)&addr,sizeof(addr)))
			{
				TraceLogError("CTcpNetTrans::Open SOCKET�󶨶˿�%dʧ��\n", nPort);
				break;
			}
		}

		// ����PackBuffer
		if(!m_PackBuffer.Create(DEFAULT_ENCRYPT_KEY, DEFAULT_ENCRYPT_KEY_SIZE))
			break;

		// ����״̬����߳�
		if(!m_CheckStateThread.StartThread(this, 1))
			break;
		// ���������߳�
		if(!m_SendThread.StartThread(this, 1))
			break;
		// ���������߳�
		if(!m_RecvThread.StartThread(this, 1))
			break;

		// ����״̬
		SetTcpState(ENUM_TCP_STATE_OPEN);

		bResult = TRUE;

	} while (FALSE);

	if(!bResult)
	{
		Close();
	}

	return bResult;
}

BOOL CTcpNetTrans::IsOpen(void) const
{
	return (INVALID_SOCKET != m_hSocket);
}

void CTcpNetTrans::Close(void)
{
	if(INVALID_SOCKET != m_hSocket)
	{
		// ����״̬
		SetTcpState(ENUM_TCP_STATE_CLOSE);

		// �ر��׽���
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;

		// �ȴ������߳��˳�
		m_RecvThread.WaitThreadExit();

		// �ȴ������߳��˳�
		m_SendEvent.SetEvent();
		m_SendThread.WaitThreadExit();

		// �ȴ�״̬����߳��˳�
		m_CheckStateThread.WaitThreadExit();

		{
			// ��շ��Ͷ���
			CCriticalAutoLock loAutoLock(m_oSendQueueLock);
			while(m_SendPacketQueue.GetCount() > 0)
			{
				tcp_packet_t* pPacket = m_SendPacketQueue.RemoveHead();
				m_PacketCache.FreePacket(pPacket);
			}
		}

		{
			// ��ս��ն���
			CCriticalAutoLock loAutoLock(m_oRecvQueueLock);
			while(m_RecvPacketQueue.GetCount() > 0)
			{
				tcp_packet_t* pPacket = m_RecvPacketQueue.RemoveHead();
				m_PacketCache.FreePacket(pPacket);
			}
		}

		// ����PackBuffer
		m_PackBuffer.Destroy();
	}
}

BOOL CTcpNetTrans::SetEncryptType(ENUM_ENCRYPT_TYPE enType)
{
	m_enEncryptType = enType;
	return (m_enEncryptType != ENUM_ENCRYPT_NONE);
}

BOOL CTcpNetTrans::SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize)
{
	return m_PackBuffer.SetEncryptKey(szEncryptKey, nKeySize);
}

BOOL CTcpNetTrans::Connet(const char* szSvrAddr, uint16_t nSvrPort)
{
	// �������
	ASSERT(szSvrAddr != NULL);
	ASSERT(nSvrPort != 0);
	if(NULL == szSvrAddr || nSvrPort == 0)
		return FALSE;

	if(!IsOpen())
		return FALSE;

	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));

	// �жϲ����Ƿ�ΪIP��ַ
	if(IsIPAddr(szSvrAddr))
	{
		// szSvrAddrΪip��ַ
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(szSvrAddr);
		addr.sin_port = htons(nSvrPort);
	}
	else
	{
		// szSvrAddrΪ����
		struct hostent*	pHostent;
		pHostent = gethostbyname(szSvrAddr);
		if (pHostent == NULL)
		{
			TraceLogError("CTcpNetTrans::Connet:gethostbyname error: %s\n", 
				szSvrAddr);
			return FALSE;
		}

		memcpy(&addr.sin_addr, pHostent->h_addr, pHostent->h_length);
		addr.sin_family = pHostent->h_addrtype;
		addr.sin_port = htons(nSvrPort);
	}

	if (connect(m_hSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		uint32_t nErrorNo = 0;
#ifdef WIN32
		nErrorNo = WSAGetLastError();
#else
		nErrorNo = errno;
#endif
		TraceLogError("CTcpNetTrans::Connet:����ָ����ַ(%s:%d)ʧ��!ʧ�ܴ��룺%d\n", 
			szSvrAddr, nSvrPort, nErrorNo);
		return FALSE;
	}

	strncpy(m_szSvrAddr, szSvrAddr, MAX_URL_ADDR_SIZE);
	m_nSvrPort = nSvrPort;

	// ����״̬
	SetTcpState(ENUM_TCP_STATE_CONNECT);

	return TRUE;
}

BOOL CTcpNetTrans::GetConnectAddr(char* szSvrAddr, uint16_t& nSvrPort)
{
	ASSERT(szSvrAddr);
	if(NULL == szSvrAddr)
		return FALSE;

	strncpy(szSvrAddr, m_szSvrAddr, MAX_URL_ADDR_SIZE);
	nSvrPort = m_nSvrPort;

	return TRUE;
}

BOOL CTcpNetTrans::IsConnet(void) const
{
	return (m_enTcpState == ENUM_TCP_STATE_CONNECT);
}

uint32_t CTcpNetTrans::Send(const char* szBuffer, uint32_t nBufferSize)
{
	if(!IsConnet())
	{
		ASSERT(FALSE);
		return 0;
	}

	// �������ݰ�
	tcp_packet_t* pPacket = m_PacketCache.MallocPacket();
	ASSERT(pPacket);
	if(NULL != pPacket)
	{
		// ���
		pPacket->m_nPackSize = m_PackBuffer.Pack(szBuffer, nBufferSize, 
			pPacket->m_szPackBuffer, MAX_PACK_BUFFER_SIZE, m_enEncryptType);

		if(pPacket->m_nPackSize > 0)
		{
			{
				// ��������Ͷ���
				CCriticalAutoLock loAutoLock(m_oSendQueueLock);
				m_SendPacketQueue.AddTail(pPacket);
			}

			// ���������¼�
			m_SendEvent.SetEvent();
			return nBufferSize;
		}
		else
		{
			// �ͷ����ݰ�
			m_PacketCache.FreePacket(pPacket);
			return 0;
		}
	}
	return 0;
}

uint32_t CTcpNetTrans::Recv(char* szBuffer, uint32_t nBufferSize)
{
	// �������
	ASSERT(szBuffer);
	ASSERT(nBufferSize >= MAX_PACK_BUFFER_SIZE);
	if(NULL == szBuffer || nBufferSize < MAX_PACK_BUFFER_SIZE)
		return 0;

	uint16_t nRecvSize = 0;
	uint16_t nTimeStamp = 0;
	BOOL m_bIsComplete = FALSE;

	// ȡ�����ݰ�
	CCriticalAutoLock loAutoLock(m_oRecvQueueLock);

	// ��ǰ���ݰ�
	tcp_packet_t* pPacket = m_RecvPacketQueue.GetHead();
	if(NULL != pPacket)
	{
		int32_t nPackSize = m_PackBuffer.UnPack(pPacket->m_szPackBuffer, 
			pPacket->m_nPackSize, szBuffer, nRecvSize, nTimeStamp);

		BOOL bIsFreePacket = TRUE;
		if(nPackSize > 0)
		{
			// �ж��Ƿ�ȫ������
			if(nPackSize < pPacket->m_nPackSize)
			{
				// �ڴ濽��
				char* pBuffer = pPacket->m_szPackBuffer + nPackSize;
				uint32_t nSize = pPacket->m_nPackSize - nPackSize;

				memmove(pPacket->m_szPackBuffer, pBuffer, nSize);
				pPacket->m_nPackSize = nSize;

				bIsFreePacket = FALSE;

				// �¼���������ʱ�������ݰ���δ���꣩
				m_EventQueue.PushEvent(TCP_NET_EVENT_RECV);
				m_CheckEvent.SetEvent();
			}
		}

		if(bIsFreePacket)
		{
			// �Ӷ�����ɾ��
			m_RecvPacketQueue.RemoveHead();
			// �����ڴ�
			m_PacketCache.FreePacket(pPacket);
		}

		return nRecvSize;
	}

	return 0;
}

//=============================================================================
/// ����TCP״̬
ENUM_TCP_STATE CTcpNetTrans::SetTcpState(ENUM_TCP_STATE enNewState)
{
	ENUM_TCP_STATE enOldState = m_enTcpState;

	if(enNewState != m_enTcpState)
	{
		m_enTcpState = enNewState;

		switch(m_enTcpState)
		{
		case ENUM_TCP_STATE_OPEN:			///< ��
			m_EventQueue.PushEvent(TCP_NET_EVENT_OPEN);
			m_CheckEvent.SetEvent();
			break;
		case ENUM_TCP_STATE_CLOSE:			///< �ر�
			m_EventQueue.PushEvent(TCP_NET_EVENT_CLOSE);
			m_CheckEvent.SetEvent();
			break;
		case ENUM_TCP_STATE_CONNECT:		///< ���ӳɹ�
			m_EventQueue.PushEvent(TCP_NET_EVENT_CONNECT);
			m_CheckEvent.SetEvent();
			break;
		case ENUM_TCP_STATE_DISSCONNECT:	///< ���ӶϿ�
			if(enOldState == ENUM_TCP_STATE_CONNECT)
			{
				m_EventQueue.PushEvent(TCP_NET_EVENT_DISSCONNECT);
				m_CheckEvent.SetEvent();
			}
			break;
		default:
			break;
		}
	}
	return enOldState;
}

/// ���TCP״̬
ENUM_TCP_STATE CTcpNetTrans::GetTcpState(void) const
{
	return m_enTcpState;
}

/// TCP�հ�������
void CTcpNetTrans::TcpRecvThreadFunc(void)
{
	for(;;)
	{
		// �������ݰ�
		tcp_packet_t* pPacket = m_PacketCache.MallocPacket();
		ASSERT(pPacket);
		if(NULL != pPacket)
		{
			// ������������
			int32_t nRecvSize = recv(m_hSocket, pPacket->m_szPackBuffer, 
				MAX_PACK_BUFFER_SIZE, 0);

			if(nRecvSize > 0)
			{
				CCriticalAutoLock loAutoLock(m_oRecvQueueLock);

				// ��������ն���
				pPacket->m_nPackSize = nRecvSize;
				m_RecvPacketQueue.AddTail(pPacket);

				// �¼�����
				m_EventQueue.PushEvent(TCP_NET_EVENT_RECV);
				m_CheckEvent.SetEvent();
			}
			else
			{
				// �ͷ����ݰ��ڴ�
				m_PacketCache.FreePacket(pPacket);
				// ����״̬
				SetTcpState(ENUM_TCP_STATE_DISSCONNECT);
				break;
			}
		}

		if(INVALID_SOCKET ==  m_hSocket)
			return;
	}
}

/// TCP����������
void CTcpNetTrans::TcpSendThreadFunc(void)
{
	for(;;)
	{
		if(INVALID_SOCKET ==  m_hSocket)
			return;

		// �ȴ��¼�
		m_SendEvent.WaitEvent();
		m_SendEvent.ResetEvent();

		CCriticalAutoLock loAutoLock(m_oSendQueueLock);
		while(!m_SendPacketQueue.IsEmpty())
		{
			tcp_packet_t* pPacket = m_SendPacketQueue.RemoveHead();
			if(NULL != pPacket)
			{
				// �������ݰ�
				send(m_hSocket, pPacket->m_szPackBuffer, pPacket->m_nPackSize, 0);
				// �������ݰ��ڴ�ռ�
				m_PacketCache.FreePacket(pPacket);

				// �¼�����
				m_EventQueue.PushEvent(TCP_NET_EVENT_SEND);
				m_CheckEvent.SetEvent();
			}
		}
	}
}

/// TCP״̬��麯��
void CTcpNetTrans::TcpCheckStateFunk(void)
{
	for(;;)
	{
		// �ȴ��¼�
		m_CheckEvent.WaitEvent();
		m_CheckEvent.ResetEvent();

		while(!m_EventQueue.IsEmpty())
		{
			// ���¼�������ȡ��һ���¼�
			ENUM_TCP_NET_EVENT enEvent = m_EventQueue.PopEvent();
			m_pNetEvent->OnEvent(enEvent);
		}		

		if(INVALID_SOCKET ==  m_hSocket)
			return;
	}
}

//TCP�հ������߳�
unsigned int CTcpNetTrans::TcpRecvThread(void *pParam)
{
	CTcpNetTrans* pThis = (CTcpNetTrans*)pParam;
	pThis->TcpRecvThreadFunc();
	return 0;
}

//TCP���������߳�
unsigned int CTcpNetTrans::TcpSendThread(void *pParam)
{
	CTcpNetTrans* pThis = (CTcpNetTrans*)pParam;
	pThis->TcpSendThreadFunc();
	return 0;
}

//TCP״̬����߳�
unsigned int CTcpNetTrans::TcpCheckStateThread(void *pParam)
{
	CTcpNetTrans* pThis = (CTcpNetTrans*)pParam;
	pThis->TcpCheckStateFunk();
	return 0;
}
