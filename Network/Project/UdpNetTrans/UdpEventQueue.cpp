#include "UdpEventQueue.h"

//=============================================================================
CUdpEventQueue::CUdpEventQueue(void)
{
}


CUdpEventQueue::~CUdpEventQueue(void)
{
	RemoveAll();
}

/// ����¼�
BOOL CUdpEventQueue::PushEvent(ENUM_UDP_NET_EVENT enEvent)
{
	CCriticalAutoLock loAutoLock(m_ListLock);
	m_EventList.AddTail(enEvent);
	return TRUE;
}

/// ɾ���¼�
ENUM_UDP_NET_EVENT CUdpEventQueue::PopEvent(void)
{
	CCriticalAutoLock loAutoLock(m_ListLock);
	ENUM_UDP_NET_EVENT enEvent = m_EventList.RemoveHead();
	return enEvent;
}

/// ɾ��ȫ���¼�
void CUdpEventQueue::RemoveAll(void)
{
	CCriticalAutoLock loAutoLock(m_ListLock);
	m_EventList.RemoveAll();
}

/// �ж϶����Ƿ�Ϊ��
BOOL CUdpEventQueue::IsEmpty(void) const
{
	CCriticalAutoLock loAutoLock(m_ListLock);
	return m_EventList.IsEmpty();
}
