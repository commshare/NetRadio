#include "Endianness.h"
#include "NetSerialize.h"

CNetSerialize::CNetSerialize(char* apBuffer, uint32_t alBufferSize, 
	ENUM_TYPE aenType)
	: CStdSerialize(apBuffer, alBufferSize, aenType)
{
}

CNetSerialize::~CNetSerialize(void)
{
}

//���л�int8_t���͵�ֵ
void CNetSerialize::Serialize(int8_t& nValue)
{
	CStdSerialize::Serialize(nValue);
}

//���л�uint8_t���͵�ֵ
void CNetSerialize::Serialize(uint8_t& nValue)
{
	CStdSerialize::Serialize(nValue);
}

//���л�int16_t���͵�ֵ
void CNetSerialize::Serialize(int16_t& nValue)
{
	if(GetType() == LOAD)
	{
		int16_t nNetValue = 0;
		CStdSerialize::Serialize(nNetValue);
		nValue = nNetValue;
		TO_LOCAL_16(nValue);
	}
	else
	{
		int16_t nNetValue = nValue;
		TO_NETWORK_16(nNetValue);
		CStdSerialize::Serialize(nNetValue);
	}
}

//���л�uint16_t���͵�ֵ
void CNetSerialize::Serialize(uint16_t& nValue)
{
	if(GetType() == LOAD)
	{
		uint16_t nNetValue = 0;
		CStdSerialize::Serialize(nNetValue);
		nValue = nNetValue;
		TO_LOCAL_16(nValue);
	}
	else
	{
		uint16_t nNetValue = nValue;
		TO_NETWORK_16(nNetValue);
		CStdSerialize::Serialize(nNetValue);
	}
}

//���л�int32_t���͵�ֵ
void CNetSerialize::Serialize(int32_t& nValue)
{
	if(GetType() == LOAD)
	{
		int32_t nNetValue = 0;
		CStdSerialize::Serialize(nNetValue);
		nValue = nNetValue;
		TO_LOCAL_32(nValue);
	}
	else
	{
		int32_t nNetValue = nValue;
		TO_NETWORK_32(nNetValue);
		CStdSerialize::Serialize(nNetValue);
	}
}

//���л�uint32_t���͵�ֵ
void CNetSerialize::Serialize(uint32_t& nValue)
{
	if(GetType() == LOAD)
	{
		uint32_t nNetValue = 0;
		CStdSerialize::Serialize(nNetValue);
		nValue = nNetValue;
		TO_LOCAL_32(nValue);
	}
	else
	{
		uint32_t nNetValue = nValue;
		TO_NETWORK_32(nNetValue);
		CStdSerialize::Serialize(nNetValue);
	}
}

//���л�int64_t���͵�ֵ
void CNetSerialize::Serialize(int64_t& nValue)
{
	if(GetType() == LOAD)
	{
		int64_t nNetValue = 0;
		CStdSerialize::Serialize(nNetValue);
		nValue = nNetValue;
		TO_LOCAL_64(nValue);
	}
	else
	{
		int64_t nNetValue = nValue;
		TO_NETWORK_64(nNetValue);
		CStdSerialize::Serialize(nNetValue);
	}
}

//���л�uint64_t���͵�ֵ
void CNetSerialize::Serialize(uint64_t& nValue)
{
	if(GetType() == LOAD)
	{
		uint64_t nNetValue = 0;
		CStdSerialize::Serialize(nNetValue);
		nValue = nNetValue;
		TO_LOCAL_64(nValue);
	}
	else
	{
		uint64_t nNetValue = nValue;
		TO_NETWORK_64(nNetValue);
		CStdSerialize::Serialize(nNetValue);
	}
}

//���л�float���͵�ֵ
void CNetSerialize::Serialize(float& fValue)
{
	if(GetType() == LOAD)
	{
		uint32_t nValue = 0;
		CStdSerialize::Serialize(nValue);
		TO_LOCAL_32(nValue);
		memcpy(&fValue, &nValue, sizeof(uint32_t));
	}
	else
	{
		uint32_t nValue = 0;
		memcpy(&nValue, &fValue, sizeof(uint32_t));
		TO_NETWORK_32(nValue);
		CStdSerialize::Serialize(nValue);
	}
}

//���л�double���͵�ֵ
void CNetSerialize::Serialize(double& fValue)
{
	if(GetType() == LOAD)
	{
		uint64_t nValue = 0;
		CStdSerialize::Serialize(nValue);
		TO_LOCAL_64(nValue);
		memcpy(&fValue, &nValue, sizeof(uint64_t));
	}
	else
	{
		uint64_t nValue = 0;
		memcpy(&nValue, &fValue, sizeof(uint64_t));
		TO_NETWORK_64(nValue);
		CStdSerialize::Serialize(nValue);
	}
}

//���л������͵�ֵ(��\0��β���ַ���)
void CNetSerialize::Serialize(char* pValue, uint16_t nMaxSize)
{
	if ( m_lBufferSize < ( m_lDataSize+2 ) )
		throw( -1 );

	uint16_t nLen = 0;
	if(m_enType == LOAD)
	{
		//���ȶ�ȡ����
		memcpy(&nLen, m_pBuffer+m_lDataSize, 2);
		// ת��Ϊ�����ֽ���

		TO_LOCAL_16(nLen);
		m_lDataSize += 2;

		//��ȡ���ݱ���
		if((nLen >= nMaxSize ) || ((m_lDataSize+nLen) > m_lBufferSize))
		{
			throw( -1 );
		}

		memcpy(pValue, m_pBuffer+m_lDataSize, nLen);
		pValue[nLen] = '\0';
		m_lDataSize += nLen;
	}
	else
	{
		//���ȴ洢����
		nLen = (uint16_t)strlen(pValue);
		if((nLen >= nMaxSize) || (nLen+m_lDataSize+2 > m_lBufferSize))
			throw( -1 );

		// ת��Ϊ�����ֽ���
		uint16_t nDataSize = nLen;
		TO_NETWORK_16(nDataSize);
		memcpy(m_pBuffer+m_lDataSize, &nDataSize, 2);
		m_lDataSize += 2;

		//�洢���ݱ���
		memcpy(m_pBuffer+m_lDataSize, pValue, nLen);
		m_lDataSize += nLen;
	}
}

// ���л���������
void CNetSerialize::Serialize(char* pValue, uint16_t nSize, uint16_t nMaxSize)
{
	CStdSerialize::Serialize(pValue, nSize, nMaxSize);
}
