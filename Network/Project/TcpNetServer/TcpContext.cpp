#include "ITcpNetServer.h"

//=============================================================================
CTcpContext::CTcpContext(void)
{
	m_hSocket = INVALID_SOCKET;
	m_i64ContextKey = 0;
	memset(&m_oSocketAddr, 0, sizeof(sockaddr_in));
}

CTcpContext::~CTcpContext(void)
{
}

/// ���SOCKET���
SOCKET CTcpContext::GetSocket(void) const
{
	return m_hSocket;
}

/// ���SOCKET��ַ
sockaddr_in CTcpContext::GetSocketAddr(void) const
{
	return m_oSocketAddr;
}

BOOL CTcpContext::CheckValid(void)
{
	return TRUE;
}
