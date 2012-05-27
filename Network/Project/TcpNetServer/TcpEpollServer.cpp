#include "TcpEpollServer.h"
#include "DebugTrace.h"

//#ifndef _WIN32
//=============================================================================
//�趨�ļ�������������λ
//����������int sockfd��Ŀ��socket
// int value����Ϊ0ʱ����������Ϊ�����ģ���Ϊ��0ֵʱ����������Ϊ��������
static BOOL SetNonblock(int sockfd, BOOL bIsNonBlock)
{
#ifdef _WIN32
	return TRUE;
#else
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
		TraceLogError("CTcpEpollServer SetNonblock ����ʧ�� ERROR=%s!\n"), 
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
#endif
}

#ifdef _WIN32
int close(SOCKET hSocket)
{
	return closesocket(hSocket);
}

#define SHUT_RDWR SD_BOTH
#endif

//=============================================================================
CTcpEpollServer::CTcpEpollServer(void)
	: m_hListenSocket(INVALID_SOCKET)
	, m_hEpollHandle(-1)
	, m_pEvent(NULL)
	, m_EpollWaitThread(&EpollWaitThread)
	, m_CheckThread(&ConnectCheckThread)

{
}


CTcpEpollServer::~CTcpEpollServer(void)
{
	Destroy();
}

/// ����TCP������
BOOL CTcpEpollServer::Create(uint16_t nSvrPort, ITcpServerEvent* pSvrEvent)
{
	// �������
	ASSERT(pSvrEvent);
	ASSERT(nSvrPort);
	if(NULL == pSvrEvent || 0 == nSvrPort)
		return FALSE;

	BOOL bResult = FALSE;
	m_pEvent = pSvrEvent;

	do 
	{
		// ����EPOLL
		m_hEpollHandle = CreateEpoll();
		if(-1 == m_hEpollHandle)
			break;

		// ����SOCKET
		m_hListenSocket = CreateSocket(nSvrPort);
		if(INVALID_SOCKET == m_hListenSocket)
			break;

		m_ListenContext.m_hSocket = m_hListenSocket;

#ifndef _WIN32
		// SOCKET��EPOLL��
		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLET;			//���¼� ETģʽ
		ev.data.ptr = (void*)&m_ListenContext;

		if (epoll_ctl(m_hEpollHandle, EPOLL_CTL_ADD, m_hListenSocket, &ev) < 0)
		{
			TraceLogError("CTcpEpollServer::Create ��Epollʧ�� ERROR=%s!\n"), 
				strerror(errno));
			break;
		}

		// ����Epoll�߳�
		uint32_t nProcessors = sysconf(_SC_NPROCESSORS_CONF);
		nProcessors = nProcessors == 0 ? 1 : nProcessors;
		uint32_t nThreadNumber = nProcessors * 2 + 2;
		if(!m_EpollWaitThread.StartThread(this, nThreadNumber))
			break;

#endif

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

//=============================================================================
/// ����Epoll
int CTcpEpollServer::CreateEpoll(void)
{
#ifdef _WIN32
	return 1;
#else
	//����EPOLL
	int hEpollHandle = epoll_create(m_nMaxContextCount);
	if (hEpollHandle == -1)
	{
		TraceLogError("CTcpEpollServer::CreateEpoll ����Epollʧ�� ERROR=%s!\n"), 
			strerror(errno));
	}
	return hEpollHandle;
#endif
}

/// ����Epoll
void CTcpEpollServer::DestroyEpoll(int hEpollHandle)
{
#ifndef _WIN32
	//�ر�EPOLL���
	if (hEpollHandle != -1)
	{
		close(hEpollHandle);
		hEpollHandle = -1;
	}
#else
}

BOOL CTcpEpollServer::EpollAcceptSocket(SOCKET hSocket, const sockaddr_in& SockAddr)
{
#ifdef _WIN32
	return TRUE;
#else

	if(SOCKET_INVALID == hSocket)
		return FALSE;

    // ���ó̷�����ʽSOCKET
    if (!SetNonblock(hSocket, TRUE))
    {
        return FALSE;
    }

	// �ص��ϲ㴴��context
	CTcpContext *pContext = m_pEvent->CreateContext();
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
		RemoveTcpContext(pContext);(
		m_pEvent->DestroyContext(pContext);
		return FALSE;
	}

	return TRUE;

#endif
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

/// ���Context
BOOL CTcpEpollServer::AddTcpContext(CTcpContext* pContext)
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
BOOL CTcpEpollServer::RemoveTcpContext(CTcpContext* pContext)
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
			m_pEvent->DestroyContext(pContext);
			bResult = TRUE;
		}
	}
	return bResult;
}

/// ���Context�Ƿ���Ч
BOOL CTcpEpollServer::ContextIsValid(const CTcpContext* pContext)
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

BOOL CTcpEpollServer::ResetContext(CTcpContext* pContext)
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
			pContext->ResetContext();
			bResult = TRUE;
		}
	}
	return bResult;
}

/// �ر�����Context
void CTcpEpollServer::CloseAllContext(void)
{
	//�ر�����������SOCKET
	CCriticalAutoLock loAutoLock(m_ContextListLock);

	POSITION pos = m_ContextList.GetHeadPosition();
	while(NULL != pos)
	{
		CTcpContext* pContext = m_ContextList.GetNext(pos);
		closesocket(pContext->m_hSocket);
		pContext->m_hSocket = INVALID_SOCKET;
	}
}

/// �ر���Ч����
void CTcpEpollServer::CheckInvalidContext(void)
{
	CCriticalAutoLock loAutoLock(m_ContextListLock);

	//�����Ч����
	POSITION pos = m_ContextList.GetHeadPosition();
	while(NULL != pos)
	{
		CTcpContext* pContext = m_ContextList.GetNext(pos);
		if(!pContext->CheckValid())
		{
			closesocket(pContext->m_hSocket);
			pContext->m_hSocket = INVALID_SOCKET;
		}
	}
}

/// �����������
uint32_t CTcpEpollServer::GetTcpContextCount(void) const
{
	CCriticalAutoLock loAutoLock(m_ContextListLock);
	return m_ContextList.GetCount();
}

/// ��ɶ˿��̺߳���
void CTcpEpollServer::EpollWaitFunc(void)
{
#ifndef _WIN32
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
			CTcpContext* lpContext = (CTcpContext*)pEpollEvent->data.ptr;
			SOCKET hSocket = lpContext->m_hSocket;

			if (hSocket == m_hListenSocket)
			{
				SOCKET hAcceptSocket = INVALID_SOCKET;
				struct sockaddr_in oAddr;
				socklen_t nAddrSize = sizeof(sockaddr_in);

				do
				{
					nAddrLen = sizeof(sockaddr_in);
					hAcceptSocket = accept(m_hListenSocket, (sockaddr*)&oAddr, &nAddrSize);
					if(INVALID_SCOKET == hAcceptSocket)
					{
						//���˿��Ƿ�����ʱ��accept���أ�1��������errnoΪEAGAIN����ʱӦ�ü�����������
						//���˿��Ƿ�����ʱ���������׽����ϲ���������ɵĲ������أ�����errnoΪEWOULDBLOCK����ʱӦ�ü�����������
						if (errno != EAGAIN && errno != EWOULDBLOCK)
						{
							continue;
						}
						break;
					}

					if(!EpollAcceptSocket(hAcceptSocket, oAddr);
					{
						closesocket(hAcceptSocket);
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
							m_pEvent->OnRecvData(szRecvBuff, nRecvSize, lpContext);
						}
						//�������ݳ���Ϊ0��˵�����ӶϿ���
						else if(nRecvSize == 0)
						{
							// �ر�����
							break;
						}
						else
						{
							//���˿��Ƿ�����ʱ��recv���أ�1��������errnoΪEAGAIN����ʱӦ�������ݶ����ˣ�
							//���˿��Ƿ�����ʱ���������׽����ϲ���������ɵĲ������أ�����errnoΪEWOULDBLOCK����ʱӦ�������ݶ�����
							if (errno != EAGAIN && errno != EWOULDBLOCK)
							{
								// �ر�����
							}
						}
					}while(nRecvSize > 0);
				}

				if(pEpollEvent->events & EPOLLOUT)
				{
					//�����У�������������
				}
			}
		}
	}
#endif
}

/// ���Ӽ���̺߳���
void CTcpEpollServer::ConnectCheckFunc(void)
{
#ifndef _WIN32
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
#endif
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
