#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "UdpPackBuffer.h"

//=============================================================================
/// UDP���ݰ��汾
#define UDP_PACK_VERSION			1
/// UDP���ݰ���ͷ����
#define TCP_PACK_HEAD_SIZE			14

/// �����չ���ݳ���
#define MAX_EXT_DATA_SIZE			32

/// TCP���ݰ�ͷ����־
static uint16_t UDP_PACK_HEAD_TAG = 1317;

//=============================================================================
BOOL _udp_pack_header::Serialize(CNetSerialize & aoNetSerialize)
{
	try
	{
		// ��ͷ��ʾ
		aoNetSerialize.Serialize(m_nPackHeadTag);
		// �汾��
		aoNetSerialize.Serialize(m_nPackVersion);
		// ʱ���
		aoNetSerialize.Serialize(m_nPackTimeStamp);

		// ��������
		aoNetSerialize.Serialize(m_nEncryptType);
		// ���ܺ����ݳ���
		aoNetSerialize.Serialize(m_nEncryptSize);
		// ������䳤��
		aoNetSerialize.Serialize(m_nExtFillSize);

		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}
}

BOOL _udp_pack_header::IsValid(void) const
{
	// ����ͷ
	if(m_nPackHeadTag != UDP_PACK_HEAD_TAG)
		return FALSE;

	// �������ͼ��
	if(m_nEncryptType < ENUM_ENCRYPT_NONE
		|| m_nEncryptType >= ENUM_ENCRYPT_COUNT)
	{
		return FALSE;
	}

	// ���ݰ����ȼ��
	if (m_nEncryptSize > MAX_PACK_BUFFER_SIZE || m_nExtFillSize > MAX_EXT_DATA_SIZE)
	{
		return FALSE;
	}

	return TRUE;
}

//=============================================================================
CUdpPackBuffer::CUdpPackBuffer(void)
	: m_pEncrypt(NULL)
{
	srand((unsigned) time(NULL));
	m_nTimeStamp = rand() % 2000;   

	m_enEncryptType = ENUM_ENCRYPT_NONE;
}


CUdpPackBuffer::~CUdpPackBuffer(void)
{
	try
	{
		Destroy();
	}
	catch (...)
	{
	}
}

BOOL CUdpPackBuffer::Create(ENUM_ENCRYPT_TYPE enEncryptType, 
	const char* szEncryKey, uint16_t nKeySize)
{
	BOOL bResult = FALSE;

	do 
	{
		if(IsCreated())
			break;

		m_enEncryptType = enEncryptType;

		if(IFAILED(CreateEncrypt(CLSID_IEncrypt, (void**)&m_pEncrypt)))
			break;

		if(NULL != szEncryKey && 0 != nKeySize)
		{
			if(!m_pEncrypt->SetEncryptKey(szEncryKey, nKeySize))
				break;
		}

		bResult = TRUE;

	} while (FALSE);

	if(!bResult)
	{
		if(NULL != m_pEncrypt)
		{
			DestroyEncrypt(CLSID_IEncrypt, m_pEncrypt);
			m_pEncrypt = NULL;
		}
	}

	return bResult;
}

void CUdpPackBuffer::Destroy(void)
{
	if(NULL != m_pEncrypt)
	{
		DestroyEncrypt(CLSID_IEncrypt, m_pEncrypt);
		m_pEncrypt = NULL;
	}

	m_enEncryptType = ENUM_ENCRYPT_NONE;
}

BOOL CUdpPackBuffer::IsCreated(void)
{
	return (NULL != m_pEncrypt);
}

/// ���ü�������
BOOL CUdpPackBuffer::SetEncryptType(ENUM_ENCRYPT_TYPE enEncryptType)
{
	m_enEncryptType = enEncryptType;
	return TRUE;
}

BOOL CUdpPackBuffer::SetEncryptKey(const char* szEncryKey, uint16_t nKeySize)
{
	BOOL bResult = FALSE;
	if(NULL != m_pEncrypt)
	{
		bResult = m_pEncrypt->SetEncryptKey(szEncryKey, nKeySize);
	}
	return bResult;
}

/// ���
uint32_t CUdpPackBuffer::Pack(const char* szInBuffer, uint16_t nInBufferSize, 
	char* szOutBuffer, uint16_t nOutBufferSize)
{
	// ����Ƿ񴴽�
	if(!IsCreated())
	{
		return 0;
	}

	// ���ȼ��
	if(nInBufferSize > nOutBufferSize
		|| nInBufferSize + 100 > MAX_PACK_BUFFER_SIZE)
	{
		return 0;
	}

	// ���ݼ���
	char szEncryptBuffer[MAX_PACK_BUFFER_SIZE] = {0};
	uint32_t nEncryptSize = 0;
	if(!m_pEncrypt->Encrypt(m_enEncryptType, szInBuffer, nInBufferSize, 
		szEncryptBuffer, nEncryptSize))
	{
		return 0;
	}

	udp_pack_header loPackHeader;
	memset(&loPackHeader, 0, sizeof(loPackHeader));

	loPackHeader.m_nPackHeadTag = UDP_PACK_HEAD_TAG;
	loPackHeader.m_nPackVersion = UDP_PACK_VERSION;
	loPackHeader.m_nPackTimeStamp = ++m_nTimeStamp;
	loPackHeader.m_nEncryptType = (uint8_t)m_enEncryptType;
	loPackHeader.m_nEncryptSize = nEncryptSize;
	loPackHeader.m_nExtFillSize = nEncryptSize - nInBufferSize;

	// ��ʼ���
	int liResult = 0;
	try
	{
		CNetSerialize loNetSerialize(szOutBuffer, nOutBufferSize, 
			CNetSerialize::STORE); 

		// ���л���ͷ
		loPackHeader.Serialize(loNetSerialize);

		// ��������
		loNetSerialize.Serialize(szEncryptBuffer, nEncryptSize, 
			MAX_PACK_BUFFER_SIZE);										

		liResult = loNetSerialize.GetDataSize();						//�õ�������ܳ�
	}
	catch(...)
	{
		liResult = 0;
	}
	return liResult;
}

/// ���
uint32_t CUdpPackBuffer::UnPack(const char* szInBuffer, uint16_t nInBufferSize, 
	char* szOutBuffer, uint16_t& nOutBufferSize, uint16_t& nTimeStamp)
{
	// ����Ƿ񴴽�
	if(!IsCreated())
	{
		return 0;
	}

	if(NULL == szInBuffer 
		|| NULL == szOutBuffer 
		|| nOutBufferSize < MAX_PACK_BUFFER_SIZE)
	{
		return 0;
	}

	try
	{
		char* pPackBuffer = const_cast<char*>(szInBuffer);
		uint32_t nPackBufferSize = nInBufferSize;

		CNetSerialize loSerialize(pPackBuffer, nPackBufferSize, 
			CNetSerialize::LOAD); 

		// ���л���ͷ
		udp_pack_header loPackHeader;
		loPackHeader.Serialize(loSerialize);
		// �жϰ�ͷ�Ƿ�����
		if(!loPackHeader.IsValid())
		{
			// ��ͷ������
			return 0;
		}

		// ���л���������
		char szEncryBuff[MAX_PACK_BUFFER_SIZE] = {0};          
		loSerialize.Serialize(szEncryBuff, loPackHeader.m_nEncryptSize, 
			MAX_PACK_BUFFER_SIZE);

		// ����
		uint32_t nBufferSize = nOutBufferSize;
		if (!m_pEncrypt->Decrypt((ENUM_ENCRYPT_TYPE)loPackHeader.m_nEncryptType, 
			szEncryBuff, loPackHeader.m_nEncryptSize, szOutBuffer, nBufferSize))
		{
			// ����ʧ��
			return 0;
		}

		nOutBufferSize = nBufferSize - loPackHeader.m_nExtFillSize;
		nTimeStamp = loPackHeader.m_nPackTimeStamp;

		return loSerialize.GetDataSize();
	}
	catch(...)
	{
		return 0;
	}
}