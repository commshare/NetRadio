#ifdef _WIN32
#include "TcpIocpServer.h"
#include "DebugTrace.h"

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")

//=============================================================================
/// ��������
#define DEFAULT_LISTEN_BACK_LOG	50
/// �ص��ṹ����سߴ�
#define OVERLAP_CACHE_SIZE		2048

/// ���������
#define MAX_CONNECT_COUNT		50000

//=============================================================================
CTcpIocpServer::CTcpIocpServer(void)
	: m_hIocp(INVALID_HANDLE_VALUE)
	, m_hListenSocket(INVALID_SOCKET)
	, m_pEvent(NULL)
	, m_lpfnAcceptEx(NULL)
	, m_lpfnGetAcceptExSockAddrs(NULL)
	, m_CompletePotrThread(&CompletePortThread)
	, m_CheckThread(&ConnectCheckThread)
	, m_OverloapCache(OVERLAP_CACHE_SIZE)
{
	WSADATA lwsaData;
	WSAStartup(MAKEWORD(2,2), &lwsaData);

	//sendʱ���õ�overlap,
	memset(&m_oWriteOverlap, 0, sizeof(WSAOVERLAPPED));
	m_oWriteOverlap.hEvent = WSACreateEvent();		  //д�¼�
	m_oWriteOverlap.hEvent = (HANDLE)((DWORD)m_oWriteOverlap.hEvent | 0x01); //д�¼�����֪ͨ��ɶϿ�
}


CTcpIocpServer::~CTcpIocpServer(void)
{
	Destroy();

	WSACleanup();
}

BOOL CTcpIocpServer::Create(uint16_t nSvrPort, ITcpServerEvent* pSvrEvent)
{
	// �������
	ASSERT(pSvrEvent);
	ASSERT(nSvrPort != 0);
	if(NULL == pSvrEvent || 0 == nSvrPort)
		return FALSE;

	// �ظ��������
	if(INVALID_SOCKET != m_hListenSocket)
		return FALSE;

	BOOL bResult = FALSE;
	do 
	{
		// ������ɶ˿�
		m_hIocp = CreateCompletePort();
		if(INVALID_HANDLE_VALUE == m_hIocp)
			break;

		// ����SOCKET���
		m_hListenSocket = CreateSocket(nSvrPort);
		if(INVALID_SOCKET == m_hListenSocket)
			break;

		// ��ɶ˿���SOCKET��
		m_oListenContext.m_hSocket = m_hListenSocket;
		if (!CreateIoCompletionPort((HANDLE)m_hListenSocket, m_hIocp, 
			(ULONG_PTR)&m_oListenContext, 0))
		{
			TraceLogError("CTcpIocpServer::Create ����ɶ˿ڹ���ʧ�� ERROR=%d\n", 
				WSAGetLastError());
			break;
		}

		// �󶨶˿�
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(nSvrPort);
		if (SOCKET_ERROR == bind(m_hListenSocket, (LPSOCKADDR)&addr, sizeof(addr)))
		{
			TraceLogError("CTcpIocpServer::Create �󶨶˿�ʧ�� PORT=%d ERROR=%d\n", 
				nSvrPort, WSAGetLastError());
			break;
		}

		// ��ʼ����
		if(listen(m_hListenSocket, DEFAULT_LISTEN_BACK_LOG) == SOCKET_ERROR)
		{
			TraceLogError("CTcpIocpServer::Create ����ʧ�� ERROR=%d!\n", 
				WSAGetLastError());
			break;
		}

		// ���AcceptEx��غ���ָ��
		m_lpfnAcceptEx = GetAcceptExPtr(m_hListenSocket);
		m_lpfnGetAcceptExSockAddrs = GetAcceptExSockAddrs(m_hListenSocket);
		if(NULL == m_lpfnAcceptEx || NULL == m_lpfnGetAcceptExSockAddrs)
			break;

		// ����ACCEPT�¼�
		m_hAcceptEvent = CreateAcceptEvent(m_hListenSocket);
		if(INVALID_HANDLE_VALUE == m_hAcceptEvent)
		{
			break;
		}

		// Ͷ��ACCEPTEX
		for (int i = 0; i < 100 ; i++)
		{
			OVERLAPPEDPLUS* pOverlap = MallocOverlap(IO_ACCEPT);
			if(NULL != pOverlap)
			{
				if(!AddAcceptEx(pOverlap))
				{
					FreeOverlap(pOverlap);
				}
			}
		}

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

void CTcpIocpServer::Destroy(void)
{
	// ����SOCKET�׽���
	if(INVALID_SOCKET != m_hListenSocket)
	{
		DestroySocket(m_hListenSocket);
		m_hListenSocket = INVALID_SOCKET;
	}

	//�رռ���߳�
	if(INVALID_HANDLE_VALUE != m_hAcceptEvent)
	{
		WSASetEvent(m_hAcceptEvent);
	}

	//�ȴ�Contextɾ�����
	while ((GetTcpContextCount() > 0 || GetAcceptExCount() > 0))
	{
		//�ر�����������SOCKET
		CloseAllContext();

		//�ر�����δ����SOCKET
		CloseAllAcceptExSocket();

		Sleep(5);
	}

	// �ȴ��߳��˳�
	m_CompletePotrThread.WaitThreadExit();
	m_CheckThread.WaitThreadExit();

	// ������ɶ˿ھ��
	if(INVALID_HANDLE_VALUE != m_hIocp)
	{
		DestroyCompletePort(m_hIocp);
		m_hIocp = INVALID_HANDLE_VALUE;
	}

	// ����ACCEPT�¼�
	if(INVALID_HANDLE_VALUE != m_hAcceptEvent)
	{
		DestroyAcceptEvent(m_hAcceptEvent);
		m_hAcceptEvent = INVALID_HANDLE_VALUE;
	}

	// ��ջ���
	m_OverloapCache.Clear();
}

/// ��������
uint32_t CTcpIocpServer::Send(SOCKET hSocket, 
	const char* szDataBuffer, uint16_t nDataSize)
{
	// �������
	ASSERT(szDataBuffer);
	ASSERT(nDataSize);
	if(NULL == szDataBuffer || 0 == nDataSize)
		return 0;

	WSABUF loWsaBuff;
	loWsaBuff.len = nDataSize;
	loWsaBuff.buf = (char *)szDataBuffer;

	//д�¼�����֪ͨ��ɶϿڣ�����˰���VC��Ļ��
	uint32_t nNumberOfBytes = 0;

	CCriticalAutoLock loAutoLock(m_oWriteOverlapLock);
	int lResult = WSASend(hSocket, &loWsaBuff,1, (DWORD*)&nNumberOfBytes, 0, 
		&m_oWriteOverlap,NULL);
	return nNumberOfBytes;
}

//=============================================================================
/// ������ɶ˿�
HANDLE CTcpIocpServer::CreateCompletePort(void)
{
	//������ɶ˿�
	HANDLE hHandle = INVALID_HANDLE_VALUE;
	hHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hHandle == INVALID_HANDLE_VALUE)
	{
		TraceLogError("CTcpIocpServer::CreateCompletePort ������ɶ˿�ʧ�� ERROR=%d\n", 
			GetLastError());
		return INVALID_HANDLE_VALUE;
	}

	// �����߳�����
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	uint32_t nProcessors = si.dwNumberOfProcessors;
	uint32_t nThreadNumber = nProcessors * 2 + 2;

	// ���������߳� 
	if(!m_CompletePotrThread.StartThread(this, nThreadNumber))
	{
		CloseHandle(hHandle);
		hHandle = INVALID_HANDLE_VALUE;
	}

	return hHandle;
}

/// ������ɶ˿�
void CTcpIocpServer::DestroyCompletePort(HANDLE hHandle)
{
	if (INVALID_HANDLE_VALUE != hHandle)
	{
		CloseHandle(hHandle);
		hHandle = INVALID_HANDLE_VALUE;
	}
}

/// ����SOCKET�׽���
SOCKET CTcpIocpServer::CreateSocket(uint16_t nSvrPort)
{
	// ����SOCKET
	SOCKET hSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP,
		NULL, 0, WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET == hSocket)
	{
		TraceLogError("CTcpIocpServer::CreateSocket ����SOCKETʧ�� ERROR=%d\n", 
			nSvrPort, WSAGetLastError());
		return INVALID_SOCKET;
	}

	return hSocket;
}

/// ����SOCKET�׽���
void CTcpIocpServer::DestroySocket(SOCKET hSocket)
{
	if(INVALID_SOCKET != hSocket)
	{
		closesocket(hSocket);
		hSocket = NULL;
	}
}

/// ����ACCEPT�¼�
HANDLE CTcpIocpServer::CreateAcceptEvent(SOCKET hSocket)
{
	HANDLE hEvent = WSACreateEvent();
	if(INVALID_HANDLE_VALUE == hEvent)
	{
		TraceLogError("CTcpIocpServer::CreateAcceptEvent ���������¼����ʧ��!\n");
		return INVALID_HANDLE_VALUE;
	}

	if(SOCKET_ERROR == WSAEventSelect(hSocket, hEvent, FD_ACCEPT))
	{
		WSACloseEvent(hEvent);
		hEvent = INVALID_HANDLE_VALUE;
		TraceLogError("CTcpIocpServer::CreateAcceptEvent Ϊ����SOCKET����ACCEPT�¼�ʧ��!\n");
	}

	return hEvent;
}

/// ����ACCEPT�¼�
void CTcpIocpServer::DestroyAcceptEvent(HANDLE hEvent)
{
	if(INVALID_HANDLE_VALUE != hEvent)
	{
		WSACloseEvent(hEvent);
		hEvent = INVALID_HANDLE_VALUE;
	}
}

/// ����AcceptEx����ָ��
LPFN_ACCEPTEX CTcpIocpServer::GetAcceptExPtr(SOCKET hSocket)
{
	// AcceptEx��GUID�����ڵ�������ָ��
	GUID GuidAcceptEx = WSAID_ACCEPTEX;

	// ʹ��AcceptEx��������Ϊ���������WinSock2�淶֮���΢�������ṩ����չ����
	// ������Ҫ�����ȡһ�º�����ָ��, ��δȡ�ú���ָ�������¾͵���AcceptEx�Ŀ����Ǻܴ��

	// ��ȡAcceptEx����ָ��
	DWORD dwBytes = 0;
	LPFN_ACCEPTEX pFnAcceptEx = NULL;

	if(SOCKET_ERROR == WSAIoctl(hSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&GuidAcceptEx,sizeof(GuidAcceptEx), &pFnAcceptEx, sizeof(pFnAcceptEx), 
		&dwBytes, NULL, NULL))  
	{  
		TraceLogError("CTcpIocpServer::GetAcceptExPtr δ�ܻ�ȡ����ָ�롣ERROR=%d\n", 
			WSAGetLastError());
	}
	return pFnAcceptEx;
}

LPFN_GETACCEPTEXSOCKADDRS CTcpIocpServer::GetAcceptExSockAddrs(SOCKET hSocket)
{
	// GetAcceptExSockaddrs��GUID�����ڵ�������ָ��
	GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 

	// ��ȡGetAcceptExSockAddrs����ָ�룬Ҳ��ͬ��
	DWORD dwBytes = 0;
	LPFN_GETACCEPTEXSOCKADDRS pFnGetAcceptExSockAddrs = NULL;

	if(SOCKET_ERROR == WSAIoctl(hSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&GuidGetAcceptExSockAddrs, sizeof(GuidGetAcceptExSockAddrs), 
		&pFnGetAcceptExSockAddrs, sizeof(pFnGetAcceptExSockAddrs),   
		&dwBytes, NULL, NULL))  
	{  
		TraceLogError("CTcpIocpServer::GetAcceptExSockAddrs δ�ܻ�ȡ����ָ�롣ERROR=%d\n", 
			WSAGetLastError());
	}
	return pFnGetAcceptExSockAddrs;
}


/// �����ص��ṹ�ڴ�
OVERLAPPEDPLUS* CTcpIocpServer::MallocOverlap(ENUM_IO_TYPE enIOType)
{
	OVERLAPPEDPLUS* pOverlap = m_OverloapCache.Malloc();
	if(NULL != pOverlap)
	{
		memset(&(pOverlap->m_Overlapped), 0, sizeof(WSAOVERLAPPED));
		pOverlap->m_enIOType = enIOType;
		pOverlap->m_wsaBuffer.buf = pOverlap->m_szBuffer;
		pOverlap->m_wsaBuffer.len = MAX_PACK_BUFFER_SIZE;   
		pOverlap->m_hSocket = INVALID_SOCKET;
		pOverlap->m_nOverlapKey = 0;
	}
	return pOverlap;
}

/// �ͷ��ص��ṹ�ڴ�
void CTcpIocpServer::FreeOverlap(OVERLAPPEDPLUS* pOverlap)
{
	if(NULL != pOverlap)
	{
		m_OverloapCache.Free(pOverlap);
	}
}

/// ���AcceptEx����
BOOL CTcpIocpServer::AddAcceptEx(OVERLAPPEDPLUS* pOverlap)
{
	// �������
	ASSERT(pOverlap);
	if(NULL == pOverlap)
		return FALSE;

	// ����Socket
	SOCKET hAcceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP,
		NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == hAcceptSocket)
	{
		TraceLogError("CTcpIocpServer::AddAcceptEx ����SOCKET���ʧ�� ERROR=%d\n", 
			WSAGetLastError());
		return FALSE;
	}

	// �Զ��̳�m_hSocket������
	setsockopt(hAcceptSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, 
		(char *)&m_hListenSocket, sizeof(m_hListenSocket));

	// ����OverlapPlus
	pOverlap->m_hSocket = hAcceptSocket;
	pOverlap->m_enIOType = IO_ACCEPT;

	{
		// ���������
		CCriticalAutoLock loAutoLock(m_AcceptExListLock);
		POSITION position = m_AcceptExList.AddTail(pOverlap);
		// ����OverlapKey(���룬�������ɾ����)
		pOverlap->m_nOverlapKey = (uint64_t)position;
	}

	// ������������
	DWORD dwBytes = 0;
	BOOL bIsSuccess = m_lpfnAcceptEx(m_hListenSocket, hAcceptSocket,
		pOverlap->m_szBuffer, 
		pOverlap->m_wsaBuffer.len - ((sizeof(SOCKADDR_IN) + 16) * 2),
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes,
		(LPWSAOVERLAPPED)pOverlap);

	if (!bIsSuccess && WSAGetLastError() != ERROR_IO_PENDING)
	{
		closesocket(hAcceptSocket);
		hAcceptSocket = INVALID_SOCKET;

		TraceLogError("CTcpIocpServer::AddAcceptEx AcceptEx()����ʧ�� ERROR=%d\n", 
			WSAGetLastError());
		return FALSE;
	}
	return TRUE;
}

/// ɾ��AcceptEx����
BOOL CTcpIocpServer::RemoveAcceptEx(OVERLAPPEDPLUS* pOverlap)
{
	// �������
	ASSERT(pOverlap);
	if(NULL == pOverlap)
		return FALSE;

	CCriticalAutoLock loAutoLock(m_AcceptExListLock);

	POSITION pos = (POSITION)pOverlap->m_nOverlapKey;
	m_AcceptExList.RemoveAt(pos);
	return TRUE;
}

/// �ر�AcceptEx�����е�ȫ��SOCKET
void CTcpIocpServer::CloseAllAcceptExSocket(void)
{
	CCriticalAutoLock loAutoLock(m_AcceptExListLock);

	//�ر�����δ����SOCKET
	POSITION pos = m_AcceptExList.GetHeadPosition();
	while(NULL != pos)
	{
		OVERLAPPEDPLUS* pOverlap = m_AcceptExList.GetNext(pos);
		if(INVALID_SOCKET != pOverlap->m_hSocket)
		{
			closesocket(pOverlap->m_hSocket);
			pOverlap->m_hSocket = INVALID_SOCKET;
		}
	}
}

/// ���AcceptEx���еĳ�ʱ����
void CTcpIocpServer::CheckAcceptExTimeout(void)
{
	CCriticalAutoLock loAutoLock(m_AcceptExListLock);

	int32_t nResult  = 0;
	uint32_t nOptVal = 0;
	int32_t nOptLen  = 0;

	POSITION pos = m_AcceptExList.GetHeadPosition();
	while(NULL != pos)
	{
		OVERLAPPEDPLUS* pOverlap = m_AcceptExList.GetNext(pos);

		//���û��ѽ������ӣ�����30����û��������Ͽ�����
		nOptLen = sizeof(nOptVal);

		nResult = getsockopt(pOverlap->m_hSocket, SOL_SOCKET, SO_CONNECT_TIME, 
			(char*)&nOptVal, &nOptLen);
		if ((SOCKET_ERROR == nResult) || ((nOptVal != 0xFFFFFFFF) && (nOptVal > 40)))
		{	
			//�ر�����
			if (pOverlap->m_hSocket != INVALID_SOCKET)
			{
				TraceLogWarn("CTcpIocpServer ���ӳ�ʱ SOCKET=%d TIME=%d(s)\n", 
					pOverlap->m_hSocket, nOptVal);
				
				closesocket(pOverlap->m_hSocket);
				pOverlap->m_hSocket = INVALID_SOCKET;
			}
		}
	}
}

/// ���AcceptEx���гߴ�
uint32_t CTcpIocpServer::GetAcceptExCount(void) const
{
	CCriticalAutoLock loAutoLock(m_AcceptExListLock);
	return m_AcceptExList.GetCount();
}

/// ���Context
BOOL CTcpIocpServer::AddTcpContext(CTcpContext* pContext)
{
	// �������
	ASSERT(pContext);
	if(NULL == pContext)
		return FALSE;

	CCriticalAutoLock loAutoLock(m_ContextListLock);
	POSITION pos = m_ContextList.AddTail(pContext);
	pContext->m_i64ContextKey = (uint32_t)pos;
	return TRUE;
}

/// ɾ��Context
BOOL CTcpIocpServer::RemoveTcpContext(CTcpContext* pContext)
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
BOOL CTcpIocpServer::ContextIsValid(const CTcpContext* pContext)
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

BOOL CTcpIocpServer::ResetContext(CTcpContext* pContext)
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
void CTcpIocpServer::CloseAllContext(void)
{
	//�ر�����������SOCKET
	CCriticalAutoLock loAutoLock(m_ContextListLock);

	POSITION pos = m_ContextList.GetHeadPosition();
	while(NULL != pos)
	{
		CTcpContext* pContext = m_ContextList.GetNext(pos);
		closesocket(pContext->m_hSocket);
	}
}

/// �ر���Ч����
void CTcpIocpServer::CheckInvalidContext(void)
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
uint32_t CTcpIocpServer::GetTcpContextCount(void) const
{
	CCriticalAutoLock loAutoLock(m_ContextListLock);
	return m_ContextList.GetCount();
}

//=============================================================================
/// ��������
BOOL CTcpIocpServer::DealRecvData(uint32_t nRecvDataLen, 
	CTcpContext *pContext, OVERLAPPEDPLUS* pOverlapPlus)
{
	// �������
	ASSERT(nRecvDataLen);
	ASSERT(pContext);
	ASSERT(pOverlapPlus);
	if(0 == nRecvDataLen || NULL == pContext || NULL == pOverlapPlus)
		return FALSE;

	m_pEvent->OnRecvData(pOverlapPlus->m_szBuffer, nRecvDataLen, pContext);

	return TRUE;
}

/// ��ɶ˿��̺߳���
void CTcpIocpServer::CompletePortFunc(void)
{
	DWORD dwNumberOfBytes = 0;
	CTcpContext *lpTcpContext = NULL;
	CTcpContext* lpNewContext = NULL;

	LPWSAOVERLAPPED lpOverlapped = NULL;
	OVERLAPPEDPLUS *lpOverlapPlus = NULL;

	int32_t nRet = 0;
	uint32_t nFlags = 0;

	struct sockaddr_in* LocalAddr = NULL;
	struct sockaddr_in* RemoteAddr = NULL;
	int32_t LocalAddrLen = 0;
	int32_t RemoteAddrLen = 0;

	while (TRUE)
	{
		lpOverlapped = NULL;
		BOOL bResult = GetQueuedCompletionStatus(m_hIocp, &dwNumberOfBytes, 
			(ULONG *)&lpTcpContext, &lpOverlapped, INFINITE);
		if (NULL == lpOverlapped)
		{
			continue;
		}

		if(!bResult)
		{
			//PostQueuedCompletionStatus������һ���յĵ�������ݣ���ʾ�߳�Ҫ�˳���
			if (NULL == lpTcpContext)
			{
				break;
			}

			lpOverlapPlus = (OVERLAPPEDPLUS *)lpOverlapped;
			switch(lpOverlapPlus->m_enIOType)
			{
			case IO_WRITE:
				{	
					//ע�⣺�����ﲻ�ܷ���lpTcpContext
					FreeOverlap(lpOverlapPlus);
					break;
				}
			case IO_READ:
				{
					// �������ݳ���Ϊ��,�ͷ�lpOverlapPlus��lpTcpContext    
					if (0 == dwNumberOfBytes)
					{
						RemoveTcpContext(lpTcpContext);
						FreeOverlap(lpOverlapPlus);
						continue;
					}

					//������������
					if (!DealRecvData(dwNumberOfBytes, lpTcpContext, lpOverlapPlus))
					{
						//�ͷ�lpOverlapPlus��lpTcpContext
						RemoveTcpContext(lpTcpContext);
						FreeOverlap(lpOverlapPlus);
						continue;
					}

					nRet = WSARecv(lpTcpContext->m_hSocket, &lpOverlapPlus->m_wsaBuffer,
						1, &dwNumberOfBytes, (DWORD*)&nFlags, (OVERLAPPED*)lpOverlapPlus, NULL);
					if (nRet == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
					{
						//�ͷ�lpOverlapPlus��lpTcpContext
						RemoveTcpContext(lpTcpContext);
						FreeOverlap(lpOverlapPlus);						
					}

					break;
				}
			case IO_ACCEPT:
				{
					if(!RemoveAcceptEx(lpOverlapPlus))
					{
						continue;
					}

					//����Ͷ�ݵ�ACCEPT��С��100���򲹳�ACCEPT����
					if (GetAcceptExCount() < 100)
					{
						OVERLAPPEDPLUS* pOverlap = MallocOverlap(IO_ACCEPT);
						if(NULL != pOverlap)
						{
							if(!AddAcceptEx(pOverlap))
							{
								FreeOverlap(pOverlap);
							}
						}
					}

					if (0 == dwNumberOfBytes)
					{
						closesocket(lpOverlapPlus->m_hSocket);
						FreeOverlap(lpOverlapPlus);
						continue;
					}

					//ȡ�öԷ�IP
					m_lpfnGetAcceptExSockAddrs(lpOverlapPlus->m_szBuffer,
						lpOverlapPlus->m_wsaBuffer.len - ((sizeof(SOCKADDR_IN) + 16) * 2),
						sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
						(SOCKADDR **)&LocalAddr, &LocalAddrLen,
						(SOCKADDR **)&RemoteAddr, &RemoteAddrLen);

					//���뵥�������,Ȼ���socket����ɶ˿ڹ�������
					lpNewContext = m_pEvent->CreateContext();
					if (!lpNewContext)
					{
						closesocket(lpOverlapPlus->m_hSocket);
						FreeOverlap(lpOverlapPlus);
						continue;
					}

					lpNewContext->m_hSocket = lpOverlapPlus->m_hSocket;
					lpNewContext->m_oSocketAddr.sin_addr.s_addr = RemoteAddr->sin_addr.s_addr;
					lpNewContext->m_oSocketAddr.sin_port = RemoteAddr->sin_port;

					//��ӵ�����ͻ������ӵ��б���
					AddTcpContext(lpNewContext);

					//���½������׽��ֹ�������ɶ˿�
					if (!CreateIoCompletionPort((HANDLE)lpNewContext->m_hSocket,\
						m_hIocp, (DWORD)lpNewContext, 0))
					{
						RemoveTcpContext(lpNewContext);
						FreeOverlap(lpOverlapPlus);							
						continue;
					}

					//������������
					if (!DealRecvData(dwNumberOfBytes, lpNewContext, lpOverlapPlus))
					{
						RemoveTcpContext(lpNewContext);
						FreeOverlap(lpOverlapPlus);
						continue;
					}

					lpOverlapPlus->m_enIOType = IO_READ;	
					nRet = WSARecv(lpNewContext->m_hSocket, 	&lpOverlapPlus->m_wsaBuffer,
						1, &dwNumberOfBytes, (DWORD*)&nFlags, (OVERLAPPED*)lpOverlapPlus, NULL);
					if (nRet == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
					{
						RemoveTcpContext(lpNewContext);
						FreeOverlap(lpOverlapPlus);
					}					
					break;
				}
			}
		}
	}
}

/// ���Ӽ���̺߳���
void CTcpIocpServer::ConnectCheckFunc(void)
{
	uint32_t nTimeNow = 0;
	uint32_t nLastCheckTime = GetTickCount();

	int32_t nResult = 0;
	int32_t nOptlen = 0;
	uint32_t nOptval = 0;
	//int32_t nAcceptCount = 0;

	OVERLAPPEDPLUS *lpOverlapPlus = NULL;
	CTcpContext* lpTcpContext = NULL;
	uint32_t nWaitResult = 0;

	while(INVALID_SOCKET != m_hListenSocket)
	{
		nWaitResult = WSAWaitForMultipleEvents(1, &m_hAcceptEvent, FALSE, 1000, FALSE);
		if (INVALID_SOCKET == m_hListenSocket)
		{
			break;
		}

		if (WSA_WAIT_EVENT_0 == nWaitResult || GetAcceptExCount() < 2) //�������ӿ���
		{
			WSAResetEvent(m_hAcceptEvent);

			//Ͷ��AcceptEx����			
			for(uint32_t nAcceptCount = 0; nAcceptCount < 100; nAcceptCount++)
			{
				OVERLAPPEDPLUS* pOverlap = MallocOverlap(IO_ACCEPT);
				if(NULL != pOverlap)
				{
					if(!AddAcceptEx(pOverlap))
					{
						FreeOverlap(pOverlap);
					}
				}
			}
		}

		//10����һ��(Ԥ���ܾ����񹥻�)
		nTimeNow = GetTickCount();
		if (nTimeNow > nLastCheckTime + 10000)
		{
			// ��������(�û��ѽ������ӣ����ڹ涨ʱ����û����)
			CheckAcceptExTimeout();
			
			//�����Ч����
			CheckInvalidContext();

			nLastCheckTime = nTimeNow;
		}
	}
}

/// ��ɶ˿��߳�
unsigned int CTcpIocpServer::CompletePortThread(void *pParam)
{
	CTcpIocpServer* pThis = (CTcpIocpServer*)pParam;
	pThis->CompletePortFunc();
	return 0;
}

/// ���Ӽ���߳�
unsigned int CTcpIocpServer::ConnectCheckThread(void* pParam)
{
	CTcpIocpServer* pThis = (CTcpIocpServer*)pParam;
	pThis->ConnectCheckFunc();
	return 0;
}

#endif
