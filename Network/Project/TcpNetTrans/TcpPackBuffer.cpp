#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "TcpPackBuffer.h"

//=============================================================================
/// TCP���ݰ��汾
#define TCP_PACK_VERSION		1
/// TCP���ݰ���ͷ����
#define TCP_PACK_HEAD_SIZE		14
/// TCP���ݰ���β����
#define TCP_PACK_TAIL_SIZE		3

/// �����չ���ݳ���
#define MAX_EXT_DATA_SIZE		32

/// TCP���ݰ�ͷ����־
static char TCP_PACK_HEAD_TAG[] = "\03\02\01\0";
/// TCP���ݰ�β����־
static char TCP_PACK_TAIL_TAG[] = "\06\05\04\0";

//=============================================================================
BOOL _tcp_pack_header::Serialize(CNetSerialize & aoNetSerialize)
{
	try
	{
		// ��ͷ��ʾ
		aoNetSerialize.Serialize(m_szPackHeadTag, 3, 3);
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

BOOL _tcp_pack_header::IsValid(void) const
{
	// ����ͷ
	if(memcmp(m_szPackHeadTag, TCP_PACK_HEAD_TAG, 3) != 0)
		return FALSE;

	// �������ͼ��
	if(m_nEncryptType <= ENUM_ENCRYPT_NONE
		|| m_nEncryptType > ENUM_ENCRYPT_COUNT)
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
CTcpPackBuffer::CTcpPackBuffer(void)
	: m_pEncrypt(NULL)
{
	srand((unsigned) time(NULL));
	m_nTimeStamp = rand() % 2000;   

	memset(m_szBuffer, 0, MAX_PACK_BUFFER_SIZE*2);
	m_nDataSize = 0;
}


CTcpPackBuffer::~CTcpPackBuffer(void)
{
	try
	{
		Destroy();
	}
	catch (...)
	{
	}
}

BOOL CTcpPackBuffer::Create(const char* szEncryKey, uint16_t nKeySize)
{
	BOOL bResult = FALSE;
	
	do 
	{
		if(IsCreated())
			break;
			
		if(NULL == szEncryKey || 0 == nKeySize)
			break;

		if(IFAILED(CreateEncrypt(CLSID_IEncrypt, (void**)&m_pEncrypt)))
			break;

		if(!m_pEncrypt->SetEncryptKey(szEncryKey, nKeySize))
			break;

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

void CTcpPackBuffer::Destroy(void)
{
	if(NULL != m_pEncrypt)
	{
		DestroyEncrypt(CLSID_IEncrypt, m_pEncrypt);
		m_pEncrypt = NULL;
	}
}

BOOL CTcpPackBuffer::IsCreated(void)
{
	return (NULL != m_pEncrypt);
}

BOOL CTcpPackBuffer::SetEncryptKey(const char* szEncryKey, uint16_t nKeySize)
{
	BOOL bResult = FALSE;
	if(NULL != m_pEncrypt)
	{
		bResult = m_pEncrypt->SetEncryptKey(szEncryKey, nKeySize);
	}
	return bResult;
}

/// ���
uint32_t CTcpPackBuffer::Pack(const char* szInBuffer, uint16_t nInBufferSize, 
	char* szOutBuffer, uint16_t nOutBufferSize, 
	ENUM_ENCRYPT_TYPE enEncryptType)
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
	uint32_t nEncryptSize = MAX_PACK_BUFFER_SIZE;
    if(!m_pEncrypt->Encrypt(enEncryptType, szInBuffer, nInBufferSize, 
		szEncryptBuffer, nEncryptSize))
    {
        return 0;
    }

	tcp_pack_header loPackHeader;
	memset(&loPackHeader, 0, sizeof(loPackHeader));

	memcpy(loPackHeader.m_szPackHeadTag, TCP_PACK_HEAD_TAG, 4);
	loPackHeader.m_nPackVersion = TCP_PACK_VERSION;
	loPackHeader.m_nPackTimeStamp = ++m_nTimeStamp;
	loPackHeader.m_nEncryptType = (uint8_t)enEncryptType;
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

		// ��β
        loNetSerialize.Serialize(TCP_PACK_TAIL_TAG, TCP_PACK_TAIL_SIZE, 
			TCP_PACK_TAIL_SIZE); 

        liResult = loNetSerialize.GetDataSize();						//�õ�������ܳ�
    }
    catch(...)
    {
        liResult = 0;
    }
    return liResult;
}

/// ���
uint32_t CTcpPackBuffer::UnPack(const char* szInBuffer, uint16_t nInBufferSize, 
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
		uint32_t nUnpackSize = m_nDataSize;
		uint32_t nPackBufferSize = nInBufferSize+m_nDataSize;

		// �������δ��ѹ������
		if(m_nDataSize > 0)
		{
			pPackBuffer = m_szBuffer;
			memcpy(m_szBuffer+m_nDataSize, szInBuffer, nInBufferSize);
		}

		CNetSerialize loSerialize(pPackBuffer, nPackBufferSize, 
			CNetSerialize::LOAD); 

		// ���л���ͷ
		tcp_pack_header loPackHeader;
		loPackHeader.Serialize(loSerialize);
		// �жϰ�ͷ�Ƿ�����
		if(!loPackHeader.IsValid())
		{
			// ��ͷ������
			return 0;
		}

		// ������ݰ��Ƿ�������
		if (nPackBufferSize < (loPackHeader.m_nEncryptSize + 
			loSerialize.GetDataSize() + TCP_PACK_TAIL_SIZE))
		{
			//���ݰ����ղ�ȫ
			memcpy(m_szBuffer, pPackBuffer, nPackBufferSize);
			m_nDataSize = nPackBufferSize;
			return 0;
		}

		// ����β�Ƿ�����
		char szPackTailTag[4] = {0};
		memcpy(szPackTailTag, 
			szInBuffer+loSerialize.GetDataSize()+ loPackHeader.m_nEncryptSize, 
			TCP_PACK_TAIL_SIZE);
		if (memcmp(szPackTailTag, TCP_PACK_TAIL_TAG, TCP_PACK_TAIL_SIZE) != 0)
		{
			// ��β������
			m_nDataSize = 0;
			return 0;
		}

		// ���л���������
		char szEncryBuff[MAX_PACK_BUFFER_SIZE] = {0};          
        loSerialize.Serialize(szEncryBuff, loPackHeader.m_nEncryptSize, 
			MAX_PACK_BUFFER_SIZE);

		// ���л���β
		loSerialize.Serialize(szPackTailTag, TCP_PACK_TAIL_SIZE, 4);

		// ����
		uint32_t nBufferSize = nOutBufferSize;
		if (!m_pEncrypt->Decrypt((ENUM_ENCRYPT_TYPE)loPackHeader.m_nEncryptType, 
			szEncryBuff, loPackHeader.m_nEncryptSize, szOutBuffer, nBufferSize)) //����ʧ��
		{
			// ����ʧ��
			m_nDataSize = 0;
			return 0;
		}

		nOutBufferSize = nBufferSize - loPackHeader.m_nExtFillSize;
		nTimeStamp = loPackHeader.m_nPackTimeStamp;

		return loSerialize.GetDataSize()-nUnpackSize;
	}
	catch(...)
	{
		return 0;
	}
}