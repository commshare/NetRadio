#include "EncryptInfo.h"
#include "DebugTrace.h"

//=============================================================================
/// Ĭ�ϼ�����Կ
#define DEFAULT_ENCRYPT_KEY			"24698B5E-69E9-485B-BAB7-FC685D4AEFCC"
/// Ĭ�ϼ�����Կ����
#define DEFAULT_ENCRYPT_KEY_SIZE	16

//=============================================================================
CEncryptInfo::CEncryptInfo(void)
	: m_enEncryptType(ENUM_ENCRYPT_NONE)
	, m_enEncryptKeySize(DEFAULT_ENCRYPT_KEY_SIZE)
{
	strncpy(m_szEncryptKey, DEFAULT_ENCRYPT_KEY, DEFAULT_ENCRYPT_KEY_SIZE);
}

CEncryptInfo::~CEncryptInfo(void)
{
}

/// ���ü�������
void CEncryptInfo::SetEncryptType(ENUM_ENCRYPT_TYPE enType)
{
	m_enEncryptType = enType;
}

/// ��ȡ�������� 
ENUM_ENCRYPT_TYPE CEncryptInfo::GetEncryptType(void) const
{
	return m_enEncryptType;
}

/// ���ü�����Կ
BOOL CEncryptInfo::SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize)
{
	ASSERT(szEncryptKey);
	ASSERT(nKeySize);
	if(NULL == szEncryptKey || 0 == nKeySize)
		return FALSE;

	m_enEncryptKeySize = TCP_SVR_ENCRYPT_KEY_SIZE;
	if(nKeySize < m_enEncryptKeySize)
		m_enEncryptKeySize = nKeySize;
	memcpy(m_szEncryptKey, szEncryptKey, m_enEncryptKeySize);
	return TRUE;
}
/// ��ȡ������Կ
BOOL CEncryptInfo::GetEncryptKey(char* szEncryptKey, uint16_t& nKeySize)
{
	/// �������
	ASSERT(szEncryptKey);
	ASSERT(nKeySize);
	if(NULL == m_enEncryptType || 0 == nKeySize 
		|| nKeySize < m_enEncryptKeySize)
		return FALSE;

	memcpy(szEncryptKey, m_szEncryptKey, m_enEncryptKeySize);
	nKeySize = m_enEncryptKeySize;
	return TRUE;
}

/// ��ü�����Կ
const char* CEncryptInfo::GetEncryptKey(void) const
{
	return m_szEncryptKey;
}

/// ��ü�����Կ����
uint16_t CEncryptInfo::GetEncryptKeySize(void) const
{
	return m_enEncryptKeySize;
}