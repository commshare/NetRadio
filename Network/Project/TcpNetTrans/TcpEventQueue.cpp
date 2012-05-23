#include "TcpEventQueue.h"

//=============================================================================
CTcpEventQueue::CTcpEventQueue(void)
{
}


CTcpEventQueue::~CTcpEventQueue(void)
{
	RemoveAll();
}

/// ����¼�
BOOL CTcpEventQueue::PushEvent(ENUM_TCP_NET_EVENT enEvent)
{
	CCriticalAutoLock loAutoLock(m_ListLock);
	m_EventList.AddTail(enEvent);
	return TRUE;
}

/// ɾ���¼�
ENUM_TCP_NET_EVENT CTcpEventQueue::PopEvent(void)
{
	CCriticalAutoLock loAutoLock(m_ListLock);
	ENUM_TCP_NET_EVENT enEvent = m_EventList.RemoveHead();
	return enEvent;
}

/// ɾ��ȫ���¼�
void CTcpEventQueue::RemoveAll(void)
{
	CCriticalAutoLock loAutoLock(m_ListLock);
	m_EventList.RemoveAll();
}

/// �ж϶����Ƿ�Ϊ��
BOOL CTcpEventQueue::IsEmpty(void) const
{
	CCriticalAutoLock loAutoLock(m_ListLock);
	return m_EventList.IsEmpty();
}
