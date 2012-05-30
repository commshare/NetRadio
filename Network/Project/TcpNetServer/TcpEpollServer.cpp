#include "TcpEpollServer.h"
#include "DebugTrace.h"
#include "NetworkAPI.h"

#ifndef _WIN32
//=============================================================================
//�趨�ļ�������������λ
//����������int sockfd��Ŀ��socket
// int value����Ϊ0ʱ����������Ϊ�����ģ���Ϊ��0ֵʱ����������Ϊ��������
static BOOL SetNonblock(int sockfd, BOOL bIsNonBlock)
{
	int oldflags = fcntl(sockfd, F_GETFL, 0);
	if(bIsNonBlock)
	{
		oldflags |= O_NONBLOCK;
	}
	else{
		oldflags &= ~O_NONBLOCK;
	}

	int result=fcntl(sockfd, F_SETFL, oldflags);
	if (result == -1)
	{
		TraceLogError("CTcpEpollServer SetNonblock ����ʧ�� ERROR=%s!\n",
			strerror(errno));
		return FALSE;
	}

	//Linux ������һ��ķ���/���ջ����������������ں˽ṹ,
	//���, ϵͳ���ƵĻ�����������ɷ��ʵĻ�����������.
	int liBufSize = 8192;
	socklen_t lilen = 4;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &liBufSize, lilen);
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &liBufSize, lilen);

	return TRUE;
}

/// ���ݰ���������
#define EPOLL_TCP_PACKET_CACHE_SIZE		10240

//=============================================================================
CTcpEpollServer::CTcpEpollServer(void)
	: m_hEpollHandle(-1)
	, m_EpollWaitThread(&EpollWaitThread)
	, m_CheckThread(&ConnectCheckThread)
	, m_PacketCache(EPOLL_TCP_PACKET_CACHE_SIZE)
{
}


CTcpEpollServer::~CTcpEpollServer(void)
{
	Destroy();
}

/// ����TCP������
BOOL CTcpEpollServer::Create(uint16_t nSvrPort, ITcpServerEvent* pSvrEvent,
	ENUM_ENCRYPT_TYPE enType)
{
	// �������
	ASSERT(pSvrEvent);
	ASSERT(nSvrPort);
	if(NULL == pSvrEvent || 0 == nSvrPort)
		return FALSE;

	BOOL bResult = FALSE;
	m_pTcpEvent = pSvrEvent;
	SetEncryptType(enType);

	do
	{
		// ����EPOLL
		uint32_t nProcessors = sysconf(_SC_NPROCESSORS_CONF);
		nProcessors = nProcessors == 0 ? 1 : nProcessors;
		uint32_t nThreadNumber = nProcessors * 2 + 2;

		m_hEpollHandle = CreateEpoll(nThreadNumber);
		if(-1 == m_hEpollHandle)
			break;

		// ����SOCKET
		m_hListenSocket = CreateSocket(nSvrPort);
		if(INVALID_SOCKET == m_hListenSocket)
			break;

		m_ListenContext.m_hSocket = m_hListenSocket;

		// SOCKET��EPOLL��
		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLET;			//���¼� ETģʽ
		ev.data.ptr = (void*)&m_ListenContext;

		if (epoll_ctl(m_hEpollHandle, EPOLL_CTL_ADD, m_hListenSocket, &ev) < 0)
		{
			TraceLogError("CTcpEpollServer::Create ��Epollʧ�� ERROR=%s!\n",
				strerror(errno));
			break;
		}

		// ����Epoll�߳�
		if(!m_EpollWaitThread.StartThread(this, nThreadNumber))
			break;

		// ��������߳�
		if(!m_CheckThread.StartThread(this))
			break;

		bResult = TRUE;
	} while (FALSE);

	if(!bResult)
	{
		Destroy();
	}

	return bResult;
}

void CTcpEpollServer::Destroy(void)
{
	// �ر�SOCKET
	if(m_hListenSocket != INVALID_SOCKET)
	{
		DestroySocket(m_hListenSocket);
		m_hListenSocket = INVALID_SOCKET;
		m_ListenContext.m_hSocket = INVALID_SOCKET;
	}

	//�ر�EPOLL���
	if (m_hEpollHandle != -1)
	{
		DestroyEpoll(m_hEpollHandle);
		m_hEpollHandle = -1;
	}

	// �ر�����SOCKET
	//�ر�����������SOCKET
	while (GetTcpContextCount() > 0)
	{
		CloseAllContext();
	}

	// �ȴ��߳��˳�
	m_EpollWaitThread.WaitThreadExit();
	m_CheckThread.WaitThreadExit();
}

/// ��������
uint32_t CTcpEpollServer::Send(SOCKET hSocket, const char* szDataBuffer,
	uint16_t nDataSize)
{
	// �������
	ASSERT(szDataBuffer);
	ASSERT(nDataSize);
	if(NULL == szDataBuffer || 0 == nDataSize)
		return 0;

	tcp_packet_t* pPacket = m_SendQueue.GetFreePacket();
	if(NULL != pPacket)
	{
		pPacket->m_nPackSize = m_SendPackBuffer.Pack(szDataBuffer, nDataSize, 
			pPacket->m_szPackBuffer, MAX_PACK_BUFFER_SIZE);
		if(pPacket->m_nPackSize > 0)
		{
			// �������ݰ�
			m_SendQueue.SendPacket(hSocket, pPacket);
			return nDataSize;
		}
	}
	return 0;

}

//=============================================================================
/// ����Epoll
int CTcpEpollServer::CreateEpoll(uint32_t nThreadNumber)
{
	//����EPOLL
	int hEpollHandle = epoll_create(nThreadNumber);
	if (hEpollHandle == -1)
	{
		TraceLogError("CTcpEpollServer::CreateEpoll ����Epollʧ�� ERROR=%s!\n",
			strerror(errno));
	}
	return hEpollHandle;
}

/// ����Epoll
void CTcpEpollServer::DestroyEpoll(int hEpollHandle)
{
	//�ر�EPOLL���
	if (hEpollHandle != -1)
	{
		close(hEpollHandle);
		hEpollHandle = -1;
	}
}

BOOL CTcpEpollServer::EpollAcceptSocket(SOCKET hSocket, const sockaddr_in& SockAddr)
{
	if(INVALID_SOCKET == hSocket)
		return FALSE;

    // ���ó̷�����ʽSOCKET
    if (!SetNonblock(hSocket, TRUE))
    {
        return FALSE;
    }

	// �ص��ϲ㴴��context
	CTcpContext *pContext = CreateContext();
	if (pContext == NULL)
	{
		return FALSE;
	}

	// Context ��ֵ
	pContext->m_hSocket = hSocket;
	memcpy(&pContext->m_oSocketAddr, &SockAddr, sizeof(sockaddr_in));

	// ���������
	AddTcpContext(pContext);

	struct epoll_event ev;
	ev.events	= EPOLLIN | EPOLLOUT | EPOLLET;  //ETģʽ+���¼�+д�¼�
	ev.data.ptr = (void*)pContext;
	if (epoll_ctl(m_hEpollHandle, EPOLL_CTL_ADD, hSocket, &ev) < 0)
	{
		RemoveTcpContext(pContext);
		DestroyContext(pContext);
		return FALSE;
	}

	return TRUE;
}

/// ����SOCKET�׽���
SOCKET CTcpEpollServer::CreateSocket(uint16_t nSvrPort)
{
	SOCKET hSocket = INVALID_SOCKET;

	BOOL bIsSucceed = FALSE;
	do
	{
		// ��������Socket
		SOCKET hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		if (hSocket == INVALID_SOCKET)
		{
			TraceLogError("CTcpEpollServer::CreateSocket ����SOCKETʧ�� ERROR=%s!\n",
				strerror(errno));
			break;
		}

		//����Socket�󶨵ķ�����IP��ַ���˿ڿ������á�
		//��ֹ�������ڷ�������ʱ���˿�δ���ͷţ�����޷����������ú��������ʹ���ˡ�
		unsigned int optval = 0x1;
		setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(unsigned int));

		//���ó̷�����ʽSOCKET
		if (!SetNonblock(hSocket,TRUE))
		{
			TraceLogError("CTcpEpollServer::CreateSocket SetNonblockʧ�� SOCKET=%d!\n",
				hSocket);
			break;
		}

		//�󶨶˿�
		sockaddr_in  addr;
		memset(&addr,0,sizeof(sockaddr_in));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(nSvrPort);
		if (SOCKET_ERROR == bind(hSocket,(sockaddr *)&addr, sizeof(sockaddr_in)))
		{
			TraceLogError("CTcpEpollServer::CreateSocket �󶨶˿�ʧ�� PORT=%d ERROR=%s!\n",
				nSvrPort, strerror(errno));
			break;
		}

		//��ʼ����
		if (listen(hSocket, 100) == SOCKET_ERROR)
		{
			TraceLogError("CTcpEpollServer::CreateSocket ����ʧ�� ERROR=%s!\n",
				strerror(errno));
			break;
		}

		bIsSucceed = TRUE;
	} while (FALSE);

	if(!bIsSucceed)
	{
		if(INVALID_SOCKET != hSocket)
		{
			close(hSocket);
			hSocket = INVALID_SOCKET;
		}
	}
	return hSocket;
}

/// ����SOCKET�׽���
void CTcpEpollServer::DestroySocket(SOCKET hSocket)
{
	if (hSocket != INVALID_SOCKET)
	{
		shutdown(hSocket, SHUT_RDWR);//�ر����Ӷ�����д
		close(hSocket);

		hSocket = INVALID_SOCKET;
	}
}

/// ��ɶ˿��̺߳���
void CTcpEpollServer::EpollWaitFunc(void)
{
	int32_t nEventCount = 0;
	struct epoll_event EpollEvent[128];	//epoll�¼�����

	while(-1 != m_hEpollHandle)
	{
		nEventCount = epoll_wait(m_hEpollHandle, EpollEvent, 128, 500);
        if (nEventCount == -1)
        {
            continue;
        }

		for (uint32_t nIndex = 0; nIndex < nEventCount; ++nIndex)
		{
		    struct epoll_event* pEpollEvent = &EpollEvent[nIndex];
			CTcpContext* lpContext = (CTcpContext*)pEpollEvent->data.ptr;
			SOCKET hSocket = lpContext->m_hSocket;

			if (hSocket == m_hListenSocket)
			{
				SOCKET hAcceptSocket = INVALID_SOCKET;
				struct sockaddr_in oAddr;
				socklen_t nAddrSize = sizeof(sockaddr_in);

				do
				{
					uint32_t nAddrLen = sizeof(sockaddr_in);
					hAcceptSocket = accept(m_hListenSocket, (sockaddr*)&oAddr, &nAddrSize);
					if(INVALID_SOCKET == hAcceptSocket)
					{
						//���˿��Ƿ�����ʱ��accept���أ�1��������errnoΪEAGAIN����ʱӦ�ü�����������
						//���˿��Ƿ�����ʱ���������׽����ϲ���������ɵĲ������أ�����errnoΪEWOULDBLOCK����ʱӦ�ü�����������
						if (errno != EAGAIN && errno != EWOULDBLOCK)
						{
						}
						break;
					}

					if(!EpollAcceptSocket(hAcceptSocket, oAddr))
					{
						close(hAcceptSocket);
						hAcceptSocket = INVALID_SOCKET;
					}
				}while(hAcceptSocket != INVALID_SOCKET);
			}
			else
			{
				if (pEpollEvent->events & EPOLLIN)
				{
					int32_t nRecvSize = 0;
					char szRecvBuff[MAX_PACK_BUFFER_SIZE];

					do
					{
						nRecvSize = recv(hSocket, szRecvBuff, MAX_PACK_BUFFER_SIZE, MSG_NOSIGNAL);
						if(nRecvSize > 0)
						{
							DealRecvData(szRecvBuff, nRecvSize, lpContext);
						}
						//�������ݳ���Ϊ0��˵�����ӶϿ���
						else if(nRecvSize == 0)
						{
							// �ر�����
							RemoveTcpContext(lpContext);
							break;
						}
						else
						{
							//���˿��Ƿ�����ʱ��recv���أ�1��������errnoΪEAGAIN����ʱӦ�������ݶ����ˣ�
							//���˿��Ƿ�����ʱ���������׽����ϲ���������ɵĲ������أ�����errnoΪEWOULDBLOCK����ʱӦ�������ݶ�����
							if (errno != EAGAIN && errno != EWOULDBLOCK)
							{
								// �ر�����
								RemoveTcpContext(lpContext);
							}
						}
					}while(nRecvSize > 0);
				}

				if(pEpollEvent->events & EPOLLOUT)
				{
					m_SendQueue.SendPacket(hSocket);
				}
			}
		}
	}
}

/// ���Ӽ���̺߳���
void CTcpEpollServer::ConnectCheckFunc(void)
{
	uint64_t i64CheckTime = GetSystemTime();
	while(INVALID_SOCKET != m_hListenSocket)
	{
		Sleep(300);

		if (GetSystemTime() > i64CheckTime + 30000)
		{
			// �ر���Ч����
			CheckInvalidContext();

			i64CheckTime = GetSystemTime();
		}
	}
}

/// ��ɶ˿��߳�
unsigned int CTcpEpollServer::EpollWaitThread(void *pParam)
{
	CTcpEpollServer* pThis = (CTcpEpollServer*)pParam;
	pThis->EpollWaitFunc();
	return 0;
}

/// ���Ӽ���߳�
unsigned int CTcpEpollServer::ConnectCheckThread(void* pParam)
{
	CTcpEpollServer* pThis = (CTcpEpollServer*)pParam;
	pThis->ConnectCheckFunc();
	return 0;
}

#endif //_WIN32
