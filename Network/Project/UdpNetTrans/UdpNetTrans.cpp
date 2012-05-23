#include "UdpNetTrans.h"
#include "DebugTrace.h"
#include "NetworkAPI.h"

//=============================================================================
#ifdef _WIN32
#pragma comment(lib,"Ws2_32.lib")
#endif

//=============================================================================
#define DEFAULT_ENCRYPT_KEY			"D264CFE4-290A-463A-AA71-CA73EE524DC7"
#define DEFAULT_ENCRYPT_KEY_SIZE	16

//=============================================================================
CUdpNetTrans::CUdpNetTrans(void)
	: m_CheckStateThread(&UdpCheckStateThread)
	, m_SendThread(&UdpSendThread)
	, m_RecvThread(&UdpRecvThread)
{
	m_pNetEvent = NULL;
	m_hSocket = INVALID_SOCKET;

	m_enUdpState = ENUM_UDP_STATE_NONE;
	m_enEncryptType = ENUM_ENCRYPT_AES;

#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
}


CUdpNetTrans::~CUdpNetTrans(void)
{
	Close();

#ifdef _WIN32
	WSACleanup();
#endif
}

BOOL CUdpNetTrans::Open(IUdpNetEvent* pNetEvent, 
	const char* szLocalAddr, uint16_t nPort)
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
		m_hSocket = socket(AF_INET, SOCK_DGRAM, 0);
		if(INVALID_SOCKET == m_hSocket)
		{
			TraceLogError("CUdpNetTrans::Open ����SOCKET�׽���ʧ�� Error=%d\n", 
				WSAGetLastError());
			break;
		}

		struct sockaddr_in addr;
		memset(&addr, 0,sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(nPort);
		if (NULL != szLocalAddr)
		{
			SetSocketAddr(addr, inet_addr(szLocalAddr));
		}
		else
		{
			SetSocketAddr(addr, htonl(INADDR_ANY));
		}

		if (SOCKET_ERROR == bind(m_hSocket, (LPSOCKADDR)&addr,sizeof(addr)))
		{
			TraceLogError("CTcpNetTrans::Open SOCKET�󶨶˿�%dʧ��\n", nPort);
			break;
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
		SetUdpState(ENUM_UDP_STATE_OPEN);

		bResult = TRUE;

	} while (FALSE);

	if(!bResult)
	{
		Close();
	}

	return bResult;
}

BOOL CUdpNetTrans::IsOpen(void) const
{
	return (INVALID_SOCKET != m_hSocket);
}

void CUdpNetTrans::Close(void)
{
	if(INVALID_SOCKET != m_hSocket)
	{
		// ����״̬
		SetUdpState(ENUM_UDP_STATE_CLOSE);

		// �ر��׽���
		shutdown(m_hSocket, 0); 
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
				udp_packet_t* pPacket = m_SendPacketQueue.RemoveHead();
				m_PacketCache.FreePacket(pPacket);
			}
		}

		{
			// ��ս��ն���
			CCriticalAutoLock loAutoLock(m_oRecvQueueLock);
			while(m_RecvPacketQueue.GetCount() > 0)
			{
				udp_packet_t* pPacket = m_RecvPacketQueue.RemoveHead();
				m_PacketCache.FreePacket(pPacket);
			}
		}

		// ����PackBuffer
		m_PackBuffer.Destroy();
	}
}

BOOL CUdpNetTrans::SetEncryptType(ENUM_ENCRYPT_TYPE enType)
{
	m_enEncryptType = enType;
	return (m_enEncryptType != ENUM_ENCRYPT_NONE);
}

BOOL CUdpNetTrans::SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize)
{
	return m_PackBuffer.SetEncryptKey(szEncryptKey, nKeySize);
}

/// �������ݰ�(�첽��ʽ)
uint32_t CUdpNetTrans::Send(const char* szBuffer, uint32_t nBufferSize, 
	const char* szSvrAddr, uint16_t nSvrPort)
{
	/// �������
	ASSERT(szBuffer);
	ASSERT(szSvrAddr);
	ASSERT(nBufferSize != 0);
	ASSERT(nSvrPort != 0);
	
	if(NULL == szBuffer || NULL == szSvrAddr 
		|| 0 == nBufferSize || 0 == nSvrPort)
	{
		return 0;
	}

	if(!IsIPAddr(szSvrAddr))
		return 0;

	if(!IsOpen())
		return 0;

	uint32_t nAddr = ntohl(inet_addr(szSvrAddr));
	return Send(szBuffer, nBufferSize, nAddr, nSvrPort);
}


uint32_t CUdpNetTrans::Send(const char* szBuffer, uint32_t nBufferSize, 
	const sockaddr_in* pSvrAddr)
{
	/// �������
	ASSERT(szBuffer);
	ASSERT(nBufferSize != 0);
	ASSERT(pSvrAddr);

	if(NULL == szBuffer || 0 == nBufferSize || NULL == pSvrAddr)
	{
		return 0;
	}

	if(!IsOpen())
		return 0;

	uint32_t nAddr = ntohl(GetSocketAddr(*pSvrAddr));
	uint32_t nPort = ntohl(pSvrAddr->sin_port);

	return Send(szBuffer, nBufferSize, nAddr, nPort);
}

uint32_t CUdpNetTrans::Send(const char* szBuffer, uint32_t nBufferSize, 
	uint32_t nSvrAddr, uint16_t nSvrPort)
{
	/// �������
	ASSERT(szBuffer);
	ASSERT(nBufferSize != 0);
	ASSERT(nSvrPort != 0);

	if(NULL == szBuffer || 0 == nBufferSize || 0 == nSvrPort)
	{
		return 0;
	}

	if(!IsOpen())
		return 0;

	// �������ݰ�
	udp_packet_t* pPacket = m_PacketCache.MallocPacket();
	ASSERT(pPacket);
	if(NULL != pPacket)
	{
		// ���
		pPacket->m_nPackSize = m_PackBuffer.Pack(szBuffer, nBufferSize, 
			pPacket->m_szPackBuffer, MAX_PACK_BUFFER_SIZE, m_enEncryptType);

		if(pPacket->m_nPackSize > 0)
		{
			pPacket->m_nSvrAddr = nSvrAddr;
			pPacket->m_nSvrPort = nSvrPort;

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


/// �������ݰ�
uint32_t CUdpNetTrans::Recv(char* szBuffer, uint32_t nBufferSize, 
	char* szSvrAddr, uint16_t& nSvrPort)
{
	uint32_t nSvrAddr = 0;
	uint32_t nResult = Recv(szBuffer, nBufferSize, nSvrAddr, nSvrPort);
	strcpy(szSvrAddr, GetIPAddr(nSvrAddr));
	return nResult;
}

uint32_t CUdpNetTrans::Recv(char* szBuffer, uint32_t nBufferSize, 
	sockaddr_in* pSvrAddr)
{
	uint32_t nSvrAddr = 0;
	uint16_t nSvrPort = 0;

	uint32_t nResult = Recv(szBuffer, nBufferSize, nSvrAddr, nSvrPort);
	
	if(NULL != pSvrAddr)
	{
		memset(pSvrAddr, 0,sizeof(sockaddr_in));
		pSvrAddr->sin_family = AF_INET;
		pSvrAddr->sin_port = htons(nSvrPort);
		SetSocketAddr(*pSvrAddr, nSvrAddr);
	}

	return nResult;
}

uint32_t CUdpNetTrans::Recv(char* szBuffer, uint32_t nBufferSize, 
	uint32_t& nSvrAddr, uint16_t& nSvrPort)
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
	udp_packet_t* pPacket = m_RecvPacketQueue.RemoveHead();
	if(NULL != pPacket)
	{
		int32_t nPackSize = m_PackBuffer.UnPack(pPacket->m_szPackBuffer, 
			pPacket->m_nPackSize, szBuffer, nRecvSize, nTimeStamp);

		nSvrAddr = pPacket->m_nSvrAddr;
		nSvrPort = pPacket->m_nSvrPort;

		// �����ڴ�
		m_PacketCache.FreePacket(pPacket);
		return nRecvSize;
	}

	return 0;
}

//=============================================================================
/// ����UDP״̬
ENUM_UDP_STATE CUdpNetTrans::SetUdpState(ENUM_UDP_STATE enNewState)
{
	ENUM_UDP_STATE enOldState = m_enUdpState;

	if(enNewState != m_enUdpState)
	{
		m_enUdpState = enNewState;

		switch(m_enUdpState)
		{
		case ENUM_UDP_STATE_OPEN:			///< ��
			m_EventQueue.PushEvent(UDP_NET_EVENT_OPEN);
			m_CheckEvent.SetEvent();
			break;
		case ENUM_UDP_STATE_CLOSE:			///< �ر�
			m_EventQueue.PushEvent(UDP_NET_EVENT_CLOSE);
			m_CheckEvent.SetEvent();
			break;
		default:
			break;
		}
	}
	return enOldState;
}

/// ���UDP״̬
ENUM_UDP_STATE CUdpNetTrans::GetUdpState(void) const
{
	return m_enUdpState;
}

/// UDP�հ�������
void CUdpNetTrans::UdpRecvThreadFunc(void)
{
	for(;;)
	{
		// �������ݰ�
		udp_packet_t* pPacket = m_PacketCache.MallocPacket();
		ASSERT(pPacket);
		if(NULL != pPacket)
		{
			struct sockaddr_in addr;
			int nAddrSize = sizeof(addr);

			// ������������
			int32_t nRecvSize = recvfrom(m_hSocket, pPacket->m_szPackBuffer, 
				MAX_PACK_BUFFER_SIZE, 0, (sockaddr*)&addr, &nAddrSize);

			if(nRecvSize > 0)
			{
				CCriticalAutoLock loAutoLock(m_oRecvQueueLock);

				// ��������ն���
				pPacket->m_nPackSize = nRecvSize;
				m_RecvPacketQueue.AddTail(pPacket);

				// �¼�����
				m_EventQueue.PushEvent(UDP_NET_EVENT_RECV);
				m_CheckEvent.SetEvent();
			}
			else
			{
				// �ͷ����ݰ��ڴ�
				m_PacketCache.FreePacket(pPacket);
			}
		}

		if(INVALID_SOCKET ==  m_hSocket)
			return;
	}
}

/// UDP����������
void CUdpNetTrans::UdpSendThreadFunc(void)
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
			udp_packet_t* pPacket = m_SendPacketQueue.RemoveHead();
			if(NULL != pPacket)
			{
				struct sockaddr_in addr;
				memset(&addr, 0,sizeof(addr));
				addr.sin_family = AF_INET;
				addr.sin_port = htons(pPacket->m_nSvrPort);
				SetSocketAddr(addr, pPacket->m_nSvrAddr);

				// �������ݰ�
				sendto(m_hSocket, pPacket->m_szPackBuffer, pPacket->m_nPackSize, 
					0, (const sockaddr*)&addr, sizeof(addr));

				// �������ݰ��ڴ�ռ�
				m_PacketCache.FreePacket(pPacket);

				// �¼�����
				m_EventQueue.PushEvent(UDP_NET_EVENT_SEND);
				m_CheckEvent.SetEvent();
			}
		}
	}
}

/// UDP״̬��麯��
void CUdpNetTrans::UdpCheckStateFunk(void)
{
	for(;;)
	{
		// �ȴ��¼�
		m_CheckEvent.WaitEvent();
		m_CheckEvent.ResetEvent();

		while(!m_EventQueue.IsEmpty())
		{
			// ���¼�������ȡ��һ���¼�
			ENUM_UDP_NET_EVENT enEvent = m_EventQueue.PopEvent();
			m_pNetEvent->OnEvent(enEvent);
		}		

		if(INVALID_SOCKET ==  m_hSocket)
			return;
	}
}

//UDP�հ������߳�
unsigned int CUdpNetTrans::UdpRecvThread(void *pParam)
{
	CUdpNetTrans* pThis = (CUdpNetTrans*)pParam;
	pThis->UdpRecvThreadFunc();
	return 0;
}

//UDP���������߳�
unsigned int CUdpNetTrans::UdpSendThread(void *pParam)
{
	CUdpNetTrans* pThis = (CUdpNetTrans*)pParam;
	pThis->UdpSendThreadFunc();
	return 0;
}

//UDP״̬����߳�
unsigned int CUdpNetTrans::UdpCheckStateThread(void *pParam)
{
	CUdpNetTrans* pThis = (CUdpNetTrans*)pParam;
	pThis->UdpCheckStateFunk();
	return 0;
}


