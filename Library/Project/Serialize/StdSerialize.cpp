#include <assert.h>
#include "StdSerialize.h"

CStdSerialize::CStdSerialize(char* apBuffer, uint32_t alBufferSize, 
	ENUM_TYPE aenType)
	: m_enType(aenType)
	, m_lDataSize(0)
	, m_pBuffer(apBuffer)
	, m_lBufferSize(alBufferSize)
{
	assert( m_pBuffer != NULL );
	assert( m_lBufferSize > 0 );
}


CStdSerialize::~CStdSerialize(void)
{
}

//ȡ�ô������
long CStdSerialize::GetDataSize(void) const
{
	return m_lDataSize;
}

// ȡ�����л�����
CStdSerialize::ENUM_TYPE CStdSerialize::GetType(void) const
{
	return m_enType;
}

//���л�int8_t���͵�ֵ
void CStdSerialize::Serialize(int8_t& nValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(int8_t)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//��ȡ
		memcpy(&nValue, m_pBuffer + m_lDataSize, sizeof(int8_t));
	}
	else
	{
		//�洢
		memcpy( m_pBuffer + m_lDataSize, &nValue, sizeof(int8_t));
	}
	m_lDataSize += sizeof(int8_t);
}

//���л�uint8_t���͵�ֵ
void CStdSerialize::Serialize(uint8_t& nValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(uint8_t)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//��ȡ
		memcpy(&nValue, m_pBuffer + m_lDataSize, sizeof(uint8_t));
	}
	else
	{
		//�洢
		memcpy( m_pBuffer + m_lDataSize, &nValue, sizeof(uint8_t));
	}
	m_lDataSize += sizeof(uint8_t);
}

//���л�int16_t���͵�ֵ
void CStdSerialize::Serialize(int16_t& nValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(int16_t)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//��ȡ
		memcpy(&nValue, m_pBuffer + m_lDataSize, sizeof(int16_t));
	}
	else
	{
		//�洢
		memcpy( m_pBuffer + m_lDataSize, &nValue, sizeof(int16_t));
	}
	m_lDataSize += sizeof(int16_t);
}

//���л�uint16_t���͵�ֵ
void CStdSerialize::Serialize(uint16_t& nValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(uint16_t)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//��ȡ
		memcpy(&nValue, m_pBuffer + m_lDataSize, sizeof(uint16_t));
	}
	else
	{
		//�洢
		memcpy( m_pBuffer + m_lDataSize, &nValue, sizeof(uint16_t));
	}
	m_lDataSize += sizeof(uint16_t);
}

//���л�int32_t���͵�ֵ
void CStdSerialize::Serialize(int32_t& nValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(int32_t)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//��ȡ
		memcpy(&nValue, m_pBuffer + m_lDataSize, sizeof(int32_t));
	}
	else
	{
		//�洢
		memcpy( m_pBuffer + m_lDataSize, &nValue, sizeof(int32_t));
	}
	m_lDataSize += sizeof(int32_t);
}

//���л�uint32_t���͵�ֵ
void CStdSerialize::Serialize(uint32_t& nValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(uint32_t)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//��ȡ
		memcpy(&nValue, m_pBuffer + m_lDataSize, sizeof(uint32_t));
	}
	else
	{
		//�洢
		memcpy( m_pBuffer + m_lDataSize, &nValue, sizeof(uint32_t));
	}
	m_lDataSize += sizeof(uint32_t);
}

//���л�int64_t���͵�ֵ
void CStdSerialize::Serialize(int64_t& nValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(int64_t)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//��ȡ
		memcpy(&nValue, m_pBuffer + m_lDataSize, sizeof(int64_t));
	}
	else
	{
		//�洢
		memcpy( m_pBuffer + m_lDataSize, &nValue, sizeof(int64_t));
	}
	m_lDataSize += sizeof(int64_t);
}

//���л�uint64_t���͵�ֵ
void CStdSerialize::Serialize(uint64_t& nValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(uint64_t)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//��ȡ
		memcpy(&nValue, m_pBuffer + m_lDataSize, sizeof(uint64_t));
	}
	else
	{
		//�洢
		memcpy( m_pBuffer + m_lDataSize, &nValue, sizeof(uint64_t));
	}
	m_lDataSize += sizeof(uint64_t);
}

//���л�float���͵�ֵ
void CStdSerialize::Serialize(float& fValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(float)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//��ȡ
		memcpy(&fValue, m_pBuffer + m_lDataSize, sizeof(float));
	}
	else
	{
		//�洢
		memcpy( m_pBuffer + m_lDataSize, &fValue, sizeof(float));
	}
	m_lDataSize += sizeof(float);
}

//���л�double���͵�ֵ
void CStdSerialize::Serialize(double& fValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(double)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//��ȡ
		memcpy(&fValue, m_pBuffer + m_lDataSize, sizeof(double));
	}
	else
	{
		//�洢
		memcpy( m_pBuffer + m_lDataSize, &fValue, sizeof(double));
	}
	m_lDataSize += sizeof(double);
}

//���л������͵�ֵ(��\0��β���ַ���)
void CStdSerialize::Serialize(char* pValue, uint16_t nMaxSize)
{
	if ( m_lBufferSize < ( m_lDataSize+2 ) )
		throw( -1 );

	uint16_t nLen = 0;
	if(m_enType == LOAD)
	{
		//���ȶ�ȡ����
		memcpy(&nLen, m_pBuffer+m_lDataSize, 2);
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

		memcpy(m_pBuffer+m_lDataSize, &nLen, 2);
		m_lDataSize += 2;

		//�洢���ݱ���
		memcpy(m_pBuffer+m_lDataSize, pValue, nLen);
		m_lDataSize += nLen;
	}
}

// ���л���������
void CStdSerialize::Serialize(char* pValue, uint16_t nSize, uint16_t nMaxSize)
{
	if((nSize > nMaxSize) || (m_lBufferSize < (m_lDataSize+nSize)))
		throw ( -1 );

	if(m_enType == LOAD)
	{
		//��Ϊ�ⲿ�ƶ��˶�ȡ���ȣ����Բ���Ҫ�����ݳ��Ƚ������л�
		memcpy(pValue, m_pBuffer+m_lDataSize, nSize);
	}
	else
	{
		//�洢���ݱ���
		memcpy(m_pBuffer+m_lDataSize, pValue, nSize);
	}
	m_lDataSize += nSize;
}
